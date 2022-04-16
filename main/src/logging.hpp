#pragma once
#include "util.hpp"
#include "Libraries/printing.hpp"
#include "task.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cstdarg>
#include <fstream>
#include <functional>

using namespace std;
using namespace pros;
//forawrd declarations
class _Task;
class Timer;
class Data;
extern _Task log_t;
/*
1 is print, if there is no sd card, print to terminal
2 is print. if there is no sd card, ignore
0 is off
*/
enum log_types{
  error = 1,
  warning =1,
  general =1,
  debug = 2,
  off = 0,
};

enum class log_locations
{
  t,
  sd,
  both,
  none
};

extern Data task_log;
extern Data controller_queue;
extern Data tracking_data;
extern Data tracking_imp;
extern Data misc;
extern Data drivers_data;
extern Data term;
extern Data motion_d;
extern Data motion_i;
extern Data log_d;
extern Data events;
extern Data graph;
extern Data state_log;
extern Data skills_d;
extern Data ERROR;

class Data{
private:
  static vector<Data*> obj_list;
  term_colours print_colour;
  int print_type;
  
public:
  static _Task log_t;

  const char* id;
  const char* name;
  log_types log_type;
  log_locations log_location;
  static vector<Data*> get_objs();

  void print(Timer* tmr,int freq, vector<function<char*()>> str) const; //How is this used?
  void log_print(char* buffer, int buffer_len) const;
  void set_print(term_colours print_colour, int time_type);

  Data(const char* obj_name, const char* id_code, log_types log_type_param, log_locations log_location_param, term_colours print_colour = term_colours::NONE, int print_type = 2);
  
  static void init();
  static char* to_char(const char* format, ...);

  //figure out how to put nice timestamps for file logs

  /**
   * @brief 
   * 
   * @param colour term_colour to print in
   * @param format printf format string
   * @param args printf args
   */
  template <typename... Params>
  void print(term_colours colour, std::string format, Params... args) const{
    std::string str = sprintf2(format, args...);
    str = std::to_string(millis()) + " | ";

    int buffer_len = str.length() + 3;
    char buffer[256];
    snprintf(buffer, 256, str.c_str());

    if(static_cast<int>(this->log_type) != 0){
      switch(log_location){
        case log_locations::t:
          printf2(colour, print_type, format, args...);
          break;
        case log_locations::sd:
          this->log_print(buffer, buffer_len);
          break;
        case log_locations::none:
          break;
        case log_locations::both:
          printf2(colour, print_type, format, args...);
          this->log_print(buffer, buffer_len);
          break;
      }
    }
  }


  /**
   * @brief 
   * 
   * @param format printf format string
   * @param args printf args
   */
  template <typename... Params>
  void print(std::string format, Params... args) const{
    print(print_colour, format, args...);
  }

};


void queue_handle(void* params);
uintptr_t data_size();
constexpr int queue_size = 1024;
constexpr int print_point = 500;
constexpr int print_max_time = 1500;
