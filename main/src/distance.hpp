#pragma once
#include "main.h"
#include "config.hpp"
#include <iostream>
#include <cmath>
#include "drive.hpp"
using namespace pros;

enum class Distance_States{
    stable,
    turn_left,
    turn_right,
};

class cDistance{

  public:
      double last_distance = 0;
};

void distance_loop(double distance);
void distance_reset(int time);
void average(int time);