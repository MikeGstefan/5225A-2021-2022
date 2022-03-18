#pragma once
#include "../Libraries/subsystem.hpp"
#include "../drive.hpp"
#include "../pid.hpp"
// #include "logging.hpp"
// #include "intake.hpp" // because lift controls intake

#define F_LIFT_STATE_LINE 2 // line on controller which "searching" and "lowered" lift states are printed on

#define NUM_OF_F_LIFT_STATES 5
#define F_LIFT_MAX_VELOCITY 100

#define NUM_OF_F_CLAW_STATES 4

enum class f_lift_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  bottom, // at lowest position
  idle,  // not doing anything
  move_to_target,  // moving to target position
  manual  // controlled by joystick
};
extern PID f_lift_pid;

class F_Lift: public Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, F_LIFT_MAX_VELOCITY> {
  Timer up_press{"Up_press"}, down_press{"Down_press"};
  int search_cycle_check_count = 0, bad_count = 0; // cycle check for safeties
  PID pid = PID(5.0,0.0,0.0,0.0);
  int lift_power; // for manual control

  // height conversion constants
  double offset_a = 365.0, offset_h = 9.75;
  double arm_len = 8.0;
  double gear_ratio = 5.0;
  int index, last_index;

public:
  vector<int> driver_positions = {20, 200, 475, 630, 800};
  vector<int> prog_positions = {20, 200, 475, 630, 800};

  const double bottom_position = 25.0, holding_position = 150.0, raised_position = 300.0, tall_dropoff_position = 475.0, platform_position = 630.0, tall_goal_position = 665.0, top_position = 675.0;

  F_Lift(Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, F_LIFT_MAX_VELOCITY> motorized_subsystem);  // constructor
  
  void handle(bool driver_array);  // contains state machine code, (if driver_array is false, uses prog_array)
  void handle_state_change(); // cleans up and preps the machine to be in the target state
  void button_handling(); // handles driver button input
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

// FRONT CLAW SUBSYSTEM

enum class f_claw_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  idle, // claw is open and NOT waiting to detect mogo
  searching,  // claw is open and waiting to detect mogo
  grabbed, // holding mogo
};

class F_Claw: public Subsystem<f_claw_states, NUM_OF_F_CLAW_STATES> {
  Timer release_timer{"release_timer"}; // when the f claw was last released

public:
  bool tilted = false;
  F_Claw(Subsystem<f_claw_states, NUM_OF_F_CLAW_STATES> subsystem);  // constructor
  void handle();  // contains state machine code
  void handle_state_change(); // cleans up and preps the machine to be in the target state
};

extern F_Claw f_claw;