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

void rush_high();
void rush_tall();
void rush_low();

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

void select_auton();
void run_auton();
void load_auton();