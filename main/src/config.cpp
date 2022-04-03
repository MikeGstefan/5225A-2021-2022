#include "config.hpp"

// NEW config
_Controller master(pros::E_CONTROLLER_MASTER);
_Controller partner(pros::E_CONTROLLER_PARTNER);
pros::ADIEncoder LeftEncoder({20,5, 6}, false), RightEncoder({20,1,2}, true), BackEncoder({20,3, 4}, false);

pros::Motor front_l(10, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor center_l(9, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_l(8, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor front_r(1, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor center_r(2, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_r(3, pros::E_MOTOR_GEARSET_18, false,pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor f_lift_m(5, pros::E_MOTOR_GEARSET_36, false,pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor b_lift_m(6, pros::E_MOTOR_GEARSET_36, false,pros::E_MOTOR_ENCODER_DEGREES);


// pros::Imu imu_sensor(4);
Piston f_claw(8,0,0);

pros::Distance b_dist(9), f_dist(12), r_reset_dist(11), l_reset_dist(12);
pros::ADIDigitalOut trans_p({{16,8}}), b_claw_p(6);//, f_claw_p(8);
pros::ADIDigitalOut Led1({{14, 7}});
pros::ADIDigitalOut Led2({{14, 8}});
pros::ADIDigitalIn intake_jam({{16, 7}});
pros::ADIDigitalIn f_touch(1);
pros::ADIAnalogIn f_lift_pot(1), b_lift_pot(2);
// pros::ADIDigitalIn intake_jam({{16, 7}});

pros::Distance l_dist(18);
pros::Distance r_dist(17);

pros::Imu ramp_imu(8);

// ADIDigitalOut f_claw_p({{16,1}});