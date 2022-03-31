#include "b_lift.hpp"

// #define master partner

// Front Lift object
B_Lift b_lift({{"B_Lift",
{
  "managed",
  "bottom",
  "idle",
  "move_to_target",
  "manual",
  "shifting_to_lift"
}
}, b_lift_m});


B_Lift::B_Lift(Motorized_subsystem<b_lift_states, NUM_OF_B_LIFT_STATES, B_LIFT_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  // state setup
  target_state = b_lift_states::move_to_target; // sends lift to bottom upon startup
  state = b_lift_states::managed;

  index = 0;

  up_press.pause();
  down_press.pause();
}

void B_Lift::handle_buttons(){
  // index incrementing and decrementing
  if(master.get_digital_new_press(lift_up_button) && !get_lift()){
    up_press.reset();
    // if state is manual, go to the closest position that's higher than the current position
    if(state == b_lift_states::manual){
      int i = 0;
      while (i < driver_positions.size()){
        if(driver_positions[i] > f_lift_pot.get_value()){
          set_state(b_lift_states::move_to_target, i);
          break;
        }
        i++;
      }
    }
    // otherwise just go to the next highest position, but doesn't increment index if it's out of bounds
    else if(index < driver_positions.size() - 1)  set_state(b_lift_states::move_to_target, ++index);
  }
  if(master.get_digital_new_press(lift_down_button) && !get_lift()){
    down_press.reset();
    // if state is manual, go to the closest position that's lower than the current position
    if(state == b_lift_states::manual){
      int i = driver_positions.size() - 1;
      while (i > -1){
        if(driver_positions[i] < f_lift_pot.get_value()){
          set_state(b_lift_states::move_to_target, i);
          break;
        }
        i--;
      }
    }
    // otherwise just go to the next lowest position, but doesn't decrement index if it's out of bounds
    else if(index > 0)  set_state(b_lift_states::move_to_target, --index);
  }
  // resets and pauses the timers if driver releases button
  if(!master.get_digital(lift_up_button)) up_press.reset(false);
  if(!master.get_digital(lift_down_button)) down_press.reset(false);

  // goes to top position if up button is held
  if(up_press.get_time() > 300) set_state(b_lift_states::move_to_target, driver_positions.size() - 1);
  // goes to bottom position of down button is held
  if(down_press.get_time() > 300) set_state(b_lift_states::move_to_target, 0);
}

void B_Lift::handle(bool driver_array){
  // decides which position vector to use
  std::vector<int>& positions = driver_array? driver_positions: prog_positions;

  switch(state){
    case b_lift_states::managed:  // being controlled externally
      break;
    case b_lift_states::bottom: // at lowest position, this state is used by the intake and f_claw
      break;
    case b_lift_states::idle: // not doing anything
      break;

    case b_lift_states::move_to_target: // moving to target
      motor.move(pid.compute(b_lift_pot.get_value(), positions[index]));
      
      // moves to next state if the lift has reached its target
      if(fabs(pid.get_error()) < end_error){
        motor.move_velocity(0); // holds motor
        // switches to idle by default or special case depending on current target
        switch(index){
          case 0: // lift is at bottom position, this state is used by intake 
            set_state(b_lift_states::bottom);
            break;
          default:
            set_state(b_lift_states::idle);
            break;
        }
      }

      // UNCOMMENT FOR LIFT SAFETY Handling
      // if the motor is drawing more than 2.3 amps for more than 100 ms safety out
      /*
      if (motor.get_current_draw() > 2300 && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
      else bad_count = 0;
      if(bad_count > 10 && state != b_lift_states::manual){
        motor.move(0);
        master.rumble("---");
        master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Manual      ");
        printf("LIFT SAFETY TRIGGERED %lf, %lf\n", target, f_lift_pot.get_value());

        set_state(b_lift_states::manual);
      }
      */
      break;

    case b_lift_states::manual:
      lift_power = master.get_analog(ANALOG_RIGHT_X);
      // holds motor if joystick is within deadzone or lift is out of range
      if (fabs(lift_power) < 10 || (lift_power < 0 && f_lift_pot.get_value() <= bottom_position) || (lift_power > 0 && f_lift_pot.get_value() >= top_position)) motor.move_velocity(0);
      else motor.move(lift_power);
      // exits manual state if up or down button is pressed or held
      break;

    case b_lift_states::shifting_to_lift:
      if(shift_timer.get_time() > 200){
        set_state(after_switch_state);
      }
      break;

  }
  handle_state_change(); // cleans up and preps the machine to be in the target state
}

void B_Lift::handle_state_change(){
  if(target_state == state) return;
  // if state has changed, performs the necessary cleanup operation before entering next state

  if(state == b_lift_states::bottom){ // if lift is leaving the bottom state, turn off the intake
    b_lock_p.set_value(HIGH);  // unlock the lift
    intake.set_state(intake_states::off);
  }

  switch(target_state){
    case b_lift_states::managed:
      break;

    case b_lift_states::bottom:
      b_lock_p.set_value(HIGH);  // lock the lift
      break;

    case b_lift_states::idle:
      break;

    case b_lift_states::move_to_target:
      break;

    case b_lift_states::manual:
      master.rumble("-");
      // we don't want the f_claw in search mode while the lift is in manual
      if(f_claw.get_state() == f_claw_states::searching){
        f_claw.set_state(f_claw_states::idle);
      }
      break;

    case b_lift_states::shifting_to_lift:
      shift_timer.reset();
      motor.move(30);
      lift_trans_p.set_value(HIGH);
      trans_p_state = HIGH;
      break;

  }
  log_state_change();  

  // if intake/lift transmission is in intake mode, shift the transmission to the lift
  if(!trans_p_state){
    after_switch_state = target_state;
    set_state(b_lift_states::shifting_to_lift);
  }
}

