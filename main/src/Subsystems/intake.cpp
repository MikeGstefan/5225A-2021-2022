#include "intake.hpp"
#include "../config.hpp"
#include "../controller.hpp"
#include "f_lift.hpp"
#include "b_lift.hpp"

// INTAKE SUBSYSTEM
Intake intake({{"Intake",
{
  "managed",
  "off",
  "on",
  "reversed",
  "unjamming",
  "shifting_to_lift_up",
  "shifting_to_lift_down",
}, intake_states::managed, intake_states::off // goes from managed to off upon startup
}, b_lift_m});


Intake::Intake(Motorized_subsystem<intake_states, NUM_OF_INTAKE_STATES, INTAKE_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem)  // constructor
{}

void Intake::handle_buttons(){
  // toggles intake state if intake button is pressed
  if(master.is_rising(intake_button)){
    // doesn't allow driver to turn on intake if lift is at bottom
    if(f_lift.get_state() != f_lift_states::bottom){
      switch(get_state()){
        case intake_states::off:
          set_state(intake_states::on);
          break;
        case intake_states::reversed:
          set_state(intake_states::on);
          break;
        case intake_states::on:
          set_state(intake_states::off);
          break;
        case intake_states::unjamming:
          set_state(intake_states::on);
          break;
      }
    }
  }
  // toggles intake reverse state if intake reverse button is pressed
  if(master.is_rising(intake_reverse_button)){
    // doesn't allow driver to turn on intake if lift is at bottom
    if(f_lift.get_state() != f_lift_states::bottom){
      switch(get_state()){
        case intake_states::off:
          set_state(intake_states::reversed);
          break;
        case intake_states::on:
          set_state(intake_states::reversed);
          break;
        case intake_states::reversed:
          set_state(intake_states::off);
          break;
        case intake_states::unjamming:
          set_state(intake_states::off);
          break;
      }
    }
  }
}

void Intake::handle(){
  // turns intake off if front lift is too low
  if(f_lift_pot.get_value() < f_lift.driver_positions[1] - 50 && get_state() != intake_states::off){
    set_state(intake_states::off);
  }
  switch(get_state()){
    case intake_states::managed:
      break;
      
    case intake_states::off:
      break;

    case intake_states::on:
      // Intake jam handling
      // if(intake_jam.get_new_press()) jam_timer.reset(); // Start timer when pressed
      // else if(!intake_jam.get_value()) jam_timer.reset(false); // End timer when unpressed
      // if(jam_timer.get_time() > 500){ // If pressed for more than 1 sec, reverse intk
      //   jam_timer.reset(false);
      //   set_state(intake_states::unjamming);
      // }
      break;

    case intake_states::reversed:
      break;

    case intake_states::unjamming:
      // if the jam limit switch isn't triggered turn intake back on
      // if(!intake_jam.get_value()) set_state(intake_states::on);
      break;

    case intake_states::shifting_to_intake_up:
      if(fabs(b_lift_m.get_target_position() - b_lift_m.get_position()) < 15){
        set_state(intake_states::shifting_to_intake_down);
      }
      break;

    case intake_states::shifting_to_intake_down:
      // switches to desired state after transmission
      if(fabs(b_lift_m.get_target_position() - b_lift_m.get_position()) < 15){
        set_state(after_switch_state);
      }
      break;

  }
  // set the motor to the appropriate power if the state has changed
  handle_state_change();
}

void Intake::handle_state_change(){
  if(get_target_state() == get_state()) return;
  // if state has changed, performs the necessary cleanup operation before entering next state

  // shifts the transmission before activating intake in desired state
  if(get_target_state() != intake_states::off && lift_t.get_state()){
    after_switch_state = get_target_state();  // the state to reach after switching the transmission to intake mode
    set_state(intake_states::shifting_to_intake_up);
    // stops back lift if it's moving
    if(b_lift.get_state() == b_lift_states::move_to_target) b_lift.Subsystem::set_state(b_lift_states::between_positions);
  }
  switch(get_target_state()){
    case intake_states::managed:
      break;

    case intake_states::off:
      motor.move(0);
      break;
    
    case intake_states::on:
      motor.move(127);
      break;
    
    case intake_states::reversed:
      motor.move(-127);
      break;
    
    case intake_states::unjamming:
      motor.move(-127);
      break;
    
    case intake_states::shifting_to_intake_up:
      lift_t.set_state(LOW);
      motor.move_relative(-30, 100);
      break;
    
    case intake_states::shifting_to_intake_down:
      motor.move_relative(-30, 100);
      break;

  }
  log_state_change();  
}