#pragma once
#include "main.h"
#include "Libraries/geometry.hpp"
#include "Libraries/util.hpp"

class _Task;

extern _Task update_t;
extern _Task move_t;

constexpr int min_move_power_a = 30;
constexpr int min_move_power_x = 40;
constexpr int min_move_power_y = 22;

void update(void* params);

class Tracking{
  friend void update(void* params);
  private:
    Position position;
    bool move_started = false;

  public:
    //global g_velocity stores x, y and angular velocities
    Position g_velocity, power;
    bool move_complete = true;
    double l_velo, r_velo, b_velo;
    double drive_error;
    int movetime = 0;
    Position coord() const;
    void wait_for_dist(double distance, int timeout = 0);
    void reset(double x, double y, double a=0.0);
    void reset(Position position = {});
    double get_dist(Position pos1);
    static double get_dist(Position pos1, Position pos2);

    //returns in radians
    double near_angle(double angle);
    double near_angle(Position pos);
};


class Gyro{
  public:
    Imu& inertial;
    double angle, last_angle;
    int time;

  public:
    Gyro(Imu& imu);

    void calibrate();
    void finish_calibrating();
    void climb_ramp();
    void level(double kP, double kD);

    double get_angle();
    double get_angle_dif();
};

extern Gyro gyro;
extern Tracking tracking;

struct pid_const{
  double p;
  double i;
  double d;
};


struct arc_params{
  Point start = {0.0,0.0}; 
  Position target;
  double radius = 0.0, max_power = 0.0, min_angle_percent = 0.0, decel_speed = 0.0, decel_dist = 0.0;
  bool positive = false, angle_relative_to_arc = false, brake = true;
  arc_params() = default;
  // arc_params()
  arc_params(const Point start, Position target, const double radius, const bool positive, const double max_power, const bool angle_relative_to_arc, const double min_angle_percent, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0);
};

struct line_params{
  Point start = {0.0,0.0};
  Position target;
  double max_power = 127.0; 
  bool overshoot = false;
  double min_angle_percent = 0.0; 
  bool brake = true;
  double decel_dist = 0.0, decel_speed = 0.0;
  line_params() = default;
  line_params(const Point start, Position target, const double max_power = 127.0, const bool overshoot = false, const double min_angle_percent = 0.0, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0);
};

struct line_old_params{
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
  Position target;
  double max_power = 0.0;
  bool overshoot = false;
  double min_angle_percent = 0.0;
  bool brake = true;
  double decel_dist = 0.0, decel_speed = 0.0;
  pid_const x_pid_const = {23.0,0.0,0.0};
  pid_const y_pid_const = {9.5,0.0,1000.0};
  pid_const a_pid_const = {125.0,0.0,1000.0};
  uint32_t time_out = 0;
  double end_error =0.5;
  double end_error_a = 5.0;
  point_params() = default;
  point_params(const Position target, const double max_power = 127.0, const bool overshoot = false, const double min_angle_percent = 0.0, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0, pid_const x_pid_const = {23.0,0.0,0.0}, pid_const y_pid_const = {9.5,0.0,1000.0}, pid_const a_pid_const = {125.0,0.0,1000.0}, const uint32_t time_out = 0, double end_error = 0.5, double end_error_a = 5.0);
};

struct tank_arc_params{
  Point start_pos = {0.0,0.0};
  Position target;
  double power = 127.0, max_power = 127.0;
  bool brake = false;
  double decel_start = 0.0, decel_end = 0.0, decel_target_speed = 0.0;
  tank_arc_params() = default;
  tank_arc_params(const Point start_pos, Position target, const double power = 127.0, const double max_power = 127.0, const bool brake = false, double decel_start = 0.0, double decel_end = 0.0, double decel_target_speed = 0.0);
};

