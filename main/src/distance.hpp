#pragma once
#include "main.h"
#include "config.hpp"
#include <iostream>
#include <cmath>
#include <numeric>
#include "drive.hpp"
#include "geometry.hpp"
#include "auton_util.hpp"
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

void ramp_distance_line_up(double distance, int timeout = 0);
void goal_line_up(int timeout = 0);
Position distance_reset_left(int cycle = 24);
Position distance_reset_right(int cycle = 24);
Position distance_reset_center(int cycle = 24);
