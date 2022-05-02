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

#define f_bottom 0
#define f_carry 1 
#define f_plat 2 
#define f_backup 3 
#define f_push 4
#define f_level 5 
#define f_top 6
#define b_bottom 0
#define b_plat 1
#define b_backup 2 
#define b_level 3
#define b_top 4
void skills();
void skills2();
void skills3();
void skills4();

void  high_short();
void high_wp_goal();
void high_line();
void high_tall();
void high_plus();
void high_awp();
void high_match_loads();

void select_auton();
void run_auton();
void load_auton();


enum autos{
  auto1,
  auto2,
  NUM_OF_ELEMENTS =2
};

extern autos cur_auto;

const string auto_names[] = {
  "high  ",
  "center"
};

void autonFile_read();
void auto_select();