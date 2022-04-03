#pragma once
#include "config.hpp"
#include "timer.hpp"
#include "drive.hpp"
#include "pid.hpp"
#include "util.hpp"

// Gyro Placement
// Gyro must be placed  vertically (port facing ground)
//These values are for it facing the outside


using namespace pros;



#define front_dist_dist 3.85


void f_claw(bool state);


double get_filtered_output(ADIUltrasonic sensor, int check_count, uint16_t lower_bound, uint16_t upper_bound, int timeout); // is blocking code


void flatten_against_wall(bool front = true, int cycles = 4);

void b_detect_goal();
void f_detect_goal(bool safety = true);

void detect_interference();

class Reset_dist{
    private: 
        static const int cycles = 3;
        static const int thresh = 10;
        const double dist_from_center;
        pros::Distance* sensor;

    public: 
    Reset_dist(pros::Distance* sensor, double dist_from_center);

    double get_dist();
};

extern Reset_dist reset_dist_r;
extern Reset_dist reset_dist_l;

