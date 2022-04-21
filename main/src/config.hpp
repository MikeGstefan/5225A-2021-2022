#pragma once
#include "main.h"
#include "controller.hpp"
#include "piston.hpp"

extern _Controller master;
extern _Controller partner;
extern Motor front_l, front_r, center_l, center_r, back_l, back_r;
extern Motor f_lift_m, b_lift_m;
extern ADIEncoder LeftEncoder, RightEncoder, BackEncoder;

extern Imu ramp_imu;
extern Distance r_dist, l_dist, b_dist, r_goal_dist, f_dist, l_reset_dist, r_reset_dist;

extern ADIAnalogIn f_lift_pot, b_lift_pot;
extern Piston f_claw_c, f_claw_o, lift_t, drive_t, b_claw, tilt_lock;