#pragma once
#include "common.hpp"

enum class lift_position_types{
  positive,
  negative,
  fastest
};

// NOTE: all y positions passed in should be negative on the inside of the lift
class Lift {
private:
  double bottom_arm_end_error = 15.0, top_arm_end_error = 15.0; // the acceptable range of error for the arms
  double top_arm_gear_ratio = 5.0/3.0, bottom_arm_gear_ratio = 7.0;
  double bottom_arm_len = 14.5, top_arm_len = 11.5;
  double dist_between_axles = 10.5;
  double base_height = 17.0;  // height from ground to bottom arm top pivot axle
  double offset_h = base_height + dist_between_axles; // height offset
  double bottom_arm_offset_a = 42.5; // this will be in arm degrees
  double top_arm_offset_a = 65.0; // this will be in arm degrees
  double bottom_arm_lower_limit = 0, bottom_arm_upper_limit = 700, top_arm_lower_limit = 0, top_arm_upper_limit = 500; // arm limits in encoder degrees

public:
  void f_bar_cal();
  void c_bar_cal();
  void f_bar_elastic_util();
  tuple<double, double, double, double, double, double, bool> find_arm_angles(double target_y, double target_z, const lift_position_types lift_position_type = lift_position_types::fastest);  // returns arm_angles
  void move_to_target(double target_y, double target_z, const lift_position_types lift_position_type = lift_position_types::fastest, const bool wait_for_complete = true, const double bottom_arm_speed = 200, const double top_arm_speed = 100);  // actual method to move the lift to a target
  void move_to_target_util(); // utility to move arm with controller
  double find_top_arm_angle(const double target_y); // finds top arm angle that will contact it with a line specified by target_y
  void touch_line(const double target_y, double speed = 100); // moves the top arm to contact the line specified by target_y
  double get_arm_velocity_ratio(const double target_y);  // gets ratio of top arm to bottom arm velocity to contact the line specified by target_y
  void move_on_line(double target_y, double target_z_start, double target_z_end); // moves the end effector in a vertical line
  void new_move_on_line(double target_y, double target_z_start, double target_z_end); // moves the end effector in a vertical line
  void pickup_rings();
};
extern Lift lift;

// old DR4B lift class
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
