#include "controller.hpp"
#include "logging.hpp"
#include "task.hpp"
#include "Libraries/printing.hpp"
#include "util.hpp"

// Buttons

// lift buttons
controller_digital_e_t lift_up_button = DIGITAL_R1;
controller_digital_e_t lift_down_button = DIGITAL_R2;
controller_digital_e_t lift_claw_toggle_button =  DIGITAL_L2;
controller_digital_e_t lift_tilt_toggle_button =  DIGITAL_L1;

// misc buttons
controller_digital_e_t intake_button = DIGITAL_Y;
controller_digital_e_t ok_button = DIGITAL_A;

controller_digital_e_t intake_reverse_button = DIGITAL_B;
controller_digital_e_t shift_button = DIGITAL_DOWN;
controller_digital_e_t hitch_toggle_button = DIGITAL_B;
controller_digital_e_t b_claw_toggle_button = DIGITAL_X;



std::array<_Controller*, num_controller> _Controller::objs; //= {nullptr};
_Task _Controller::controller_task = nullptr;

int constructed = 0;

_Controller::_Controller(pros::controller_id_e_t id): pros::Controller{id}
{
  objs[constructed] = this;
  constructed++;
  this->controller_num = constructed;
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
    if (get_digital(ok_button)) return !OK_except;
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

controller_digital_e_t _Controller::wait_for_press(std::vector<controller_digital_e_t> buttons, int timeout){
  int start_time = millis();
  controller_queue.print("%d| waiting for button press from controller %d\n", millis(), this->controller_num);
  controller_digital_e_t button = static_cast<controller_digital_e_t>(0);
  
  wait_until(button != static_cast<controller_digital_e_t>(0)){
    for(std::vector<controller_digital_e_t>::iterator it = buttons.begin(); it != buttons.end(); it++){
      if(get_digital_new_press(*it)) button = *it;
    }

    if(timeout != 0 && millis() - start_time > timeout){
      controller_queue.print("%d| timed out on waiting for button press from controller %d\n", millis(), this->controller_num);
      return static_cast<controller_digital_e_t>(0);
    }
  }
  controller_queue.print("%d| button %d pressed from controller %d\n", millis(), button, this->controller_num);

  return button;
}


//create wait for press for multiple buttons and return the one that was pressed
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
