#include "task.hpp"

struct point_params{ 
  Position target;
  const double max_power;
  const bool overshoot;
  const double min_angle_percent;
  const bool brake;
  const double decel_dist, decel_speed;
  point_params(Position target, const double max_power = 127.0, const bool overshoot = false, const double min_angle_percent = 0.0, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0);
};

_Task::_Task(pros::task_fn_t function, const char* name, void* params, std::uint32_t prio, std::uint16_t stack_depth){
  this->function = function;
  this->name = name;
  this->params = std::make_tuple(this,std::move(params));
  this->prio = prio;
  this->stack_depth = stack_depth;
}

_Task::~_Task(){
  task_log.print("%d| %s entered deconstructor\n",millis(), this->name);
  this->kill();
}

_Task* _Task::get_obj(void* params){
  //gets the _Task class pointer out of tuple passed as void*
  return std::get<0>(*(std::tuple<_Task*, void*>*)(params));
}

void* _Task::get_params(void* params){
  //gets void* params from the tuple
  return std::get<1>(*(std::tuple<_Task*, void*>*)(params));
}


pros::Task* _Task::get_task_ptr()const{
  return this->task_ptr;
}

void _Task::start(void* params){
   task_log.print("%d| %s starting\n", millis(), this->name);
   if(this->task_ptr != NULL){
     task_log.print("%d| %s was already started\n", millis(), this->name);
     this->kill();
   }
  //  printf("before move: %f\n",static_cast<point_params*>(params)->target.x);
   this->params = std::make_tuple(this,std::move(params));
  //  printf("after move: %f\n",static_cast<point_params*>(_Task::get_params((void*)&this->params))->target.x);
   this->task_ptr = new pros::Task(this->function, &this->params, this->prio, this->stack_depth, this->name);
   task_log.print("%d| %s started\n", millis(), this->name);
}

void _Task::kill(){
  if(this->task_ptr != NULL){
    task_log.print("%d| %s killing\n", millis(), this->name);
    this->task_ptr->notify_ext((int)stop, E_NOTIFY_ACTION_OWRITE,NULL);
    task_log.print("%d| %s notified\n", millis(), this->name);
    wait_until(this->task_ptr->get_state() == 4){
      task_log.print("%d| %s state %d\n", millis(), this->name, this->task_ptr->get_state());
    }
    task_log.print("%d| %s state check passed\n", millis(), this->name);
    delete this->task_ptr;
    this->task_ptr = NULL;
    task_log.print("%d| %s killed\n", millis(), this->name);
  }
  else {
    task_log.print("%d| %s kill failed: already dead\n", millis(), this->name);
  }
}

bool _Task::notify_handle(){
  switch((notify_types)this->task_ptr->notify_take(1,0)){
    case stop:
      return true;
    break;
    case reset:
      task_log.print("%d| %s paused\n", millis(), this->name);
      this->task_ptr->suspend();

    break;
    default:

    break;
  }
  return false;
}

bool _Task::data_update(){
  if(this->task_ptr == NULL ||this->task_ptr->get_state() >=3)return false;
  task_log.print("%d| %s pausing for data\n", millis(), this->name);
  this->task_ptr->notify_ext((int)reset, E_NOTIFY_ACTION_OWRITE,NULL);
  wait_until (this->task_ptr->get_state()== 3);
  return true;
}

bool _Task::done_update(){
  if(this->task_ptr == NULL || this->task_ptr->get_state() !=3)return false;
  this->task_ptr->resume();
  task_log.print("%d| %s done data update, resuming\n", millis(), this->name);
  return true;
}



void _Task::rebind(pros::task_fn_t function, void* params){
  task_log.print("%d| %s rebinding\n", millis(), this->name);
  this->kill();
  this->function = function;
  this->start(params);
  task_log.print("%d| %s rebound\n", millis(), this->name);
}
