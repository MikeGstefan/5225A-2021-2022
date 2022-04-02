
// NEW CONFIG
#pragma once
#include "main.h"
#include "controller.hpp"

class _Controller;
class Text_;

extern _Controller master;
extern _Controller partner;
extern pros::Motor front_l, front_r, back_l, back_r;
extern pros::Motor f_lift_m, b_lift_m, intk;
extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;

extern pros::Imu ramp_imu;
extern pros::Distance r_dist, l_dist, b_dist, f_dist, l_reset_dist, r_reset_dist, f_lift_dist;

extern pros::ADIDigitalOut trans_p, b_claw_p_1, b_claw_p_2, f_claw_p, lift_trans_p, b_lock_p, Led1, Led2;
extern pros::ADIDigitalIn f_touch;
extern pros::ADIDigitalIn intake_jam;
extern pros::ADIAnalogIn f_lift_pot, b_lift_pot;