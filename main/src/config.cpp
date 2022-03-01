#include "config.hpp"
#include "pros/adi.hpp"

// NEW config
_Controller master(pros::E_CONTROLLER_MASTER);
_Controller partner(pros::E_CONTROLLER_PARTNER);
pros::ADIEncoder LeftEncoder({16,5, 6}, true), RightEncoder({16,1,2}, true), BackEncoder({16,3, 4}, false);

pros::Motor front_l(2, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor front_r(4, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_l(1, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_r(3, pros::E_MOTOR_GEARSET_18, true,pros::E_MOTOR_ENCODER_DEGREES);

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

pros::Distance l_dist(18);
pros::Distance r_dist(17);

pros::Imu ramp_imu(5);

// ADIDigitalOut f_claw_p({{14,1}});

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
