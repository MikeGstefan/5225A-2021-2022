
// NEW CONFIG
#pragma once
#include "main.h"
#include "controller.hpp"

class _Controller;
class Text;

extern _Controller master;
extern pros::Motor front_l, front_r, back_l, back_r;
extern pros::Motor f_lift_m, b_lift_m;
extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;

extern pros::Imu ramp_imu;
extern pros::Distance r_dist, l_dist, b_dist, f_dist, r_reset_dist, l_reset_dist;

extern pros::ADIDigitalOut trans_p, b_claw_p, f_claw_p, Led1, Led2;
extern pros::ADIDigitalIn f_touch;
