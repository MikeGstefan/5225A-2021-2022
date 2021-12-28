#pragma once
#include "config.hpp"
#include "timer.hpp"


double get_filtered_output(ADIUltrasonic sensor, int check_count, uint16_t lower_bound, uint16_t upper_bound, int timeout); // is blocking code
