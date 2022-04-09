#pragma once
#include "../Libraries/subsystem.hpp"
#include "../drive.hpp"
#include "../pid.hpp"
#include <atomic>


#define NUM_OF_F_LIFT_STATES 6
#define F_LIFT_MAX_VELOCITY 100

enum class f_lift_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  bottom, // at lowest position
  idle,  // not doing anything
  move_to_target,  // moving to target position
  between_positions,  // not moving, but in between positions in the array
  manual  // controlled by joystick
};

class F_Lift: public Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, F_LIFT_MAX_VELOCITY> {
  Timer up_press{"Up_press"}, down_press{"Down_press"};
  int search_cycle_check_count = 0, bad_count = 0; // cycle check for safeties
  PID pid = PID(1.0,0.0,0.0,0.0);
  int lift_power; // for manual control

  // height conversion constants
  double offset_a = 365.0, offset_h = 9.75;
  double arm_len = 8.0;
  double gear_ratio = 5.0;

  // to avoid race conditions these are atomic
  std::atomic<uint8_t> index{0};
  std::atomic<int32_t> speed{127}; // max pwm applied to the lifts during a move to target

public:

  vector<int> driver_positions = {1200, 1500, 2050, 2750};
  vector<int> prog_positions = {1200, 1500, 2050, 2750};

  F_Lift(Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, F_LIFT_MAX_VELOCITY> motorized_subsystem);  // constructor
  
  void handle(bool driver_array);  // contains state machine code, (if driver_array is false, uses prog_array)
  void handle_state_change(); // cleans up and preps the machine to be in the target state
  // void handle_buttons(); // handles driver button input
  // THIS IS AN OVERLOAD for the existing set state function, accepts an index for the lift
  void set_state(const f_lift_states next_state, const uint8_t index, const int32_t speed = 127);  // requests a state change and logs it
  
  // getter because index is private
  int get_index() const{
    return index.load();
  }
  
  void elastic_util(); // up time should be about 1100mms (ignore this time, it was on the old lift), down time should be slightly slower than that
};

extern F_Lift f_lift;

#define NUM_OF_F_CLAW_STATES 5

// FRONT CLAW SUBSYSTEM

enum class f_claw_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  idle, // claw is open and NOT waiting to detect mogo
  about_to_search, // claw is open and will search in 2 seconds
  searching,  // claw is open and waiting to detect mogo
  grabbed, // holding mogo
};

class F_Claw: public Subsystem<f_claw_states, NUM_OF_F_CLAW_STATES> {
  Timer search_timer{"Search_timer"};
public:
  F_Claw(Subsystem<f_claw_states, NUM_OF_F_CLAW_STATES> subsystem);  // constructor
  void handle();  // contains state machine code
  void handle_state_change(); // cleans up and preps the machine to be in the target state
};

extern F_Claw f_claw_obj;