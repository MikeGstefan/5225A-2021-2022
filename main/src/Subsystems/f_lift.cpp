#include "f_lift.hpp"

// #define master partner

// Front Lift object
F_Lift f_lift({{"F_Lift",
{
  "managed",
  "bottom",
  "idle",
  "move_to_target",
  "manual",
}
}, f_lift_m});


F_Lift::F_Lift(Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, F_LIFT_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  // state setup
  target_state = f_lift_states::idle;
  state = f_lift_states::managed;

  index = 0;
  last_index = index;

  up_press.pause();
  down_press.pause();
}

void F_Lift::button_handling(){
  // index incrementing and decrementing
  if(master.get_digital_new_press(lift_up_button) && index < driver_positions.size() - 1){
    up_press.reset();
    set_state(f_lift_states::move_to_target, ++index);
  }
  if(master.get_digital_new_press(lift_down_button) && index > 0){
    down_press.reset();
    set_state(f_lift_states::move_to_target, --index);
  }
  // resets and pauses the timers if driver releases button
  if(!master.get_digital(lift_up_button)) up_press.reset(false);
  if(!master.get_digital(lift_down_button)) down_press.reset(false);

  // goes to top position of top button is held
  if(up_press.get_time() > 300) set_state(f_lift_states::move_to_target, driver_positions.size() - 1);
  // goes to bottom position of top button is held
  if(down_press.get_time() > 300) set_state(f_lift_states::move_to_target, 0);

  // joystick control
  lift_power = master.get_analog(ANALOG_RIGHT_Y);
  if(state != f_lift_states::manual){
    // switches to manual control if lift joystick exceeds threshold
    if(fabs(lift_power) > 80){
      master.rumble("-");
      master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Manual      ");

      set_state(f_lift_states::manual);
    }
  }
}

void F_Lift::handle(bool driver_array){
  // decides which position vector to use
  std::vector<int>& positions = driver_array? driver_positions: prog_positions;

  switch(state){
    case f_lift_states::managed:  // being controlled externally
      break;
    case f_lift_states::bottom: // at lowest position, this state is used by the intake and f_claw
      break;
    case f_lift_states::idle: // not doing anything
      break;

    case f_lift_states::move_to_target: // moving to target
      motor.move(pid.compute(motor.get_position(), positions[index]));
      
      // moves to next state if the lift has reached its target
      if(fabs(pid.get_error()) < end_error){
        motor.move_velocity(0); // holds motor
        // switches to idle by default or special case depending on current target
        switch(index){
          case 0: // lift is at bottom position, this state is used by intake 
            set_state(f_lift_states::bottom);
            break;
          default:
            set_state(f_lift_states::idle);
            break;
        }
      }

      // UNCOMMENT FOR LIFT SAFETY Handling
      // if the motor is drawing more than 2.3 amps for more than 100 ms safety out
      /*
      if (motor.get_current_draw() > 2300 && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
      else bad_count = 0;
      if(bad_count > 10 && state != f_lift_states::manual){
        motor.move(0);
        master.rumble("---");
        master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Manual      ");
        printf("LIFT SAFETY TRIGGERED %lf, %lf\n", target, motor.get_position());

        set_state(f_lift_states::manual);
      }
      */
      break;

    case f_lift_states::manual:

      // holds motor if joystick is within deadzone or lift is out of range
      if (fabs(lift_power) < 10 || (lift_power < 0 && motor.get_position() <= bottom_position) || (lift_power > 0 && motor.get_position() >= top_position)) motor.move_velocity(0);
      else motor.move(lift_power);
      // exits manual state if up or down button is pressed or held
      break;
  }
  handle_state_change(); // cleans up and preps the machine to be in the target state
}

void F_Lift::handle_state_change(){
  if(target_state == state) return;
  // if state has changed, performs the necessary cleanup operation before entering next state

  // NOTE: this switch is commented out because currently no cleanup is needed
  /*
  switch(target_state){
    case f_lift_states::managed:
      break;

    case f_lift_states::bottom:
      break;

    case f_lift_states::idle:
      break;

    case f_lift_states::move_to_target:
      break;

    case f_lift_states::manual:
      break;
  }
  */
  log_state_change();  
}

