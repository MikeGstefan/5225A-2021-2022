#include "config.hpp"

static constexpr int e_port = 20;

// 6 Motor config - Apr 3 2022
_Controller master(pros::E_CONTROLLER_MASTER);
_Controller partner(pros::E_CONTROLLER_PARTNER);
pros::ADIEncoder LeftEncoder(1, 2, true), RightEncoder(7, 8, true), BackEncoder({e_port, 3, 4}, false);

pros::Motor front_l(10, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor center_l(9, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_l(8, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor front_r(1, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor center_r(2, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_r(3, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor f_lift_m(4, pros::E_MOTOR_GEARSET_18, false,pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor b_lift_m(5, pros::E_MOTOR_GEARSET_36, false,pros::E_MOTOR_ENCODER_DEGREES);


// pros::Imu imu_sensor(4);
Piston f_claw_c(4,"",1,0);
Piston f_claw_o(6,"",0,0);
Piston drive_t(3,"",0,0);
Piston lift_t(4,"",1,0);
Piston b_claw(5,"",0,0);
Piston tilt_lock(3,"",0,0);

pros::ADIAnalogIn f_lift_pot(5), b_lift_pot({e_port, 3});
pros::Distance b_dist(11), r_goal_dist(1), f_dist(6);
pros::Distance r_reset_dist(14), l_reset_dist(19);
// pros::ADIDigitalIn intake_jam({{16, 7}});

pros::Distance l_dist(18);
pros::Distance r_dist(12);

pros::Imu ramp_imu(17);

// ADIDigitalOut f_claw_p({{16,1}});
