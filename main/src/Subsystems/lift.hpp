#pragma once
#include "../Libraries/subsystem.hpp"

#define NUM_OF_LIFT_STATES 11
#define LIFT_MAX_VELOCITY 100


enum class lift_states{
  searching,  // released but waiting for goal
  grabbed, // has goal
  releasing, // waiting for goal to leave trigger
  lifting, // lifting to target height
  raised, // at height to keep mogo away from opponent
  platform, // at platform height
  level_platform_prep, // going to to top position
  level_platform, // at top position, about to smash platform to level it
  released, // mogo released at target height
  lowering,  // lowering to target height
  manual  // controlled by joystick
};

class Lift: public Motorized_subsystem<lift_states, NUM_OF_LIFT_STATES, LIFT_MAX_VELOCITY> {
  int released_cycle_check = 0, bad_count = 0; // cycle check for safeties


  int lift_power; // for manual control

  // height conversion constants
  double offset_a = 365.0, offset_h = 9.75;
  double arm_len = 8.0;
  double gear_ratio = 5.0;

public:
  const double bottom_position = 30.0, raised_position = 250.0, platform_position = 575.0, top_position = 650.0;

  Lift(Motorized_subsystem<lift_states, NUM_OF_LIFT_STATES, LIFT_MAX_VELOCITY> motorized_subsystem);  // constructor
  void handle();  // contains state machine code
  double pos_to_height(const double position);
  double height_to_pos(const double height);
};

extern Lift lift;
