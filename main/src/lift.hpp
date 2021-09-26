#pragma once
#include "main.h"
#include "util.hpp"
using namespace pros;
using namespace std;

struct Vector2D{
    Vector2D(double x, double y) : x(x), y(y) {}
    double x, y;
};

void f_bar_cal();
void c_bar_cal();
void find_arm_angles(Vector2D target);

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
