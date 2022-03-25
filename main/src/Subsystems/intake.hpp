#pragma once
#include "../Libraries/subsystem.hpp"
#include "../drive.hpp"

#define NUM_OF_INTAKE_STATES 7
#define INTAKE_MAX_VELOCITY 600

enum class intake_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  off, // not spinning
  on, // spinning forward
  reversed,  // spinning backwards
  unjamming, // spinning backwards until jam has stopped
  shifting_to_intake, // lift/intake transmission switching to intake mode
  wait_for_lift_lowering, // waits for lift to reach bottom before switching into on or reverse state
};

class Intake: public Motorized_subsystem<intake_states, NUM_OF_INTAKE_STATES, INTAKE_MAX_VELOCITY> {
  Timer jam_timer{"jam_timer"};
  Timer shift_timer{"intake_lift_shift_timer"};  // how long since intake was set to shift
  intake_states after_switch_state; // the state the intake will go to after transmission switches to intake

public:
  Intake(Motorized_subsystem<intake_states, NUM_OF_INTAKE_STATES, INTAKE_MAX_VELOCITY> motorized_subsystem);  // constructor
  void handle_buttons();  // handles driver button input
  void handle();  // contains state machine code
  void handle_state_change(); // cleans up and preps the machine to be in the target state
};

extern Intake intake;