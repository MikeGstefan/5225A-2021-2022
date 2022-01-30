#pragma once
#include "main.h"
#include "config.hpp"
#include <iostream>
#include <cmath>
using namespace pros;

enum class Vision_States{
    stable,
    left_high,
    right_high,
    left_spasm,
    right_spasm,
    left_gone,
    right_gone,
    spasming
};

class vision{

  public:
      double last_distance = 0;
      bool sus_spasm = 0;
      bool  spasm = 0;
      void spasm_check();
};

void vision_loop();
