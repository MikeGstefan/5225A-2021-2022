#pragma once
#include "../Libraries/subsystem.hpp"

#define NUM_OF_LIFT_STATES 8

enum class lift_states{
  searching,  // released but waiting for goal
  grabbed, // has goal
  releasing, // waiting for goal to leave trigger
  lifting, // lifting to platform height
  platform, // at platform height
  released, // mogo released at platform height
  lowering,  // lowering from released to lowered state
  manual  // controlled by joystick
};

class Lift: public Motorized_subsystem<lift_states, NUM_OF_LIFT_STATES> {
  int released_cycle_check = 0;

  int bad_count = 0; // cycle check for safeties

  // height conversion concstants
  double offset_a = 365.0, offset_h = 9.75;
  double arm_len = 8.0;
  double gear_ratio = 5.0;

public:
  const double bottom_position = 30.0, platform_position = 600.0, top_position = 650.0;
  
  Lift(Motorized_subsystem<lift_states, NUM_OF_LIFT_STATES> motorized_subsystem);  // constructor
  void handle();  // contains state machine code
  double pos_to_height(const double position);
  double height_to_pos(const double height);

};

extern Lift lift;
