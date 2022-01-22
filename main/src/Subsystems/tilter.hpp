#pragma once
#include "../Libraries/subsystem.hpp"
#include "../Tracking.hpp"
#include "spinner.hpp"

#define TILTER_STATE_LINE 2 // line on controller which "idle" and "searching" tilter states are printed on

#define NUM_OF_TILTER_STATES 6
#define TILTER_MAX_VELOCITY 100

enum class tilter_states {
  lowered,  // at bottom position, waiting for button to grab goal
  searching, // waiting for ultrasonic to detect goal and grab it
  raised, // has goal and raised
  lowering,  // going from raised to lowered state_type
  tall_goal,  // filling up rings on tall goal
  manual // controlled by joystick
};

class Tilter: public Motorized_subsystem<tilter_states, NUM_OF_TILTER_STATES, TILTER_MAX_VELOCITY> {
  Timer pickup_timer{"auto-pickup"}; // times how long searching procedure takes
  Timer lifting_timer{"tilter_lifting_timer"};  // time how long lifting mogo takes
  int tilter_power;
  int bad_count = 0; // cycle check for safeties
  bool held = false;

public:
  const double bottom_position = 500.0, raised_position = 150.0, top_position = 50.0;
  Tilter(Motorized_subsystem<tilter_states, NUM_OF_TILTER_STATES, TILTER_MAX_VELOCITY> motorized_subsystem);  // constructor
  void handle();  // contains state machine code

};

extern Tilter tilter;
