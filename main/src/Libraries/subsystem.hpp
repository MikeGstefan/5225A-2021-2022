#pragma once
#include "../util.hpp"
#include "../timer.hpp"
#include "../config.hpp"
#include "../controller.hpp"



using namespace pros;

// non-motorized subsystem
template <typename state_type, int num_of_states>
class Subsystem{
protected:
  state_type state;
  state_type last_state;
  const char* name;
  std::array<const char*, num_of_states> state_names;

public:

  // constructor for non-motorized subsystem
  Subsystem(const char* name, std::array<const char*, num_of_states> state_names):
    name(name), state_names(state_names)
  {}

  void set_state(const state_type next_state){
    printf("%s | Going from %s to %s\n", state_names[static_cast<int>(state)], state_names[static_cast<int>(next_state)]);
    last_state = state;
    state = next_state;
  }

  void handle();  // has a switch containing the state machine for a given subsystem
};

template <typename state_type, int num_of_states>
class Motorized_subsystem: public Subsystem<state_type, num_of_states>, public Motor{
public:
  Motorized_subsystem(Subsystem<state_type, num_of_states> subsystem, Motor& motor):
    Subsystem<state_type, num_of_states>(subsystem), Motor(motor){}

  void reset(){
    move(-60);
    Timer vel_rise_timeout("vel_rise");
    // waits for motor's velocity to rise or timeout to trigger
    while(fabs(get_actual_velocity()) < 45.0){
      printf("%s's velocity is (rising loop): %lf\n", this->name, get_actual_velocity());
      if (vel_rise_timeout.get_time() > 50){
        printf("%s's rising loop timed out\n", this->name);
        break;
      }
      delay(10);
    }
    printf("%s's velocity done rising\n", this->name);
    // waits until motors velocity slows down for 5 cycles
    cycleCheck(fabs(get_actual_velocity()) < 5.0, 5, 10)
    tare_position();  // resets subsystems position
    printf("%d, %s's reset %lf\n", millis(), this->name, get_position());
    move(0);
  }

};
