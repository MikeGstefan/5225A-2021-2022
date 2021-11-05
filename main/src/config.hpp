#pragma once
#include "main.h"
#include "controller.hpp"

class Controller_;

extern Controller_ master;
// extern Controller_ partner;
extern pros::Motor front_l, front_r, back_l, back_r, f_bar, c_bar, intk, uptk;
extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;
extern pros::ADIDigitalOut claw_in, claw_out;
extern pros::ADIDigitalIn claw_touch;
