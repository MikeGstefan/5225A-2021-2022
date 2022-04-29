#include "b_lift.hpp"

// #define master partner

// Back Lift object
B_Lift b_lift({{"B_Lift",
{
  "managed",
  "bottom",
  "top",
  "idle",
  "move_to_target",
  "park pos",
  "manual",
  "intake_off",
  "intake_on",
  "intk_jam",
  "intake_reversed",
  "shifting_to_lift_up",
  "shifting_to_lift_down",
  "reshift"
},  b_lift_states::managed, b_lift_states::idle // goes from managed to idle upon startup
}, b_lift_m});


B_Lift::B_Lift(Motorized_subsystem<b_lift_states, NUM_OF_B_LIFT_STATES, B_LIFT_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor
  index = 0;
  speed = 127;
}

void B_Lift::handle(bool driver_array){
  // decides which position vector to use
  std::vector<int>& positions = driver_array? driver_positions: prog_positions;

  switch(get_state()){
    case b_lift_states::managed:  // being controlled externally
      break;
    case b_lift_states::bottom: // at lowest position, this state is used by the intake and f_claw
      break;
    case b_lift_states::top: // at highest position
      break;
    case b_lift_states::idle: // not doing anything
      // if(fabs(b_lift_pot.get_value() - positions[index]) > detection_end_error){
      //   state_log.print("B_lift slipped out of position, index: %d, position: %d", index.load(), b_lift_pot.get_value());
      //   set_state(b_lift_states::move_to_target, index);
      // }
      break;

    case b_lift_states::move_to_target: // moving to target
      // output is scoped below to prevent other states from accessing it
      { // don't power the motor if the state is no longer move to target
        int output = pid.compute(b_lift_pot.get_value(), positions[index]);
        if (abs(output) > speed) output = speed * sgn(output); // cap the output at speed
        if(abs(output) < 50) output = 50 * sgn(output); // enforces a minimum of 30 power
        motor.move(output);
      }
      // moves to next state if the lift has reached its target
      if(fabs(pid.get_error()) < end_error){
        // switches to idle by default or special case depending on current target
        if(index == 0)  Subsystem::set_state(b_lift_states::bottom);  // lift is at bottom position
        else if(index == positions.size() - 1) Subsystem::set_state(b_lift_states::top); // lift is at top position
        else Subsystem::set_state(b_lift_states::idle);
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
        printf2("LIFT SAFETY TRIGGERED %lf, %lf", target, b_lift_pot.get_value());

        set_state(b_lift_states::manual);
      }
      */
      break;

    case b_lift_states::park_position:
      { // don't power the motor if the state is no longer move to target
        int output = pid.compute(b_lift_pot.get_value(), park_position);
        if (abs(output) > speed) output = speed * sgn(output); // cap the output at speed
        if(abs(output) < 50) output = 50 * sgn(output); // enforces a minimum of 30 power
        motor.move(output);
      }
      // moves to next state if the lift has reached its target
      if(fabs(pid.get_error()) < end_error){
        Subsystem::set_state(b_lift_states::idle);
      }
    break;

    case b_lift_states::manual:
      lift_power = master.get_analog(ANALOG_RIGHT_X);
      // if master controller joystick isn't active, take partner input instead
      if(fabs(lift_power) < 10) lift_power = partner.get_analog(ANALOG_RIGHT_Y);
      // holds motor if joystick is within deadzone or lift is out of range
      if (fabs(lift_power) < 10 || (lift_power < 0 && b_lift_pot.get_value() <= positions[0]) || (lift_power > 0 && b_lift_pot.get_value() >= positions[positions.size() - 1])){
        lift_power = 0;
      }
      // if back lift is below a threshold and the hitch is open, close it to not break it
      if(b_lift_pot.get_value() < 1600 && lift_power < 0){
        if(hitch_obj.get_state() != hitch_states::grabbed){
          hitch_obj.set_state(hitch_states::grabbed);
        }
        // if the goal is detected, don't power the lift
        if(hitch_dist.get() < 25) lift_power = 0;
      }

      motor.move_velocity(lift_power);
      break;

    case b_lift_states::intake_off:
      break;

    case b_lift_states::intake_on:
      // printf("vel:%lf\n", motor.get_actual_velocity());
      if(fabs(motor.get_actual_velocity()) < 5.0) not_moving_count++;
      else not_moving_count = 0;
      if(intake_safe.get_time() < 300 && not_moving_count > 20){
        not_moving_count = 0;
        after_shift_state = b_lift_states::reshift;
        shift(TRANS_LIFT_STATE); // shifts to lift
        intake_safe.reset(false);
      }
      // printf("current:%lf\n", motor.get_actual_velocity());
      else{
        if(!intk_t.get_value())intk_jam_count++;
        else intk_jam_count = 0;
        if(intk_jam_count == 4){
          intake_safe.reset(false);
          Subsystem::set_state(b_lift_states::intk_jam);
          
        }
      }
      printf("sensor: %d\n", intk_t.get_value());
      break;

    case b_lift_states::intk_jam:
      if(millis() - jam_time > 500)b_lift.Subsystem::set_state(b_lift_states::intake_on);
    break;

    case b_lift_states::intake_reversed:
      break;

    case b_lift_states::shifting_up:
      if(fabs(motor.get_target_position() - motor.get_position()) < 15){
        Subsystem::set_state(b_lift_states::shifting_down);
      }
      break;

    case b_lift_states::shifting_down:
      // switches to desired state after transmission
      if(fabs(motor.get_target_position() - motor.get_position()) < 15){
        Subsystem::set_state(after_shift_state);
      }
      break;

    case b_lift_states::reshift:  // shifts to intake 
      break;

  }
  handle_state_change(); // cleans up and preps the machine to be in the target state
}

