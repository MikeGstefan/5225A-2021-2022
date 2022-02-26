#include "config.hpp"
#include <utility>

// NEW config
_Controller master(pros::E_CONTROLLER_MASTER);

pros::Motor front_l(19, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor front_r(20, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_l(17, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_r(18, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);

// tank drive config
// pros::Motor front_l(19, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor front_r(17, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor back_l(20, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor back_r(18, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor lift_motor(16, pros::E_MOTOR_GEARSET_36, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor tilter_motor(14, pros::E_MOTOR_GEARSET_36, true, pros::E_MOTOR_ENCODER_DEGREES);

// pros::Motor c_bar(10, pros::E_MOTOR_GEARSET_36, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor intake_motor(9, pros::E_MOTOR_GEARSET_36, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor spinner_motor(8, pros::E_MOTOR_GEARSET_36, true, pros::E_MOTOR_ENCODER_DEGREES);

// pros::Motor uptk(11);
pros::Distance tilter_dist(8);
pros::ADIDigitalOut lift_piston(7);
pros::ADIDigitalIn lift_trigger(8);
pros::ADIEncoder LeftEncoder(3, 4, false), RightEncoder(1, 2, true), BackEncoder(6, 5, true);

pros::ADIDigitalOut tilter_bottom_piston({{1, 1}});
pros::ADIDigitalOut tilter_top_piston({{1, 2}});

pros::ADIDigitalOut intake_piston({{1, 6}});

pros::ADIDigitalIn spinner_trigger({{1, 5}});
pros::ADIDigitalOut ring_piston({{1, 4}});

//1:temp(leave as 0), 2:long name, 3:short name
std::array<std::tuple<pros::Motor*, int, const char*, const char*, Text_*>, 8> motors_for_gui = {
  std::make_tuple(&front_l, 0, "Front Left", "FL", nullptr),
  std::make_tuple(&front_r, 0, "Front Right", "FR", nullptr),
  std::make_tuple(&back_l, 0, "Back Left", "BL", nullptr),
  std::make_tuple(&back_r, 0, "Back Right", "BR", nullptr),
  std::make_tuple(&lift_motor, 0, "Lift", "LF", nullptr),
  std::make_tuple(&tilter_motor, 0, "Tilter", "TL", nullptr),
  std::make_tuple(&intake_motor, 0, "Intake", "IN", nullptr),
  std::make_tuple(&spinner_motor, 0, "Spinner", "SP", nullptr),
}; //std::make_tuple(nullptr, 0, "", "", nullptr),

std::array<std::pair<pros::ADIDigitalOut*, const char*>, 8> pneumatics_for_gui = {
  std::make_pair(&lift_piston, "Lift"),
  std::make_pair(&tilter_bottom_piston, "Tilter Bottom"),
  std::make_pair(&tilter_top_piston, "Tilter Top"),
  std::make_pair(&tilter_bottom_piston, "Intake"),
  std::make_pair(&tilter_top_piston, "End Effector"),
  std::make_pair(nullptr, ""),
  std::make_pair(nullptr, ""),
  std::make_pair(nullptr, ""),
}; //std::make_pair(nullptr, ""),