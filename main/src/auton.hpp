#pragma once
#include "Subsystems/lift.hpp"
#include "Subsystems/hitch.hpp"
#include "Libraries/gui/gui.hpp"
#include "drive.hpp"
#include "controller.hpp"
#include "pid.hpp"
#include "Tracking.hpp"
#include "task.hpp"
#include "auton_util.hpp"
#include "config.hpp"
#include <fstream>

enum class autons{
  Auto1,
  Auto2,
  Auto3,
  NUM_OF_ELEMENTS,

  Skills=Auto1,
};

const std::string auton_names[static_cast<int>(autons::NUM_OF_ELEMENTS)] = {"Skills"};

enum class alliances{
  RED,
  BLUE
};

const std::string alliance_names[2] = {"Red", "Blue"};

void switch_alliance();
void auton_file_update();
void auton_file_read();

void red_tall_rush();
void skills();

extern autons cur_auton;
extern alliances cur_alliance;
