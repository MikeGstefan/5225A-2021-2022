#pragma once
#include "main.h"
#include "timer.hpp"
#include "task.hpp"
#include "logging.hpp"
#include <cstdarg>
#include <array>
#include <functional>

using namespace pros;
// Buttons

// back lift buttons
extern controller_digital_e_t b_lift_up_button;
extern controller_digital_e_t b_lift_down_button;
extern controller_digital_e_t b_tall_goal_dropoff_button;

// front lift buttons
extern controller_digital_e_t f_lift_up_button;
extern controller_digital_e_t f_lift_down_button;
extern controller_digital_e_t f_tall_goal_dropoff_button;

// misc buttons
extern controller_digital_e_t cancel_button;
extern controller_digital_e_t shift_button;
extern controller_digital_e_t intake_button;
extern controller_digital_e_t reverse_drive_button;

#define num_controller 1

class _Task;

class _Controller : public pros::Controller{
private:
  std::array<std::function<void()>,20> queue;
  int front = 0, back = 0;
  static std::array<_Controller*, num_controller> objs;
  void add_to_queue(std::function<void()>);
  void queue_handle();
  int controller_num;

public:
  _Controller(pros::controller_id_e_t id);
  static void print_queue(void* params = NULL);
  static _Task controller_task;
  static void init();

  void print(std::uint8_t line, std::uint8_t col, const char* fmt, ... );
  void clear_line (std::uint8_t line);
  void clear();
  void rumble(const char* rumble_pattern);
  bool interrupt(bool analog=true, bool digital=true, bool A_except = true);
};
