#include "task.hpp"

Tasks::Tasks(pros::task_fn_t function, const char* name, void* params, std::uint32_t prio, std::uint16_t stack_depth){
  this->function = function;
  this->name = name;
  this->params = std::make_tuple(this,params);
  this->prio = prio;
  this->stack_depth = stack_depth;
  printf("here3\n");
}
// Tasks::~Tasks(){
//   if(this->task_ptr->get_state() != 5)
// }

Tasks* Tasks::get_obj(void* params){
  //gets the Tasks class pointer out of tuple passed as void*
  return std::get<0>(*(std::tuple<Tasks*, void*>*)(params));
}

// void* Tasks::get_params(void* params){
//   //gets void* params from the tuple
//   return std::get<1>(*(std::tuple<Tasks*, void*>*)(params));
// }


pros::Task* Tasks::get_task_ptr()const{
  return this->task_ptr;
}

void Tasks::task_start(){
   printf("here5\n");
   if(this->task_ptr == NULL)printf("its null \n");
   else printf("not null \n");
   cout<<this->task_ptr<<"\n";
   this->task_ptr = new pros::Task(this->function, &this->params, this->prio, this->stack_depth, this->name);
   // pros::Task task_ptr(this->function, this->params, this->prio, this->stack_depth, this->name);
   // task_log.print("%d starting task: %s \n", this->name);
}

void Tasks::task_kill(bool wait_for_comp){
  if(this->task_ptr != NULL){
    printf("killing %s \n", this->name);
    this->task_ptr->notify_ext((int)kill, E_NOTIFY_ACTION_OWRITE,NULL);
    while(this->task_ptr->get_state()!=4)delay(10);
    delete this->task_ptr;
    this->task_ptr = NULL;
  }
  else {
    printf("already gone \n");
  }
}

bool Tasks::notify_handle(){
  switch((notify_types)this->task_ptr->notify_take(1,0)){
    case kill:
      return true;
    break;
    case reset:
      this->task_ptr->suspend();
    break;
    default:

    break;
  }
  return false;
}

bool Tasks::data_update(){
  if(this->task_ptr->get_state() >=3)return false;
  this->task_ptr->notify_ext((int)reset, E_NOTIFY_ACTION_OWRITE,NULL);
  while(this->task_ptr->get_state()!=3)delay(10);
  return true;
}

bool Tasks::done_update(){
  if(this->task_ptr->get_state() !=3)return false;
  this->task_ptr->resume();
  return true;
}



void Tasks::task_rebind(pros::task_fn_t function){
  this->task_kill(true);
  this->function = function;
  this->task_start();
}
