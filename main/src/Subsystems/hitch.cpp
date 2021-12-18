#include "hitch.hpp"

// hitch object
Hitch hitch({"Hitch",
{
  "searching",
  "grabbed"
}
});

Hitch::Hitch(Subsystem<hitch_states, NUM_OF_HITCH_STATES> subsystem): Subsystem(subsystem)  // constructor
{
  // state setup
  state = hitch_states::searching;
  last_state = state;
}

void Hitch::handle(){
  switch(state){
    case hitch_states::searching:
      // grabs mogo if trigger is hit or if hitch button is pressed
      if(hitch_trigger.get_value() || master.get_digital_new_press(hitch_button)){
        hitch_piston.set_value(HIGH);
        set_state(hitch_states::grabbed);
      }
      break;

    case hitch_states::grabbed:
      if(master.get_digital_new_press(hitch_button)){ // releases mogo if hitch button is pressed
        hitch_piston.set_value(LOW);
        set_state(hitch_states::searching);
      }
      break;

  }
}
