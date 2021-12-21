#pragma once
#include "../Libraries/subsystem.hpp"

#define NUM_OF_ANGLER_STATES 5

enum class angler_states {
  lowered,  // at bottom position, waiting for sensor or button to grab goal
  searching,  // waiting to driv a certain distance before grabbing goal
  raised, // has goal and raised
  lowering,  // going from raised to lowered state_type
  manual // controlled by joystick
};

class Angler: public Motorized_subsystem<angler_states, NUM_OF_ANGLER_STATES> {
  const double bottom_position = 20.0, raised_position = 200.0, top_position = 300.0;

  int bad_count = 0; // cycle check for safeties
  int angler_encoder_position; // position of left tracking wheel when searching

public:
  Angler(Motorized_subsystem<angler_states, NUM_OF_ANGLER_STATES> motorized_subsystem);  // constructor
  void handle();  // contains state machine code

};

extern Angler angler;
