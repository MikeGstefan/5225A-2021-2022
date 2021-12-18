#pragma once
#include "../Libraries/subsystem.hpp"

#define NUM_OF_LIFT_STATES 7

enum class lift_states{
  // searching,  // released but waiting for goal
  lowered, // released but not waiting for goal
  grabbed, // has goal
  releasing, // waiting for goal to leave trigger
  lifting, // lifting to platform height
  platform, // at platform height
  released, // mogo released at platform height
  lowering  // lowering from released to lowered state
};

class Lift: public Motorized_subsystem<lift_states, NUM_OF_LIFT_STATES> {
  const double bottom_position = 10.0, platform_position = 400.0, top_position = 500.0;
public:
  Lift(Motorized_subsystem<lift_states, NUM_OF_LIFT_STATES> motorized_subsystem);  // constructor
  void handle();  // contains state machine code

};

extern Lift lift;
