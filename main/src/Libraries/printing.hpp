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


template <typename T, typename = typename std::enable_if_t<!(std::is_integral_v<T> || std::is_floating_point_v<T>), void>> //Forces double/int overload instead
std::string convert_all_args(const std::string& fmt, const T& arg){
  char buffer[150];
  snprintf(buffer, 150, fmt.c_str(), arg);
  return buffer;
}

template <typename T, typename = typename std::enable_if_t<std::is_floating_point_v<T> || std::is_integral_v<T>, void>>
std::string convert_all_args(const std::string& fmt, T arg){
  const char* format = fmt.c_str();
  std::string fmt_safe = "   " + fmt;
  char buffer[150];
  char end[3] = {static_cast<char>(tolower(*(fmt.end()-1))), static_cast<char>(*(fmt.end()-2)), static_cast<char>(*(fmt.end()-3))};

  if(end[1] == 'L') snprintf(buffer, 150, format, static_cast<long double>(arg));
  else if(end[0] == 'f' || end[0] == 'e' || end[0] == 'g' || end[0] == 'a') snprintf(buffer, 150, format, static_cast<double>(arg));
  else if(end[0] == 'd' || end[0] == 'i'){
    if(end[2] == 'h') snprintf(buffer, 150, format, static_cast<signed char>(arg));
    else if(end[2] == 'l') snprintf(buffer, 150, format, static_cast<long long int>(arg));
    else if(end[1] == 'h') snprintf(buffer, 150, format, static_cast<short int>(arg));
    else if(end[1] == 'l') snprintf(buffer, 150, format, static_cast<long int>(arg));
    else if(end[1] == 'j') snprintf(buffer, 150, format, static_cast<intmax_t>(arg));
    else snprintf(buffer, 150, format, static_cast<int>(arg));
  }
  else if(end[0] == 'u' || end[0] == 'o' || end[0] == 'x'){
    if(end[2] == 'h') snprintf(buffer, 150, format, static_cast<unsigned char>(arg));
    else if(end[2] == 'l') snprintf(buffer, 150, format, static_cast<unsigned long long int>(arg));
    else if(end[1] == 'h') snprintf(buffer, 150, format, static_cast<unsigned short int>(arg));
    else if(end[1] == 'l') snprintf(buffer, 150, format, static_cast<unsigned long int>(arg));
    else if(end[1] == 'j') snprintf(buffer, 150, format, static_cast<uintmax_t>(arg));
    else snprintf(buffer, 150, format, static_cast<unsigned int>(arg));
  }
  else if(end[1] == 'z') snprintf(buffer, 150, format, static_cast<size_t>(arg));
  else if(end[1] == 't') snprintf(buffer, 150, format, static_cast<ptrdiff_t>(arg));

  else snprintf(buffer, 150, format, arg);
  return buffer;
}

template <typename _Tp>
std::string convert_all_args(const std::string& fmt, const std::vector<_Tp>& arg){
  if(fmt.back() == 'p') return convert_all_args(fmt, arg.data());
  std::string str;
	str += '{';
  for (typename std::vector<_Tp>::const_iterator it = arg.begin(); it != arg.end(); it++){
    str += convert_all_args(fmt, *it);
    if (it+1 != arg.end()) str += ", ";
  }
  str += '}';
  return str;
}

template <typename _Tp, std::size_t _Nm>
std::string convert_all_args(const std::string& fmt, const std::array<_Tp, _Nm>& arg){
  if(fmt.back() == 'p') return convert_all_args(fmt, arg.data());
  std::string str;
	str += '{';
  for (typename std::array<_Tp, _Nm>::const_iterator it = arg.begin(); it != arg.end(); it++){
    str += convert_all_args(fmt, *it);
    if (it+1 != arg.end()) str += ", ";
  }
  str += '}';
  return str;
}

template <typename _Tp>
std::string convert_all_args(const std::string& fmt, const std::initializer_list<_Tp>& arg){
  std::string str;
	str += '{';
  for (typename std::initializer_list<_Tp>::const_iterator it = arg.begin(); it != arg.end(); it++){
    str += convert_all_args(fmt, *it);
    if (it+1 != arg.end()) str += ", ";
  }
  str += '}';
  return str;
}

template <typename _Tp, std::size_t _Nm>
std::string convert_all_args(const std::string& fmt, const _Tp (&arg)[_Nm]){
  if(fmt.back() == 'p') return convert_all_args(fmt, const_cast<_Tp*>(arg));
  std::string str;
	str += '{';
  for (int i = 0; i < _Nm; i++){
    str += convert_all_args(fmt, arg[i]);
    if (i+1 != _Nm) str += ", ";
  }
  str += '}';
  return str;
}

template <size_t _Nb>
std::string convert_all_args(const std::string& fmt, const std::bitset<_Nb>& arg){
  std::string str;
  str += '{';
  if(fmt.back() == 's') str += arg.to_string();
  else{
    for(int i = 0; i < arg.size(); i++){
      str += convert_all_args(fmt, arg[i]);
      if (i+1 != arg.size()) str += ", ";
    }
  }
  str += '}';
  return str;
}

template <typename... Args>
std::string convert_all_args(const std::string& fmt, const std::tuple<Args...>& arg){
  std::string str;
  str += '{';
  for(int i = 0; i < std::tuple_size<std::tuple<Args...>>(); i++){
    str += convert_all_args(fmt, std::get<i>(arg));
  }
  str += '}';
  return str;
}

//Non templated versions
std::string convert_all_args(const std::string& fmt, const std::string& arg);
std::string convert_all_args(const std::string& fmt, const Position& arg);
std::string convert_all_args(const std::string& fmt, const Point& arg);


//For %n, doesn't work yet.
template <typename T, typename = typename std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value, void>>
void set_length(T* ptr, const std::string& start, const std::string& converted){
  *ptr = start.length() + converted.length();
}

template <typename T>
void set_length(T ptr, const std::string& start, const std::string& converted){
  //Only exists for compile time overload resolution. Should never be called
  throw std::invalid_argument("Cannot use \%n on non signed integer * type.");
}

/**
 * @brief Formats a printf-style function call into an std::string with more safety
 * Calls itself recurrsively to handle all the arguments to the function
 * @param fmt Format string
 * @param arg 1st argument
 * @param args Rest of the arguments
 * @return std::string
 */
//No support for %n, *, and .* formats
template <typename Param, typename... Params>
std::string printf_to_string(std::string fmt, const Param& arg, const Params&... args){
  std::string::iterator first = fmt.begin(), second;

  first = std::find(first, fmt.end(), '%');
  second = fmt.begin() + fmt.find_first_of("diuoxXfFeEgGaAcspn%", first-fmt.begin()+1) + 1;
  
  if(second == fmt.begin()) return fmt; //No valid format specifier found

  std::string start = std::string(fmt.begin(), first);
  std::string format_specifier = std::string(first, second);
  std::string converted;
  std::string rest = std::string(second, fmt.end());

  if(format_specifier.back() == '%'){
    converted = "%";
    rest = printf_to_string(rest, arg, args...);
  }
  else if(format_specifier.back() == 'n'){
    converted = "";
    printf("S:%s, C:%s\n", start.c_str(), converted.c_str());
    set_length(arg, start, converted);
    rest = printf_to_string(rest, args...);
  }
  else{
    converted = convert_all_args(format_specifier, arg);
    rest = printf_to_string(rest, args...);
  }
  
  return start + converted + rest;
}

//Template Recursion Base case
std::string printf_to_string(const std::string& fmt);

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