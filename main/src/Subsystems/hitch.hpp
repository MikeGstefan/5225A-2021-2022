#pragma once
#include "../Libraries/subsystem.hpp"

#define NUM_OF_HITCH_STATES 5

// HITCH SUBSYSTEM

enum class hitch_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  idle, // claw is open and NOT waiting to detect mogo
  about_to_search, // claw is open and will search in 2 seconds
  searching,  // claw is open and waiting to detect mogo
  grabbed, // holding mogo
};

class Hitch: public Subsystem<hitch_states, NUM_OF_HITCH_STATES> {
    Timer search_timer{"Search_timer"};
  public:
    Hitch(Subsystem<hitch_states, NUM_OF_HITCH_STATES> subsystem);  // constructor
    void handle();  // contains state machine code
    void handle_state_change(); // cleans up and preps the machine to be in the target state
};

extern Hitch hitch_obj;