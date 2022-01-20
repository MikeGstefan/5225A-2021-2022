#pragma once
#include "config.hpp"
#include "timer.hpp"
#include "drive.hpp"


#define front_dist_dist 3.85


double get_filtered_output(ADIUltrasonic sensor, int check_count, uint16_t lower_bound, uint16_t upper_bound, int timeout); // is blocking code


void flatten_against_wall(bool right);


//true is closed
void claw_set_state(bool state);

void find_goal_lift(bool move_stop_b = true);

double get_front_dist();