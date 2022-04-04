#include "piston.hpp"
// #include "pros/adi.hpp"

Piston::Piston(std::uint8_t adi_port, bool open_state, bool init_state) : ADIDigitalOut(adi_port, init_state), open_state{open_state}{}
Piston::Piston(ext_adi_port_pair_t port_pair, bool open_state, bool init_state): ADIDigitalOut(port_pair, init_state), open_state{open_state}{ }


void Piston::set_state(bool state){
    this->state = state != open_state;
    this->set_value(this->state);
    this->change_time = millis();
}


bool Piston::get_state(){
    return this->state != open_state;
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