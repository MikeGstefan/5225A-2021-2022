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
#include "vision_loop.hpp"
#include "Subsystem/b_lift.hpp"
#include "Subsystem/f_lift.hpp"
#include <fstream>



void skills();
void skills2();
void skills3();
void skills4();
void skillsPark();

void lrt_auton();







enum class autons{
  Skills,
  Auto2,
  Auto3,
  NUM_OF_ELEMENTS,
  Default = Skills,
};

enum class start_pos{
  Pos1,
  Pos2,
  Pos3,
  NUM_OF_ELEMENTS,
  Default = Pos1,
};

enum class alliances{
  RED,
  BLUE,
  Default = BLUE
};

extern autons cur_auton;
extern start_pos cur_start_pos;
extern alliances cur_alliance;
extern int cur_goal;

void switch_alliance(alliances new_ally);
void switch_alliance();
void set_target_goal(int goal = cur_goal);
void next_auton();
void prev_auton();
void next_start_pos();
void prev_start_pos();
void auton_file_read();
void pos_auton_file_read();


