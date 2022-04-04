#pragma once
#include "main.h"
#include "controller.hpp"
#include "piston.hpp"

class _Controller;
class Text_;

extern _Controller master;
extern _Controller partner;
extern pros::Motor front_l, front_r, center_l, center_r, back_l, back_r;
extern pros::Motor f_lift_m, b_lift_m;
extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;

extern pros::Imu ramp_imu;
extern pros::Distance r_dist, l_dist, b_dist, f_dist, r_reset_dist, l_reset_dist;

extern pros::ADIAnalogIn f_lift_pot, b_lift_pot;