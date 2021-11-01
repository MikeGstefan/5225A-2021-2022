#include "task.hpp"

Tasks::Tasks(pros::task_fn_t function, const char* name, void* params, std::uint32_t prio, std::uint16_t stack_depth){
  this->function = function;
  this->name = name;
  this->params = std::make_tuple(this,params);
  this->prio = prio;
  this->stack_depth = stack_depth;
}

Tasks::~Tasks(){
  task_log.print("%d| %s entered deconstructor\n",millis(), this->name);
  this->kill();
}

Tasks* Tasks::get_obj(void* params){
  //gets the Tasks class pointer out of tuple passed as void*
  return std::get<0>(*(std::tuple<Tasks*, void*>*)(params));
}

void* Tasks::get_params(void* params){
  //gets void* params from the tuple
  return std::get<1>(*(std::tuple<Tasks*, void*>*)(params));
}


pros::Task* Tasks::get_task_ptr()const{
  return this->task_ptr;
}

void Tasks::start(void* params){
   task_log.print("%d| %s starting", millis(), this->name);
   if(this->task_ptr != NULL){
     task_log.print("%d| %s was already started", millis(), this->name);
     this->kill();
   }
   this->params = std::make_tuple(this,params);
   this->task_ptr = new pros::Task(this->function, &this->params, this->prio, this->stack_depth, this->name);
   task_log.print("%d| %s started", millis(), this->name);
}

void Tasks::kill(){
  if(this->task_ptr != NULL){
    task_log.print("%d| %s killing", millis(), this->name);
    this->task_ptr->notify_ext((int)stop, E_NOTIFY_ACTION_OWRITE,NULL);
    while(this->task_ptr->get_state()!=4)delay(10);
    delete this->task_ptr;
    this->task_ptr = NULL;
    task_log.print("%d| %s killed", millis(), this->name);
  }
  else {
    task_log.print("%d| %s kill failed: already dead", millis(), this->name);
  }
}

bool Tasks::notify_handle(){
  switch((notify_types)this->task_ptr->notify_take(1,0)){
    case stop:
      return true;
    break;
    case reset:
      this->task_ptr->suspend();
      task_log.print("%d| %s paused", millis(), this->name);
    break;
    default:

    break;
  }
  return false;
}

bool Tasks::data_update(){
  if(this->task_ptr->get_state() >=3)return false;
  task_log.print("%d| %s pausing for data", millis(), this->name);
  this->task_ptr->notify_ext((int)reset, E_NOTIFY_ACTION_OWRITE,NULL);
  while(this->task_ptr->get_state()!=3)delay(10);
  return true;
}

bool Tasks::done_update(){
  if(this->task_ptr->get_state() !=3)return false;
  this->task_ptr->resume();
  task_log.print("%d| %s done data update, resuming", millis(), this->name);
  return true;
}



void Tasks::rebind(pros::task_fn_t function, void* params){
  task_log.print("%d| %s rebinding", millis(), this->name);
  this->kill();
  this->function = function;
  this->start(params);
  task_log.print("%d| %s rebound", millis(), this->name);
}
