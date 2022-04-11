#pragma once
#include "main.h"
#include "Libraries/printing.hpp"

//forawrd declarations
class _Task;
class Timer;
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

// enum class term_colours;


// void logging_task_start();
// void logging_task_stop();

class Data{
private:
  static vector<Data*> obj_list;
  term_colours print_colour = term_colours::NONE;
  int print_type = 0;
  
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

  Data(const char* obj_name, const char* id_code, log_types log_type_param, log_locations log_location_param, term_colours print_colour = term_colours::NONE, int print_type = 0);
  
  static void init();
  static char* to_char(const char* format, ...);

  /**
  * @brief printf that handles strings and automatically newlines. Can print coloured and a timestamp
  * 
  * @param colour The colour to print in 
  * @param time_type:
  * -1: no timestamp
  * 0: 1500
  * 1: 1500ms
  * 2: 1s 500ms
  * @param fmt printf format string
  * @param args printf args
  */
  template <typename... Params>
  void print(std::string format, Params... args) const{
    std::string str = sprintf2(format, args...);
    int buffer_len = str.length() + 3;
    char buffer[256];
    snprintf(buffer, 256, str.c_str());

    if(int(this->log_type) != 0){
      switch(log_location){
        case log_locations::t:
          printf2(print_colour, print_type, "%s", str);
          break;
        case log_locations::sd:
          this->log_print(buffer, buffer_len);
          break;
        case log_locations::none:
          break;
        case log_locations::both:
          printf2(print_colour, print_type, "%s", str);
          this->log_print(buffer, buffer_len);
          break;
      }
    }
  }
};


void queue_handle(void* params);
uintptr_t data_size();
constexpr int queue_size = 1024;
constexpr int print_point = 500;
constexpr int print_max_time = 1500;


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
