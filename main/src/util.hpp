#pragma once
#include "main.h"
#include "config.hpp"
#include "timer.hpp"
#include <cmath>

using namespace pros;

extern Timer screen_timer;  // timer to measure how long it's been since last screen update

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

double operator "" _deg(long double degree);
double operator "" _rad(long double radians);

double rad_to_deg(double rad);
double deg_to_rad(double deg);

double near_angle(double angle, double reference);

template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

template <typename T> T map(T x, T in_min, T in_max, T out_min, T out_max) { return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; }

template <typename T>
T map_set(T input, T in_min, T in_max, T out_min, T out_max, T range, T val) {
  if (input <= range) return map(input, in_min, range, out_min, val);
}

template <typename T, typename... Ts>
T map_set(T input, T in_min, T in_max, T out_min, T out_max, T range1, T val_1, Ts... args) {
  if (input <= range1) return map(input, in_min, range1, out_min, val_1);
  else return map_set(input, range1, in_max, val_1, out_max, args...);
}

template <typename T> T operator++ (T enum_type, int) { return static_cast<T>(static_cast<int>(enum_type) + 1); }
template <typename T> T operator-- (T enum_type, int) { return static_cast<T>(static_cast<int>(enum_type) - 1); }


template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, void>::type> T next_enum_value(T enum_type) {
  if (static_cast<int>(enum_type) < static_cast<int>(T::NUM_OF_ELEMENTS) - 1) return enum_type++;
  else return static_cast<T>(0);
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, void>::type> T previous_enum_value(T enum_type) {
  if (static_cast<int>(enum_type) > static_cast<int>(static_cast<T>(0))) return enum_type--;
  else return static_cast<T>(static_cast<int>(T::NUM_OF_ELEMENTS) - 1);
}
