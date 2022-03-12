#pragma once
#include "main.h"
#include "config.hpp"
#include "timer.hpp"
#include "geometry.hpp"
#include "logging.hpp"
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

#define waitUntil(condition) while(delay(10),!(condition))

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

#define inRange(value, min, max) ((min <= value && value <= max) || (max <= value && value <= min))


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
 * @brief format string to std::string
 * 
 * @param fmt format string
 * @param arg printf style args
 * @return std::string 
 */
std::string printf_to_string(const char* fmt, va_list arg);

/**
 * @brief outputs heap allocated string representing user friendly time
 * 
 * @param milliseconds 
 * @return char* const 
 */
char* const millis_to_str(std::uint32_t milliseconds);

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

//Assumes enum values are consecutive
template <typename T>
T operator++ (T enum_type, int) {
  return static_cast<T>(static_cast<int>(enum_type) + 1);
}

//Assumes enum values are consecutive
template <typename T>
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


//I'll get rid of this at some point. For now leave it

// template <typename R=std::nullptr_t>
// struct Value{
//   private:
//     std::function<R()> func;
    
//   public://Commented out some unused constructors
//     typedef R value_type;

//     //Blank (no value)
//     Value (){
//       func = nullptr;
//     }

//     //Var
//     template <typename = typename std::enable_if<!std::is_array_v<R> && !std::is_member_function_pointer_v<R>, void>>
//     Value (const R& val){
//       func = [&](){return val;};
//     }

//     //Array
//     template <typename I, typename = typename std::enable_if_t<std::is_scalar_v<I>, std::nullptr_t>>
//     Value (const R* array, const I& index){
//       // static_assert(std::is_scalar_v<I>, "Text Index type must be int-convertible");
//       func = [array, &index](){return array[static_cast<uint>(index)];};
//     }

//     //String
//     Value (std::string str){
//       func = [&](){return str.c_str();};
//     }

//     // //STL random access containers
//     // template <typename Container, typename I, typename = typename std::enable_if_t<std::is_base_of_v<typename std::random_access_iterator_tag, typename std::iterator_traits<typename Container::iterator>::iterator_category> && std::is_convertible_v<Container::size_type, int>, std::nullptr_t>>
//     // Value (Container& c, I& index){
//     //   // static_assert(std::is_scalar_v<typename Container::size_type>, "Text Index type must be int-convertible");
//     //   func = [&](){return c[static_cast<uint>(index)];};
//     // }

//     //STL random access associative containers
//     // template <typename Container, typename I, typename = typename std::enable_if_t<std::is_base_of_v<typename std::forward_iterator_tag, typename std::iterator_traits<typename Container::iterator>::iterator_category> && !std::is_base_of_v<typename std::random_access_iterator_tag, typename std::iterator_traits<typename Container::iterator>::iterator_category>, std::nullptr_t>>
//     // Value (Container& c, I& index){
//     //   func = [&](){return c[index];};
//     // }

//     //Function ptr with params
//     template <typename... Args>
//     Value (R (*const function)(Args...), Args... args){
//       func = std::bind(function, args...);
//     }

//     //std::function with params
//     template <typename... Args>
//     Value (std::function<R(Args...)> function, Args... args){
//       func = std::bind(function, args...);
//     }

//     // Member Functions
//     template <typename Class, typename... Args>
//     Value (R (Class::* const & function)(Args...), Class& object, Args... args){
//       func = std::bind(function, object, args...);
//     }

//     // Const Member Functions, because for some reason they can't be the same as normal member functions
//     template <typename Class, typename... Args>
//     Value (R (Class::* const & function)(Args...) const, Class& object, Args... args){
//       func = std::bind(function, object, args...);
//     }

//     explicit operator bool() const{
//       return bool(func);
//     }

//     R operator ()() const{
//       return func();
//     }

// };


// //void specialization
// template <>
// struct Value<void>{
//   private:
//     std::function<void()> func;
    
//   public:
//     typedef void value_type;

//     Value(){
//       func = [](){};
//     }

//     //Function ptr with params
//     template <typename... Args>
//     explicit Value (void (*const function)(Args...), Args... args){
//       func = std::bind(function, args...);
//     }

//     //std::function with params
//     template <typename... Args>
//     Value (std::function<void(Args...)> function, Args... args){
//       func = std::bind(function, args...);
//     }

//     // Member Functions
//     template <typename Class, typename... Args>
//     Value (void (Class::* const & function)(Args...), Class& object, Args... args){
//       func = std::bind(function, object, args...);
//     }

//     // Const Member Functions, because for some reason they can't be the same as normal member functions
//     template <typename Class, typename... Args>
//     Value (void (Class::* const & function)(Args...) const, Class& object, Args... args){
//       func = std::bind(function, object, args...);
//     }

//     explicit operator bool() const{
//       return bool(func);
//     }

//     void operator ()() const{
//       return func();
//     }

// };

// //Deduction Guides for Value struct
// Value() -> Value<std::nullptr_t>;

// template <typename Container, typename I>
// Value(Container&, I&) -> Value<typename Container::value_type>;

// template <typename Container, typename I>
// Value(Container&, I&) -> Value<typename Container::mapped_type>;