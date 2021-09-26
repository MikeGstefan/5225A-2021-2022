#pragma once
#include "main.h"
#include "config.hpp"
#include "timer.hpp"
#include <cmath>

using namespace pros;

#define waitUntil(condition) while(!(condition)) delay(10);

// cycle check macro (checks a condition for a specified amount of cycles)
#define cycleCheck(condition, checkCount, delayTime) \
{\
    int successCount = 0;\
    while (successCount < checkCount){\
        if (condition) successCount++;\
        else successCount = 0;\
        delay(delayTime);\
    }\
}

// enum class claw_state{in,out,neut};

// void set_claw_state(claw_state state);

int sgn(int n);
int sgn(double n);

double operator "" _deg(long double degree);
double operator "" _rad(long double radians);

double rad_to_deg(double rad);
double deg_to_rad(double deg);

struct Coord{   // stores members variables for a coordinate: x, y and angle    MUST BE DEINFED HERE, OTHERWISE CIRCULAR HEADER FILE DEPENDENCY OCCURS
    double x, y, angle;
    Coord(double x, double y, double angle);
    Coord();
    // default constructor
};
