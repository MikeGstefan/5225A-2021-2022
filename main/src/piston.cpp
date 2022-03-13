#include "piston.hpp"

array<Piston*, 8> Piston::list_for_gui = {
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

Piston::Piston(std::uint8_t adi_port, const char* name, bool open_state, bool init_state) : ADIDigitalOut(adi_port, init_state), open_state{open_state}, name(name){
  if(count < 8) list_for_gui[count] = this;
  count++;
}
Piston::Piston(ext_adi_port_pair_t port_pair, const char* name, bool open_state, bool init_state): ADIDigitalOut(port_pair, init_state), open_state{open_state}, name(name){
  if(count < 8) list_for_gui[count] = this;
  count++;
}


void Piston::set_state(bool state){
  this->state = state != open_state;
  this->set_value(this->state);
  this->change_time = millis();
}


bool Piston::get_state(){
  return this->state;
}

bool Piston::toggle_state(){
  this->state = !this->state;
  this->set_value(state);
  this->change_time = millis();
  return state;
}


int Piston::get_state_time(){
  return millis() - this->change_time;
}

const char* Piston::get_name(){
  return this->name;
}