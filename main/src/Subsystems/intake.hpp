#pragma once
#include "../Libraries/subsystem.hpp"
#include "../drive.hpp"

#define INTAKE_MAX_VELOCITY 100

enum class intake_states{
  off, // not spinning
  on, // spinning forward
  reversed,  // spinning backwards
  unjamming // spinning backwards until jam has stopped
};

void intake_handle(void* ignore = nullptr);
void intake_target_state_handle();

extern intake_states target_state; // what the intake is TRYING to reach
extern intake_states state; // what the intake CURRENTLY is
extern Timer jam_timer;