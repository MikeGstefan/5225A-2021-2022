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
#include "Subsystems/b_lift.hpp"
#include "Subsystems/f_lift.hpp"
#include <fstream>



void skills();
void skills2();
void skills3();
void skills4();
void skillsPark();

void lrt_auton();

void blue_highside();
void blue_highside_tall();
void blue_lowside();
namespace Autons{

  enum class autons;
  enum class start_pos;
  enum class alliances;

  extern const char* alliance_names[];
  extern const char* start_pos_names[];
  extern const char* auton_names[];

  extern autons cur_auton;
  extern start_pos cur_start_pos;
  extern alliances cur_alliance;
  extern int cur_goal;

  void switch_alliance(alliances new_ally);
  void switch_alliance();
  void set_target_goal(int goal = cur_goal);
  void next_route();
  void prev_route();
  void next_start_pos();
  void prev_start_pos();
  void file_read();
  void pos_file_read();

}