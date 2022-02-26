#pragma once
#include "Subsystems/lift.hpp"
#include "Subsystems/hitch.hpp"
#include "Libraries/gui.hpp"
#include "drive.hpp"
#include "controller.hpp"
#include "pid.hpp"
#include "Tracking.hpp"
#include "task.hpp"
#include "auton_util.hpp"
#include "config.hpp"
#include <fstream>

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
void next_auton();
void prev_auton();
void next_start_pos();
void prev_start_pos();
void auton_file_read();
void pos_auton_file_read();

void red_tall_rush();
void skills();
