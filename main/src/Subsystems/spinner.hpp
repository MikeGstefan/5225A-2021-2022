#pragma once
#include "../Libraries/subsystem.hpp"
#include "lift.hpp"
#include "tilter.hpp"
#include "intake.hpp"


#define NUM_OF_SPINNER_STATES 8
#define SPINNER_MAX_VELOCITY 100

enum class spinner_states {
  idle,  // not doing anything (obviously)
  prep, // waiting for lift and tilter to reach respectice positions
  aligning_1, // rotating goal for the first time
  release_1,  // releasing first group of rings
  wait_for_arms,  // waits for lift and tilter to leave room for rings
  aligning_2, // rotating goal for the first time
  wait_for_arms_2,  // waits for lift and tilter come back to scoring position
  release_2,  // releasing second group of rings
};

class Spinner: public Motorized_subsystem<spinner_states, NUM_OF_SPINNER_STATES, SPINNER_MAX_VELOCITY> {
  Timer releasing_timer{"spinner_releasing_timer"}; // gives 200 ms for rings to dropoff
  int bad_count = 0; // cycle check for safeties

public:
  Spinner(Motorized_subsystem<spinner_states, NUM_OF_SPINNER_STATES, SPINNER_MAX_VELOCITY> motorized_subsystem);  // constructor
  void handle();  // contains state machine code

};

extern Spinner spinner;
