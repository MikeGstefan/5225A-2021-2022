#pragma once
// #include "Subsystems/lift.hpp"
// #include "Subsystems/hitch.hpp"
#include "Libraries/gui.hpp"
#include "drive.hpp"
#include "controller.hpp"
#include "pid.hpp"
#include "Tracking.hpp"
#include "task.hpp"
#include "auton_util.hpp"
#include "config.hpp"
// #include "vision_loop.hpp"
#include "Subsystem/b_lift.hpp"
#include "Subsystem/f_lift.hpp"
#include <fstream>



void skills();
void skills2();
void skills3();
void skills4();
void skillsPark();






enum class autons{
  Skills,
  Auto2,
  Auto3,
  NUM_OF_ELEMENTS,
};

enum class alliances{
  RED,
  BLUE
};

extern autons cur_auton;
extern alliances cur_alliance;

void switch_alliance(alliances = static_cast<alliances>(!static_cast<int>(cur_alliance)));
void next_auton();
void prev_auton();
void auton_file_read();
