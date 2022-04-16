#pragma once
#include "main.h"

using namespace std;
using namespace pros;

class Button;

//maybe a get_port if needed?
class Piston : public ADIDigitalOut{
  private:    
    bool state;
    bool open_state;
    int change_time = 0;
    std::string name;

    static int count;
  public:
    static array<std::pair<Piston*, Button*>, 8> list_for_gui;

    Piston(std::uint8_t adi_port, std::string name, bool open_state = 0, bool init_state = LOW);
    Piston(ext_adi_port_pair_t port_pair, std::string name, bool open_state = 0, bool init_state = LOW);

    void set_state(bool state);
    bool get_state() const;
    bool toggle_state();

    int get_state_time() const;
    std::string get_name() const;

    //static overloads (mostly for nathan)
    static std::string get_name(int number);
};