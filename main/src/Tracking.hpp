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


struct arc_params{
  const int fuck_you = 11;
  const Point start; 
  Position target;
  const double radius, max_power, min_angle_percent, decel_speed, decel_dist;
  const bool positive, angle_relative_to_arc, brake = true;
  arc_params(const Point start, Position target, const double radius, const bool positive, const double max_power, const bool angle_relative_to_arc, const double min_angle_percent, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0);
};

struct line_params{
  const int fuck_you = 11;
  const Point start;
  Position target;
  const double max_power = 127.0; 
  const bool overshoot = false;
  const double min_angle_percent = 0.0; 
  const bool brake = true;
  const double decel_dist = 0.0, decel_speed = 0.0;
  line_params(const Point start, Position target, const double max_power = 127.0, const bool overshoot = false, const double min_angle_percent = 0.0, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0);
};

struct point_params{ 
  const int fuck_you = 11;
  const Position target;
  const double max_power;
  const bool overshoot;
  const double min_angle_percent;
  const bool brake;
  const double decel_dist, decel_speed;
  point_params(const Position target, const double max_power = 127.0, const bool overshoot = false, const double min_angle_percent = 0.0, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0);
};

struct tank_arc_params{ 
  const int fuck_you = 11;
  const Point start_pos;
  Position target;
  const double power, max_power = 127.0;
  const bool brake = false;
  tank_arc_params(const Point start_pos, Position target, const double power = 127.0, const double max_power = 127.0, const bool brake = false);
};

struct tank_point_params{ 
  const int fuck_you = 11;
  const Position target;
  const bool turn_dir_if_0;
  const double max_power, min_angle_percent;
  const bool brake;
  tank_point_params(const Position target, const bool turn_dir_if_0, const double max_power = 127.0, const double min_angle_percent = 1.0, const bool brake = true);
};

struct turn_angle_params{ 
  const int fuck_you = 11;
  const double target_a;
  const bool brake;
  turn_angle_params(const double target_a, const bool brake = true);
};

struct turn_point_params{ 
  const int fuck_you = 11;
  const Point target;
  const bool brake;
  turn_point_params(const Point target, const bool brake = true);
};



enum class move_types{
  arc,
  line,
  point,
  tank_arc,
  tank_point,
  turn_angle,
  turn_point
};


void move_start(move_types type, std::variant<arc_params, line_params, tank_arc_params, point_params, tank_point_params, turn_angle_params, turn_point_params> params, bool wait_for_comp = true);
bool move_wait_for_complete();
void move_stop(bool brake = false);


void rush_goal(double target_x, double target_y, double target_a);
void rush_goal2(double target_x, double target_y, double target_a);


void move_to_point(void* params);
void move_on_arc(void* params);
void move_on_line(void* params);

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

