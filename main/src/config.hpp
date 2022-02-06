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
extern pros::Distance left_distance;
extern pros::Distance right_distance;

extern pros::Imu ramp_imu;
extern pros::Distance b_dist, r_dist;

extern pros::ADIDigitalOut trans_p, b_claw_p, f_claw_p;