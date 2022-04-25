#include "controller.hpp"
#include "Libraries/task.hpp"
#include "Libraries/util.hpp"

std::array<_Controller*, controller_count> _Controller::objs; //= {nullptr};
_Task _Controller::controller_task = nullptr;

int constructed = 0;

_Controller::_Controller(controller_id_e_t id): Controller{id}{
  objs[constructed] = this;
  constructed++;
  this->controller_num = constructed;
}

void _Controller::print_queue(void* params){
  _Task* ptr = _Task::get_obj(params);
  while(true){
    for(int i = 0; i < controller_count; i++){
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
    controller_queue.print("adding to controller: %d queue", this->controller_num);
    this->queue[this->back] = func;
    if(this->back == this->queue.size() -1){
      this->back = 0;
    }
    else this->back++;

  }
}

void _Controller::queue_handle(){
  if(this->front != this->back){
    controller_queue.print("running command on controller %d", this->controller_num);
    this->queue[this->front]();
    if(this->front == this->queue.size()-1) this->front = 0;
    else this->front++;
  }
}
void _Controller::clear_line (std::uint8_t line){
  std::function<void()> func = [=](){
    Controller::clear_line(line);
    controller_queue.print("clearing line %d for controller %d", line, this->controller_num);
  };
  this->add_to_queue(func);
  controller_queue.print("adding clear_line to queue for controller %d", this->controller_num);
}

void _Controller::clear(){
  std::function<void()> func = [=](){
    Controller::clear();
    controller_queue.print("clearing %d", this->controller_num);
  };
  this->add_to_queue(func);
  controller_queue.print("adding clear to queue for controller %d", this->controller_num);
}


void _Controller::rumble(const char* rumble_pattern){
  std::function<void()> func = [=](){
    Controller::rumble(rumble_pattern);
    controller_queue.print("rumble controller %d", this->controller_num);
  };
  this->add_to_queue(func);
  controller_queue.print("adding rumble to queue for controller %d", this->controller_num);
}

bool _Controller::interrupt(bool analog, bool digital, bool OK_except){
  if (analog){
    if (abs(get_analog(ANALOG_LEFT_X)) > 15) return true;
    if (abs(get_analog(ANALOG_LEFT_Y)) > 15) return true;
    if (abs(get_analog(ANALOG_RIGHT_X)) > 15) return true;
    if (abs(get_analog(ANALOG_RIGHT_Y)) > 15) return true;
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
  if(competition::is_autonomous()){
    ERROR.print("Cannot get controller press in autonomous!");
    return static_cast<controller_digital_e_t>(0);
  }
  int start_time = millis();
  controller_queue.print("waiting for button press from controller %d", this->controller_num);
  controller_digital_e_t button = static_cast<controller_digital_e_t>(0);
  
  wait_until(button != static_cast<controller_digital_e_t>(0)){
    for(std::vector<controller_digital_e_t>::const_iterator it = buttons.begin(); it != buttons.end(); it++){
      if(get_digital_new_press(*it)) button = *it;
    }

    if(timeout != 0 && millis() - start_time > timeout){
      controller_queue.print("timed out on waiting for button press from controller %d", this->controller_num);
      return static_cast<controller_digital_e_t>(0);
    }
  }
  controller_queue.print("button %d pressed from controller %d", button, this->controller_num);
  return button;
}

void _Controller::wait_for_press(controller_digital_e_t button, int timeout){
  if(competition::is_autonomous()){
    ERROR.print("Cannot get controller press in autonomous!");
    return;
  }
  int start_time = millis();
  controller_queue.print("waiting for button %d from controller %d", button, this->controller_num);
  
  wait_until(get_digital_new_press(button)){
    if(timeout != 0 && millis() - start_time > timeout){
      controller_queue.print("timed out on waiting for button %d press from controller %d", button, this->controller_num);
      return;
    }
  }
  controller_queue.print("button %d pressed from controller %d", button, this->controller_num);
}

void _Controller::update_buttons(){
  for(int i = 0; i < 12; i++){
    last_press_arr[i] = cur_press_arr[i];
    // + 6 because controller_digital_e_t starts with 6 instead of 0
    cur_press_arr[i] = this->get_digital(static_cast<controller_digital_e_t>(i + 6));
    // printf2("%d, %d ", i, cur_press_arr[i]);
  }
  // printf2("");
}

bool _Controller::get_button_state(controller_digital_e_t button){
  return cur_press_arr[static_cast<int>(button) - 6];
}

bool _Controller::get_button_last_state(controller_digital_e_t button){
  return cur_press_arr[static_cast<int>(button) - 6];
}

bool _Controller::is_rising(controller_digital_e_t button){
  int index = static_cast<int>(button) - 6;
  return !last_press_arr[index] && cur_press_arr[index];
}

bool _Controller::is_falling(controller_digital_e_t button){
  int index = static_cast<int>(button) - 6;
  return last_press_arr[index] && !cur_press_arr[index];
}

