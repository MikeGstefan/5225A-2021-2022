#pragma once
#include "../Libraries/subsystem.hpp"
#include "../drive.hpp"
#include "../pid.hpp"

#define NUM_OF_B_LIFT_STATES 7
#define B_LIFT_MAX_VELOCITY 100

enum class b_lift_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  bottom, // at lowest position
  idle,  // not doing anything
  move_to_target,  // moving to target position
  manual,  // controlled by joystick
  shifting_to_lift_up, // lift/intake transmission switching to lift mode, moving up
  shifting_to_lift_down // lift/intake transmission switching to lift mode, moving down
};

class B_Lift: public Motorized_subsystem<b_lift_states, NUM_OF_B_LIFT_STATES, B_LIFT_MAX_VELOCITY> {
  Timer up_press{"Up_press"}, down_press{"Down_press"};
  b_lift_states after_switch_state; // the state the lift will go to after transmission switches to lift
  int search_cycle_check_count = 0, bad_count = 0; // cycle check for safeties
  PID pid = PID(1.0,0.0,0.0,0.0);
  int lift_power; // for manual control

  // height conversion constants
  double offset_a = 365.0, offset_h = 9.75;
  double arm_len = 8.0;
  double gear_ratio = 5.0;

public:
  int index, last_index;
  vector<int> driver_positions = {1042, 1500, 2050, 2750};
  vector<int> prog_positions = {1042, 1500, 2050, 2750};


  B_Lift(Motorized_subsystem<b_lift_states, NUM_OF_B_LIFT_STATES, B_LIFT_MAX_VELOCITY> motorized_subsystem);  // constructor
  
  void handle(bool driver_array);  // contains state machine code, (if driver_array is false, uses prog_array)
  void handle_state_change(); // cleans up and preps the machine to be in the target state
  void handle_buttons(); // handles driver button input
  void set_state(const b_lift_states next_state);  // requests a state change and logs it (NORMAL set state)
  // THIS IS AN OVERLOAD for the existing set state function, accepts an index for the lift
  void set_state(const b_lift_states next_state, const double index);  // requests a state change and logs it
  
  // getters because index is private
  int get_index() const{
    return index;
  }
  int get_last_index()  const{
    return last_index;
  }
  
  void elastic_util(); // up time should be about 1100mms (ignore this time, it was on the old lift), down time should be slightly slower than that
};

extern B_Lift b_lift;

#define NUM_OF_B_CLAW_STATES 5

// FRONT CLAW SUBSYSTEM

enum class b_claw_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  idle, // claw is open and NOT waiting to detect mogo
  searching,  // claw is open and is waiting to detect mogo
  tilted, // holding mogo tilted
  flat, // holding mogo flat
};

class B_Claw: public Subsystem<b_claw_states, NUM_OF_B_CLAW_STATES> {
  Timer toggle_press_timer{"Toggle_timer"};  // when the toggle buttons was last pressed
  int search_cycle_check_count = 0; // for searching for mogo's lip

public:
  B_Claw(Subsystem<b_claw_states, NUM_OF_B_CLAW_STATES> subsystem);  // constructor
  void handle_buttons(); // handles driver button input
  void handle();  // contains state machine code
  void handle_state_change(); // cleans up and preps the machine to be in the target state
};

extern B_Claw b_claw_obj;