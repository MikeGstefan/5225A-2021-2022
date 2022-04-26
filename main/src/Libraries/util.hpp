#pragma once
#include "main.h"
#include "geometry.hpp"
#include "printing.hpp"

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

/**
 * @brief converts radians to degrees
 * 
 * @param rad 
 * @return degrees
 */
constexpr double rad_to_deg(double rad){
  return rad / M_PI * 180.0;
}

/**
 * @brief converts degrees to radians
 * 
 * @param deg 
 * @return radians
 */
constexpr double deg_to_rad(double deg){
  return deg / 180.0 * M_PI;
}


//degrees to radians
constexpr double operator "" _deg(long double degree){
  return deg_to_rad(degree);
}

//radians to degrees
constexpr double operator "" _rad(long double radians){
  return rad_to_deg(radians);
}

//rotations to radians
constexpr double operator "" _rot(long double rotations){
  return rotations * M_TWOPI;
}

/**
 * @brief returns the closest equivalent angle to refrence in radians
 * 
 * @param angle 
 * @param reference 
 * @return double 
 */
constexpr double near_angle(double angle, double reference){
	return round((reference - angle)/(M_TWOPI)) * (M_TWOPI) + angle - reference;
}

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

//removed constrain because std::clamp already exists

/**
 * @brief returns a random bool with probability of being true
 * 
 * @param ratio ratio:1 of getting true. 1:1 is 50/50. 2:1 means true is twice as likely than false.
 * @return true
 */
bool random_bool(int ratio = 1);

int random_direction();

constexpr int bool2sgn(bool boolean){
  return boolean ? 1 : -1;
}
constexpr bool sgn2bool(int sgn){
  return sgn > 0;
}

// gets the sign of a value (0, 1 or -1)
template <typename T>
constexpr int sgn(T value){
  return (T(0) < value) - (value < T(0));
}

template <typename T>
constexpr bool contains(T& container, typename T::value_type item){
  return std::find(container.begin(), container.end(), item) != container.end();
}

/**
 * @brief Returns a scaled average between two numbers.
 * 
 * @param first The first number.
 * @param second The second number.
 * @param first_scale Percentage to scale first number by. (20% is 0.2)
 * @return double 
 */
constexpr double weighted_avg(double first, double second, double first_scale = 0.5){
  return first*first_scale + second*(1-first_scale);
}

//please rename map. With proper includes it conflicts with std::map
// maps a value to a range
template <typename T>
constexpr T map(T x, T in_min, T in_max, T out_min, T out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// base case for recursive function map_set
template <typename T>
constexpr T map_set(T input, T in_min, T in_max, T out_min, T out_max, T range, T val){
  if (input <= range) return map(input, in_min, range, out_min, val);
  else {
    printf2("INVALID INPUT IN MAP FUNCTION");
    return static_cast<T>(0);
  }
}
// maps a values to a set of maps (a piecewise function)
template <typename T, typename... Ts>
constexpr T map_set(T input, T in_min, T in_max, T out_min, T out_max, T range1, T val_1, Ts... args){
  if (input <= range1) return map(input, in_min, range1, out_min, val_1);
  else return map_set(input, range1, in_max, val_1, out_max, args...);
}

//ENUM HELPERS
//Assumes enum values are consecutive

template <typename E, typename = typename std::enable_if_t<std::is_enum_v<E>, void>>
constexpr E next_enum_value(E enum_value){
  int value = static_cast<int>(enum_value);

  if (value < static_cast<int>(E::NUM_OF_ELEMENTS) - 1) value++;
  else value = 0;

  return static_cast<E>(value);
}

template <typename E, typename = typename std::enable_if_t<std::is_enum_v<E>, void>>
constexpr E previous_enum_value(E enum_value){
  int value = static_cast<int>(enum_value);
  
  if (value > 0) value--;
  else value = static_cast<int>(E::NUM_OF_ELEMENTS) - 1;

  return static_cast<E>(value);
}

//prefix increment
template <typename E, typename = typename std::enable_if_t<std::is_enum_v<E>, void>>
constexpr E& operator++ (E& enum_value){
  enum_value = next_enum_value(enum_value);
  return enum_value;
}

//prefix decrement
template <typename E, typename = typename std::enable_if_t<std::is_enum_v<E>, void>>
constexpr E& operator-- (E& enum_value){
  enum_value = previous_enum_value(enum_value);
  return enum_value;
}

//postfix increment
template <typename E, typename = typename std::enable_if_t<std::is_enum_v<E>, void>>
constexpr E operator++ (E& enum_value, int){
  E old = enum_value;
  ++enum_value;
  return old;
}

//postfix decrement
template <typename E, typename = typename std::enable_if_t<std::is_enum_v<E>, void>>
constexpr E operator-- (E& enum_value, int){
  E old = enum_value;
  --enum_value;
  return old;
}