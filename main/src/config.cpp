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

std::array<std::tuple<pros::Motor*, Text*, int, std::string, const char*>, 8> motors = {
  std::make_tuple(&front_l, &mot_temp_1, 0, "Front Left", "FL"),
  std::make_tuple(&front_r, &mot_temp_2, 0, "Front Right", "FR"),
  std::make_tuple(&back_l, &mot_temp_3, 0, "Back Left", "BL"),
  std::make_tuple(&back_r, &mot_temp_4, 0, "Back Right", "BR"),
  // std::make_tuple(&intk, &mot_temp_5, 0, "Intake", "IN"),
  std::make_tuple(nullptr, nullptr, 0, "", ""),
  std::make_tuple(&f_bar, &mot_temp_6, 0, "Four Bar", "FB"),
  std::make_tuple(&c_bar, &mot_temp_7, 0, "Chain Bar", "CB"),
  std::make_tuple(&uptk, &mot_temp_8, 0, "Uptake", "UP"),
}; // std::make_tuple(nullptr, nullptr, 0, "", ""),
