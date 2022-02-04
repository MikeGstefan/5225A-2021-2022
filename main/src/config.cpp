#include "config.hpp"

// NEW config
_Controller master(pros::E_CONTROLLER_MASTER);
pros::ADIEncoder LeftEncoder(3, 4, false), RightEncoder(5,6, false), BackEncoder(1, 2, true);

pros::Motor front_l(5, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor front_r(1, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_l(3, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_r(2, pros::E_MOTOR_GEARSET_18, true,pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor f_lift_m(16), b_lift_m(9, pros::E_MOTOR_GEARSET_36, true,pros::E_MOTOR_ENCODER_DEGREES);

// tank drive config
// pros::Motor front_l(19, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor front_r(17, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor back_l(20, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor back_r(18, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);

pros::Imu imu_sensor(4);
pros::Distance b_dist(6), r_reset_dist(10);
pros::ADIDigitalOut trans_p(7), b_claw_p(8);


pros::Distance left_distance(2);
pros::Distance right_distance(3);

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
