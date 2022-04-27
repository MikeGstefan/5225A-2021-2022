#pragma once
#include "../Libraries/subsystem.hpp"

//HITCH SUBSYSTEM

enum class hitch_states{
  managed, //being managed externally (NOT DOING ANYTHING)
  idle, //claw is open and NOT waiting to detect mogo
  about_to_search, //claw is open and will search in 2 seconds
  searching,  //claw is open and waiting to detect mogo
  grabbed, //holding mogo
  NUM_OF_ELEMENTS, //number of elements
};

constexpr int num_of_hitch_states = static_cast<int>(hitch_states::NUM_OF_ELEMENTS);

class Hitch: public Subsystem<hitch_states, num_of_hitch_states> {
    Timer search_timer{"Search_timer"};
  public:
    Hitch(Subsystem<hitch_states, num_of_hitch_states> subsystem);  //constructor
    void handle();  //contains state machine code
    void handle_state_change(); //cleans up and preps the machine to be in the target state
};

extern Hitch hitch_obj;