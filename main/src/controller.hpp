#pragma once
#include "main.h"
#include "timer.hpp"
#include "task.hpp"
#include "logging.hpp"
// #include "drive.hpp"
#include <cstdarg>
#include <array>
#include <functional>

using namespace pros;

// Buttons

// lift buttons
extern controller_digital_e_t lift_up_button;
extern controller_digital_e_t lift_down_button;
extern controller_digital_e_t lift_claw_toggle_button;
extern controller_digital_e_t lift_tilt_toggle_button;

// misc buttons
extern controller_digital_e_t intake_button;
extern controller_digital_e_t intake_reverse_button;
extern controller_digital_e_t shift_button;
extern controller_digital_e_t hitch_toggle_button;
extern controller_digital_e_t ok_button;
extern controller_digital_e_t b_claw_toggle_button;


#define num_controller 2

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
  bool interrupt(bool analog=true, bool digital=true, bool OK_except = true);
  void wait_for_press(controller_digital_e_t button, int timeout = 0);
};


