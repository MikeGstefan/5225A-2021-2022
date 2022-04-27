#include "task.hpp"
#include "logging.hpp"
#include "util.hpp"

_Task::_Task(task_fn_t function, std::string name, void* params, std::uint32_t prio, std::uint16_t stack_depth){
  this->function = function;
  this->name = name;
  this->params = {this, std::move(params)};
  this->prio = prio;
  this->stack_depth = stack_depth;
}

_Task::~_Task(){
  task_log.print(term_colours::RED, "%s entered destructor", this->name);
  this->kill();
}

_Task* _Task::get_obj(void* params){
  //gets the _Task class pointer out of pair passed as void*
  return (static_cast<std::pair<_Task*, void*>*>(params))->first;
}

void* _Task::get_params(void* params){
  //gets void* params from the pair
  return (static_cast<std::pair<_Task*, void*>*>(params))->second;
}


Task* _Task::get_task_ptr() const{
  return this->task_ptr;
}

void _Task::start(void* params){
  task_log.print(term_colours::GREEN, "%s starting", this->name);
  if(this->task_ptr != NULL){
    task_log.print(term_colours::ERROR, "%s was already started", this->name);
    this->kill();
  }
  this->params = {this, std::move(params)};
  this->task_ptr = new Task(this->function, &this->params, this->prio, this->stack_depth, this->name.c_str());
  task_log.print(term_colours::GREEN, "%s started", this->name);
}

void _Task::kill(){
  if(this->task_ptr != NULL){
    task_log.print("%s killing", this->name);
    this->task_ptr->notify_ext(static_cast<int>(stop), E_NOTIFY_ACTION_OWRITE, NULL);
    task_log.print("%s notified", this->name);
    wait_until(this->task_ptr->get_state() == 4){
      task_log.print("%s state %d", this->name, this->task_ptr->get_state());
    }
    task_log.print("%s state check passed", this->name);
    delete this->task_ptr;
    this->task_ptr = NULL;
    task_log.print("%s killed", this->name);
  }
  else {
    task_log.print(term_colours::ERROR, "%s kill failed: already dead", this->name);
  }
}

bool _Task::notify_handle(){
  switch(static_cast<notify_types>(this->task_ptr->notify_take(1, 0))){
    case stop:
      return true;
      break;

    case reset:
      task_log.print("%s paused", this->name);
      this->task_ptr->suspend();
      break;

    default:
      break;
  }

  return false;
}

bool _Task::data_update(){
  if(this->task_ptr == NULL || this->task_ptr->get_state() >= 3) return false;
  task_log.print("%s pausing for data update", this->name);
  this->task_ptr->notify_ext(reset, E_NOTIFY_ACTION_OWRITE, NULL);
  wait_until(this->task_ptr->get_state() == 3);
  task_log.print("%s paused", this->name);
  return true;
}

bool _Task::done_update(){
  if(this->task_ptr == NULL || this->task_ptr->get_state() != 3) return false;
  this->task_ptr->resume();
  task_log.print("%s done data update, resuming", this->name);
  return true;
}

void _Task::rebind(task_fn_t function, void* params){
  task_log.print("%s rebinding", this->name);
  this->kill();
  this->function = function;
  this->start(params);
  task_log.print("%s rebound", this->name);
}
