#include "intake.hpp"

// INTAKE SUBSYSTEM
Intake intake({{"Intake",
{
  "managed",
  "off",
  "on",
  "reversed",
  "unjamming"
}
}, intk});


Intake::Intake(Motorized_subsystem<intake_states, NUM_OF_INTAKE_STATES, INTAKE_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem)  // constructor
{
  // state setup
  target_state = intake_states::off;
  state = intake_states::managed;
}


void Intake::handle(){
  // turns intake off if lift is too low
  if(f_lift.motor.get_position() < f_lift.driver_positions[1] - 50 && state != intake_states::off){
    set_state(intake_states::off);
  }

  switch(state){
    case intake_states::managed:
      break;
      
    case intake_states::off:
      // doesn't allow driver to turn on intake if lift is at bottom
      if(f_lift.get_state() != f_lift_states::bottom){
        if(master.get_digital_new_press(intake_button)) set_state(intake_states::on);
        if(master.get_digital_new_press(intake_reverse_button)) set_state(intake_states::reversed);
      }
      break;

    case intake_states::on:
      if(master.get_digital_new_press(intake_button)) set_state(intake_states::off);
      if(master.get_digital_new_press(intake_reverse_button)) set_state(intake_states::reversed);
      // Intake jam handling
      if(intake_jam.get_new_press()) jam_timer.reset(); // Start timer when pressed
      else if(!intake_jam.get_value()) jam_timer.reset(false); // End timer when unpressed
      if(jam_timer.get_time() > 500){ // If pressed for more than 1 sec, reverse intk
        jam_timer.reset(false);
        set_state(intake_states::unjamming);
      }
      break;

    case intake_states::reversed:
      if(master.get_digital_new_press(intake_button)) set_state(intake_states::on);
      if(master.get_digital_new_press(intake_reverse_button)) set_state(intake_states::off);
      break;

    case intake_states::unjamming:
      if(master.get_digital_new_press(intake_button)) set_state(intake_states::on);
      if(master.get_digital_new_press(intake_reverse_button)) set_state(intake_states::off);
      // if the jam limit switch isn't triggered turn back on
      if(!intake_jam.get_value()) set_state(intake_states::on);
      break;
  }
  // set the motor to the appropriate power if the state has changed
  handle_state_change();
}

void Intake::handle_state_change(){
  if(target_state == state) return;
  // if state has changed, performs the necessary cleanup operation before entering next state
  // (set the motor to the appropriate power)
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
  }
  log_state_change();  
}