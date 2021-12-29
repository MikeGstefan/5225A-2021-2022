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
}

void Tilter::handle(){
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

      // if(tilter_dist.get() < 100){  // throws into searching state when distance sensor get triggered
      //   tilter_encoder_position = LeftEncoder.get_value()/360.0 *(2.75*M_PI);
      //   set_state(tilter_states::searching);
      // }
      if(master.get_digital_new_press(tilter_button)){  // grabs goal and raises tilter when tilter_button is pressed
        // lifting_timer
        tilter_bottom_piston.set_value(HIGH);
        tilter_top_piston.set_value(HIGH);
        delay(100);
        move_absolute(raised_position);
        set_state(tilter_states::raised);
      }
      break;
    case tilter_states::searching:
      // waits until robot thinks it hits the branch to grab the goal
      // if(LeftEncoder.get_value()/360.0 *(2.75*M_PI) - tilter_encoder_position > 2.0){
      //   move_absolute(raised_position);
      //   tilter_bottom_piston.set_value(HIGH);
      //   tilter_top_piston.set_value(HIGH);
      //   set_state(tilter_states::raised);
      // }
      // if(master.get_digital_new_press(tilter_button)){  // cancels search when tilter_button is pressed
      //   set_state(tilter_states::lowered);
      // }
      if(master.get_digital_new_press(tilter_button)){  // grabs goal and raises tilter when tilter_button is pressed
        lifting_timer.reset();
        tilter_bottom_piston.set_value(HIGH);
        tilter_top_piston.set_value(HIGH);
      }
      if(lifting_timer.get_time() > 100){
        lifting_timer.reset(false); // pauses timer
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
        tilter_bottom_piston.set_value(LOW);
        tilter_top_piston.set_value(LOW);
        set_state(tilter_states::searching);
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
