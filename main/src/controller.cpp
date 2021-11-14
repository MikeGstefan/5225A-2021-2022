#include "controller.hpp"
std::array<_Controller*, num_controller> _Controller::objs; //= {nullptr};
_Task _Controller::controller_task = nullptr;

int constructed = 0;

_Controller::_Controller(pros::controller_id_e_t id): pros::Controller{id}
  {
      objs[constructed] = this;
      this->controller_num = constructed+1;
      constructed++;
  }

void _Controller::print_queue(void* params){
  _Task* ptr = _Task::get_obj(params);
  while(true){
    for(int i = 0; i < num_controller; i++){
      objs[i]->queue_handle();
      delay(50);
    }
    if(ptr->notify_handle())break;

  }
}

void _Controller::init(){
  _Task ct(print_queue,"controller task");
  _Controller::controller_task = std::move(ct);
  controller_task.start();
}


void _Controller::add_to_queue(std::function<void()> func){
  if(this->back +1 != this->front || (this->back == this->queue.size() -1 && this->front == 0)){
    controller_queue.print("%d| adding to controller: %d queue\n",millis(), this->controller_num);
    this->queue[this->back] = func;
    if(this->back == this->queue.size() -1){
      this->back = 0;
    }
    else this->back++;

  }
}

void _Controller::queue_handle(){
  if(this->front != this->back){
    controller_queue.print("%d| running command on controller %d\n",millis(), this->controller_num);
    this->queue[this->front]();
    if(this->front == this->queue.size()-1) this->front = 0;
    else this->front++;
  }
}

void _Controller::print(std::uint8_t line, std::uint8_t col, const char* fmt, ... ){
  char buffer[19];
  std::va_list args;
  va_start(args, fmt);
  vsnprintf(buffer,19,fmt,args);
  va_end(args);
  std::function<void()> func = [=](){
    pros::Controller::print(line,col,buffer);
    controller_queue.print("%d| printing %s to %d\n",millis(), buffer, this->controller_num);
  };
  this->add_to_queue(func);
  controller_queue.print("%d| adding print to queue for controller %d\n",millis(), this->controller_num);

}
void _Controller::clear_line (std::uint8_t line){
  std::function<void()> func = [=](){
    pros::Controller::clear_line(line);
    controller_queue.print("%d| clearing line %d for controller %d\n",millis(), line, this->controller_num);
  };
  this->add_to_queue(func);
  controller_queue.print("%d| adding clear_line to queue for controller %d\n",millis(), this->controller_num);
}

void _Controller::clear(){
  std::function<void()> func = [=](){
    pros::Controller::clear();
    controller_queue.print("%d| clearing %d\n",millis(), this->controller_num);
  };
  this->add_to_queue(func);
  controller_queue.print("%d| adding clear to queue for controller %d\n",millis(), this->controller_num);
}
