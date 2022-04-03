#include "config.hpp"
#include <utility>
#include "pros/adi.hpp"

// NEW config
_Controller master(pros::E_CONTROLLER_MASTER);
_Controller partner(pros::E_CONTROLLER_PARTNER);
pros::ADIEncoder LeftEncoder({20,5, 6}, false), RightEncoder({20,1,2}, true), BackEncoder({20,3, 4}, false);

pros::Motor front_l(10, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor center_l(9, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_l(8, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor front_r(1, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor center_r(2, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_r(3, pros::E_MOTOR_GEARSET_18, false,pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor f_lift_m(4, pros::E_MOTOR_GEARSET_36, false,pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor b_lift_m(5, pros::E_MOTOR_GEARSET_36, false,pros::E_MOTOR_ENCODER_DEGREES);


// pros::Imu imu_sensor(4);
Piston f_claw_c(7,1,0);
Piston f_claw_o(6,0,0);
Piston drive_t({20,7},0,0);
Piston lift_t(4,0,0);
Piston b_claw(5,0,0);

pros::Distance b_dist(11), f_dist(6);

pros::ADIAnalogIn f_lift_pot(8), b_lift_pot({20,8});
// pros::ADIDigitalIn intake_jam({{16, 7}});

pros::Distance l_dist(18);
pros::Distance r_dist(16), r_reset_dist(11), l_reset_dist(12);

pros::Imu ramp_imu(17);

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
  std::make_tuple(nullptr, 0, "", "", nullptr),
  std::make_tuple(nullptr, 0, "", "", nullptr),
};//std::make_tuple(nullptr, 0, "", "", nullptr),

std::array<std::pair<pros::ADIDigitalOut*, const char*>, 8> pneumatics_for_gui = {
  std::make_pair(nullptr, "Transmission"),
  std::make_pair(nullptr, "Back Claw"),
  std::make_pair(nullptr, "Front Claw"),
  std::make_pair(nullptr, "LED1"),
  std::make_pair(nullptr, "LED 2"),
  std::make_pair(nullptr, ""),
  std::make_pair(nullptr, ""),
  std::make_pair(nullptr, ""),
}; //std::make_pair(nullptr, ""),