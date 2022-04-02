#pragma once
#include "../Libraries/subsystem.hpp"
#include "../drive.hpp"

#define NUM_OF_INTAKE_STATES 5
#define INTAKE_MAX_VELOCITY 600

enum class intake_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  off, // not spinning
  on, // spinning forward
  reversed,  // spinning backwards
  unjamming // spinning backwards until jam has stopped
};

class Intake: public Motorized_subsystem<intake_states, NUM_OF_INTAKE_STATES, INTAKE_MAX_VELOCITY> {
  Timer jam_timer{"jam_timer"};
public:
  Intake(Motorized_subsystem<intake_states, NUM_OF_INTAKE_STATES, INTAKE_MAX_VELOCITY> motorized_subsystem);  // constructor
  void handle();  // contains state machine code
  void handle_state_change(); // cleans up and preps the machine to be in the target state
};

extern Intake intake;