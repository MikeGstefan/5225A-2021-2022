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
