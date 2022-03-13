#include "controller.hpp"

// Buttons


//aidan these ones
controller_digital_e_t lift_up_button = DIGITAL_R1;
controller_digital_e_t lift_down_button = DIGITAL_R2;
controller_digital_e_t lift_release_button =  DIGITAL_L2;
controller_digital_e_t lift_both_down_button = DIGITAL_A;


controller_digital_e_t shift_button = DIGITAL_X;
controller_digital_e_t intake_button = DIGITAL_B;
controller_digital_e_t reverse_drive_button = DIGITAL_L1;
controller_digital_e_t partner_intk_reverse = DIGITAL_B;
controller_digital_e_t partner_intk_on = DIGITAL_X;

controller_digital_e_t partner_f_up = DIGITAL_R1;
controller_digital_e_t partner_f_down = DIGITAL_R2;
controller_digital_e_t partner_b_up = DIGITAL_L1;
controller_digital_e_t partner_b_down = DIGITAL_L2;


// back lift buttons
controller_digital_e_t b_lift_up_button = DIGITAL_R1;
controller_digital_e_t b_lift_down_button = DIGITAL_R2;
controller_digital_e_t b_lift_release_button = DIGITAL_DOWN;

// front lift buttons
controller_digital_e_t f_lift_up_button = DIGITAL_RIGHT;
controller_digital_e_t f_lift_down_button = DIGITAL_L2;
controller_digital_e_t f_lift_release_button = DIGITAL_UP;



// misc buttons
controller_digital_e_t cancel_button = DIGITAL_L2;
controller_digital_e_t ok_button = DIGITAL_LEFT;
controller_digital_e_t tracking_button = DIGITAL_RIGHT;


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


void _Controller::rumble(const char* rumble_pattern){
  std::function<void()> func = [=](){
    pros::Controller::rumble(rumble_pattern);
    controller_queue.print("%d| rumble controller %d\n",millis(), this->controller_num);
  };
  this->add_to_queue(func);
  controller_queue.print("%d| adding rumble to queue for controller %d\n",millis(), this->controller_num);
}

bool _Controller::interrupt(bool analog, bool digital, bool OK_except){
  if (analog){
    if (fabs(get_analog(ANALOG_LEFT_X)) > 15) return true;
    if (fabs(get_analog(ANALOG_LEFT_Y)) > 15) return true;
    if (fabs(get_analog(ANALOG_RIGHT_X)) > 15) return true;
    if (fabs(get_analog(ANALOG_RIGHT_Y)) > 15) return true;
  }
  if(digital){
    if (!OK_except && get_digital(ok_button)) return true;
    if (get_digital(DIGITAL_A)) return true;
    if (get_digital(DIGITAL_B)) return true;
    if (get_digital(DIGITAL_Y)) return true;
    if (get_digital(DIGITAL_X)) return true;
    if (get_digital(DIGITAL_R1)) return true;
    if (get_digital(DIGITAL_R2)) return true;
    if (get_digital(DIGITAL_L1)) return true;
    if (get_digital(DIGITAL_L2)) return true;
    if (get_digital(DIGITAL_RIGHT)) return true;
    if (get_digital(DIGITAL_DOWN)) return true;
    if (get_digital(DIGITAL_LEFT)) return true;
    if (get_digital(DIGITAL_UP)) return true;
  }

  return false;
}

void _Controller::wait_for_press(controller_digital_e_t button, int timeout){
  int start_time = millis();
  controller_queue.print("%d| waiting for button %d from controller %d\n", millis(), button, this->controller_num);
  wait_until(get_digital_new_press(button)){
    if(timeout != 0 && millis() - start_time > timeout){
      controller_queue.print("%d| timed out on waiting for button %d press from controller %d\n", millis(), button, this->controller_num);
      return;
    }
  }
  controller_queue.print("%d| button %d pressed from controller %d\n", millis(), button, this->controller_num);
}
