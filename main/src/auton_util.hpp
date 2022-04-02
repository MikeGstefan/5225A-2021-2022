#pragma once
#include "main.h"
#include "geometry.hpp"
#include "config.hpp"
#include "Libraries/gui.hpp"
#include "drive.hpp"
#include "Tracking.hpp"

void save_positions();
void load_positions();



#define front_dist_dist 3.85


double get_filtered_output(ADIUltrasonic sensor, int check_count, uint16_t lower_bound, uint16_t upper_bound, int timeout); // is blocking code

void flatten_against_wall(bool front = true, int cycles = 4);

void b_detect_goal();
void f_detect_goal(bool safety = true);

void detect_interference();

class Reset_dist{
  private:
    static constexpr int cycles = 3;
    static constexpr int thresh = 10;
    const double dist_from_center;
    pros::Distance* sensor;

  public:
    Reset_dist(pros::Distance* sensor, double dist_from_center);

    double get_dist();
};

extern Reset_dist reset_dist_r;
extern Reset_dist reset_dist_l;

