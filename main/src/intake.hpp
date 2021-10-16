#include "main.h"
#include "config.hpp"
#include "gui.hpp"
#include <iostream>
#include <cmath>
#include <memory>
using namespace pros;


enum class Intake_States{
  Jammed,
  Off,
  Searching,
  Intaking,
  Holding
};

void Intake_loop();
void setIntakeState(Intake_States state);
void On_Off_Task();
void Intake_debug();
void Intake_brake(int time);
void Intake_Setup();
void Reset();

extern Intake_States Intake_State;
