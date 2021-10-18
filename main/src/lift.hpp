#pragma once
#include <tuple>
#include "main.h"
#include "util.hpp"
#include "drive.hpp"
#include "pid.hpp"
using namespace pros;
using namespace std;

enum class lift_position_types{
  positive,
  negative,
  fastest
};

// NOTE: all y positions passed in should be negative on the inside of the lift
class Lift {
private:

  // lift calculation constants
  double bottom_arm_end_error = 15.0, top_arm_end_error = 15.0; // the acceptable range of error for the arms
  double top_arm_gear_ratio = 5.0/3.0, bottom_arm_gear_ratio = 7.0;
  double bottom_arm_len = 14.5, top_arm_len = 11.5;
  double dist_between_axles = 10.5;
  double base_height = 17.0;  // height from ground to bottom arm top pivot axle
  double offset_h = base_height + dist_between_axles; // height offset
  double bottom_arm_offset_a = 42.5; // this will be in arm degrees
  double top_arm_offset_a = 65.0; // this will be in arm degrees
  double bottom_arm_lower_limit = 0, bottom_arm_upper_limit = 700, top_arm_lower_limit = 0, top_arm_upper_limit = 500; // arm limits in encoder degrees

  // lift state machine variables
  enum states {
    idle,
    full,
    down,
    raised,
    platform,
    tip,
    dropoff
  };

  states state;
  states last_state;

  Task* task = nullptr;

  bool dropoff_front = true;


public:

  void f_bar_cal();
  void c_bar_cal();

  void f_bar_elastic_util();

  tuple<double, double, double, double, double, double, bool> find_arm_angles(double target_y, double target_z, const lift_position_types lift_position_type = lift_position_types::fastest);  // solves for arm_angles

  void move_to_target(double target_y, double target_z, const lift_position_types lift_position_type = lift_position_types::fastest, const bool wait_for_complete = true, const double bottom_arm_speed = 200, const double top_arm_speed = 100);  // actual method to move the lift to a target

  void move_to_target_util(); // utility to move arm with controller

  double find_y_pos();  // solves for current y position of end effector
  double find_z_pos();  // solves for current z position of end effector

  // methods relating to ring pickup
  double find_top_arm_angle(const double target_y); // finds top arm angle that will contact it with a line specified by target_y
  void touch_line(const double target_y, double speed = 100); // moves the top arm to contact the line specified by target_y
  double get_arm_velocity_ratio(const double target_y);  // gets ratio of top arm to bottom arm velocity to contact the line specified by target_y
  void move_on_line(const double target_y, const double target_z_start, const double target_z_end, const double speed); // moves the end effector in a vertical line

  // state machine methods
  void handle();  // is called once per loop in drive to handle current state of lift
  void set_state(const states next_state); // sets state of machine and logs the change

  void stop_pickup_task();
  void pickup_rings();  // creates a task to pickup rings
  void lower_f_bar();
  void move_to_neutral_position();
  void move_to_tip_mogo_position();

};

extern Lift lift;

void pickup_rings();  // async overload of Lift::pickup_rings
