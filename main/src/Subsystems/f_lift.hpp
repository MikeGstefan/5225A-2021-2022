#pragma once
#include "../Libraries/subsystem.hpp"
// #include "intake.hpp" // because lift controls intake

#define F_LIFT_STATE_LINE 2 // line on controller which "searching" and "idle" lift states are printed on

#define NUM_OF_F_LIFT_STATES 6
#define LIFT_MAX_VELOCITY 100

#define NUM_OF_F_CLAW_STATES 3


enum class f_lift_states{
  bottom, // at lowest position
  holding, // holding mogo above the rings
  side_dropoff, // at height to dropoff mogo from the SIDE of the platform
  dropoff,  // at height to dropoff mogo from the FRONT of the platform
  top,  // at highest position
  manual  // controlled by joystick
};

class F_Lift: public Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, LIFT_MAX_VELOCITY> {
  int search_cycle_check_count = 0, bad_count = 0; // cycle check for safeties
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
  const double bottom_position = 45.0, holding_position = 150.0, side_dropoff_position = 475.0, dropoff_position = 630.0, tall_goal_position = 665.0, top_position = 750.0;

  F_Lift(Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, LIFT_MAX_VELOCITY> motorized_subsystem);  // constructor
  void handle();  // contains state machine code
  double pos_to_height(const double position);
  double height_to_pos(const double height);
  void elastic_util(); // up time should be about 1100mms, down time should be slightly slower than that
};

extern F_Lift f_lift;

// FRONT CLAW SUBSYSTEM

enum class f_claw_states{
  idle,  // claw is open and NOT waiting for mogo
  searching,  // claw is open and waiting to detect mogo
  grabbed // holding goal
};

class F_Claw: public Subsystem<f_claw_states, NUM_OF_F_CLAW_STATES> {

public:
  F_Claw(Subsystem<f_claw_states, NUM_OF_F_CLAW_STATES> subsystem);  // constructor
  void handle();  // contains state machine code

};

extern F_Claw f_claw;