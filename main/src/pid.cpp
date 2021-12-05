#include "pid.hpp"

PID::PID(double kP, double kI, double kD, double bias, bool integral_sgn_reset, double integral_lower_bound, double integral_upper_bound):
    kP(kP), kI(kI), kD(kD),
    bias(bias),
    integral_sgn_reset(integral_sgn_reset),
    integral_upper_bound(integral_upper_bound), integral_lower_bound(integral_lower_bound)
{}

double PID::get_error() const {
    return error;
}

double PID::get_output() const {
    return output;
}

double PID::get_proportional() const{
  return proportional;
}

double PID::compute(double input, double target){

    error = target - input;

    proportional = error * kP;

    if ((kI || kD) && last_update_timer.get_time() > 0.0){  // if only P is activated, don't compute I and D-related variables
        // resets integral if sign of error flips and user enabled this feature, or error is out of bounds
        if ((integral_sgn_reset && sgn(error) != last_error_sgn) || (fabs(error) < integral_lower_bound && fabs(error) > integral_upper_bound)){
          integral = 0;
          // printf("INTEGRAL RESET ***********************\n");
        }
        else    integral += error * last_update_timer.get_time() * kI;

        derivative = (error - last_error) / last_update_timer.get_time() * kD;

        // updates variables for next iteration of loop
        last_update_timer.reset();
        last_error_sgn = sgn(error);
        last_error = error;
    }
    output = proportional + integral + derivative + bias;
    return output;
}
