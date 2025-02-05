#pragma once
#include "main.h"
#include "controller.hpp"
#include "piston.hpp"

class _Controller;

extern _Controller master;
extern _Controller partner;
extern pros::Motor front_l, front_r, center_l, center_r, back_l, back_r;
extern pros::Motor f_lift_m, b_lift_m;
extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;

extern pros::Imu ramp_imu;
extern pros::Distance r_dist, l_dist, b_dist, r_goal_dist, f_dist, l_reset_dist, r_reset_dist, hitch_dist;

extern pros::ADIAnalogIn f_lift_pot, b_lift_pot;
extern pros::ADIDigitalIn intk_t;
extern Piston f_claw_c, f_claw_o, lift_t, drive_t, b_claw, tilt_lock, hitch;