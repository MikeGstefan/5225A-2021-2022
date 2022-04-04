#pragma once
#include "main.h"
#include "../timer.hpp"
#include <bitset>

//feel free to rename term_colours to something shorter
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
 * @brief Converts an argument to a c-safe alternative for printing (usually just const char*)
 * 
 * @tparam T the type of the argument to convert (passed as const&)
 * @param arg Argument to convert
 * @return T Usually const char*
 */
template <typename T>
T convert_all_args(const T& arg){
	return arg;
}

const char* convert_all_args(const std::string& arg);
const char* convert_all_args(const Position& arg);
const char* convert_all_args(const Point& arg);

template <typename _Tp>
const char* convert_all_args(const std::vector<_Tp>& arg){
  std::string str;
	str += '{';
  for (typename std::vector<_Tp>::const_iterator it = arg.begin(); it != arg.end(); it++){
    str += convert_all_args(*it);
    if (it+1 != arg.end()) str += ", ";
  }
  str += '}';
  return convert_all_args(str);
}

template <typename _Tp, std::size_t _Nm>
const char* convert_all_args(const std::array<_Tp, _Nm>& arg){
  std::string str;
	str += '{';
  for (typename std::array<_Tp, _Nm>::const_iterator it = arg.begin(); it != arg.end(); it++){
    str += convert_all_args(*it);
    if (it+1 != arg.end()) str += ", ";
  }
  str += '}';
  return convert_all_args(str);
}

template <typename _Tp>
const char* convert_all_args(const std::initializer_list<_Tp>& arg){
  std::string str;
	str += '{';
  for (typename std::vector<_Tp>::const_iterator it = arg.begin(); it != arg.end(); it++){
    str += convert_all_args(*it);
    if (it+1 != arg.end()) str += ", ";
  }
  str += '}';
  return convert_all_args(str);
}

template <typename _Tp, std::size_t _Nm>
const char* convert_all_args(const _Tp (&arg)[_Nm]){
  std::string str;
	str += '{';
  for (int i = 0; i < _Nm; i++){
    str += convert_all_args(arg[i]);
    if (i+1 != _Nm) str += ", ";
  }
  str += '}';
  return convert_all_args(str);
}

template <size_t _Nb>
const char* convert_all_args(const std::bitset<_Nb>& arg){
  std::string str;
  str += '{';
	str += arg.to_string();
  str += '}';
  return convert_all_args(str);
}

template <typename... Args>
const char* convert_all_args(const std::tuple<Args...>& arg){
  std::string str;
  str += '{';
  for(int i = 0; i < std::tuple_size<Args...>(); i++){ //might need std::tuple_size<std::tuple<Args...>>
    str += convert_all_args(std::get<i>(arg));
  }
  str += '}';
  return convert_all_args(str);
}


static std::string printf_to_string_helper(const char* fmt, ...){
  char buffer[150];

  std::va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, 150, fmt, args);
  va_end(args);
  
  return buffer;
}

/**
 * @brief
 * 
 * @tparam Params 
 * @param fmt 
 * @param args 
 * @return std::string 
 */
template <typename... Params>
std::string printf_to_string(const char* fmt, Params... args){
  return printf_to_string_helper(fmt, convert_all_args(args)...);
}

/**
 * @brief Get the terminal control string to modify its printing colour 
 * 
 * @param colour the colour to use
 * @return const char* 
 */
const char* get_term_colour(term_colours colour);

/**
 * @brief prints a new line to the terminal
 * 
 * @param count how many lines to print
 */
void newline(int count = 1);

/**
 * @brief printf that handles strings and automatically newlines.
 * 
 * @param fmt printf format string
 * @param args printf args
 */
template <typename... Params>
void printf2(const char* fmt, Params... args){
  printf("%s\n", printf_to_string(fmt, args...).c_str());
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
  if(time_type) printf("%s | %s%s\033[0m\n", Timer::to_string(millis(), timing_units::millis, 0, time_type-1).c_str(), get_term_colour(colour), printf_to_string(fmt, args...).c_str());
  else printf("%s%s\033[0m\n", get_term_colour(colour), printf_to_string(fmt, args...).c_str());
}