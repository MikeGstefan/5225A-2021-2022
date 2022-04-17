#include "f_lift.hpp"

// #define master partner

// Front Lift object
F_Lift f_lift({{"F_Lift",
{
  "managed",
  "bottom",
  "top",
  "idle",
  "move_to_target",
  "between_positions",
  "manual",
}, f_lift_states::managed, f_lift_states::idle // goes from managed to idle upon startup
}, f_lift_m});


F_Lift::F_Lift(Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, F_LIFT_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor
  index = 0;
  speed = 127;
}

// void F_Lift::move_absolute(double position, double velocity, bool wait_for_comp, double end_error){ //blocking
//   if (end_error == 0.0) end_error = this->end_error;
//   int output;
//   pid.compute(f_lift_pot.get_value(), position);
//   wait_until(fabs(pid.get_error()) < end_error){
//     output = pid.compute(f_lift_pot.get_value(), position);
//     if (abs(output) > speed) output = speed * sgn(output); // cap the output at speed  
//     motor.move(output);
//   }
//   motor.move(0);
// }

void F_Lift::handle(bool driver_array){
  // decides which position vector to use
  std::vector<int>& positions = driver_array ? driver_positions : prog_positions;

  switch(get_state()){
    case f_lift_states::managed:  // being controlled externally
      break;
    case f_lift_states::bottom: // at lowest position, this state is used by the intake and f_claw
      break;
    case f_lift_states::top: // at highest position
      break;
    case f_lift_states::idle: // not doing anything
      // if(fabs(f_lift_pot.get_value() - driver_positions[index]) > detection_end_error){
      //   state_log.print("F_lift slipped out of position, index: %d, position: %d", index.load(), f_lift_pot.get_value());
      //   set_state(f_lift_states::move_to_target, index);
      // }
      break;

    case f_lift_states::move_to_target: // moving to target
      // printf2("target: %d, pos:%d ", positions[index], f_lift_pot.get_value());

      // move slowly if going down to the bottom with a goal
      // if(index == 0 && f_lift_pot.get_value() < 1500 && f_claw_obj.get_state() == f_claw_states::grabbed)  motor.move(-70);
      { // otherwise calculate output with a pid as usual
        int output = pid.compute(f_lift_pot.get_value(), positions[index]);
        if (abs(output) > speed) output = speed * sgn(output); // cap the output at speed  
        if(abs(output) < 30) output = 30 * sgn(output); // enforces a minimum of 30 power
        motor.move(output);
      }
      // moves to next state if the lift has reached its target
      if(fabs(pid.get_error()) < end_error){
        // switches to idle by default or special case depending on current target
        if(index == 0)  Subsystem::set_state(f_lift_states::bottom);  // lift is at bottom position
        else if(index == driver_positions.size() - 1) Subsystem::set_state(f_lift_states::top); // lift is at top position
        else Subsystem::set_state(f_lift_states::idle);
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
        printf2("LIFT SAFETY TRIGGERED %lf, %lf", target, f_lift_pot.get_value());

        set_state(f_lift_states::manual);
      }
      */
      break;

    case f_lift_states::between_positions:
      break;

    case f_lift_states::manual:
      lift_power = master.get_analog(ANALOG_RIGHT_Y);
      // if master controller joystick isn't active, take partner input instead
      if(fabs(lift_power) < 10) lift_power = partner.get_analog(ANALOG_LEFT_Y);
      // holds motor if joystick is within deadzone or lift is out of range
      if (fabs(lift_power) < 10 || (lift_power < 0 && f_lift_pot.get_value() <= driver_positions[0]) || (lift_power > 0 && f_lift_pot.get_value() >= driver_positions[driver_positions.size() - 1])){
        lift_power = 0;
      } 
      motor.move_velocity(lift_power);
      break;
  }
  handle_state_change(); // cleans up and preps the machine to be in the target state
}

void F_Lift::handle_state_change(){
  if(get_target_state() == get_state()) return;
  // if state has changed, performs the necessary cleanup operation before entering next state

  switch(get_target_state()){
    case f_lift_states::managed:
      break;

    case f_lift_states::bottom:
      motor.move(-10); // slight down holding power
      break;

    case f_lift_states::top:
      motor.move(10); // slight up holding power
      break;

    case f_lift_states::idle:
      motor.move_velocity(0); // applies holding power
      break;

    case f_lift_states::move_to_target:
      break;
    
    case f_lift_states::between_positions:
      break;

    case f_lift_states::manual:
      // we don't want the f_claw in search mode while the lift is in manual
      if(f_claw_obj.get_state() == f_claw_states::searching)  f_claw_obj.set_state(f_claw_states::idle);
      break;
  }
  log_state_change();  
}

// accepts an index argument used specifically for a move to target
void F_Lift::set_state(const f_lift_states next_state, const uint8_t index, const int32_t speed){  // requests a state change and logs it
  // confirms state change only if the state is actually move to target
  if (next_state == f_lift_states::move_to_target){
    this->index = index;
    this->speed = speed;
    state_log.print("%s | State change requested index is: %d \t", name, index);
    Subsystem::set_state(next_state);
  }
  else state_log.print("%s | INVALID move to target State change requested from %s to %s, index is: %d", name, state_names[static_cast<int>(get_state())], state_names[static_cast<int>(next_state)], index);
}

