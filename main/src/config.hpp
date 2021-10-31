#pragma once
#include "main.h"

extern pros::Controller master;
extern pros::Motor front_l, front_r, back_l, back_r, lift_m, f_bar, c_bar, mogo;
extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;
extern pros::ADIDigitalOut claw_in, claw_out;
extern pros::ADIDigitalIn claw_touch, mogo_limit;