struct tank_point_params{
  Position target;
  bool turn_dir_if_0 = false;
  double max_power = 127.0, min_angle_percent = 1.0;
  bool brake= true;
  double kp_y = 6.4;
  double kp_a = 70.0;
  double kd_a = 0.0;
  int timeout = 0;
  Point end_error = {0.5, 0.5};
  double min_power_y = min_move_power_y;
  tank_point_params() = default;
  tank_point_params(const Position target, const bool turn_dir_if_0 = false, const double max_power = 127.0, const double min_angle_percent = 1.0, const bool brake = true, double kp_y = 6.4, double kp_a =70.0, double kd_a = 0.0, int timeout = 0, Point end_error = {0.5, 0.5}, double min_power_y = min_move_power_y);
};

struct tank_rush_params{
  Position target;
  bool turn_dir_if_0 = false;
  double max_power = 127.0, min_angle_percent = 1.0;
  bool brake= true;
  double kp_a = 150.0;
  double kd_a = 0.0;
  double dist_past = 10.0;
  tank_rush_params() = default;
  tank_rush_params(const Position target, const bool turn_dir_if_0, const double max_power = 127.0, const double min_angle_percent = 1.0, const bool brake = true, double kp_a =150.0, double kd_a = 0.0, double dist_past = 10.0);
};


struct turn_angle_params{
  double target_a = 0.0;
  bool brake = true;
  bool near = true;
  double kp = 5.0, kd = 0.0;
  double kp_v = 10.0, kd_v = 20.0;
  double max_speed = 127;
  int timeout = 0;
  double min_power_a = min_move_power_a;
  double end_error = 5.0;
  turn_angle_params() = default;
  turn_angle_params(const double target_a, const bool brake = true, bool near = true, double kp = 5.0, double kd = 0.0, double kp_v = 10.0, double kd_v = 20.0, double max_speed = 127.0, int timeout = 0, double min_power_a = min_move_power_a, double end_error = 5.0);
};

struct turn_point_params{
  Point target = {0.0,0.0};
  bool brake = true;
  double max_power = 127;
  int timeout = 0;
  turn_point_params() = default;
  turn_point_params(const Point target, const bool brake = true, double max_power = 127, int timeout = 0);
};



enum class move_types{
  arc,
  line,
  point,
  tank_arc,
  tank_point,
  turn_angle,
  turn_point,
  line_old,
  tank_rush
};


void move_start(move_types type, std::variant<arc_params, line_params, tank_arc_params, point_params, tank_point_params, turn_angle_params, turn_point_params, line_old_params,tank_rush_params> params, bool wait_for_comp = true);
bool move_wait_for_complete();
void move_wait_for_error(double error);
void move_stop(bool brake = false);
bool get_move_state();

void move_to_point(void* params);
void move_on_arc(void* params);
void move_on_line(void* params);
void move_on_line_old(void* params);

// tank drive move algorithms
void tank_move_to_target(void* params);
void tank_move_on_arc(void* params);  // min speed for smooth move is 100
void tank_rush_goal(void* params);

void turn_to_angle(void* params);
//overloaded to be called in another function DO NOT CALL ALONE
void turn_to_angle(double target_a, const bool brake, double max_power, int timeout, _Task* ptr);
void turn_to_point(void* params);




constexpr bool xy_enable = true;

// macros for convenient modification of move algorithms
constexpr Vector polar_to_vector_line(Point start, double magnitude, double theta, double angle){
  return Vector();
  //I have no idea what this is actually supposed to be
  // return start, {start.x + sin(deg_to_rad(theta)) * magnitude, start.y + cos(deg_to_rad(theta)) * magnitude, angle};
}
constexpr Vector polar_to_vector_facing_line(Point start, double magnitude, double angle){
  return polar_to_vector_line(start, magnitude, angle, angle);
}
double point_to_angle(Point point);
constexpr Position polar_to_vector_point(Point start, double magnitude, double theta, double angle){
  return {start.x + sin(deg_to_rad(theta)) * magnitude, start.y + cos(deg_to_rad(theta)) * magnitude, angle};
}
Position polar_to_vector_point(double magnitude, double theta, double angle);