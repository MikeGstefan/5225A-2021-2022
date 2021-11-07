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
  Fast,
  Holding,
  DropOff
};

class Intake{

public:
  double error;
  double ring_count = 0;
  double target = 950;
  bool running = 0;
  bool have_ring = 0;
  double power = 80;
  void setIntakeState(Intake_States state);
  void Intake_brake(int time);
  void Intake_Setup();
  void StartInTask();
  void MoveTo(double speed, double pos);

};
void Reset();
void On_Off_Task();
void ringup();
void ringdown();
void Intake_loop();

extern Intake intake;



extern Intake_States Intake_State;
