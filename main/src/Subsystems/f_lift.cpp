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
}, f_lift_states::managed, f_lift_states::idle // goes from managed to idle upon startup
}, f_lift_m});


F_Lift::F_Lift(Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, F_LIFT_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  index = 0;

  up_press.pause();
  down_press.pause();
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
      // printf("target: %d, pos:%d \n", positions[index], f_lift_pot.get_value());

      // move slowly if going down to the bottom with a goal
      if(index == 0 && f_lift_pot.get_value() < 1500 && f_claw_obj.get_state() == f_claw_states::grabbed)  motor.move(-50);
      else motor.move(pid.compute(f_lift_pot.get_value(), positions[index]));
      
      // moves to next state if the lift has reached its target
      if(fabs(pid.get_error()) < end_error){
        motor.move_velocity(0); // holds motor
        // switches to idle by default or special case depending on current target
        switch(index){
          case 0: // lift is at bottom position, this state is used by intake 
            Subsystem::set_state(f_lift_states::bottom);
            break;
          default:
            Subsystem::set_state(f_lift_states::idle);
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
        printf("LIFT SAFETY TRIGGERED %lf, %lf\n", target, f_lift_pot.get_value());

        set_state(f_lift_states::manual);
      }
      */
      break;

    case f_lift_states::manual:
      lift_power = master.get_analog(ANALOG_RIGHT_Y);
      // holds motor if joystick is within deadzone or lift is out of range
      if (fabs(lift_power) < 10 || (lift_power < 0 && f_lift_pot.get_value() <= driver_positions[0]) || (lift_power > 0 && f_lift_pot.get_value() >= driver_positions[driver_positions.size() - 1])){
        motor.move_velocity(0);
      } 
      else motor.move(lift_power);
      // exits manual state if up or down button is pressed or held
      break;
  }
  handle_state_change(); // cleans up and preps the machine to be in the target state
}

void F_Lift::handle_state_change(){
  if(get_target_state() == state) return;
  // if state has changed, performs the necessary cleanup operation before entering next state

  switch(get_target_state()){
    case f_lift_states::managed:
      break;

    case f_lift_states::bottom:
      motor.move(-10); // slight down holding power
      break;

    case f_lift_states::idle:
      motor.move_velocity(0); // applies holding power
      break;

    case f_lift_states::move_to_target:
      break;

    case f_lift_states::manual:
      master.rumble("-");
      // we don't want the f_claw in search mode while the lift is in manual
      if(f_claw_obj.get_state() == f_claw_states::searching)  f_claw_obj.set_state(f_claw_states::idle);
      break;
  }
  log_state_change();  
}

// accepts an index argument used specifically for a move to target
void F_Lift::set_state(const f_lift_states next_state, const uint8_t index){  // requests a state change and logs it
  // confirms state change only if the state is actually move to target
  if (next_state == f_lift_states::move_to_target){
    this->index = index;
    state_log.print("%s | State change requested index is: %d \t", name, index);
    Subsystem::set_state(next_state);
  }
  else state_log.print("%s | INVALID move to target State change requested from %s to %s, index is: %d\n", name, state_names[static_cast<int>(state)], state_names[static_cast<int>(next_state)], index);
}

int elastic_f_up_time, elastic_f_down_time; //from gui_construction.cpp

void F_Lift::elastic_util(){
  motor.move(-10);
  // GUI::go("Start Elastic Utility", "Press to start the elastic utility.", 500);
  master.get_digital_new_press(DIGITAL_A);
  Timer move_timer{"move"};
  set_state(f_lift_states::move_to_target, driver_positions.size() - 1); // moves to top
  // // intake_piston.set_value(HIGH);  // raises intake
  wait_until(state == f_lift_states::idle);
  move_timer.print();
  elastic_f_up_time = move_timer.get_time();
  master.print(1, 0, "up time: %d", elastic_f_up_time);

  move_timer.reset();
  set_state(f_lift_states::move_to_target, 0); // moves to bottom
  wait_until(state == f_lift_states::bottom);
  move_timer.print();
  elastic_f_down_time = move_timer.get_time();
  master.print(2, 0, "down time: %d", elastic_f_up_time);
}



// FRONT CLAW SUBSYSTEM
F_Claw f_claw_obj({"F_Claw",
{
  "managed",
  "idle",
  "searching",
  "grabbed",
}, f_claw_states::managed, f_claw_states::idle // goes from managed to idle upon startup
});

F_Claw::F_Claw(Subsystem<f_claw_states, NUM_OF_F_CLAW_STATES> subsystem): Subsystem(subsystem)  // constructor
{}

void F_Claw::handle_buttons(){
  if(master.get_digital_new_press(claw_toggle_button) && get_lift()){
    switch(state){
      case f_claw_states::idle:
        set_state(f_claw_states::grabbed);
        break;
      case f_claw_states::grabbed:
        set_state(f_claw_states::idle);
        break;
    }
  }
}

void F_Claw::handle(){
  switch(state){
    case f_claw_states::managed:
      break;

    case f_claw_states::idle:
      break;

    case f_claw_states::searching:
      if(f_dist.get() < 30)  set_state(f_claw_states::grabbed);  // grabs goal if mogo is detected
      break;

    case f_claw_states::grabbed:
      break;
  }
  handle_state_change(); // cleans up and preps the machine to be in the target state
}

void F_Claw::handle_state_change(){
  if(get_target_state() == state) return;
  // if state has changed, performs the necessary cleanup operation before entering next state

  switch(get_target_state()){
    case f_claw_states::managed:
      break;

    case f_claw_states::idle:
      master.rumble("-");
      f_claw(LOW);
      break;

    case f_claw_states::searching:
      f_claw(LOW);
      break;

    case f_claw_states::grabbed:
      master.rumble("-");
      f_claw(HIGH);
      // raises mogo above rings automatically if lift is in bottom state
      if(f_lift.get_state() == f_lift_states::bottom){
        f_lift.set_state(f_lift_states::move_to_target, 1); // sends f_lift to raised position
      }
      break;
  }
  log_state_change();  
}


