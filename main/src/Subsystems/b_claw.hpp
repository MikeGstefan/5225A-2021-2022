#pragma once
#include "../Libraries/subsystem.hpp"
#include "../config.hpp"

#define NUM_OF_B_CLAW_STATES 5

// FRONT CLAW SUBSYSTEM

enum class b_claw_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  idle, // claw is open and NOT waiting to detect mogo, will enter search_lip state after 2 seconds of inactivity
  search_lip,  // claw is open and waiting to detect mogo's lip (FIRST SEARCH STATE)
  search_bowl,  // claw is open and waiting to detect mogo's bowl (SECOND SEARCH STATE)
  grabbed, // holding mogo
};

class B_Claw: public Subsystem<b_claw_states, NUM_OF_B_CLAW_STATES> {
  Timer release_timer{"release_timer"}; // when the f claw was last released
  int search_cycle_check_count = 0; // for searching for mogo's lip
public:
  B_Claw(Subsystem<b_claw_states, NUM_OF_B_CLAW_STATES> subsystem);  // constructor
  void handle();  // contains state machine code
  void handle_state_change(); // cleans up and preps the machine to be in the target state
};

extern B_Claw b_claw;
