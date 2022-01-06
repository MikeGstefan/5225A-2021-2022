#include "tilter.hpp"

// Tilter object
Tilter tilter({{"Tilter",
{
  "lowered",
  "searching",
  "raised",
  "lowering",
  "manual"
}
}, tilter_motor});

Tilter::Tilter(Motorized_subsystem<tilter_states, NUM_OF_TILTER_STATES, TILTER_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = tilter_states::lowered;
  last_state = state;
  target = bottom_position;
  last_target = target;

  raised_position = 50.0;
  held = false;
}

void Tilter::handle(){

// analog control
/*
  if(master.get_digital_new_press(tilter_button)){
    if(held){
      tilter_bottom_piston.set_value(LOW);
      tilter_top_piston.set_value(LOW);
      held = false;
    }
    else{
      tilter_bottom_piston.set_value(HIGH);
      tilter_top_piston.set_value(HIGH);
      held = true;
    }

  }
  int tilter_power = -master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
  // if (fabs(tilter_power) < 10) motor.move_relative(0, 100); // holds position
  if (fabs(tilter_power) < 10) motor.move(-10); // holds position
  else motor.move(tilter_power);
*/


  if (fabs(target - motor.get_position()) > end_error && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
  else bad_count = 0;
  if(bad_count > 25 && state != tilter_states::manual){
    motor.move(0);
    delay(50);
    master.rumble("---");
    delay(50);
    printf("TILTER SAFETY TRIGGERED\n");
    set_state(tilter_states::manual);
  }

  switch(state){
    case tilter_states::lowered:

      if(master.get_digital_new_press(tilter_button)){  // grabs goal and raises tilter when tilter_button is pressed
        tilter_top_piston.set_value(HIGH);
        delay(100); // waits for top piston to fully close
        tilter_bottom_piston.set_value(LOW);
        move_absolute(raised_position);
        set_state(tilter_states::raised);
      }
      break;

    case tilter_states::raised:
      if(master.get_digital_new_press(tilter_button)){  // lowers tilter to bottom when tilter_button is pressed
        move_absolute(bottom_position);
        set_state(tilter_states::lowering);
      }
      break;

    case tilter_states::lowering:
      if(master.get_digital_new_press(tilter_button)){  // raises tilter when tilter_button is pressed
        move_absolute(raised_position);
        set_state(tilter_states::raised);
      }
      if(fabs(tilter_motor.get_position() - bottom_position) < end_error){  // releases goal once tilter reaches bottom
        tilter_bottom_piston.set_value(HIGH);
        tilter_top_piston.set_value(LOW);
        set_state(tilter_states::lowered);
      }
      break;
      
    case tilter_states::manual:
      tilter_power = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
      if (tilter_power < 0 && motor.get_position() >= bottom_position) tilter_power = 0;
      if (tilter_power > 0 && motor.get_position() <= top_position) tilter_power = 0;
      motor.move(tilter_power);
      if(master.get_digital_new_press(tilter_button)){
        bad_count = 0;  // resets the safety
        motor.move_absolute(bottom_position, 100);
        set_state(tilter_states::lowered);
      }
      break;
  }

}
