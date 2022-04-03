#include "intake.hpp"

// INTAKE SUBSYSTEM
Intake intake({{"Intake",
{
  "managed",
  "off",
  "on",
  "reversed",
  "unjamming",
  "shifting_to_lift"
}
}, b_lift_m});


Intake::Intake(Motorized_subsystem<intake_states, NUM_OF_INTAKE_STATES, INTAKE_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem)  // constructor
{
  // state setup
  target_state = intake_states::off;
  state = intake_states::managed;
}

void Intake::handle_buttons(){
  // toggles intake state if intake button is pressed
  if(master.get_digital_new_press(intake_button)){
    // doesn't allow driver to turn on intake if lift is at bottom
    if(f_lift.get_state() != f_lift_states::bottom){
      if(state == intake_states::off)  set_state(intake_states::on);
      else if(state == intake_states::on)  set_state(intake_states::off);
      else if(state == intake_states::unjamming)  set_state(intake_states::on);
    }
  }
  // toggles intake reverse state if intake reverse button is pressed
  if(master.get_digital_new_press(intake_reverse_button)){
    // doesn't allow driver to turn on intake if lift is at bottom
    if(f_lift.get_state() != f_lift_states::bottom){
      if(state == intake_states::off)  set_state(intake_states::reversed);
      else if(state == intake_states::reversed)  set_state(intake_states::off);
      else if(state == intake_states::unjamming)  set_state(intake_states::off);
    }
  }
}

void Intake::handle(){
  // turns intake off if front lift is too low
  if(f_lift_pot.get_value() < f_lift.driver_positions[1] - 50 && state != intake_states::off){
    set_state(intake_states::off);
  }
  switch(state){
    case intake_states::managed:
      break;
      
    case intake_states::off:
      break;

    case intake_states::on:
      // Intake jam handling
      if(intake_jam.get_new_press()) jam_timer.reset(); // Start timer when pressed
      else if(!intake_jam.get_value()) jam_timer.reset(false); // End timer when unpressed
      if(jam_timer.get_time() > 500){ // If pressed for more than 1 sec, reverse intk
        jam_timer.reset(false);
        set_state(intake_states::unjamming);
      }
      break;

    case intake_states::reversed:
      break;

    case intake_states::unjamming:
      // if the jam limit switch isn't triggered turn intake back on
      if(!intake_jam.get_value()) set_state(intake_states::on);
      break;

    case intake_states::shifting_to_intake:
      // switches to desired state after the transmission
      if(shift_timer.get_time() > 200){
        set_state(after_switch_state);
      }
      break;

  }
  // set the motor to the appropriate power if the state has changed
  handle_state_change();
}

void Intake::handle_state_change(){
  if(target_state == state) return;
  // if state has changed, performs the necessary cleanup operation before entering next state

  // shifts the transmission before activating intake in desired state
  if(target_state != intake_states::off && b_lift.trans_p_state){
    after_switch_state = target_state;  // the state to reach after switching the transmission to intake mode
    set_state(intake_states::shifting_to_intake);
  }
  switch(target_state){
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
    
    case intake_states::shifting_to_intake:
      shift_timer.reset();
      motor.move(30);
      lift_t.set_value(LOW);
      b_lift.trans_p_state = LOW;
      break;
  }
  log_state_change();  
}