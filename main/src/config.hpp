// NEW CONFIG
#pragma once
#include "main.h"
#include "controller.hpp"
#include <vector>

class _Controller;
class Text;

extern _Controller master;
extern pros::Motor front_l, front_r, back_l, back_r;
extern pros::Motor f_lift_m, b_lift_m;
extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;
extern pros::Distance back_right_dist;
extern pros::Distance back_left_dist;

extern pros::Imu imu_sensor;
extern pros::Distance b_dist, r_dist;

extern pros::ADIDigitalOut trans_p, b_claw_p;
