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
#include "distance.hpp"
#include "Subsystems/b_lift.hpp"
#include "Subsystems/f_lift.hpp"
#include <fstream>



void skills();
void skills2();
void new_skills3();

void skills3();
void skills4();
void skillsPark();

void lrt_auton();

void blue_highside();
void blue_highside_tall();
void blue_lowside();

namespace Autons{
  constexpr bool normal = false;

  enum class autons;
  enum class start_pos;
  enum class alliances; //alliances and goals really shouldn't be enums, but it makes my code more consistent
  enum class goals;

  extern const char* start_pos_names[];
  extern const char* auton_names[];
  extern const char* alliance_names[];
  extern const char* goal_names[];

  extern autons cur_auton;
  extern start_pos cur_start_pos;
  extern alliances cur_alliance;
  extern goals cur_goal;

  void file_read();
  void next_route();
  void prev_route();
  void next_start_pos();
  void prev_start_pos();
  void prev_goal();
  void next_goal();
  void set_target_goal(goals goal);
  void switch_alliance(alliances new_ally = static_cast<alliances>(!static_cast<bool>(cur_alliance)));
  void give_up();
  void selector();

  void file_update();
  void file_read();

}