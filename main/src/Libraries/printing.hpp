#pragma once
#include "../timer.hpp"
#include <iostream>
#include <limits>
#include <cstdarg>

//feel free to rename this to something shorter
enum class term_colours{
  BLACK,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  WHITE,
  NONE,
  ERROR,
  WARNING,
  NOTIF,
};

/**
 * @brief format string to std::string
 * 
 * @param fmt format string
 * @param arg variadic args list
 * @return std::string 
 */
std::string printf_to_string(const char* fmt, va_list args);

/**
 * @brief format string to std::string
 * 
 * @param fmt format string
 * @param ... printf args
 * @return std::string 
 */
std::string printf_to_string(const char* fmt, ...);

const char* get_term_colour(term_colours colour);

template <typename... Params>
void printf2(const char* text, Params... args){
  printf("%s\n", printf_to_string(text, screen::convert_args(args)...).c_str());
}

template <typename... Params>
void printf2(term_colours colour, int time_type, const char* text, Params... args){
  if(time_type) printf("%s | %s%s\033[0m\n", Timer::to_string(millis(), timing_units::millis, 0, time_type-1).c_str(), get_term_colour(colour), printf_to_string(text, screen::convert_args(args)...).c_str());
  else printf("%s%s\033[0m\n", get_term_colour(colour), printf_to_string(text, screen::convert_args(args)...).c_str());
}