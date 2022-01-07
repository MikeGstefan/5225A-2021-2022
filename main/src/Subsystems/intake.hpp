#pragma once
#include "../Libraries/subsystem.hpp"

#define NUM_OF_INTAKE_STATES 4
#define INTAKE_MAX_VELOCITY 600

#define INTAKE_POWER 100

enum class intake_states {
  off,  // at bottom position, intake is off
  on, // at bottom position, intake is on
  raised,  // at top position, intake is off
  jammed // at bottom position, running slowly in reverse to clear jam
};

class Intake: public Motorized_subsystem<intake_states, NUM_OF_INTAKE_STATES, INTAKE_MAX_VELOCITY> {
  int bad_count = 0; // cycle check for jamming
  int tilter_encoder_position; // position of left tracking wheel when searching
  bool held = false;

public:
  Intake(Motorized_subsystem<intake_states, NUM_OF_INTAKE_STATES, INTAKE_MAX_VELOCITY> motorized_subsystem);  // constructor
  void handle();  // contains state machine code
  void toggle();  // when intake button press is detected, turns intake on if already off and off if already on
  void raise_and_disable(); // used by lift state machine to move intake out of the way
};

extern Intake intake;
