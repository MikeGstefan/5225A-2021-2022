#include "config.hpp"

// NEW config
_Controller master(pros::E_CONTROLLER_MASTER);

pros::Motor front_l(19, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor front_r(20, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_l(17, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_r(18, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor lift_m(5, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES); // DR4B lift
pros::Motor lift_motor(16, pros::E_MOTOR_GEARSET_36, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor f_bar(5, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor c_bar(10, pros::E_MOTOR_GEARSET_36, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor intk(12, pros::E_MOTOR_GEARSET_36, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor uptk(11);

pros::ADIEncoder LeftEncoder(3, 4, false), RightEncoder(1, 2, true), BackEncoder(5, 6, false);
pros::ADIDigitalOut lift_piston(7,0);
pros::ADIDigitalIn claw_touch(8);
pros::ADIDigitalIn lift_trigger({7, 1});