// regular set state method (common to all subsystems)
void B_Lift::set_state(const b_lift_states next_state){  // requests a state change and logs it (NORMAL set state)
  state_log.print("%s | State change requested from %s to %s, index is: %d\n", name, state_names[static_cast<int>(state)], state_names[static_cast<int>(next_state)], index);
  target_state = next_state;
}
// accepts an index argument used specifically for a move to target
void B_Lift::set_state(const b_lift_states next_state, const double index){  // requests a state change and logs it
  // confirms state change only if the state is actually move to target
  if (target_state == b_lift_states::move_to_target){
    state_log.print("%s | State change requested from %s to %s, index is: %d\n", name, state_names[static_cast<int>(state)], state_names[static_cast<int>(next_state)], index);
    target_state = next_state;
    this->index = index;
  }
  else state_log.print("%s | INVALID move to target State change requested from %s to %s, index is: %d\n", index, name, state_names[static_cast<int>(state)], state_names[static_cast<int>(next_state)], index);
}

extern int elastic_f_up_time, elastic_f_down_time; //from gui_construction.cpp

void B_Lift::elastic_util(){
  reset();
  motor.move(-10);
  GUI::go("Start Elastic Utility", "Press to start the elastic utility.", 500);
  f_claw_p.set_value(HIGH);
  Timer move_timer{"move"};
  move_absolute(top_position);
  // // intake_piston.set_value(HIGH);  // raises intake
  waitUntil(fabs(f_lift_pot.get_value() - top_position) < end_error);
  move_timer.print();
  elastic_f_up_time = move_timer.get_time();
  master.print(1, 0, "up time: %d", elastic_f_up_time);

  move_timer.reset();
  move_absolute(bottom_position);
  waitUntil(fabs(f_lift_pot.get_value() - bottom_position) < end_error);
  move_timer.print();
  elastic_f_down_time = move_timer.get_time();
  master.print(2, 0, "down time: %d", elastic_f_up_time);
}


// BACK CLAW SUBSYSTEM
B_Claw b_claw({"B_Claw",
{
  "managed",
  "idle",
  "search_lip",
  "search_bowl",
  "tilted",
  "flat"
}
});

B_Claw::B_Claw(Subsystem<b_claw_states, NUM_OF_B_CLAW_STATES> subsystem): Subsystem(subsystem)  // constructor
{
  // state setup
  target_state = b_claw_states::idle;
  state = b_claw_states::managed;
}

void B_Claw::handle_buttons(){
  // resets the press timer if toggle button is pressed
  if(master.get_digital_new_press(claw_toggle_button) && !get_lift()){
    toggle_press_timer.reset();
    // grabs goal if toggle button is pressed
    if(state == b_claw_states::idle) set_state(b_claw_states::tilted);
  }
  if(toggle_press_timer.get_time() > 300){  // if claw button was held
    // toggles tilt state
    if(state == b_claw_states::tilted) set_state(b_claw_states::flat);
    else if(state == b_claw_states::flat) set_state(b_claw_states::tilted);
    toggle_press_timer.reset(false); // resets and pauses the timer 
  }
  else{
    if(!master.get_digital(claw_toggle_button)){
      toggle_press_timer.reset(false);  // resets and pauses the timer 
      // releases goal if toggle button is released before the button hold timeout triggers
      if(state == b_claw_states::tilted || state == b_claw_states::flat)  set_state(b_claw_states::idle);
    }
  }
}

void B_Claw::handle(){
  switch(state){
    case b_claw_states::managed:
      break;

    case b_claw_states::idle:
      break;

    case b_claw_states::search_lip:
      // searches for bowl of mogo once lip is detected
      if(b_dist.get() < 70) set_state(b_claw_states::search_bowl);
      break;

    case b_claw_states::search_bowl:
      // grabs goal if up button is pressed or bowl is detected
      if (b_dist.get() > 70 && b_dist.get() < 95) search_cycle_check_count++;
      if(search_cycle_check_count >= 2) set_state(b_claw_states::tilted);
      break;

    case b_claw_states::tilted:
      break;

    case b_claw_states::flat:
      break;
  }
  handle_state_change();  // cleans up and preps the machine to be in the target state
}

void B_Claw::handle_state_change(){
  if(target_state == state) return;
  // if state has changed, performs the necessary cleanup operation before entering next state
  switch(target_state){
    case b_claw_states::managed:
      break;

    case b_claw_states::idle:
      master.rumble("-");
      b_claw_p_1.set_value(LOW);
      b_claw_p_2.set_value(LOW);    
      break;

    case b_claw_states::search_lip:
      b_claw_p_1.set_value(LOW);
      b_claw_p_2.set_value(LOW);
      break;

    case b_claw_states::search_bowl:
      b_claw_p_1.set_value(LOW);
      b_claw_p_2.set_value(LOW);
      search_cycle_check_count = 0; // resets search cycle count
      break;

    case b_claw_states::tilted:
      master.rumble("-");
      b_claw_p_1.set_value(HIGH); // grabs mogo
      b_claw_p_2.set_value(HIGH);
      b_lock_p.set_value(LOW);  // holds goal tilted
      break;

    case b_claw_states::flat:
      b_claw_p_1.set_value(HIGH); // grabs mogo
      b_claw_p_2.set_value(HIGH);
      b_lock_p.set_value(HIGH); // holds goal flat

      break;
  }
  log_state_change();  
}