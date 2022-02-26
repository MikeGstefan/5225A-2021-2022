#pragma once
#include "main.h"
#include "config.hpp"
#include <iostream>
#include <cmath>
#include "drive.hpp"
using namespace pros;

enum class Vision_States{
    stable,
    turn_left,
    turn_right,
};

class vision{

  public:
      double last_distance = 0;
      bool sus_spasm = 0;
      bool  spasm = 0;
      void spasm_check();
};

void vision_loop(double distance, int timeout = 0);
