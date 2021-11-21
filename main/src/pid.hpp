#pragma once
#include "main.h"
#include "config.hpp"
#include "util.hpp"
#include "Tracking.hpp"
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include <array>

using namespace pros;

class PID{
    Timer last_update_timer{"last_update_timer"};
    bool integral_sgn_reset; // if the integral should when the sgn fo the error flips
    uint8_t last_error_sgn;   // used if user wants to reset the integral after the sgn changes
    double error, last_error, kP, kI, kD, proportional, integral, integral_lower_bound, integral_upper_bound, derivative, bias, output;

public:
    PID(double kP, double kI, double kD, double bias, bool integralSgnReset = true, double integralLowerBound = 0, double integralUpperBound = 1000000);
    double get_error() const;
    double get_output() const;
    double compute(double input, double target);

};