int F_Lift::find_index(){
  for(size_t i = 0; i < driver_positions.size(); i++){
    // if the lift is at a position in the array, return that index
    if(fabs(driver_positions[i] - f_lift_pot.get_value()) < detection_end_error)  return i;
  }
  return -1;
}

void F_Lift::increment_index(){
  state_log.print("f_lift_increment");
  int cur_index = find_index();
  state_log.print("F_LIFT_CUR_INDEX: %d\n", cur_index);
  // if state is between_positions, go to the closest position that's higher than the current position
  if(cur_index == -1){
    // if lift is above topmost position, go to top
    if(f_lift_pot.get_value() > f_lift.driver_positions[f_lift.driver_positions.size() - 1]){
      f_lift.set_state(f_lift_states::move_to_target, f_lift.driver_positions.size() - 1);
    }
    else{
      for (size_t i = 0; i < f_lift.driver_positions.size(); i++){
        if(driver_positions[i] > f_lift_pot.get_value()){
          f_lift.set_state(f_lift_states::move_to_target, i);
          break;
        }
      }
    }
  }
  // otherwise just go to the next highest position, but doesn't increment index if it's out of bounds
  else if(cur_index < f_lift.driver_positions.size() - 1)  f_lift.set_state(f_lift_states::move_to_target, cur_index + 1);
}

void F_Lift::decrement_index(){
  state_log.print("f_lift_decrement");
  int cur_index = find_index();
  state_log.print("F_LIFT_CUR_INDEX: %d\n", cur_index);
  // if state is between_positions, go to the closest position that's lower than the current position
  if(cur_index == -1){
    // if lift is below bottommost position, go to bottom
    if(f_lift_pot.get_value() < f_lift.driver_positions[0]){
      f_lift.set_state(f_lift_states::move_to_target, 0);
    }
    else{
      for (size_t i = f_lift.driver_positions.size() - 1; i >= 0; i--){
        if(f_lift.driver_positions[i] < f_lift_pot.get_value()){
          f_lift.set_state(f_lift_states::move_to_target, i);
          break;
        }
      }
    }
  }
  // otherwise just go to the next lowest position, but doesn't decrement index if it's out of bounds
  else if(cur_index > 0)  f_lift.set_state(f_lift_states::move_to_target, cur_index - 1);
}

int elastic_f_up_time, elastic_f_down_time; //for gui_construction.cpp

void F_Lift::elastic_util(int high){ //935 as of April 10th
  GUI::prompt("Press to start front elastic test", "", 500);
  Timer move_timer{"move"};
  while(abs(f_lift_m.get_position() - high) > 15) f_lift_m.move(127);
  move_timer.print();
  elastic_f_up_time = move_timer.get_time();
  master.print(1, 0, "Up Time: %d", elastic_f_up_time);

  move_timer.reset();
  while(abs(f_lift_m.get_position() - 0) > 15) f_lift_m.move(-127);
  move_timer.print();
  elastic_f_down_time = move_timer.get_time();
  master.print(2, 0, "Down Time: %d", elastic_f_up_time);
  f_lift_m.move(0);
}



// FRONT CLAW SUBSYSTEM
F_Claw f_claw_obj({"F_Claw",
{
  "managed",
  "idle",
  "about_to_search",
  "searching",
  "grabbed",
}, f_claw_states::managed, f_claw_states::searching // goes from managed to searching
});

F_Claw::F_Claw(Subsystem<f_claw_states, NUM_OF_F_CLAW_STATES> subsystem): Subsystem(subsystem)  // constructor
{}


void F_Claw::handle(){
  switch(get_state()){
    case f_claw_states::managed:
      break;

    case f_claw_states::idle:
      // forces claw into searching if lift is at bottom
      if(F_LIFT_AT_BOTTOM) set_state(f_claw_states::searching);
      break;
    
    case f_claw_states::about_to_search:
      // start searching again after 2 seconds
      if(search_timer.get_time() > 2000) set_state(f_claw_states::searching);
      
      // doesn't let driver search if lift isn't at bottom
      if(!F_LIFT_AT_BOTTOM) set_state(f_claw_states::idle);
      break;

    case f_claw_states::searching:
      if(f_dist.get() < 30)  set_state(f_claw_states::grabbed);  // grabs goal if mogo is detected
      
      // doesn't let driver search if lift isn't at bottom
      if(!F_LIFT_AT_BOTTOM) set_state(f_claw_states::idle);
      break;

    case f_claw_states::grabbed:
      break;
  }
  handle_state_change(); // cleans up and preps the machine to be in the target state
}

void F_Claw::handle_state_change(){
  if(get_target_state() == get_state()) return;
  // if state has changed, performs the necessary cleanup operation before entering next state

  switch(get_target_state()){
    case f_claw_states::managed:
      break;

    case f_claw_states::idle:
      f_claw(LOW);
      break;

    case f_claw_states::about_to_search:
      f_claw(LOW);
      search_timer.reset();
      break;

    case f_claw_states::searching:
      f_claw(LOW);
      break;

    case f_claw_states::grabbed:
      f_claw(HIGH);
      // raises mogo above rings automatically if lift is in bottom state
      if(f_lift.get_state() == f_lift_states::bottom){
        f_lift.set_state(f_lift_states::move_to_target, 1); // sends f_lift to raised position
      }
      break;
  }
  log_state_change();  
}


