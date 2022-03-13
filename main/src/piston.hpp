#pragma once //
#include "main.h"
#include "util.hpp"
#include "pros/adi.hpp"

using namespace std;
using namespace pros;


class Piston : public ADIDigitalOut{
  friend class Button;
  private:    
    bool state;
    bool open_state;
    int change_time = 0;
    const char* name;

    int count = 0;
  public:
    static array<Piston*, 8> list_for_gui;

    Piston(std::uint8_t adi_port, const char* name, bool open_state = 0, bool init_state = LOW);
    Piston(ext_adi_port_pair_t port_pair, const char* name, bool open_state = 0, bool init_state = LOW);

    void set_state(bool state);
    bool get_state();
    bool toggle_state();

    int get_state_time();
    const char* get_name();
};