#pragma once
#include "util.hpp"
#include "timer.hpp"
#include "task.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cstdarg>
#include <fstream>

using namespace std;
using namespace pros;
class _Task;
extern _Task log_t;
/*
1 is print, if there is no sd card, print to terminal
2 is print. if there is no sd card, ignore
0 is off
*/
enum log_types{
  error = 1,
  warning =1,
  general =2,
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
  const char* id;
  const char* name;
  log_types log_type;
  log_locations log_location;
  static vector<Data*> get_objs();
  void print(const char* format, ...);
  void log_print(char* buffer, int buffer_len);
  Data(const char* obj_name, const char* id_code, log_types log_type_param, log_locations log_location_param);
  static void init();
};


void queue_handle(void* params);
uintptr_t data_size();
const int queue_size = 1024;
const int print_point = 800;
const int print_max_time = 2000;


extern Data task_log;
extern Data controller_queue;
extern Data tracking_data;
