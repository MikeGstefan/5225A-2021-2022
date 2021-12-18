#pragma once
#include "../Libraries/subsystem.hpp"

#define NUM_OF_HITCH_STATES 2

enum class hitch_states{
  searching,  // released and waiting for goal
  grabbed // has goal
};

class Hitch: public Subsystem<hitch_states, NUM_OF_HITCH_STATES> {

public:
  Hitch(Subsystem<hitch_states, NUM_OF_HITCH_STATES> subsystem);  // constructor
  void handle();  // contains state machine code

};

extern Hitch hitch;
