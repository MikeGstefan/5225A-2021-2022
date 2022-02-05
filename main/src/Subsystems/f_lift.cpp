#include "f_lift.hpp"

// Lift object
F_Lift f_lift({{"Lift",
{
  "idle",
  "manual",
}
}, f_lift_m});

F_Lift::F_Lift(Motorized_subsystem<f_lift_states, NUM_OF_LIFT_STATES, LIFT_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = f_lift_states::idle;
  last_state = state;
}


void F_Lift::handle(){
 


}



