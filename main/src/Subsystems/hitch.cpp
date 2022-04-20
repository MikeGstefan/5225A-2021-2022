#include "hitch.hpp"

// Hitch SUBSYSTEM
Hitch hitch_obj({"Hitch",
{
  "managed",
  "idle",
  "about_to_search",
  "searching",
  "grabbed",
}, hitch_states::managed, hitch_states::grabbed // goes from managed to grabbed upon startup
});

Hitch::Hitch(Subsystem<hitch_states, NUM_OF_HITCH_STATES> subsystem): Subsystem(subsystem)  // constructor
{}


void Hitch::handle(){
  switch(get_state()){
    case hitch_states::managed:
      break;

    case hitch_states::idle:
      break;
    
    case hitch_states::about_to_search:
      // start searching again after 2 seconds
      if(search_timer.get_time() > 2000) set_state(hitch_states::searching);
      
      break;

    case hitch_states::searching:
      if(hitch_dist.get() < 25)  set_state(hitch_states::grabbed);  // grabs goal if mogo is detected
      break;

    case hitch_states::grabbed:
      break;
  }
  handle_state_change(); // cleans up and preps the machine to be in the target state
}

void Hitch::handle_state_change(){
  if(get_target_state() == get_state()) return;
  // if state has changed, performs the necessary cleanup operation before entering next state

  switch(get_target_state()){
    case hitch_states::managed:
      break;

    case hitch_states::idle:
      hitch.set_state(LOW);
      break;

    case hitch_states::about_to_search:
      hitch.set_state(LOW);
      search_timer.reset();
      break;

    case hitch_states::searching:
      hitch.set_state(LOW);
      break;

    case hitch_states::grabbed:
      hitch.set_state(HIGH);
      break;
  }
  log_state_change();  
}
