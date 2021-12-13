#pragma once
#include "util.hpp"
#include "timer.hpp"
#include "config.hpp"

using namespace pros;

template <typename state_type, bool motorized, int num_of_states>
class Subsystem: public Motor {
  state_type state;
  state_type last_state;

public:
  const char* name;
  std::array<const char*, num_of_states> state_names;


  // constructor for motorized subsystem
  template <class = std::enable_if<motorized>>
  Subsystem(Motor& motor, const char* name, std::array<const char*, num_of_states> state_names);


  // constructor for non-motorized subsystem
  template <class = std::enable_if<!motorized>>
  Subsystem(const char* name, std::array<const char*, num_of_states> state_names);


  void set_state(const state_type next_state){
    printf("%s | Going from %s to %s\n", state_names[state], state_names[next_state]);
    last_state = state;
    state = next_state;
  }

  void handle();  // has a switch containing the state machine for a given subsystem

  // only compiles the reset method for motorized subsystems
  template <class = std::enable_if<motorized>>
  void reset(){
    move(-60);
    Timer vel_rise_timeout("vel_rise");
    // waits for motor's velocity to rise or timeout to trigger
    while(fabs(get_actual_velocity()) < 45.0){
      printf("%s's velocity is (rising loop): %lf\n", name, get_actual_velocity());
      if (vel_rise_timeout.get_time() > 50){
        printf("%s's rising loop timed out\n", name);
        break;
      }
      delay(10);
    }
    printf("%s's velocity done rising\n", name);
    // waits until motors velocity slows down for 5 cycles
    cycleCheck(fabs(get_actual_velocity()) < 5.0, 5, 10)
    tare_position();  // resets subsystems position
    printf("%d, %s's reset %lf\n", millis(), name, get_position());
    move(0);
  }
};


enum class six_bar_states{
  searching,  // at lowest height waiting for lim switch to pickup goals
  lowered,  //  at lowest height, not waiting to pikcup mogos
  raised, // at height for intake to fill alliance mogo
  platform, // holding mogo at platform dropoff height
  released, // mogo is released at platform height
  lowering  // on the way to lowered state from released
};

class Six_bar: public Subsystem<six_bar_states, true, 6> {

public:
  Six_bar(Subsystem<six_bar_states, true, 6> subsystem);  // constructor
  void handle();  // contains state machine code

};
