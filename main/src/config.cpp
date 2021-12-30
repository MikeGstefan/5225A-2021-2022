#include "config.hpp"

_Controller master(pros::E_CONTROLLER_MASTER);

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

std::array<std::tuple<pros::Motor*, int, std::string, const char*, Text*>, 8> motors = {
  std::make_tuple(&front_l, 1, "Front Left", "FL", nullptr),
  std::make_tuple(&front_r, 1, "Front Right", "FR", nullptr),
  std::make_tuple(&back_l, 1, "Back Left", "BL", nullptr),
  std::make_tuple(&back_r, 1, "Back Right", "BR", nullptr),
  std::make_tuple(&intk, 1, "Intake", "IN", nullptr),
  std::make_tuple(&f_bar, 1, "Four Bar", "FB", nullptr),
  std::make_tuple(&c_bar, 1, "Chain Bar", "CB", nullptr),
  std::make_tuple(&uptk, 0, "Uptake", "UP", nullptr),
};//std::make_tuple(nullptr, 0, "", "", nullptr),
