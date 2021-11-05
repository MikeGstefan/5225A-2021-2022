#pragma once
#include "main.h"
#include "timer.hpp"
#include "task.hpp"
#include "logging.hpp"
#include <cstdarg>
#include <array>
#include <functional>

#define num_controller 1

class Tasks;

class Controller_ : public pros::Controller{
private:
  std::array<std::function<void()>,5> queue;
  int front = 0, back = 0;
  static std::array<Controller_*, num_controller> objs;
  void add_to_queue(std::function<void()>);
  void queue_handle();
  int controller_num;

public:
  Controller_(pros::controller_id_e_t id);
  static void print_queue(void* params = NULL);
  static Tasks controller_task;
  static void init();


  void print(std::uint8_t line, std::uint8_t col, const char* fmt, ... );
  void clear_line (std::uint8_t line);
  void clear();

};
