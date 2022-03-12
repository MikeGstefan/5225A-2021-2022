#pragma once
#include "../Libraries/subsystem.hpp"
#include "../drive.hpp"

#define NUM_OF_B_CLAW_STATES 4

// FRONT CLAW SUBSYSTEM

enum class b_claw_states{
  idle, // claw is open and NOT waiting to detect mogo
  search_lip,  // claw is open and waiting to detect mogo's lip
  search_bowl,  // claw is open and waiting to detect mogo's bowl
  grabbed, // holding mogo
};

class B_Claw: public Subsystem<b_claw_states, NUM_OF_B_CLAW_STATES> {
  Timer release_timer{"release_timer"}; // when the f claw was last released
  int search_cycle_check_count = 0; // for searching for mogo's lip
public:
  B_Claw(Subsystem<b_claw_states, NUM_OF_B_CLAW_STATES> subsystem);  // constructor
  void handle();  // contains state machine code

};

extern B_Claw b_claw;
