#pragma once
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

class Intake{

public:
  double error;
  double ring_count = 0;
  double target = 950;
};

extern Intake intake;

void Intake_loop();
void setIntakeState(Intake_States state);
void On_Off_Task();
void Intake_brake(int time);
void Intake_Setup();
void Reset();
void StartInTask();
void ringup();
void ringdown();

extern Intake_States Intake_State;
