#pragma once //
#include "main.h" 

using namespace std;
using namespace pros;


class Piston : public ADIDigitalOut{
    private:    
        bool state;
        bool open_state;
        int change_time = 0;

    public: 
        Piston(std::uint8_t adi_port, bool open_state = 0, bool init_state = LOW);
        Piston(ext_adi_port_pair_t port_pair, bool open_state = 0, bool init_state = LOW);

        void set_state(bool state);
        bool get_state();
        bool toggle_state();

        int get_state_time();
};