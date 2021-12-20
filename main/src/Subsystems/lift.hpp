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
  const double bottom_position = 10.0, platform_position = 400.0, top_position = 500.0;
  int released_cycle_check = 0;
public:
  Lift(Motorized_subsystem<lift_states, NUM_OF_LIFT_STATES> motorized_subsystem);  // constructor
  void handle();  // contains state machine code

};

extern Lift lift;

// non-class stuff
extern lift_states lift_state;
extern lift_states last_lift_state;
extern int lift_released_cycle_check;

extern const int lift_bottom_position;
extern const int lift_platform_position;
extern const int lift_top_position;
extern int lift_bad_count;

extern std::array<const char*, NUM_OF_LIFT_STATES> lift_state_names;

void set_lift_state();
void lift_handle();
void lift_reset();
