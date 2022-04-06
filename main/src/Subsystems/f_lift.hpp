#pragma once
#include "../Libraries/subsystem.hpp"
#include "../drive.hpp"
#include "../pid.hpp"

#define NUM_OF_F_LIFT_STATES 5
#define F_LIFT_MAX_VELOCITY 100

enum class f_lift_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  bottom, // at lowest position
  idle,  // not doing anything
  move_to_target,  // moving to target position
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

public:
  uint8_t index, last_index;
  vector<int> driver_positions = {1200, 1500, 2050, 2750};
  //                            btm   carry plat  clear level top
  vector<int> prog_positions = {1170, 1350, 1950, 2020, 2300, 2750};

  F_Lift(Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, F_LIFT_MAX_VELOCITY> motorized_subsystem);  // constructor
  
  void handle(bool driver_array);  // contains state machine code, (if driver_array is false, uses prog_array)
  void handle_state_change(); // cleans up and preps the machine to be in the target state
  void handle_buttons(); // handles driver button input
  void set_state(const f_lift_states next_state);  // requests a state change and logs it (NORMAL set state)
  // THIS IS AN OVERLOAD for the existing set state function, accepts an index for the lift
  void set_state(const f_lift_states next_state, const double index);  // requests a state change and logs it
  
  // getters because index is private
  int get_index() const{
    return index;
  }
  int get_last_index()  const{
    return last_index;
  }
  
  void elastic_util(); // up time should be about 1100mms (ignore this time, it was on the old lift), down time should be slightly slower than that
};

extern F_Lift f_lift;

#define NUM_OF_F_CLAW_STATES 4

// FRONT CLAW SUBSYSTEM

enum class f_claw_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  idle, // claw is open and NOT waiting to detect mogo
  searching,  // claw is open and waiting to detect mogo
  grabbed, // holding mogo
};

class F_Claw: public Subsystem<f_claw_states, NUM_OF_F_CLAW_STATES> {

public:
  F_Claw(Subsystem<f_claw_states, NUM_OF_F_CLAW_STATES> subsystem);  // constructor
  void handle_buttons();  // handles driver button input
  void handle();  // contains state machine code
  void handle_state_change(); // cleans up and preps the machine to be in the target state
};

extern F_Claw f_claw_obj;