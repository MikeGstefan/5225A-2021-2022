#include "b_lift.hpp"

// Lift object
B_Lift b_lift({{"Lift",
{
  "idle",
  "manual",
}
}, b_lift_m});

B_Lift::B_Lift(Motorized_subsystem<b_lift_states, NUM_OF_LIFT_STATES, LIFT_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = b_lift_states::idle;
  last_state = state;
}


void B_Lift::handle(){
 


}



