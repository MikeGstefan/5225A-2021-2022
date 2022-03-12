#include "intake.hpp"

std::array<const char*, 4> intake_state_names = {
  "off",
  "on",
  "reversed",
  "unjamming"
};

intake_states target_state = intake_states::off;
intake_states state = target_state; // this exists only for intake
Timer jam_timer{"Jam_timer"}; // for jam detection

void intake_set_state(intake_states next_state){
  motion_d.print("Intake | Going from %s to %s\n", intake_state_names[static_cast<int>(state)], intake_state_names[static_cast<int>(next_state)]);
  target_state = next_state;
}

void intake_handle(void* ignore){
  // turns intake off if lift is too low
  if(f_lift.motor.get_position() < f_lift.positions[1] - 50 && state != intake_states::off){
    intake_set_state(intake_states::off);
    intk.move(0);
  }

  switch(state){
    case intake_states::off:
    // doesn't allow driver to turn on intake if lift is at bottom
    if(f_lift.get_state() != f_lift_states::bottom){
      if(master.get_digital_new_press(intake_button)){  // turns on intake if intake button is pressed
        intake_set_state(intake_states::on);
      }
      if(master.get_digital_new_press(intake_reverse_button)){  // turns intake in reverse if reverse button is pressed
        intake_set_state(intake_states::reversed);
      }
    }
      break;

    case intake_states::on:
      if(master.get_digital_new_press(intake_button)){  // turns off intake if intake button is pressed
        intake_set_state(intake_states::off);
      }
      if(master.get_digital_new_press(intake_reverse_button)){  // turns intake in reverse if reverse button is pressed
        intake_set_state(intake_states::reversed);
      }
      // Intake jam handling
      if(intake_jam.get_new_press()) jam_timer.reset(); // Start timer when pressed
      else if(!intake_jam.get_value()) jam_timer.reset(false); // End timer when unpressed
      if(jam_timer.get_time() > 500){ // If pressed for more than 1 sec, reverse intk
        jam_timer.reset(false);
        intake_set_state(intake_states::unjamming);
      }
      break;

    case intake_states::reversed:
      if(master.get_digital_new_press(intake_button)){  // turns on intake if intake button is pressed
        intake_set_state(intake_states::on);
      }
      if(master.get_digital_new_press(intake_reverse_button)){  // turns intake off if reverse button is pressed
        intake_set_state(intake_states::off);
      }
      break;
    case intake_states::unjamming:
      if(master.get_digital_new_press(intake_button)){  // turns on intake if intake button is pressed
        intake_set_state(intake_states::on);
      }
      if(master.get_digital_new_press(intake_reverse_button)){  // turns intake off if reverse button is pressed
        intake_set_state(intake_states::off);
      }
      // if the jam limit switch isn't triggered turn back on
      if(!intake_jam.get_value()){
        delay(100);
        intake_set_state(intake_states::on);
      }
      break;
  }
  // set the motor to the appropriate power if the state has changed
  intake_target_state_handle();
}

void intake_target_state_handle(){
  // if the state changed set the motor to the appropriate power
  if(target_state != state){
    switch(target_state){
      case intake_states::off:
        intk.move(0);
        break;
      case intake_states::on:
        intk.move(127);
        break;
      case intake_states::reversed:
        intk.move(-127);
        break;
      case intake_states::unjamming:
        intk.move(-127);
        break;
    }
    state = target_state; // updates state
  }
}