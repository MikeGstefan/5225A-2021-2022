#pragma once
#include "main.h"
#include "util.hpp"
using namespace pros;
using namespace std;

enum class lift_position_types{
  positive,
  negative,
  fastest
};

void f_bar_cal();
void c_bar_cal();
void f_bar_elastic_util();
void find_arm_angles(double target_x, double target_y, lift_position_types lift_position_type = lift_position_types::fastest);

// class Lift: public pros::Motor {  // subclasses pros::Motor
//   double mecanum_wheel_radius, parallel_arm_len, gear_diameter, lift_base_height, arm_len, gear_ratio, offset_a, offset_h; // offset_a should be in motor encoder degrees
// public:
//   Lift (pros::Motor& lift_motor, double mecanum_wheel_radius, double parallel_arm_len, double gear_diameter, double lift_base_height, double arm_len, double gear_ratio, double offset_a);
//   void cal(void * params = nullptr);
//   double pos_to_height(double pos);
//   double height_to_pos(double height);
//   void lift_height_util();
// };
//
// extern Lift lift;
