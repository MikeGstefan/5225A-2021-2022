#pragma once
#include "main.h"
#include "../timer.hpp"
#include "../geometry.hpp"
#include <bitset>

using namespace pros;

/**
 * -1: no timestamp
 * 0: 1500 ms
 * 1: 1500 millis
 * 2: 1500 milliseconds
 * 3: 1 s 500 ms
 * 4: 1 sec 500 millis
 * 5: 1 second 500 milliseconds
 */
constexpr int default_time_fmt = 3;
constexpr int n_printf_max = 50;

//feel free to suggest shorter names for term_colours
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

//General case
template <typename T, typename = typename std::enable_if_t<!std::is_arithmetic_v<T>, void>> //Forces double/int overload instead
std::string convert_all_args(const std::string& fmt, const T& arg){
  char buffer[n_printf_max];
  snprintf(buffer, n_printf_max, fmt.c_str(), arg);
  return buffer;
}

//For arithmetic types
template <typename T, typename = typename std::enable_if_t<std::is_arithmetic_v<T>, void>>
std::string convert_all_args(const std::string& fmt, T arg){ //Not const T& because that duplicates against the non-arithmetic template overload
  const char* format = fmt.c_str();
  std::string fmt_safe = "   " + fmt;
  char buffer[n_printf_max];
  char end[3] = {static_cast<char>(tolower(*(fmt.end()-1))), static_cast<char>(*(fmt.end()-2)), static_cast<char>(*(fmt.end()-3))};

  if(end[1] == 'L') snprintf(buffer, n_printf_max, format, static_cast<long double>(arg));
  else if(end[0] == 'f' || end[0] == 'e' || end[0] == 'g' || end[0] == 'a') snprintf(buffer, n_printf_max, format, static_cast<double>(arg));
  else if(end[0] == 'd' || end[0] == 'i'){
    if(end[2] == 'h') snprintf(buffer, n_printf_max, format, static_cast<signed char>(arg));
    else if(end[2] == 'l') snprintf(buffer, n_printf_max, format, static_cast<long long int>(arg));
    else if(end[1] == 'h') snprintf(buffer, n_printf_max, format, static_cast<short int>(arg));
    else if(end[1] == 'l') snprintf(buffer, n_printf_max, format, static_cast<long int>(arg));
    else if(end[1] == 'j') snprintf(buffer, n_printf_max, format, static_cast<intmax_t>(arg));
    else snprintf(buffer, n_printf_max, format, static_cast<int>(arg));
  }
  else if(end[0] == 'u' || end[0] == 'o' || end[0] == 'x'){
    if(end[2] == 'h') snprintf(buffer, n_printf_max, format, static_cast<unsigned char>(arg));
    else if(end[2] == 'l') snprintf(buffer, n_printf_max, format, static_cast<unsigned long long int>(arg));
    else if(end[1] == 'h') snprintf(buffer, n_printf_max, format, static_cast<unsigned short int>(arg));
    else if(end[1] == 'l') snprintf(buffer, n_printf_max, format, static_cast<unsigned long int>(arg));
    else if(end[1] == 'j') snprintf(buffer, n_printf_max, format, static_cast<uintmax_t>(arg));
    else snprintf(buffer, n_printf_max, format, static_cast<unsigned int>(arg));
  }
  else if(end[0] == 'c'){
    if(end[1] == 'l') snprintf(buffer, n_printf_max, format, static_cast<wint_t>(arg));
    else snprintf(buffer, n_printf_max, format, static_cast<int>(arg));
  }
  else if(end[1] == 'z') snprintf(buffer, n_printf_max, format, static_cast<size_t>(arg));
  else if(end[1] == 't') snprintf(buffer, n_printf_max, format, static_cast<ptrdiff_t>(arg));

  else snprintf(buffer, n_printf_max, format, arg);
  return buffer;
}

//Vectors
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

//Arrays (C++)
template <typename _Tp, std::size_t _Nm>
std::string convert_all_args(const std::string& fmt, const std::array<_Tp, _Nm>& arg){
  if(fmt.back() == 'p') return convert_all_args(fmt, arg.data());
  else if(std::is_same<_Tp, char>::value && fmt.back() == 's') return convert_all_args(fmt, arg.data());
  std::string str;
	str += '{';
  for (typename std::array<_Tp, _Nm>::const_iterator it = arg.begin(); it != arg.end(); it++){
    str += convert_all_args(fmt, *it);
    if (it+1 != arg.end()) str += ", ";
  }
  str += '}';
  return str;
}

//Arrays (C)
template <typename _Tp, std::size_t _Nm>
std::string convert_all_args(const std::string& fmt, const _Tp (&arg)[_Nm]){
  if(fmt.back() == 'p') return convert_all_args(fmt, const_cast<_Tp*>(arg));
  else if(std::is_same<_Tp, char>::value && fmt.back() == 's') return convert_all_args(fmt, const_cast<_Tp*>(arg));
  std::string str;
	str += '{';
  for (int i = 0; i < _Nm; i++){
    str += convert_all_args(fmt, arg[i]);
    if (i+1 != _Nm) str += ", ";
  }
  str += '}';
  return str;
}

//Initializer lists
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

//Bitsets
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



//Template Recursion Base case
std::string sprintf2(const std::string& fmt);

/**
 * @brief Formats a printf-style function call into an std::string with more safety.
 * Calls itself recursively to handle all the arguments to the function.
 * @param fmt Format string
 * @param arg 1st argument
 * @param args Rest of the arguments
 * @return std::string
 */