void B_Lift::handle_state_change(){
  if(get_target_state() == get_state()) return;
  // if(get_target_state() == b_lift_states::intake_on) return; // doesn't let you turn the intake on
  // if state has changed, performs the necessary cleanup operation before entering next state

  // if intake/lift transmission is in the wrong state, shift
  handle_shift();

  switch(get_target_state()){
    case b_lift_states::managed:
      break;

    case b_lift_states::bottom:
      motor.move(-20); // slight down holding power
      break;

    case b_lift_states::top:
      motor.move(10); // slight up holding power
      break;

    case b_lift_states::idle:
      motor.move_velocity(0); // applies holding power
      break;

    case b_lift_states::move_to_target:
      break;

    case b_lift_states::manual:
      // we don't want the b_claw in search mode while the lift is in manual
      if(b_claw_obj.get_state() == b_claw_states::searching){
        b_claw_obj.set_state(b_claw_states::idle);
      }
      break;

    case b_lift_states::intake_off:
      motor.move(0);
      break;

    case b_lift_states::intake_on:
      intake_safe.reset();
      motor.move(-127);
      break;
    case b_lift_states::intk_jam:
      jam_time= millis();
      motor.move(127);
      printf("AHHHHHHHHHHHHHH here \n\n\n\n");
      break;
    case b_lift_states::intake_reversed:
      motor.move(127);
      break;

    case b_lift_states::shifting_up:
      motor.move_relative(30, 100);
      break;

    case b_lift_states::shifting_down:
      motor.move_relative(-30, 100);
      break;

    case b_lift_states::reshift:
      after_shift_state = b_lift_states::intake_on;
      shift(TRANS_LIFT_STATE); // shifts to lift
      break;
    default:
    break;

  }
  log_state_change();
}

// accepts an index argument used specifically for a move to target
void B_Lift::set_state(const b_lift_states next_state, const uint8_t index, const int32_t speed){  // requests a state change and logs it
  // confirms state change only if the state is actually move to target
  if (next_state == b_lift_states::move_to_target){
    // if back lift is below a threshold and the hitch is open, close it to not break it
    if(index == 0){
      if(hitch_obj.get_state() != hitch_states::grabbed){
        hitch_obj.set_state(hitch_states::grabbed);
      }
      // if the goal is detected, don't power the lift
      // if(hitch_dist.get() > 30){
      this->index = index;
      this->speed = speed;
      state_log.print("%s | State change requested index is: %d \t", name, index);
      Subsystem::set_state(next_state);
      // }
    }
    else{
      this->index = index;
      this->speed = speed;
      state_log.print("%s | State change requested index is: %d \t", name, index);
      Subsystem::set_state(next_state);
    }
  }
  else state_log.print("%s | INVALID move to target State change requested from %s to %s, index is: %d", name, state_names[static_cast<int>(get_state())], state_names[static_cast<int>(next_state)], index);
}

int B_Lift::find_index(){
  for(size_t i = 0; i < driver_positions.size(); i++){
    // if the lift is at a position in the array, return that index
    if(fabs(driver_positions[i] - b_lift_pot.get_value()) < detection_end_error)  return i;
  }
  return -1;
}

void B_Lift::handle_shift(){
  // if the transmission is in intake mode and the target state is a lift state, shift
  if(lift_t.get_state() == TRANS_INTAKE_STATE){
    switch(get_target_state()){
      case b_lift_states::bottom:
        shift(TRANS_LIFT_STATE);
        break;
      case b_lift_states::top:
        shift(TRANS_LIFT_STATE);
        break;
      case b_lift_states::idle:
        shift(TRANS_LIFT_STATE);
        break;
      case b_lift_states::move_to_target:
        shift(TRANS_LIFT_STATE);
        break;        
      case b_lift_states::manual:
        shift(TRANS_LIFT_STATE);
        break;
    }
  }
  // if the transmission is in lift mode and the target state is an intake state, shift
  else{
    switch(get_target_state()){
      case b_lift_states::intake_off:
        shift(TRANS_INTAKE_STATE);
        break;
      case b_lift_states::intake_on:
        shift(TRANS_INTAKE_STATE);
        break;
      case b_lift_states::intake_reversed:
        shift(TRANS_INTAKE_STATE);
        break;
    }
  }
}

