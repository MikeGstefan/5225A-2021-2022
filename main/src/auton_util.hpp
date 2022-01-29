#pragma once
#include "config.hpp"
#include "timer.hpp"
#include "drive.hpp"
#include "pid.hpp"
#include "util.hpp"

// Gyro Placement
// Gyro must be placed  vertically (port facing ground)
//These values are for it facing the outside

// Right: get_roll, -1
// Left: get_roll, 1
// Back: get_pitch, 1
// Front: get_pitch, -1

#define GYRO_AXIS get_pitch
#define GYRO_SIDE -1


double get_filtered_output(ADIUltrasonic sensor, int check_count, uint16_t lower_bound, uint16_t upper_bound, int timeout); // is blocking code


void flatten_against_wall(bool right);

void score_on_top(void* params);  // scores on tall goal

class Gyro{
  private:
    Imu& inertial;
    double angle;

  public:
    Gyro(Imu& imu);

    void calibrate();
    void finish_calibrating();
    void climb_ramp();
    void level(double kP, double kD);

    double get_angle();
};

extern Gyro gyro;
