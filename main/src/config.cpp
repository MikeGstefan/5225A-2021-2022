#include "config.hpp"
#include <utility>
#include "pros/adi.hpp"

// NEW config
_Controller master(pros::E_CONTROLLER_MASTER);
_Controller partner(pros::E_CONTROLLER_PARTNER);
pros::ADIEncoder LeftEncoder(3,4, false), RightEncoder(1,2, false), BackEncoder({16,3, 4}, false);

pros::Motor front_l(13, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor front_r(8, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_l(11, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_r(10, pros::E_MOTOR_GEARSET_18, false,pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor center_l(12, pros::E_MOTOR_GEARSET_18, true,pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor center_r(9, pros::E_MOTOR_GEARSET_18, false,pros::E_MOTOR_ENCODER_DEGREES);


pros::Motor f_lift_m(5, pros::E_MOTOR_GEARSET_36, false,pros::E_MOTOR_ENCODER_DEGREES), b_lift_m(7, pros::E_MOTOR_GEARSET_36, true,pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor intk(6, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
// tank drive config
// pros::Motor front_l(19, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor front_r(17, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor back_l(20, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor back_r(18, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);

// pros::Imu imu_sensor(4);
pros::Distance b_dist(9), f_dist(12), r_reset_dist(19), l_reset_dist(20);
pros::ADIDigitalOut trans_p({{16,8}}), b_claw_p(6), f_claw_p(8);
pros::ADIDigitalOut Led1({{14, 7}});
pros::ADIDigitalOut Led2({{14, 8}});
pros::ADIDigitalIn intake_jam({{16, 7}});
pros::ADIDigitalIn f_touch(7);
// pros::ADIDigitalIn intake_jam({{16, 7}});

pros::Distance l_dist(18);
pros::Distance r_dist(17);

pros::Imu ramp_imu(5);

// ADIDigitalOut f_claw_p({{16,1}});

//1:temp(leave as 0), 2:long name, 3:short name
// std::array<std::tuple<pros::Motor*, int, const char*, const char*, Text_*>, 8> motors = {
//   std::make_tuple(&front_l, 1, "Front Left", "FL", nullptr),
//   std::make_tuple(&front_r, 1, "Front Right", "FR", nullptr),
//   std::make_tuple(&back_l, 1, "Back Left", "BL", nullptr),
//   std::make_tuple(&back_r, 1, "Back Right", "BR", nullptr),
//   std::make_tuple(&b_lift_m, 0, "", "", nullptr),
//   std::make_tuple(&f_lift_m, 0, "", "", nullptr),
//   std::make_tuple(&intk, 0, "", "", nullptr),
std::array<std::tuple<pros::Motor*, int, const char*, const char*, Text_*>, 8> motors_for_gui = {
  std::make_tuple(&front_l, 0, "Front Left", "FL", nullptr),
  std::make_tuple(&front_r, 0, "Front Right", "FR", nullptr),
  std::make_tuple(&back_l, 0, "Back Left", "BL", nullptr),
  std::make_tuple(&back_r, 0, "Back Right", "BR", nullptr),
  std::make_tuple(&f_lift_m, 0, "Front Lift", "F", nullptr),
  std::make_tuple(&b_lift_m, 0, "Back Lift", "B", nullptr),
  std::make_tuple(&intk, 0, "Intake", "IN", nullptr),
  std::make_tuple(nullptr, 0, "", "", nullptr),
};//std::make_tuple(nullptr, 0, "", "", nullptr),

std::array<std::pair<pros::ADIDigitalOut*, const char*>, 8> pneumatics_for_gui = {
  std::make_pair(&trans_p, "Transmission"),
  std::make_pair(&b_claw_p, "Back Claw"),
  std::make_pair(&f_claw_p, "Front Claw"),
  std::make_pair(&Led1, "LED1"),
  std::make_pair(&Led2, "LED 2"),
  std::make_pair(nullptr, ""),
  std::make_pair(nullptr, ""),
  std::make_pair(nullptr, ""),
}; //std::make_pair(nullptr, ""),