// regular set state method (common to all subsystems)
void F_Lift::set_state(const f_lift_states next_state){  // requests a state change and logs it (NORMAL set state)
  state_log.print("%s | State change requested from %s to %s, index is: %d\n", name, state_names[static_cast<int>(state)], state_names[static_cast<int>(next_state)], index);
  target_state = next_state;
}
// accepts an index argument
void F_Lift::set_state(const f_lift_states next_state, const double index){  // requests a state change and logs it
  state_log.print("%s | State change requested from %s to %s, index is: %d\n", name, state_names[static_cast<int>(state)], state_names[static_cast<int>(next_state)], index);
  target_state = next_state;
  // updates index only if it's valid (the state is actually move to target)
  if (target_state == f_lift_states::move_to_target)  this->index = index;
}

extern int elastic_f_up_time, elastic_f_down_time; //from gui_construction.cpp

void F_Lift::elastic_util(){
  reset();
  motor.move(-10);
  GUI::go("Start Elastic Utility", "Press to start the elastic utility.", 500);
  f_claw_p.set_value(HIGH);
  Timer move_timer{"move"};
  move_absolute(top_position);
  // // intake_piston.set_value(HIGH);  // raises intake
  waitUntil(fabs(motor.get_position() - top_position) < end_error);
  move_timer.print();
  elastic_f_up_time = move_timer.get_time();
  master.print(1, 0, "up time: %d", elastic_f_up_time);

  move_timer.reset();
  move_absolute(bottom_position);
  waitUntil(fabs(motor.get_position() - bottom_position) < end_error);
  move_timer.print();
  elastic_f_down_time = move_timer.get_time();
  master.print(2, 0, "down time: %d", elastic_f_up_time);
}



// FRONT CLAW SUBSYSTEM
F_Claw f_claw({"F_Claw",
{
  "managed",
  "idle",
  "searching",
  "grabbed",
}
});

F_Claw::F_Claw(Subsystem<f_claw_states, NUM_OF_F_CLAW_STATES> subsystem): Subsystem(subsystem)  // constructor
{
  // state setup
  target_state = f_claw_states::searching;
  state = f_claw_states::managed;
}

void F_Claw::handle(){
  switch(state){
    case f_claw_states::managed:
      break;

    case f_claw_states::idle:
      // enters search mode if the lift is at the bottom and it's been 2 seconds since the mogo was released
      if(f_lift.get_state() == f_lift_states::bottom && release_timer.get_time() > 2000){
        set_state(f_claw_states::searching);
      }
      // grabs goal if toggle button is pressed
      if(master.get_digital_new_press(lift_claw_toggle_button)){
        master.rumble("-");
        set_state(f_claw_states::grabbed);
      }
      break;

    case f_claw_states::searching:
      // grabs goal if toggle button is pressed or mogo is detected
      if(master.get_digital_new_press(lift_claw_toggle_button) || f_touch.get_value()){
        master.rumble("-");
        set_state(f_claw_states::grabbed);
      }
      break;

    case f_claw_states::grabbed:
      // releases goal if down button is pressed
      if(master.get_digital_new_press(lift_claw_toggle_button)) set_state(f_claw_states::idle);
      break;
  }
  handle_state_change(); // cleans up and preps the machine to be in the target state
}

void F_Claw::handle_state_change(){
  if(target_state == state) return;
  // if state has changed, performs the necessary cleanup operation before entering next state

  switch(target_state){
    case f_claw_states::managed:
      break;

    case f_claw_states::idle:
      release_timer.reset(); // timer to wait 2 seconds before entering search again
      f_claw_p.set_value(LOW);
      break;

    case f_claw_states::searching:
      f_claw_p.set_value(LOW);
      break;

    case f_claw_states::grabbed:
      // raises mogo above rings automatically if lift is in bottom state
      if(f_lift.get_state() == f_lift_states::bottom){
        f_lift.set_state(f_lift_states::move_to_target, 1); // sends f_lift to raised position
      }
      f_claw_p.set_value(HIGH);
      break;
  }
  log_state_change();  
}