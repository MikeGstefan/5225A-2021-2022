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
#define f_level 4 
#define f_top 5
void skills();
void skills2();

void skills4();

void select_auton();
void run_auton();
void load_auton();