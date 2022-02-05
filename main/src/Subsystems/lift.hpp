#pragma once
#include "../Libraries/subsystem.hpp"
// #include "intake.hpp" // because lift controls intake

#define LIFT_STATE_LINE 2 // line on controller which "searching" and "lowered" lift states are printed on

#define NUM_OF_LIFT_STATES 11
#define LIFT_MAX_VELOCITY 100


enum class lift_states{
  idle,  // at bottom_position, waiting for button to search for mogo
  // searching,
  search_lip,  // at bottom_position and waiting to detect mogo's lip or button to grab mogo
  search_bowl, // at bottom_position and waiting to detect mogo's lip or button to grab mogo
  grabbed, // has goal
  tip, // at height to keep mogo away from opponent/ tip mogos
  platform, // at platform height
  tall_platform, // tall goal at platform height
  dropoff, // mogo released at platform height
  lowering, // on the way to grabbed or searching state
  tall_goal,  // filling up rings on tall goal
  manual  // controlled by joystick
};

class Lift: public Motorized_subsystem<lift_states, NUM_OF_LIFT_STATES, LIFT_MAX_VELOCITY> {
  int search_cycle_check_count = 0, bad_count = 0; // cycle check for safeties
  bool held; // if the lift is holding a mogo (used for joystick control)

  int lift_power; // for manual control

  // height conversion constants
  double offset_a = 365.0, offset_h = 9.75;
  double arm_len = 8.0;
  double gear_ratio = 5.0;

public:
  const double bottom_position = 35.0, raised_position = 400.0, tall_dropoff_position = 550, platform_position = 660.0, tall_goal_position = 665.0, top_position = 675.0;

  Lift(Motorized_subsystem<lift_states, NUM_OF_LIFT_STATES, LIFT_MAX_VELOCITY> motorized_subsystem);  // constructor
  void handle();  // contains state machine code
  double pos_to_height(const double position);
  double height_to_pos(const double height);
  void elastic_util(); // up time should be about 1100mms, down time should be slightly slower than that
};

extern Lift lift;
