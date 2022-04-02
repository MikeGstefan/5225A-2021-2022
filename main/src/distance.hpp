#pragma once
#include "main.h"
#include "Libraries/geometry.hpp"

enum class Distance_States{
  stable,
  turn_left,
  turn_right,
};

class cDistance{
  public:
    double last_distance = 0;
};

void distance_loop(double distance, int timeout = 0);
Position distance_reset_left(int time);
Position distance_reset_right(int time);