void B_Lift::shift(bool piston_state){
  after_shift_state = get_target_state();
  state_log.print("after_switch_state:%s", b_lift.state_names[(int)after_shift_state]);
  lift_t.set_state(piston_state);
  Subsystem::set_state(b_lift_states::shifting_up);
}

void B_Lift::increment_index(){
  state_log.print("b_lift_increment");
  int cur_index = find_index();
  state_log.print("B_LIFT_CUR_INDEX: %d\n", cur_index);
  // if state is between_positions, go to the closest position that's higher than the current position
  if(cur_index == -1){
    // if lift is above topmost position, go to top
    if(b_lift_pot.get_value() > b_lift.driver_positions[b_lift.driver_positions.size() - 1]){
      b_lift.set_state(b_lift_states::move_to_target, b_lift.driver_positions.size() - 1);
    }
    else{
      for (size_t i = 0; i < b_lift.driver_positions.size(); i++){
        if(driver_positions[i] > b_lift_pot.get_value()){
          b_lift.set_state(b_lift_states::move_to_target, i);
          break;
        }
      }
    }
  }
  // otherwise just go to the next highest position, but doesn't increment index if it's out of bounds
  else if(cur_index < b_lift.driver_positions.size() - 1)  b_lift.set_state(b_lift_states::move_to_target, cur_index + 1);
}

void B_Lift::decrement_index(){
  state_log.print("b_lift_decrement");
  int cur_index = find_index();
  printf("B_LIFT_CUR_INDEX: %d\n", cur_index);
  // if state is between_positions, go to the closest position that's lower than the current position
  if(cur_index == -1){
    // if lift is below bottommost position, go to bottom
    if(b_lift_pot.get_value() < b_lift.driver_positions[0]){
      b_lift.set_state(b_lift_states::move_to_target, 0);
    }
    else{
      for (size_t i = b_lift.driver_positions.size() - 1; i >= 0; i--){
        if(b_lift.driver_positions[i] < b_lift_pot.get_value()){
          b_lift.set_state(b_lift_states::move_to_target, i);
          break;
        }
      }
    }
  }
  // otherwise just go to the next lowest position, but doesn't decrement index if it's out of bounds
  else if(cur_index > 0)  b_lift.set_state(b_lift_states::move_to_target, cur_index - 1);
}

int elastic_b_up_time, elastic_b_down_time; //for gui_construction.cpp

void B_Lift::elastic_util(int high){ //1011 as of April 10th
  GUI::prompt("Press to start back elastic test", "", 500);
  Timer move_timer{"move"};
  while(abs(b_lift_m.get_position() - high) > 15) b_lift_m.move(127);
  move_timer.print();
  elastic_b_up_time = move_timer.get_time();
  master.print(1, 0, "up time: %d", elastic_b_up_time);

  move_timer.reset();
  while(abs(b_lift_m.get_position() - 0) > 15) b_lift_m.move(-127);
  move_timer.print();
  elastic_b_down_time = move_timer.get_time();
  master.print(2, 0, "down time: %d", elastic_b_up_time);
  b_lift_m.move(0);
}

void B_Lift::move_to_top(){
  set_state(b_lift_states::move_to_target, prog_positions.size()-1);
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
{
  state_at_toggle_press = b_claw_states::idle; // don't change this
}

void B_Claw::handle(){

  switch(get_state()){
    case b_claw_states::managed:
      break;

    case b_claw_states::idle:
      // forces claw into searching if lift is at bottom
      if(B_LIFT_AT_BOTTOM && b_lift.get_state() != b_lift_states::manual) set_state(b_claw_states::searching);
      break;

    case b_claw_states::about_to_search:
      // start searching again after 2 seconds
      if(search_timer.get_time() > 2000) set_state(b_claw_states::searching);

      // doesn't let driver search if lift isn't at bottom
      if(!B_LIFT_AT_BOTTOM) set_state(b_claw_states::idle);
      break;

    case b_claw_states::searching:
      // grabs goal if bowl is detected
      if(b_dist.get() < 45) set_state(b_claw_states::tilted);

      // doesn't let driver search if lift isn't at bottom
      if(!B_LIFT_AT_BOTTOM) set_state(b_claw_states::idle);
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
      tilt_lock.set_state(HIGH);
      b_claw.set_state(LOW);
      break;

    case b_claw_states::about_to_search:
      tilt_lock.set_state(HIGH);
      b_claw.set_state(LOW);
      search_timer.reset(); // to wait 2 seconds before entering search
      break;

    case b_claw_states::searching:
      b_claw.set_state(LOW);
      break;

    case b_claw_states::tilted:
      b_claw.set_state(HIGH); // grabs mogo
      break;

    case b_claw_states::flat:
      tilt_lock.set_state(LOW);
      delay(50);
      b_claw.set_state(LOW);
      break;
  }
  log_state_change();
}
