#pragma once
#include "../Libraries/subsystem.hpp"
// #include "intake.hpp" // because lift controls intake

#define B_LIFT_STATE_LINE 2 // line on controller which "searching" and "idle" lift states are printed on

#define NUM_OF_B_LIFT_STATES 5
#define LIFT_MAX_VELOCITY 100

#define NUM_OF_B_CLAW_STATES 4


enum class b_lift_states{
  bottom, // at lowest position
  side_dropoff, // at height to dropoff mogo from the SIDE of the platform
  dropoff,  // at height to dropoff mogo from the FRONT of the platform
  top,  // at highest position
  lowering, // going to the bottom position
  manual  // controlled by joystick
};

class B_Lift: public Motorized_subsystem<b_lift_states, NUM_OF_B_LIFT_STATES, LIFT_MAX_VELOCITY> {
  int bad_count = 0; // cycle check for safeties
  bool held; // if the lift is holding a mogo (used for joystick control)

  int lift_power; // for manual control

  // height conversion constants
  double offset_a = 365.0, offset_h = 9.75;
  double arm_len = 8.0;
  double gear_ratio = 5.0;

  Timer release_timer = Timer("release_timer", false);
  Timer up_button_hold_timer = Timer("up_button_hold_timer", false);
  Timer down_button_hold_timer = Timer("down_button_hold_timer", false);

public:
  const double bottom_position = 35.0, side_dropoff_position = 550, dropoff_position = 660.0, tall_goal_position = 665.0, top_position = 750.0;

  B_Lift(Motorized_subsystem<b_lift_states, NUM_OF_B_LIFT_STATES, LIFT_MAX_VELOCITY> motorized_subsystem);  // constructor
  void handle();  // contains state machine code
  double pos_to_height(const double position);
  double height_to_pos(const double height);
  void elastic_util(); // up time should be about 1100mms, down time should be slightly slower than that
};

extern B_Lift b_lift;

// FRONT CLAW SUBSYSTEM

enum class b_claw_states{
  idle,  // claw is open and NOT waiting for mogo
  searching_lip,  // claw is open and waiting to detect mogo's LIP
  searching_bowl,  // claw is open and waiting to detect mogo's BOWL
  grabbed // holding goal
};

class B_Claw: public Subsystem<b_claw_states, NUM_OF_B_CLAW_STATES> {
  int search_cycle_check_count = 0;
public:
  B_Claw(Subsystem<b_claw_states, NUM_OF_B_CLAW_STATES> subsystem);  // constructor
  void handle();  // contains state machine code

};

extern B_Claw b_claw;