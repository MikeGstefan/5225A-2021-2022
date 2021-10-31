#pragma once
#include "main.h"
#include "logging.hpp"
#include <tuple>


/*
  Tasks take 10ms to start

  task.get_state():
  E_TASK_STATE_RUNNING = 0, activally being accessed
  E_TASK_STATE_READY = 1, is going to be started, hasnt yet
  E_TASK_STATE_BLOCKED = 2, is in a delay or being blocked by another operation ( mutexes and shit)
  E_TASK_STATE_SUSPENDED = 3, paused
  E_TASK_STATE_DELETED = 4, deleted
  E_TASK_STATE_INVALID = 5, other
*/

enum notify_types{
  none = 0,
  kill = 1,
  reset = 2,
};

class Tasks{
private:
  pros::Task* task_ptr;
  pros::task_fn_t function;
  std::tuple<Tasks*, void*>params;
  std::uint32_t prio;
  std::uint16_t stack_depth;
  const char* name;


public:
  Tasks(pros::task_fn_t function, const char* name = "", void* params = NULL, std::uint32_t prio = TASK_PRIORITY_DEFAULT, std::uint16_t stack_depth = TASK_STACK_DEPTH_DEFAULT);
  // ~Tasks();

  static Tasks* get_obj(void* params);
  static void* get_params(void* params);

  bool notify_handle();
  void task_start();
  void task_kill(bool wait_for_comp = true);
  void task_rebind(pros::task_fn_t);


  pros::Task* get_task_ptr()const;


  bool data_update();
  bool done_update();



};
