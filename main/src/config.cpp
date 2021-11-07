#include "config.hpp"

pros::Controller master(pros::E_CONTROLLER_MASTER);

// new robot config
// pros::Motor front_l(1, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor front_r(2, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor back_l(4, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor back_r(3, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// old robot config:
pros::Motor front_l(1, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor front_r(2, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_l(3, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_r(4, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor intk(6, pros::E_MOTOR_GEARSET_06, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor f_bar(7, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor c_bar(10, pros::E_MOTOR_GEARSET_36, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor uptk(11);

pros::ADIEncoder LeftEncoder(1, 2, false), RightEncoder(3, 4, true), BackEncoder(5, 6, false);
pros::ADIDigitalOut claw_in(7), claw_out(8);
pros::ADIDigitalIn claw_touch({{6, 6}});
