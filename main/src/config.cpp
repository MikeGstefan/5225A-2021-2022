#include "config.hpp"

static constexpr int e_port = 20;

_Controller
  master(E_CONTROLLER_MASTER),
  partner(E_CONTROLLER_PARTNER);

ADIEncoder
  LeftEncoder({e_port, 1, 2}, false),
  RightEncoder({e_port, 5, 6}, true),
  BackEncoder({e_port, 3, 4}, false);

Motor
  front_l(10, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES),
  center_l(9, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES),
  back_l(8, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES),

  front_r(1, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES),
  center_r(2, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES),
  back_r(3, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES),

  f_lift_m(4, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES),
  b_lift_m(5, E_MOTOR_GEARSET_36, false, E_MOTOR_ENCODER_DEGREES);

Piston
  tilt_lock(3, "Tilt Lock", HIGH),
  lift_t(4, "Lift Trans", HIGH),
  b_claw(5, "B Claw", LOW),
  f_claw_o(6, "F Claw O", LOW, 0),
  f_claw_c(7, "F Claw C", HIGH),
  hitch({e_port, 7}, "Hitch", LOW),
  drive_t({e_port, 8}, "Drive Trans", LOW);

Distance
  b_dist(11), f_dist(6),
  r_reset_dist(13), l_reset_dist(14),
  r_dist(12), l_dist(18),
  hitch_dist(7);

ADIAnalogIn f_lift_pot(8), b_lift_pot(2);
ADIDigitalIn intk_t(1);
Imu ramp_imu(17);