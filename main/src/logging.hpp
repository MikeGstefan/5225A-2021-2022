#pragma once
#include "util.hpp"
#include "timer.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cstdarg>
#include <array>
#include <fstream>
#include "main.h"

using namespace std;
using namespace pros;

extern Task *printing;
enum log_types{
  error = 1,
  warning =1,
  data = 1
};

enum class log_locations
{
  t,
  sd,
  both,
  none
};


void logging_task_start();
void logging_task_stop();

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
  bool will_log();
  Data(const char* obj_name, const char* id_code, log_types log_type_param, log_locations log_location_param);
  static void log_init();
};


void queue_handle(void* params);
uintptr_t data_size();
const int queue_size = 1024;
const int print_point = 800;
const int print_max_time = 2000;


extern Data imu_data;
