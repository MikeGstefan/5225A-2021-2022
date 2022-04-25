#include "config.hpp"

static constexpr int e_port = 20;

// 6 Motor config - Apr 12 2022
_Controller master(E_CONTROLLER_MASTER);
_Controller partner(E_CONTROLLER_PARTNER);
ADIEncoder LeftEncoder({e_port, 1, 2}, false), RightEncoder({e_port, 5, 6}, true), BackEncoder({e_port, 3, 4}, false);

Motor front_l(10, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
Motor center_l(9, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);
Motor back_l(8, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);

Motor front_r(1, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);
Motor center_r(2, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
Motor back_r(3, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);

Motor f_lift_m(4, E_MOTOR_GEARSET_18, false,E_MOTOR_ENCODER_DEGREES); 
Motor b_lift_m(5, E_MOTOR_GEARSET_36, false,E_MOTOR_ENCODER_DEGREES);

Piston f_claw_c(7, "F Claw C", 1, 0);
Piston f_claw_o(6, "F Claw O", 0, 0);
Piston drive_t({e_port, 8}, "Drive Trans", 0, 0);
Piston lift_t(4, "Lift Trans", 1, 0);
Piston b_claw(5, "B Claw", 0, 0);
Piston tilt_lock(3, "Tilt Lock", 1, 0);
Piston hitch({e_port, 7}, "Hitch", 0, 0);

ADIAnalogIn f_lift_pot(8), b_lift_pot(2);
ADIDigitalIn intk_t(1);
Distance b_dist(11), f_dist(6);
Distance r_reset_dist(13), l_reset_dist(14);
Distance r_dist(12), l_dist(18);
Distance hitch_dist(7);
// ADIDigitalIn intake_jam({{16, 7}});

Imu ramp_imu(17);

//Sorry motors_for_gui has been moved to gui_construction.cpp. If I keep it here that means 32 externs.