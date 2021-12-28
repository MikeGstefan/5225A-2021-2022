#include "angler.hpp"

// Angler object
Angler angler({{"Angler",
{
  "lowered",
  "searching",
  "raising",
  "lowering",
  "manual"
}
}, angler_motor});

Angler::Angler(Motorized_subsystem<angler_states, NUM_OF_ANGLER_STATES, ANGLER_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = angler_states::lowered;
  last_state = state;
}

void Angler::handle(){
  if (fabs(motor.get_voltage()) > 2000 && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
  else bad_count = 0;
  if(bad_count > 50){
    printf("LIFT SAFETY TRIGGERED\n");
    motor.move(0);
    set_state(angler_states::manual);
  }

  switch(state){
    case angler_states::lowered:
      if(angler_dist.get() < 100){  // throws into searching state when distance sensor get triggered
        angler_encoder_position = LeftEncoder.get_value()/360.0 *(2.75*M_PI);
        set_state(angler_states::searching);
      }
      if(master.get_digital_new_press(angler_button)){  // grabs goal and raises angler when angler_button is pressed
        move_absolute(raised_position);
        angler_piston.set_value(HIGH);
        set_state(angler_states::raised);
      }
      break;
    case angler_states::searching:
      // waits until robot thinks it hits the branch to grab the goal
      if(LeftEncoder.get_value()/360.0 *(2.75*M_PI) - angler_encoder_position > 2.0){
        move_absolute(raised_position);
        angler_piston.set_value(HIGH);
        set_state(angler_states::raised);
      }
      if(master.get_digital_new_press(angler_button)){  // cancels search when angler_button is pressed
        set_state(angler_states::lowered);
      }
      break;

    case angler_states::raised:
      if(master.get_digital_new_press(angler_button)){  // lowers angler to bottom when angler_button is pressed
        move_absolute(bottom_position);
        set_state(angler_states::lowering);
      }
      break;

    case angler_states::lowering:
      if(master.get_digital_new_press(angler_button)){  // raises angler when angler_button is pressed
        move_absolute(raised_position);
        set_state(angler_states::raised);
      }
      if(fabs(angler_motor.get_position() - bottom_position) < end_error){  // releases goal once angler reaches bottom
        angler_piston.set_value(LOW);
        set_state(angler_states::searching);
      }
      break;
    case angler_states::manual:
      int angler_power = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
      if (angler_power < 0 && motor.get_position() <= bottom_position) angler_power = 0;
      if (angler_power > 0 && motor.get_position() >= top_position) angler_power = 0;
      motor.move(angler_power);
      if(master.get_digital_new_press(angler_button)){
        motor.move_absolute(bottom_position, 100);
        set_state(angler_states::lowered);
      }
      break;
  }

}
