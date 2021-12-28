#pragma once
#include "../Libraries/subsystem.hpp"

#define NUM_OF_TILTER_STATES 5
#define TILTER_MAX_VELOCITY 100

enum class tilter_states {
  lowered,  // at bottom position, waiting for sensor or button to grab goal
  searching,  // waiting to driv a certain distance before grabbing goal
  raised, // has goal and raised
  lowering,  // going from raised to lowered state_type
  manual // controlled by joystick
};

class Tilter: public Motorized_subsystem<tilter_states, NUM_OF_TILTER_STATES, TILTER_MAX_VELOCITY> {
  const double bottom_position = 20.0, raised_position = 200.0, top_position = 300.0;

  int bad_count = 0; // cycle check for safeties
  int tilter_encoder_position; // position of left tracking wheel when searching

public:
  Tilter(Motorized_subsystem<tilter_states, NUM_OF_TILTER_STATES, TILTER_MAX_VELOCITY> motorized_subsystem);  // constructor
  void handle();  // contains state machine code

};

extern Tilter tilter;
