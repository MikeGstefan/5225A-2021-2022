#pragma once
#include "../timer.hpp"

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
 * @param ... printf args
 * @return std::string 
 */
std::string printf_to_string(const char* fmt, ...);

const char* get_term_colour(term_colours colour);

void newline(int count = 1);

/**
 * @brief printf that handles strings and automatically newlines.
 * 
 * @param fmt printf format string
 * @param args printf args
 */
template <typename... Params>
void printf2(const char* fmt, Params... args){
  printf("%s\n", printf_to_string(fmt, screen::convert_args(args)...).c_str());
}

/**
 * @brief printf that handles strings and automatically newlines. Can print coloured and a timestamp
 * 
 * @param colour The colour to print in 
 * @param time_type:
 * 0: no timestamp
 * 1: 1500ms
 * 2: 1s 500ms
 * @param fmt printf format string
 * @param args printf args
 */
template <typename... Params>
void printf2(term_colours colour, int time_type, const char* fmt, Params... args){
  if(time_type) printf("%s | %s%s\033[0m\n", Timer::to_string(millis(), timing_units::millis, 0, time_type-1).c_str(), get_term_colour(colour), printf_to_string(fmt, screen::convert_args(args)...).c_str());
  else printf("%s%s\033[0m\n", get_term_colour(colour), printf_to_string(fmt, screen::convert_args(args)...).c_str());
}