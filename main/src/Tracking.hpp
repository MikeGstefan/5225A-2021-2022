#pragma once
#include "main.h"
#include "config.hpp"
#include "util.hpp"
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>

using namespace pros;



// extern Task *updateTask = nullptr;
extern Task *moveTask;
// extern Task *moveTask = nullptr;


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
    Coord g_velocity;   // global g_velocity stores x, y and angular velocities
    void wait_for_dist(double distance);

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



const int min_power_a = 18;
const int min_power_x = 35;
const int min_power_y = 12;


// extern bool lineup_first_default;
