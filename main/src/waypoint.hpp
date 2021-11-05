#pragma once
#include "main.h"
struct Coord{   // stores members variables for a coordinate: x, y and angle    MUST BE DEINFED HERE, OTHERWISE CIRCULAR HEADER FILE DEPENDENCY OCCURS
    double x, y, angle;
    Coord(double x, double y, double angle);
    Coord();
    // default constructor
};
#include "config.hpp"

#include "util.hpp"
#include "auton.hpp"
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>

using namespace pros;
using namespace std;

enum class brake_types{
    none,
    soft_Stop,
    harsh_Stop,
};

class waypoint{

    // we want error_x, y, d anf move_time to be private so they cannot be accidentally modified when calling the waypoint constrcutor
    double error_x, error_y, error_d, local_x, local_y, d_angle;
    long move_time;

public:
    // constructors
    waypoint(Coord coord, double end_error = 0.5, double end_error_a = 5, bool overshoot = false, bool angle_on_path = true, double min_power_a = 0.0, double offset = 0.0);
    waypoint();
    // members that belong to waypoint class
    Coord coord = {};
    double end_error, end_error_a;
    bool overshoot, angle_on_path;  // if robot's desired angle is parallel to the path
    double min_power_a, offset;
    // double min_power_x_line; x_line not in use yet

    // static vars
    static long total_movement_time;
    static bool movement_complete;
    static double total_error_d, error_d_next_waypoint;
    static int next_waypoint, path_size;

    // getters and setters for private members
    double get_error_x() const;
    void set_error_x(double error_x);
    double get_error_y() const;
    void set_error_y(double error_y);
    double get_error_d() const;
    void set_error_d(double error_d);
    double get_move_time() const;
    void set_move_time(double time);
    double get_local_x() const;
    void set_error_x(double error_x);
    double get_local_y() const;
    void set_error_y(double error_y);
    double get_d_angle()const;
    void set_d_angle(double d_angle);


    // static methods
    static void wait_for_complete();
    static void wait_for_distance_from_waypoint(double distance = 0.5, int waypoint = 0);   // waypoint param refers to index of path
    static void wait_for_waypoint(int waypoint = 0);

};

struct waypoint_target_params{
  vector<waypoint> path;
  bool debug;
  brake_types brake_type;
  bool full_speed;
  int max_power;
};

// waypoint task
extern Task *waypointTask;

// move functions
void move_through_waypoints(void* params);
void move_through_waypoints_async(vector<waypoint> path, bool debug = false, brake_types brake_type = brake_types::harsh_Stop, bool full_speed = false, int max_power = 127);
void move_through_waypoints_sync(vector<waypoint> path, bool debug = false, brake_types brake_type = brake_types::harsh_Stop, bool full_speed = false, int max_power = 127);

// waypoints task function
void waypointStartTask(const waypoint_target_params& waypoint_params);
void waypointStopTask();

// waypoint functions
void set_offsets(vector<waypoint>& path);
void set_angles(vector<waypoint>& path);
void set_errors(vector<waypoint>& path);
double set_error_d_waypoints_left(vector<waypoint>& path);
double set_error_x_waypoints_left(vector<waypoint>& path);
double set_error_y_waypoints_left(vector<waypoint>& path);


void powered_brake();
