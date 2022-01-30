// NEW CONFIG
#pragma once
#include "main.h"
#include "controller.hpp"

class _Controller;
class Text;

extern _Controller master;
extern pros::Motor front_l, front_r, back_l, back_r;
extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;

extern pros::Imu imu_sensor;

