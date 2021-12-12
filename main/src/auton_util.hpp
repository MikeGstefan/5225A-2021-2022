#pragma once
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

bool controller_interrupt(bool analog=true, bool digital=true);

class Gyro{
  private:
    Imu* inertial;
    double angle;

  public:
    Gyro(Imu* imu);

    void calibrate();
    void finish_calibrating();
    void climb_ramp();
    void level(double kP, double kD);

    double get_angle();
};

extern Gyro gyro;
