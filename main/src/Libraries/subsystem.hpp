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
  // target state is what the subsystem is trying to reach 
  // state is the current state
  state_type target_state, state;
  const char* name;
  std::array<const char*, num_of_states> state_names;

public:

  // constructor for non-motorized subsystem
  Subsystem(const char* name, std::array<const char*, num_of_states> state_names):
    name(name), state_names(state_names)
  {}

  void set_state(const state_type next_state){  // requests a state change and logs it
    state_log.print("%s | State change requested from %s to %s\n", name, state_names[static_cast<int>(state)], state_names[static_cast<int>(next_state)]);
    target_state = next_state;
  }

  void log_state_change(){  // confirms state change, logs it, and updates state to be the target state
    state_log.print("%s | State change confirmed from %s to %s\n", name, state_names[static_cast<int>(state)], state_names[static_cast<int>(target_state)]);
    state = target_state;  
  }
  
  //shouldn't handle be a virtual function
  void handle();  // has a switch containing the state machine for a given subsystem
  void handle_state_change(); // cleans up and preps the machine to be in the target state

  // public getters for accessing 
  state_type get_target_state() const{
    return target_state;
  }

  state_type get_state() const{
    return state;
  }

};

template <typename state_type, int num_of_states, int default_velocity>
class Motorized_subsystem: public Subsystem<state_type, num_of_states>{
// protected:

public:
  const int end_error; // how close the subsystem is to a target before considered "reached"

  Motor& motor;
  Motorized_subsystem(Subsystem<state_type, num_of_states> subsystem, Motor& motor, int end_error = 10):
    Subsystem<state_type, num_of_states>(subsystem), motor(motor), end_error(end_error){}

  void reset(){
    motor.move(-60);
    Timer vel_rise_timeout("vel_rise");
    // waits for motor's velocity to rise or timeout to trigger
    wait_until(fabs(motor.get_actual_velocity()) > 45.0){
      printf("%s's velocity is (rising loop): %lf\n", this->name, motor.get_actual_velocity());
      if (vel_rise_timeout.get_time() > 200){
        printf("%s's rising loop timed out\n", this->name);
        break;
      }
    }
    printf("%s's velocity done rising\n", this->name);
    // waits until motors velocity slows down for 5 cycles
    cycleCheck(fabs(motor.get_actual_velocity()) < 5.0, 5, 10)
    motor.tare_position();  // resets subsystems position
    printf("%d, %s's reset %lf\n", millis(), this->name, motor.get_position());
    motor.move(0);
  }

  void move_absolute(double position, double velocity = default_velocity, bool wait_for_comp = false, double end_error = 0.0){ // sets target and last target
    if (end_error == 0.0) end_error = this->end_error;
    motor.move_absolute(position, velocity);
    if (wait_for_comp) wait_until(fabs(motor.get_position() - position) < end_error);
  }

  void move(double speed){
    motor.move(speed);
  }

};
