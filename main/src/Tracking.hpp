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

using namespace pros;

extern _Task update_t;
extern Task *moveTask;



void update(void* params);
void updateStartTask();
void updateStopTask();

class Tracking{

public:
    double x_coord = 0.0; double y_coord = 0.0; double global_angle = 0.0; //double target_x = 0.0;
    double power_x, power_y, power_a;
    double l_velo, r_velo, b_velo;
    double drive_error;
    bool move_complete = true, move_started = false;
    int movetime = 0;

    void move_start_task();
    void move_stop_task();
    void wait_for_complete();
    void wait_for_error(double distance);
    Position g_velocity;   // global g_velocity stores x, y and angular velocities
    void wait_for_dist(double distance);

    void reset(double x, double y, double a);

};
struct move_target_params{
  double target_x;
  double target_y;
  double target_a;
  bool Brake;
  bool debug;
  int max_power;
  bool Overshoot;
  double end_error;
  double end_error_a;
  double p;
  double lpercent;
  double apercent;
};
extern Tracking tracking;
extern move_target_params move_params;

void move_to_target(void* params);
void move_to_target_async(double target_x, double target_y, double target_a, bool Brake = true, bool debug = false, int max_power = 127, bool Overshoot = false, double end_error = 0.5, double end_error_a = 5.0, double p = 15.0, double lpercent = 0.0, double apercent = 0.0);
void move_to_target_sync(double target_x, double target_y, double target_a, bool Brake = true, bool debug = false, int max_power = 127, bool Overshoot = false, double end_error = 0.5, double end_error_a = 5.0, double p = 15.0, double lpercent = 0.0, double apercent = 0.0);
void rush_goal(double target_x, double target_y, double target_a);
void rush_goal2(double target_x, double target_y, double target_a);
void move_to_point(Position target, const double max_power = 127.0, const bool overshoot = false, const double min_angle_percent = 0.0, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0);
void move_on_arc(const Point start, Position target, const double radius, const bool positive, const double max_power, const bool angle_relative_to_arc, const double min_angle_percent, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0);
void move_on_line(const Point start, Position target, const double max_power = 127.0, const bool overshoot = false, const double min_angle_percent = 0.0, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0);

// tank drive move algorithms
void tank_move_to_target(const Position target, const bool turn_dir_if_0, const double max_power = 127.0, const double min_angle_percent = 1.0, const bool brake = true);
void tank_turn_to_angle(const double target_a, const bool brake = true);
void tank_turn_to_target(const Point target, const bool reverse = false, const bool brake = true);
void tank_move_on_arc(const Point start_pos, Position target, const double power, const double max_power = 127.0, const bool brake = false);  // min speed for smooth move is 100

const int min_move_power_a = 23;
const int min_move_power_x = 40;
const int min_move_power_y = 17;

#define xy_enable a

// macros for convenient modification of move algorithms

#define polar_to_vector_line(start_x, start_y, magnitude, theta, angle)\
  {start_x, start_y}, {start_x + sin(deg_to_rad(theta)) * magnitude, start_y + cos(deg_to_rad(theta)) * magnitude, angle}\

#define polar_to_vector_point(start_x, start_y, magnitude, theta, angle)\
  {start_x + sin(deg_to_rad(theta)) * magnitude, start_y + cos(deg_to_rad(theta)) * magnitude, angle}\

