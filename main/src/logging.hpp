#pragma once
#include "util.hpp"
#include "timer.hpp"
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


// void logging_task_start();
// void logging_task_stop();

class Data{
private:
  static vector<Data*> obj_list;
  
public:
  static _Task log_t;


  const char* id;
  const char* name;
  log_types log_type;
  log_locations log_location;
  static vector<Data*> get_objs();
  void print(const char* format, ...);
  void print(Timer* tmr,int freq, vector<function<char*()>> str);
  void log_print(char* buffer, int buffer_len);
  Data(const char* obj_name, const char* id_code, log_types log_type_param, log_locations log_location_param);
  static void init();
  static char* to_char(const char* format, ...);

};


void queue_handle(void* params);
uintptr_t data_size();
const int queue_size = 1024;
const int print_point = 500;
const int print_max_time = 1500;


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