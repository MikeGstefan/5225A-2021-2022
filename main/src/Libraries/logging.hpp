#pragma once
#include "main.h"
#include "printing.hpp"

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
  warning = 1,
  general = 1,
  debug = 2,
  off = 0,
};

enum class log_locations{
  t,
  sd,
  both,
  none
};

extern Data
  task_log,
  controller_queue,
  tracking_data,
  tracking_imp,
  misc,
  drivers_data,
  term,
  motion_d,
  motion_i,
  log_d,
  graph,
  state_log,
  skills_d,
  ERROR,
  safety;

class Data{
private:
  static std::vector<Data*> obj_list;
  term_colours print_colour;
  int print_type;
  
public:
  static _Task log_t;

  std::string id;
  std::string name;
  log_types log_type;
  log_locations log_location;
  static std::vector<Data*> get_objs();

  void log_print(char* buffer, int buffer_len) const;
  void set_print(term_colours print_colour, int time_type);

  Data(std::string obj_name, int id_code, log_types log_type_param, log_locations log_location_param, term_colours print_colour = term_colours::NONE, int print_type = default_time_fmt);
  
  static void init();

  /**
   * @brief 
   * 
   * @param colour term_colour to print in
   * @param format printf format string
   * @param args printf args
   */
  template <typename... Params>
  void print(term_colours colour, std::string format, Params... args) const{
    std::string str = sprintf2_no_colour(print_type, format, args...);

    int buffer_len = str.length() + 3;
    char buffer[256];
    snprintf(buffer, 256, str.c_str());

    if(static_cast<int>(this->log_type) != 0){
      switch(log_location){
        case log_locations::t:
          printf2(colour, print_type, format.c_str(), args...);
          break;
        case log_locations::sd:
          this->log_print(buffer, buffer_len);
          break;
        case log_locations::none:
          break;
        case log_locations::both:
          printf2(colour, print_type, format.c_str(), args...);
          this->log_print(buffer, buffer_len);
          break;
      }
    }
  }

  /*
   * @brief 
   * 
   * @param format printf format string
   * @param args printf args
   */
  template <typename... Params>
  void print(std::string format, Params... args) const{
    print(print_colour, format, args...);
  }

  void print(Timer* tmr, int freq, std::vector<std::function<std::string()>> str) const; //How is this used?
};


void queue_handle(void* params);
uintptr_t data_size();
constexpr int queue_size = 1024;
constexpr int print_point = 500;
constexpr int print_max_time = 1500;
