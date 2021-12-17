#include "hitch.hpp"

Hitch::Hitch(Subsystem<hitch_states, 3> subsystem): Subsystem(subsystem){ // constructor
  state = hitch_states::released;
  last_state = state;
}

void Hitch::handle(){
  switch(state){
    case hitch_states::searching:
      if(hitch_trigger.get_value()){  // grabs mogo
        hitch_piston.set_value(HIGH);
        set_state(hitch_states::grabbed);
      }
      break;
    case hitch_states::released:
      if(master.get_digital_new_press(hitch_button)){ // throws into searching state
        set_state(hitch_states::searching);
      }
      break;
    case hitch_states::grabbed:
      if(master.get_digital_new_press(hitch_button)){ // releases mogo
        hitch_piston.set_value(LOW);
        set_state(hitch_states::released);
      }
      break;
  }
}
