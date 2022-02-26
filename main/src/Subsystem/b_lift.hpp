#pragma once
#include "../Libraries/gui.hpp"
#include "../Libraries/subsystem.hpp"

#define LIFT_STATE_LINE 2 // line on controller which "searching" and "lowered" lift states are printed on

#define NUM_OF_LIFT_STATES 2
#define LIFT_MAX_VELOCITY 100


enum class b_lift_states{
  idle,  // at bottom_position, waiting for button to search for mogo
  manual  // controlled by joystick
};

class B_Lift: public Motorized_subsystem<b_lift_states, NUM_OF_LIFT_STATES, LIFT_MAX_VELOCITY> {
  int released_cycle_check = 0, bad_count = 0; // cycle check for safeties
public:
  const double bottom_position = 35.0, raised_position = 250.0, platform_position = 560.0, tall_goal_position = 665.0, top_position = 675.0;

  B_Lift(Motorized_subsystem<b_lift_states, NUM_OF_LIFT_STATES, LIFT_MAX_VELOCITY> motorized_subsystem);  // constructor
  void handle();  // contains state machine code
};

extern B_Lift b_lift;
