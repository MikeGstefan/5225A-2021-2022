#pragma once
#include "main.h"
#include "config.hpp"
#include "util.hpp"
#include "geometry.hpp"
#include "pid.hpp"
#include "drive.hpp"
#include "geometry.hpp"
#include "task.hpp"
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include <functional>
#include <variant>

using namespace pros;

extern _Task update_t;
extern _Task move_t;



void update(void* params);


class Tracking{

public:
    double x_coord = 0.0; double y_coord = 0.0; double global_angle = 0.0; //double target_x = 0.0;
    double power_x, power_y, power_a;
    double l_velo, r_velo, b_velo;

    double drive_error;
    bool move_complete = true, move_started = false;
    int movetime = 0;


    Position g_velocity;   // global g_velocity stores x, y and angular velocities
    void wait_for_dist(double distance);

    void reset(double x, double y, double a);

};


extern Tracking tracking;

struct pid_const{
  double p;
  double i;
  double d;
};


struct arc_params{
  int fuck_you = 11;
  Point start = {0.0,0.0}; 
  Position target = {0.0,0.0,0.0};
  double radius = 0.0, max_power = 0.0, min_angle_percent = 0.0, decel_speed = 0.0, decel_dist = 0.0;
  bool positive = false, angle_relative_to_arc = false, brake = true;
  arc_params() = default;
  // arc_params()
  arc_params(const Point start, Position target, const double radius, const bool positive, const double max_power, const bool angle_relative_to_arc, const double min_angle_percent, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0);
};

struct line_params{
  int fuck_you = 11;
  Point start = {0.0,0.0};
  Position target = {0.0,0.0,0.0};
  double max_power = 127.0; 
  bool overshoot = false;
  double min_angle_percent = 0.0; 
  bool brake = true;
  double decel_dist = 0.0, decel_speed = 0.0;
  line_params() = default;
  line_params(const Point start, Position target, const double max_power = 127.0, const bool overshoot = false, const double min_angle_percent = 0.0, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0);
};

struct line_old_params{
  int fuck_you = 11;
  double start_x = 0.0;
  double start_y = 0.0;
  double target_x = 0.0;
  double target_y =0.0;
  double target_a = 0.0;
  bool Brake = true;
  bool debug = false;
  int max_power = 127;
  bool Overshoot = false;
  double end_error = 0.5;
  double end_error_a = 5.0;
  double p = 15.0;
  double lpercent = 0.0;
  double apercent = 0.0;
  line_old_params() = default;
  line_old_params(double start_x, double start_y, double target_x, double target_y, double target_a, bool Brake = true, bool debug = false, int max_power = 127, bool Overshoot = false, double end_error = 0.5, double end_error_a = 5.0, double p = 15.0, double lpercent = 0.0, double apercent = 0.0);
};

struct point_params{ 
  int fuck_you = 11;
  Position target = {0.0,0.0,0.0};
  double max_power = 0.0;
  bool overshoot = false;
  double min_angle_percent = 0.0;
  bool brake = true;
  double decel_dist = 0.0, decel_speed = 0.0;
  pid_const x_pid_const = { 23.0,0.0,0.0};
  pid_const y_pid_const = { 9.5,0.0,1000.0};
  pid_const a_pid_const = { 125.0,0.0,1000.0};
  uint32_t time_out = 0;
  point_params() = default;
  point_params(const Position target, const double max_power = 127.0, const bool overshoot = false, const double min_angle_percent = 0.0, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0, pid_const x_pid_const = { 23.0,0.0,0.0}, pid_const y_pid_const = { 9.5,0.0,1000.0}, pid_const a_pid_const = { 125.0,0.0,1000.0}, const uint32_t time_out = 0);
};

struct tank_arc_params{ 
  int fuck_you = 11;
  Point start_pos = {0.0,0.0};
  Position target = {0.0,0.0,0.0};
  double power = 127.0, max_power = 127.0;
  bool brake = false;
  tank_arc_params() = default;
  tank_arc_params(const Point start_pos, Position target, const double power = 127.0, const double max_power = 127.0, const bool brake = false);
};

struct tank_point_params{ 
  int fuck_you = 11;
  Position target = {0.0,0.0,0.0};
  bool turn_dir_if_0 = false;
  double max_power = 127.0, min_angle_percent = 1.0;
  bool brake= true;
  tank_point_params() = default;
  tank_point_params(const Position target, const bool turn_dir_if_0, const double max_power = 127.0, const double min_angle_percent = 1.0, const bool brake = true);
};

struct turn_angle_params{ 
  int fuck_you = 11;
  double target_a = 0.0;
  bool brake = true;
  turn_angle_params() = default;
  turn_angle_params(const double target_a, const bool brake = true);
};

struct turn_point_params{ 
  int fuck_you = 11;
  Point target = {0.0,0.0};
  bool brake = true;
  turn_point_params() = default;
  turn_point_params(const Point target, const bool brake = true);
};



enum class move_types{
  arc,
  line,
  point,
  tank_arc,
  tank_point,
  turn_angle,
  turn_point,
  line_old
};


void move_start(move_types type, std::variant<arc_params, line_params, tank_arc_params, point_params, tank_point_params, turn_angle_params, turn_point_params, line_old_params> params, bool wait_for_comp = true);
bool move_wait_for_complete();
void move_wait_for_error(double error);
void move_stop(bool brake = false);


void rush_goal(double target_x, double target_y, double target_a);
void rush_goal2(double target_x, double target_y, double target_a);


void move_to_point(void* params);
void move_on_arc(void* params);
void move_on_line(void* params);
void move_on_line_old(void* params);

// tank drive move algorithms
void tank_move_to_target(void* params);
void tank_move_on_arc(void* params);  // min speed for smooth move is 100

void turn_to_angle(void* params);
//overloaded to be called in another function DO NOT CALL ALONE
void turn_to_angle(double target_a, const bool brake, _Task* ptr);
void turn_to_point(void* params);


const int min_move_power_a = 23;
const int min_move_power_x = 40;
const int min_move_power_y = 17;

#define xy_enable a

// macros for convenient modification of move algorithms

#define polar_to_vector_line(start_x, start_y, magnitude, theta, angle)\
  {start_x, start_y}, {start_x + sin(deg_to_rad(theta)) * magnitude, start_y + cos(deg_to_rad(theta)) * magnitude, angle}\

#define polar_to_vector_facing_line(start_x, start_y, magnitude, angle){\
  polar_to_vector(start_x, start_y, magnitude, angle, angle)\
}
#define polar_to_vector_point(start_x, start_y, magnitude, theta, angle)\
  {start_x + sin(deg_to_rad(theta)) * magnitude, start_y + cos(deg_to_rad(theta)) * magnitude, angle}\

