#pragma once
#include "main.h"
#include "util.hpp"
using namespace pros;
using namespace std;

class Lift: public pros::Motor {  // subclasses pros::Motor
  double mecanum_wheel_radius, parallel_arm_len, gear_diameter, lift_base_height, arm_len, gear_ratio, offset_a, offset_h; // offset_a should be in degrees
public:
  Lift (pros::Motor& lift_motor, double mecanum_wheel_radius, double parallel_arm_len, double gear_diameter, double lift_base_height, double arm_len, double gear_ratio, double offset_a);
  void cal(void * params = nullptr);
  double deg_to_height(double deg);
  double height_to_deg(double height);
};

extern Lift lift;
