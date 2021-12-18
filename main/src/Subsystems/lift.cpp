#include "lift.hpp"

// Lift object
Lift lift(Motorized_subsystem<lift_states, NUM_OF_LIFT_STATES>(Subsystem<lift_states, NUM_OF_LIFT_STATES> ("Lift",
{
  // "searching",
  "lowered",
  "grabbed",
  "releasing"
  "lifting",
  "platform",
  "released",
  "lowering"
}
), lift_motor));


Lift::Lift(Motorized_subsystem<lift_states, NUM_OF_LIFT_STATES> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor
  state = lift_states::lowered;
  last_state = state;
}

void Lift::handle(){
  switch(state){
    case lift_states::lowered:
      if(master.get_digital(lift_up_button)){ // grabs goal if up button is pressed
        lift_piston.set_value(HIGH);
        set_state(lift_states::grabbed);
      }
      if(master.get_digital(lift_down_button)){
        set_state(lift_states::searching);
      }
      break;

    case lift_states::grabbed:
      if(master.get_digital(lift_up_button)){ // lifts goal to platform height if up button is pressed
        move_absolute(platform_position, 100);
        set_state(lift_states::lifting);
      }
      if(master.get_digital(lift_down_button)){ // releases goal if down button is pressed
        lift_piston.set_value(LOW);
        set_state(lift_states::searching);
      }
      break;
    case lift_states::releasing:

      break;
    case lift_states::lifting:
      if(fabs(get_position() - platform_position) < 25){ // waits for lift to reach platform position before setting state to lowered
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
      if(fabs(get_position() - bottom_position) < 25){  // waits for lift to reach bottom before setting state to lowered
        set_state(lift_states::lowered);
      }
      break;

  }
}
