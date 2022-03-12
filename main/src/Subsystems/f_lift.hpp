#pragma once
#include "../Libraries/subsystem.hpp"
#include "../drive.hpp"
#include "../pid.hpp"
// #include "intake.hpp" // because lift controls intake

#define F_LIFT_STATE_LINE 2 // line on controller which "searching" and "lowered" lift states are printed on

#define NUM_OF_F_LIFT_STATES 4
#define LIFT_MAX_VELOCITY 100

#define NUM_OF_F_CLAW_STATES 3

enum class f_lift_states{
  bottom, // at lowest position
  idle,  // not doing anything
  move_to_target,  // moving to target position
  manual  // controlled by joystick
};
extern PID f_lift_pid;

class F_Lift: public Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, LIFT_MAX_VELOCITY> {
  Timer up_press{"Up_press"}, down_press{"Down_press"};
  int search_cycle_check_count = 0, bad_count = 0; // cycle check for safeties
  PID pid = PID(5.0,0.0,0.0,0.0);
  int lift_power; // for manual control

  // height conversion constants
  double offset_a = 365.0, offset_h = 9.75;
  double arm_len = 8.0;
  double gear_ratio = 5.0;

public:
  int index, last_index;
  array<int, 5> positions = {20, 200, 475, 630, 800};

  const double bottom_position = 25.0, holding_position = 150.0, raised_position = 300.0, tall_dropoff_position = 475.0, platform_position = 630.0, tall_goal_position = 665.0, top_position = 675.0;

  F_Lift(Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, LIFT_MAX_VELOCITY> motorized_subsystem);  // constructor
  void handle();  // contains state machine code
  double pos_to_height(const double position);
  double height_to_pos(const double height);
  void elastic_util(); // up time should be about 1100mms, down time should be slightly slower than that
};

extern F_Lift f_lift;

// FRONT CLAW SUBSYSTEM

enum class f_claw_states{
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

};

extern F_Claw f_claw;