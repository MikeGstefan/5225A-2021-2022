#pragma once
#include "main.h"
#include "config.hpp"
#include <cmath>

void move(int x, int y, int a);
void move(double x, double y, double a);
void brake();

int sgn(int n);
int sgn(double n);

double operator "" _deg(long double degree);
double operator "" _rad(long double radians);

double rad_to_deg(double rad);
double deg_to_rad(double deg);

#pragma once
#include "main.h"
#include <iostream>
#include <cmath>
using namespace pros;

class Timer{

    uint32_t last_reset_time, time;
    const char* name;   // what to call the timer

public:

    Timer(const char* name);    // constructor takes in time
    uint32_t get_last_reset_time(); // get time of last reset
    void reset();   // resets the timer time
    uint32_t get_time();    // gets the time since last reset of timer
    void print();   // prints the current time of the timer
};
