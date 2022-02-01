#include "config.hpp"

// NEW config
_Controller master(pros::E_CONTROLLER_MASTER);
pros::ADIEncoder LeftEncoder(3, 4, false), RightEncoder(1, 2, true), BackEncoder(6, 5, true);

pros::Motor front_l(19, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor front_r(20, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_l(17, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_r(18, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);

// tank drive config
// pros::Motor front_l(19, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor front_r(17, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor back_l(20, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor back_r(18, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);

pros::Imu imu_sensor(1);
pros::ADIDigitalOut lift_piston(7);



pros::Imu ramp_imu(11);

//1:temp(leave as 0), 2:long name, 3:short name
std::array<std::tuple<pros::Motor*, int, const char*, const char*, Text*>, 8> motors = {
  std::make_tuple(&front_l, 1, "Front Left", "FL", nullptr),
  std::make_tuple(&front_r, 1, "Front Right", "FR", nullptr),
  std::make_tuple(&back_l, 1, "Back Left", "BL", nullptr),
  std::make_tuple(&back_r, 1, "Back Right", "BR", nullptr),
  std::make_tuple(nullptr, 0, "", "", nullptr),
  std::make_tuple(nullptr, 0, "", "", nullptr),
  std::make_tuple(nullptr, 0, "", "", nullptr),
  std::make_tuple(nullptr, 0, "", "", nullptr),
};//std::make_tuple(nullptr, 0, "", "", nullptr),
