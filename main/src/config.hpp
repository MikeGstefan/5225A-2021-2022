// NEW CONFIG
#pragma once
#include "main.h"
#include "controller.hpp"

class _Controller;

extern _Controller master;
extern pros::Motor front_l, front_r, back_l, back_r, f_bar, c_bar, intk, uptk, lift_motor, angler_motor;
extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;
extern pros::ADIDigitalOut claw, claw_out;
extern pros::ADIDigitalIn claw_touch;
extern pros::ADIDigitalIn lift_trigger, tilter_trigger, hitch_trigger;
extern pros::ADIDigitalOut lift_piston, angler_piston, hitch_piston;
extern pros::Distance angler_dist;
