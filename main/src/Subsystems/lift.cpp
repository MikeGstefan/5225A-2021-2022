#include "lift.hpp"

// Lift object
Lift lift({{"Lift",
{
  "searching",
  "grabbed",
  "releasing"
  "lifting",
  "platform",
  "released",
  "lowering"
}
}, lift_motor});


Lift::Lift(Motorized_subsystem<lift_states, NUM_OF_LIFT_STATES> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = lift_states::searching;
  last_state = state;
  released_cycle_check = 0;
}

void Lift::handle(){
  switch(state){
    case lift_states::searching:
      // grabs goal if limit switch is triggered or if up button is pressed
      if(lift_trigger.get_value() || master.get_digital(lift_up_button)){
        lift_piston.set_value(HIGH);
        set_state(lift_states::grabbed);
      }
      break;

    case lift_states::grabbed:
      if(master.get_digital(lift_up_button)){ // lifts goal to platform height if up button is pressed
        move_absolute(platform_position, 100);
        set_state(lift_states::lifting);
      }
      if(master.get_digital(lift_down_button)){ // releases goal if down button is pressed
        lift_piston.set_value(LOW);
        set_state(lift_states::releasing);
      }
      break;

    case lift_states::releasing:
      // waits for goal to leave trigger to enter searching state (checks for 5 cycles)
      if(!lift_trigger.get_value()) released_cycle_check++;
      else released_cycle_check = 0;
      if(released_cycle_check >= 5)  set_state(lift_states::searching);
      break;

    case lift_states::lifting:
      if(master.get_digital(lift_down_button)){ // lowers goal if down button is pressed
        move_absolute(platform_position, 100);
        set_state(lift_states::lowering);
      }
      if(fabs(get_position() - platform_position) < 25){ // waits for lift to reach platform position before setting state to platform
        set_state(lift_states::platform);
      }
      break;

    case lift_states::platform:
      if(master.get_digital(lift_up_button)){ // releases goal if up button is pressed
        lift_piston.set_value(LOW);
        set_state(lift_states::released);
      }
      if(master.get_digital(lift_down_button)){ // lowers goal if down button is pressed
        move_absolute(platform_position, 100);
        set_state(lift_states::lowering);
      }
      break;

    case lift_states::released:
      if(master.get_digital(lift_up_button)){ // releases goal if up button is pressed
        move_absolute(platform_position, 100);
        set_state(lift_states::lowering);
      }
      if(master.get_digital(lift_down_button)){ // lowers goal if down button is pressed
        move_absolute(platform_position, 100);
        set_state(lift_states::lowering);
      }
      break;

    case lift_states::lowering:
      if(master.get_digital(lift_up_button)){ // lifts goal to platform height if up button is pressed
        move_absolute(platform_position, 100);
        set_state(lift_states::lifting);
      }
      if(fabs(get_position() - bottom_position) < 25){  // waits for lift to reach bottom before setting state to searching
        if(lift_trigger.get_value())  set_state(lift_states::grabbed);
        else set_state(lift_states::releasing);
      }
      break;

  }
}
