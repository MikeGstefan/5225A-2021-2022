#include "controller.hpp"

std::array<Controller_*, num_controller> Controller_::objs = {nullptr};
Tasks Controller_::controller_task = nullptr;


Controller_::Controller_(pros::controller_id_e_t id): pros::Controller{id}
  {
    //dont pass this in te contructor

    if(num_controller >1 && !Controller_::objs.empty()){
      objs[1] = this;
      this->controller_num = 2;
    }
    else{
      objs[0] = this;
      this->controller_num = 1;
    }
  }

void Controller_::print_queue(void* params){
  Tasks* ptr = Tasks::get_obj(params);
  while(true){
    for(int i = 0; i < num_controller; i++){
      controller_queue.print("%d| handle queue for controller %d\n", millis(), i+1);
      objs[i]->queue_handle();
    }
    if(ptr->notify_handle())break;
    delay(50);
  }
}

void Controller_::init(){
  Tasks controller_task(print_queue,"controller task");
  Controller_::controller_task.start();
}


void Controller_::add_to_queue(std::function<void()> func){
  if(this->back +1 != this->front || (this->back == this->queue.size() -1 && this->front == 0)){
    controller_queue.print("%d| adding to controller: %d queue\n",millis(), this->controller_num);
    if(this->back != this->queue.size() -1){
      this->back = 0;
    }
    else this->back++;
    this->queue[this->back] = func;
  }
}

void Controller_::queue_handle(){
  if(this->front != this->back){
    controller_queue.print("%d| running command on controller %d\n",millis(), this->controller_num);
    this->queue[this->front]();
    if(this->front == this->queue.size()-1) this->front = 0;
    else this->front++;
  }
}

void Controller_::print(std::uint8_t line, std::uint8_t col, const char* fmt, ... ){
  char buffer[19];
  std::va_list args;
  va_start(args, fmt);
  vsnprintf(buffer,19,fmt,args);
  va_end(args);
  std::function<void()> func = [=](){pros::Controller::print(line,col,buffer);};
  this->add_to_queue(func);
  controller_queue.print("%d| adding print to queue for controller %d\n",millis(), this->controller_num);

}
void Controller_::clear_line (std::uint8_t line){
  std::function<void()> func = [=](){pros::Controller::clear_line(line);};
  this->add_to_queue(func);
  controller_queue.print("%d| adding clear_line to queue for controller %d\n",millis(), this->controller_num);
  // return std::bind(pros::Controller::clear_line,line);
}
void Controller_::clear(){
  std::function<void()> func = [=](){pros::Controller::clear();};
  this->add_to_queue(func);
  controller_queue.print("%d| adding print to clear for controller %d\n",millis(), this->controller_num);
}
