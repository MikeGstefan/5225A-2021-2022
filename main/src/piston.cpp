#include "piston.hpp"
int Piston::count = 0;

extern Button pneum_1, pneum_2, pneum_3, pneum_4, pneum_5, pneum_6, pneum_7, pneum_8;

array<std::pair<Piston*, Button*>, 8> Piston::list_for_gui {{
  {nullptr, &pneum_1},
  {nullptr, &pneum_2},
  {nullptr, &pneum_3},
  {nullptr, &pneum_4},
  {nullptr, &pneum_5},
  {nullptr, &pneum_6},
  {nullptr, &pneum_7},
  {nullptr, &pneum_8},
}};

Piston::Piston(std::uint8_t adi_port, std::string name, bool open_state, bool init_state) : ADIDigitalOut(adi_port, init_state), open_state{open_state}, name(name){
  if(count < 8) list_for_gui[count].first = this;
  count++;
}
Piston::Piston(ext_adi_port_pair_t port_pair, std::string name, bool open_state, bool init_state): ADIDigitalOut(port_pair, init_state), open_state{open_state}, name(name){
  if(count < 8) list_for_gui[count].first = this;
  count++;
}


void Piston::set_state(bool state){
  this->state = state != open_state;
  this->set_value(this->state);
  this->change_time = millis();
}


bool Piston::get_state() const{
  return this->state != open_state;
}

bool Piston::toggle_state(){
  this->state = !this->state;
  this->set_value(state);
  this->change_time = millis();
  return state;
}


int Piston::get_state_time() const{
  return millis() - this->change_time;
}

std::string Piston::get_name() const{
  return this->name;
}

std::string Piston::get_name(int number){
  return list_for_gui[number-1].first ? Piston::list_for_gui[number-1].first->get_name() : "No Piston";
}