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

  constexpr bool normal_auton = true;

  enum class autons;
  enum class start_pos;
  enum class alliances; //alliances and goals really shouldn't be enums, but it makes my code more consistent
  enum class goals;

  extern autons cur_auton;
  extern start_pos cur_start_pos;
  extern alliances cur_alliance;
  extern goals cur_goal;

  void auton_file_read();
  void auton_file_update();
  void auton_give_up();
  void auton_selector();