#pragma once
#include "main.h"
#include "config.hpp"
#include "timer.hpp"
#include "geometry.hpp"
#include "logging.hpp"
// #include "../include/okapi/api/util/mathUtil.hpp" // bool2sgn, motor cartridge colours, adi letter to number...
#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

using namespace pros;
using namespace std;
//forward declarations
class Timer;

#define wait_until(condition) while(delay(10), !(condition))

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

/**
 * @brief checks whether a value is in range
 * 
 * @param value value to be checked
 * @param minimum range minimum value
 * @param maximum range maximum value
 * @return whether value is between minimum and maximum. will work even if maximum < minimum
 */
template <typename T>
bool in_range(T value, T minimum, T maximum){
  return (minimum <= value && value <= maximum) || (maximum <= value && value <= minimum);
}

double operator "" _deg(long double degree);
double operator "" _rad(long double radians);

/**
 * @brief converts radians to degrees
 * 
 * @param rad 
 * @return degrees
 */
double rad_to_deg(double rad);

/**
 * @brief converts degrees to radians
 * 
 * @param deg 
 * @return radians
 */
double deg_to_rad(double deg);

/**
 * @brief returns the closest equivalent angle to refrence in radians
 * 
 * @param angle 
 * @param reference 
 * @return double 
 */
double near_angle(double angle, double reference);

/**
 * @brief 
 * 
 * @param f base function to scale
 * @param p1 1st point to pass through
 * @param p2 2nd point to pass through. Defaults to (0, 0)
 * @param control Control value
 * @return std::function<long double (long double)> 
 */
//Scales a function using two points to pass through and a control value

//Not a template because that won't allow implicit lambda
std::function<long double (long double)> func_scale(std::function<long double(long double)> f, Point p1, Point p2 = {0.0, 0.0}, double control = 1.0);

//std::clamp already exists
// restricts a value to a range
template <typename T>
T constrain(T value, T min, T max){
  return value > min ? value < max ? value : max : min;
}

// gets the sign of a value (0, 1 or -1)
template <typename T>
int sgn(T value){
  return (T(0) < value) - (value < T(0));
}

template <typename T>
bool contains(T& container, typename T::value_type item){
  return std::find(container.begin(), container.end(), item) != container.end();
}

double weighted_avg(double first, double second, double first_scale = 0.5);

//please rename map. With proper includes it conflicts with std::map
// maps a value to a range
template <typename T>
T map(T x, T in_min, T in_max, T out_min, T out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// base case for recrusive function map_set
template <typename T>
T map_set(T input, T in_min, T in_max, T out_min, T out_max, T range, T val) {
  if (input <= range) return map(input, in_min, range, out_min, val);
  else {
    printf("%d || INVALID INPUT IN MAP FUNCTION\n", millis());
    return static_cast<T>(0);
  }
}
// maps a values to a set of maps (a piecewise function)
template <typename T, typename... Ts>
T map_set(T input, T in_min, T in_max, T out_min, T out_max, T range1, T val_1, Ts... args) {
  if (input <= range1) return map(input, in_min, range1, out_min, val_1);
  else return map_set(input, range1, in_max, val_1, out_max, args...);
}

//ENUM HELPERS

//Assumes enum values are consecutive
template <typename T, typename = typename std::enable_if_t<std::is_enum_v<T>, void>>
T operator++ (T enum_type, int) {
  return static_cast<T>(static_cast<int>(enum_type) + 1);
}

//Assumes enum values are consecutive
template <typename T, typename = typename std::enable_if_t<std::is_enum_v<T>, void>>
T operator-- (T enum_type, int) {
  return static_cast<T>(static_cast<int>(enum_type) - 1);
}

template <typename T, typename = typename std::enable_if_t<std::is_enum_v<T>, void>>
T next_enum_value(T enum_type) {
  if (static_cast<int>(enum_type) < static_cast<int>(T::NUM_OF_ELEMENTS) - 1) return enum_type++;
  else return static_cast<T>(0);
}

template <typename T, typename = typename std::enable_if_t<std::is_enum_v<T>, void>>
T previous_enum_value(T enum_type) {
  if (static_cast<int>(enum_type) > static_cast<int>(static_cast<T>(0))) return enum_type--;
  else return static_cast<T>(static_cast<int>(T::NUM_OF_ELEMENTS) - 1);
}
