#pragma once
#include "main.h"
#include "Libraries/task.hpp"

void
  save_positions(),
  load_positions(),

  f_claw(bool state),

  flatten_against_wall(bool front = true, int cycles = 4), //param cycles is unuse;

  b_detect_goal(),
  tilt_goal(),
  f_detect_goal(bool safety = true),

  detect_interference();


double get_filtered_output(ADIUltrasonic sensor, int check_count, uint16_t lower_bound, uint16_t upper_bound, int timeout); //is blocking code


class Reset_dist{
  private:
    static constexpr int cycles = 3;
    static constexpr int thresh = 10;
    const double dist_from_center;
    Distance* sensor;

  public:
    Reset_dist(Distance* sensor, double dist_from_center);

    double get_dist();
};

extern Reset_dist reset_dist_r, reset_dist_l;


void subsystem_handle_t(void* params);
inline _Task lift_handle_t(subsystem_handle_t, "LIFT task");