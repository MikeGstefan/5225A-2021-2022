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


double get_filtered_output(ADIUltrasonic sensor, int check_count, uint16_t lower_bound, uint16_t upper_bound, int timeout); // is blocking code


void flatten_against_wall(bool front = true);

void detect_goal();


//true is closed

// double get_front_dist();