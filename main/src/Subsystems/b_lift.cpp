#include "b_lift.hpp"

// #define master partner

// Front Lift object
B_Lift b_lift({{"B_Lift",
{
  "managed",
  "bottom",
  "idle",
  "move_to_target",
  "between_positions",
  "manual",
  "shifting_to_lift_up",
  "shifting_to_lift_down",
},  b_lift_states::managed, b_lift_states::between_positions // goes from managed to between_states upon startup
}, b_lift_m});


B_Lift::B_Lift(Motorized_subsystem<b_lift_states, NUM_OF_B_LIFT_STATES, B_LIFT_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  index = 0;

  up_press.pause();
  down_press.pause();
}

void B_Lift::handle(bool driver_array){
  // decides which position vector to use
  std::vector<int>& positions = driver_array? driver_positions: prog_positions;

  switch(get_state()){
    case b_lift_states::managed:  // being controlled externally
      break;
    case b_lift_states::bottom: // at lowest position, this state is used by the intake and f_claw
      break;
    case b_lift_states::idle: // not doing anything
      break;

    case b_lift_states::move_to_target: // moving to target
      // output is scoped below to prevent other states from accessing it
      {
        int output = pid.compute(b_lift_pot.get_value(), positions[index]);
        if (abs(output) > speed) output = speed * sgn(output); // cap the output at speed
        motor.move(output);
      }
      // moves to next state if the lift has reached its target
      if(fabs(pid.get_error()) < end_error){
        motor.move_velocity(0); // holds motor
        // switches to idle by default or special case depending on current target
        switch(index){
          case 0: // lift is at bottom position, this state is used by intake
            Subsystem::set_state(b_lift_states::bottom);
            break;
          default:
            Subsystem::set_state(b_lift_states::idle);
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
        printf("LIFT SAFETY TRIGGERED %lf, %lf\n", target, b_lift_pot.get_value());

        set_state(b_lift_states::manual);
      }
      */
      break;

    case b_lift_states::between_positions:
      break;

    case b_lift_states::manual:
      lift_power = master.get_analog(ANALOG_RIGHT_X);
      // if master controller joystick isn't active, take partner input instead
      if(fabs(lift_power) < 10) lift_power = partner.get_analog(ANALOG_RIGHT_Y);
      // holds motor if joystick is within deadzone or lift is out of range
      if (fabs(lift_power) < 10 || (lift_power < 0 && b_lift_pot.get_value() <= driver_positions[0]) || (lift_power > 0 && b_lift_pot.get_value() >= driver_positions[driver_positions.size() - 1])) motor.move_velocity(0);
      else motor.move(lift_power);
      // exits manual state if up or down button is pressed or held
      break;

    case b_lift_states::shifting_to_lift_up:
      if(fabs(b_lift_m.get_target_position() - b_lift_m.get_position()) < 15){
        Subsystem::set_state(b_lift_states::shifting_to_lift_down);
      }
      break;

    case b_lift_states::shifting_to_lift_down:
      // switches to desired state after transmission
      if(fabs(b_lift_m.get_target_position() - b_lift_m.get_position()) < 15){
        Subsystem::set_state(after_switch_state);
      }
      break;

  }
  handle_state_change(); // cleans up and preps the machine to be in the target state
}

void B_Lift::handle_state_change(){
  if(get_target_state() == get_state()) return;
  // if state has changed, performs the necessary cleanup operation before entering next state

  // if intake/lift transmission is in intake mode, shift the transmission to the lift
  if(!lift_t.get_state() && get_state() != b_lift_states::shifting_to_lift_up){
    after_switch_state = get_target_state();
    printf("after_switch_state:%s", b_lift.state_names[(int)after_switch_state]);
    Subsystem::set_state(b_lift_states::shifting_to_lift_up);
    intake.set_state(intake_states::off);
  }

  if(get_state() == b_lift_states::bottom){ // if lift is leaving the bottom state, turn off the intake
    // b_lock_p.set_value(HIGH);  // unlock the lift
    intake.set_state(intake_states::off);
  }

  switch(get_target_state()){
    case b_lift_states::managed:
      break;

    case b_lift_states::bottom:
      motor.move(-10); // slight down holding power
      // b_lock_p.set_value(HIGH);  // lock the lift
      break;

    case b_lift_states::idle:
      motor.move_velocity(0); // applies holding power
      break;

    case b_lift_states::move_to_target:
      break;

    case b_lift_states::between_positions:
      break;

    case b_lift_states::manual:
      master.rumble("-");
      // we don't want the b_claw in search mode while the lift is in manual
      if(b_claw_obj.get_state() == b_claw_states::searching){
        b_claw_obj.set_state(b_claw_states::idle);
      }
      break;

    case b_lift_states::shifting_to_lift_up:
      lift_t.set_state(HIGH);
      motor.move_relative(30, 100);
      break;

    case b_lift_states::shifting_to_lift_down:
      motor.move_relative(-30, 100);
      break;

  }
  log_state_change();
}

// accepts an index argument used specifically for a move to target
void B_Lift::set_state(const b_lift_states next_state, const uint8_t index, const int32_t speed){  // requests a state change and logs it
  // confirms state change only if the state is actually move to target
  if (next_state == b_lift_states::move_to_target){
    this->index = index;
    this->speed = speed;
    state_log.print("%s | State change requested index is: %d \t", name, index);
    Subsystem::set_state(next_state);
  }
  else state_log.print("%s | INVALID move to target State change requested from %s to %s, index is: %d\n", name, state_names[static_cast<int>(get_state())], state_names[static_cast<int>(next_state)], index);
}

extern int elastic_f_up_time, elastic_f_down_time; //from gui_construction.cpp

void B_Lift::elastic_util(int high){ //1011 as of April 10th
  master.get_digital_new_press(DIGITAL_A);
  Timer move_timer{"move"};
  while(abs(b_lift_m.get_position() - high) > 15) b_lift_m.move(127);
  move_timer.print();
  move_timer.reset();
  while(abs(b_lift_m.get_position() - 0) > 15) b_lift_m.move(-127);
  move_timer.print();
  b_lift_m.move(0);
}


// BACK CLAW SUBSYSTEM
B_Claw b_claw_obj({"B_Claw",
{
  "managed",
  "idle",
  "about_to_search",
  "searching",
  "tilted",
  "flat"
}, b_claw_states::managed, b_claw_states::idle // goes from managed to idle upon startup
});

B_Claw::B_Claw(Subsystem<b_claw_states, NUM_OF_B_CLAW_STATES> subsystem): Subsystem(subsystem)  // constructor
{}

void B_Claw::handle(){

  switch(get_state()){
    case b_claw_states::managed:
      break;

    case b_claw_states::idle:
      // forces claw into searching if lift is at bottom
      if(b_lift.get_state() == b_lift_states::bottom) set_state(b_claw_states::searching);
      break;

    case b_claw_states::about_to_search:
      // start searching again after 2 seconds
      if(search_timer.get_time() > 2000) set_state(b_claw_states::searching);

      // doesn't let driver search if lift isn't at bottom
      if(b_lift.get_state() != b_lift_states::bottom) set_state(b_claw_states::idle);
      break;

    case b_claw_states::searching:
      // grabs goal if bowl is detected
      if(b_dist.get() < 40) set_state(b_claw_states::tilted);

      // doesn't let driver search if lift isn't at bottom
      if(b_lift.get_state() != b_lift_states::bottom) set_state(b_claw_states::idle);
      break;

    case b_claw_states::tilted:
      break;

    case b_claw_states::flat:
      break;
  }
  handle_state_change();  // cleans up and preps the machine to be in the target state
}

void B_Claw::handle_state_change(){
  if(get_target_state() == get_state()) return;
  // if state has changed, performs the necessary cleanup operation before entering next state
  switch(get_target_state()){
    case b_claw_states::managed:
      break;

    case b_claw_states::idle:
      master.rumble("-");
      tilt_lock.set_state(HIGH);
      b_claw.set_state(LOW);
      break;

    case b_claw_states::about_to_search:
      tilt_lock.set_state(HIGH);
      b_claw.set_state(LOW);
      search_timer.reset(); // to wait 2 seconds before entering search
      break;

    case b_claw_states::searching:
      master.rumble("-");
      b_claw.set_state(LOW);
      break;

    case b_claw_states::tilted:
      master.rumble("-");
      b_claw.set_state(HIGH); // grabs mogo
      // delay(100);
      // tilt_lock.set_state(HIGH);
      break;

    case b_claw_states::flat:
      master.rumble("-");
      tilt_lock.set_state(LOW);
      delay(100);
      b_claw.set_state(LOW);
      break;
  }
  log_state_change();
}
