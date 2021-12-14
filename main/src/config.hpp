#pragma once
#include "main.h"
#include "controller.hpp"

class _Controller;
class Text;

extern _Controller master;
extern Text mot_temp_1, mot_temp_2, mot_temp_3, mot_temp_4, mot_temp_5, mot_temp_6, mot_temp_7, mot_temp_8;
extern pros::Motor front_l, front_r, back_l, back_r, f_bar, c_bar, intk, uptk;
extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;
extern pros::ADIDigitalOut claw_in, claw_out;
extern pros::ADIDigitalIn claw_touch;

extern std::array<std::tuple<pros::Motor*, Text*, int, std::string, const char*>, 8> motors;
