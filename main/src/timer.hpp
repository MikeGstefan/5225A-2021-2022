#pragma once
// #include "common.hpp"
#include "main.h"

enum class timing_units{
    millis,
    micros
};

class Timer{
    // 'time' is the time counted by the timer until the last call of pause()
    uint64_t last_reset_time, time, last_play_time;
    const char* name;
    bool paused;    // state of timer
    timing_units timing_unit;
    uint64_t get_time_in_timing_unit(); // returns time in either millis micros

public:

    Timer(const char* name, const bool& play = true, timing_units timing_unit = timing_units::millis);
    uint64_t get_last_reset_time();
    void reset(const bool& play = true);
    uint64_t get_time();
    void play();
    void pause();
    void print(const char* str = "");
    bool playing();
};
