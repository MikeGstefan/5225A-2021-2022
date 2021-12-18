#pragma once
#include "../Libraries/subsystem.hpp"

#define NUM_OF_HITCH_STATES 3

enum class hitch_states{
  searching,  // released but waiting for goal
  released, // released but not waiting for goal
  grabbed // has goal
};

class Hitch: public Subsystem<hitch_states, NUM_OF_HITCH_STATES> {

public:
  Hitch(Subsystem<hitch_states, NUM_OF_HITCH_STATES> subsystem);  // constructor
  void handle();  // contains state machine code

};

extern Hitch hitch;
