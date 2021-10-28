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
public:

  bool full = false; // if end_effector has rings
  const char* state_names[16] = {
    "neutral", "ring_pickup", "tip", "lowering", "down", "raised", "above_platform", "in_platform", "release_mogo",
    "ring_dropoff", "dropoff_start", "dropoff_back_alliance", "dropoff_back_mid", "dropoff_back_top", "dropoff_front_mid", "dropoff_front_top"
  };

  // public lift state machine variables
  enum states {
    neutral,
    ring_pickup,
    tip,
    lowering,
    down,
    raised,
    above_platform,
    in_platform,
    release_mogo,
    ring_dropoff,
    dropoff_start,  // state is just used avoid repeated code
    dropoff_back_alliance,
    dropoff_back_mid,
    dropoff_back_top,
    dropoff_front_mid,
    dropoff_front_top,
  };

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

  // latest arm targets for lift state machine
  double cur_top_arm_target;
  double cur_bottom_arm_target;

  // private state machine variables

  states state;
  states last_state;

  Task* task_ptr = nullptr;

  bool task_removed;

  // ring dropoff variables
  bool dropoff_front;
  int ring_dropoff_level;
  array<array<Vector2D, 3>, 2> dropoff_coords;  // ring dropoff coords
  Timer last_dropoff_press_timer{"last_dropoff"}; // used to detect multiple consecutive ring dropoff button presses
  static constexpr int dropoff_double_press_time = 500;

public:

  Lift(); // constructor

  // arm calibration methods
  void f_bar_cal();
  void c_bar_cal();

  void f_bar_elastic_util();  // utility to test of elastics on f_bar are worn out

  tuple<double, double, double, double, double, double, bool> find_arm_angles(double target_y, double target_z, const lift_position_types lift_position_type = lift_position_types::fastest);  // solves for arm_angles

  // methods relating to move to target
  void move_to_target(const Vector2D& target, const lift_position_types lift_position_type = lift_position_types::fastest, const bool wait_for_complete = true, const double bottom_arm_speed = 200, const double top_arm_speed = 100);  // actual method to move the lift to a target
  void wait_for_complete(); // waits for move_to_target to reach

  void move_to_target_util(); // utility to move arm with controller

  void move_f_bar_to_height(double target_z, const double speed = 200);

  double find_y_pos();  // solves for current y position of end effector
  double find_z_pos();  // solves for current z position of end effector

  // methods relating to ring pickup
  double find_top_arm_angle(const double target_y); // finds top arm angle that will contact it with a line specified by target_y
  void touch_line(const double target_y, double speed = 100); // moves the top arm to contact the line specified by target_y
  double get_arm_velocity_ratio(const double target_y);  // gets ratio of top arm to bottom arm velocity to contact the line specified by target_y
  void move_on_line(const double target_y, const double target_z_start, const double target_z_end, const double speed); // moves the end effector in a vertical line

  // state machine methods

  void set_state(const states next_state); // sets state of machine and logs the change
  void handle();  // is called once per loop in drive to handle current state of lift

  void start_task(task_fn_t function);  // starts a task given a function
  void stop_task(); // ends that task

  void move_to_neutral(); // moves lift to position right above ring stack
  void move_f_bar_tip();
  void lower_f_bar();
  void raise_f_bar(); // brings f_bar just above the ground
  void raise_f_bar_above_platform();
  void lower_f_bar_in_platform();

  void open_forks();
  void close_forks();
  void open_stabber();
  void close_stabber();

  // getters (for async lift functions)

  // for lower f bar function
  double get_bottom_arm_target() const;
  double get_bottom_arm_end_error() const;

  // for ring dropoff function
  array<array<Vector2D, 3>, 2>& get_dropoff_coords();
  bool is_dropoff_front() const;
  double get_ring_dropoff_level() const;

};

extern Lift lift;

// Async functions
// Note: These functions should be methods of the lift class, but that doesn't work for creating async methods

void pickup_rings(void* params);
void dropoff_rings(void* params);