//No support for %n format
template <typename Param, typename... Params>
std::string sprintf2(std::string fmt, const Param& arg, const Params&... args){
  std::string::const_iterator first = fmt.begin(), second;

  first = std::find(first, fmt.cend(), '%');
  second = fmt.begin() + fmt.find_first_of("diuoxXfFeEgGaAcspn%*", first-fmt.begin()+1) + 1;
  
  if(second == fmt.begin()) return fmt; //No valid format specifier found

  std::string start = std::string(fmt.cbegin(), first);
  std::string format_specifier = std::string(first, second);
  std::string converted;
  std::string rest = std::string(second, fmt.cend());

  if(format_specifier.back() == '%'){
    converted = "%";
    rest = sprintf2(rest, arg, args...);
  }
  else if(format_specifier.back() == '*'){
    format_specifier.pop_back();
    format_specifier += convert_all_args("%d", arg);
    rest = sprintf2(format_specifier + rest, args...);
  }
  else if(format_specifier.back() == 'n'){
    converted = "";
    // set_length(arg, start, converted);
    rest = sprintf2(rest, args...);
  }
  else{
    converted = convert_all_args(format_specifier, arg);
    rest = sprintf2(rest, args...);
  }
  
  return start + converted + rest;
}

/**
 * @brief printf that handles strings and automatically newlines. Can print coloured and a timestamp
 * 
 * @param colour The colour to print in 
 * @param fmt printf format string
 * @param args printf args
 */
template <typename... Params>
std::string sprintf2_format(term_colours colour, int time_type, std::string fmt, Params... args){
  std::string str = sprintf2(fmt, args...);
  std::string timestamp;

  int white_count = str.find_first_not_of(" \n");
  std::string whitespace = std::string(str.begin(), str.begin()+white_count);
  str.erase(str.begin(), str.begin()+white_count);

  if(time_type == -1) timestamp = "";
  else if(time_type == 0) timestamp = sprintf2("%-9s| ", Timer::to_string(millis(), timing_units::millis, 0, 0));
  else if(time_type == 1) timestamp = sprintf2("%-12s| ", Timer::to_string(millis(), timing_units::millis, 1, 0));
  else if(time_type == 2) timestamp = sprintf2("%-18s| ", Timer::to_string(millis(), timing_units::millis, 2, 0));
  else if(time_type == 3) timestamp = sprintf2("%-15s| ", Timer::to_string(millis(), timing_units::millis, 0, 1));
  else if(time_type == 4) timestamp = sprintf2("%-21s| ", Timer::to_string(millis(), timing_units::millis, 1, 1));
  else if(time_type == 5) timestamp = sprintf2("%-30s| ", Timer::to_string(millis(), timing_units::millis, 2, 1));

  return whitespace + timestamp + get_term_colour(colour) + str + get_term_colour(term_colours::NONE);
}

/**
 * @brief printf that handles strings and automatically newlines. Can print coloured and a timestamp
 * 
 * @param colour The colour to print in 
 * @param fmt printf format string
 * @param args printf args
 */
template <typename... Params>
std::string sprintf2_no_colour(int time_type, std::string fmt, Params... args){
  std::string str = sprintf2(fmt, args...);
  std::string timestamp;

  int white_count = str.find_first_not_of(" \n");
  std::string whitespace = std::string(str.begin(), str.begin()+white_count);
  str.erase(str.begin(), str.begin()+white_count);

  if(time_type == -1) timestamp = "";
  else if(time_type == 0) timestamp = sprintf2("%-9s| ", Timer::to_string(millis(), timing_units::millis, 0, 0));
  else if(time_type == 1) timestamp = sprintf2("%-12s| ", Timer::to_string(millis(), timing_units::millis, 1, 0));
  else if(time_type == 2) timestamp = sprintf2("%-18s| ", Timer::to_string(millis(), timing_units::millis, 2, 0));
  else if(time_type == 3) timestamp = sprintf2("%-15s| ", Timer::to_string(millis(), timing_units::millis, 0, 1));
  else if(time_type == 4) timestamp = sprintf2("%-21s| ", Timer::to_string(millis(), timing_units::millis, 1, 1));
  else if(time_type == 5) timestamp = sprintf2("%-30s| ", Timer::to_string(millis(), timing_units::millis, 2, 1));

  return whitespace + timestamp + str;
}

/**
 * @brief printf that handles all datatypes and automatically newlines. Can print coloured and a timestamp
 *
 * @param colour The colour to print in 
 * @param fmt printf format string
 * @param args printf args
 */
template <typename... Params>
int printf2(term_colours colour, int time_type, const char* fmt, Params... args){
  return printf("%s\n", sprintf2_format(colour, time_type, fmt, args...).c_str());
}

/**
 * @brief printf that handles all datatypes and automatically newlines. Can print coloured
 *
 * @param colour The colour to print in 
 * @param fmt printf format string
 * @param args printf args
 */
template <typename... Params>
int printf2(term_colours colour, std::string fmt, Params... args){
  return printf("%s\n", sprintf2_format(colour, default_time_fmt, fmt, args...).c_str());
}

/**
 * @brief printf that handles all datatypes and automatically newlines.
 * 
 * @param fmt printf format string
 * @param args printf args
 */
template <typename... Params>
int printf2(const char* fmt, Params... args){
  return printf("%s\n", sprintf2(fmt, args...).c_str());
}
