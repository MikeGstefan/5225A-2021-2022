#include "config.hpp"

_Controller master(pros::E_CONTROLLER_MASTER);

// new robot config
// pros::Motor front_l(1, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor front_r(2, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor back_l(4, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor back_r(3, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// old robot config:
pros::Motor front_l(19, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor front_r(20, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_l(17, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_r(18, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor lift_m(5, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES); // DR4B lift
pros::Motor lift(16, pros::E_MOTOR_GEARSET_36, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor f_bar(5, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor c_bar(10, pros::E_MOTOR_GEARSET_36, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor intk(12, pros::E_MOTOR_GEARSET_36, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor uptk(11);

pros::ADIEncoder LeftEncoder(3, 4, false), RightEncoder(1, 2, true), BackEncoder(6, 5, false);
pros::ADIDigitalOut claw(7,0);
pros::ADIDigitalIn claw_touch(8);
