#include "controller.hpp"
std::array<Controller_*, num_controller> Controller_::objs; //= {nullptr};
Tasks Controller_::controller_task = nullptr;

int constructed = 0;

Controller_::Controller_(pros::controller_id_e_t id): pros::Controller{id}
  {
      objs[constructed] = this;
      this->controller_num = constructed+1;
      constructed++;
  }

void Controller_::print_queue(void* params){
  Tasks* ptr = Tasks::get_obj(params);
  while(true){
    for(int i = 0; i < num_controller; i++){
      controller_queue.print("%d| handle queue for controller %d\n", millis(), objs[i]->controller_num);
      objs[i]->queue_handle();
      delay(50);
    }
    if(ptr->notify_handle())break;

  }
}

void Controller_::init(){
  printf("here2\n");
  Tasks ct(print_queue,"controller task");
  Controller_::controller_task = std::move(ct);
  controller_task.start();
}


void Controller_::add_to_queue(std::function<void()> func){
  if(this->back +1 != this->front || (this->back == this->queue.size() -1 && this->front == 0)){
    controller_queue.print("%d| adding to controller: %d queue\n",millis(), this->controller_num);
    this->queue[this->back] = func;
    if(this->back == this->queue.size() -1){
      this->back = 0;
    }
    else this->back++;
    printf("%d, %d, %d\n", this->queue.size() -1,this->back, this->front);

  }
}

void Controller_::queue_handle(){
  printf("in queue handle %d\n",this->controller_num);
  if(this->front != this->back){
    controller_queue.print("%d| running command on controller %d\n",millis(), this->controller_num);
    this->queue[this->front]();
    if(this->front == this->queue.size()-1) this->front = 0;
    else this->front++;
  }
  else{
    printf("in the else\n");
  }
  printf("%d, %d, %d\n", this->queue.size() -1,this->back, this->front);
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
  std::function<void()> func = [=](){pros::Controller::clear();printf("in function\n");};
  this->add_to_queue(func);
  controller_queue.print("%d| adding clear to queue for controller %d\n",millis(), this->controller_num);
}
