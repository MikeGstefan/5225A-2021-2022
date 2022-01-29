#include "tilter.hpp"

// high closes top, opens bottom

// Tilter object
Tilter tilter({{"Tilter",
{
  "lowered",
  "raised",
  "lowering",
  "tall_goal",
  "manual"
}
}, tilter_motor});

Tilter::Tilter(Motorized_subsystem<tilter_states, NUM_OF_TILTER_STATES, TILTER_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = tilter_states::lowered;
  last_state = state;
  target = bottom_position;
  last_target = target;

  held = false; // for manual control testing
}

void Tilter::handle(){

  if (fabs(target - motor.get_position()) > end_error && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
  else bad_count = 0;
  if(bad_count > 25 && state != tilter_states::manual){
    motor.move(0);
    master.rumble("---");
    printf("TILTER SAFETY TRIGGERED\n");
    set_state(tilter_states::manual);
  }

  // switches to manual control if tilter manual button is pressed
  if(master.get_digital_new_press(tilter_manual_button))  set_state(tilter_states::manual);

  switch(state){

    case tilter_states::lowered:
      if(master.get_digital_new_press(tilter_raise_button)){  // lifts tilter out of the way (not holding a mogo)
        move_absolute(raised_position);

        set_state(tilter_states::raised);
      }
      else if(master.get_digital_new_press(tilter_search_button)){  // grabs goal and raises tilter when tilter_button is pressed
        printf("pressed\n");
        Timer cur{"auto-pickup"};
        drivebase.move(0.0, 80.0, 0.0);
        while(true){ // waits until distance sensor detects mogo
          // cancels search if button is pressed or times out
          if(master.get_digital_new_press(tilter_search_button) || cur.get_time() > 1000){
            break;  // cancels search
          }

          // lifts goal when distance sensor detects it
          if(tilter_dist.get() < 100){
            tilter_top_piston.set_value(LOW);
            delay(200); // waits for top piston to fully close
            tilter_bottom_piston.set_value(HIGH);
            delay(300); // waits for bottom piston to fully close

            held = true;
            move_absolute(raised_position); // raises mogo

            set_state(tilter_states::raised);
            break;
          }

          delay(10);
        }

      }
      break;

    case tilter_states::raised:
      // lowers tilter to bottom when either tilter button is pressed
      if(master.get_digital_new_press(tilter_raise_button) || master.get_digital_new_press(tilter_search_button)){
        move_absolute(bottom_position);

        set_state(tilter_states::lowering);
      }
      if(master.get_digital_new_press(fill_top_goal_button)){ // throws all subsystems into managed states except spinner
        intake.raise_and_disable();
        delay(100);

        spinner.set_state(spinner_states::prep);

        lift.move_absolute(lift.tall_goal_position);
        lift.set_state(lift_states::tall_goal);

        move_absolute(tall_goal_position);

        set_state(tilter_states::tall_goal);
      }
      break;

    case tilter_states::lowering:
      if(master.get_digital_new_press(tilter_search_button)){  // raises tilter when tilter_button is pressed
        move_absolute(raised_position);

        set_state(tilter_states::raised);
      }
      if(fabs(tilter_motor.get_position() - bottom_position) < end_error){  // releases goal once tilter reaches bottom
        tilter_top_piston.set_value(HIGH);
        tilter_bottom_piston.set_value(LOW);

        held = false;

        set_state(tilter_states::lowered);
      }
      break;

    case tilter_states::tall_goal:

      break;

    case tilter_states::manual:
      tilter_power = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
      // gives holding power if joystick is within deadzone or tilter is out of range
      if (fabs(tilter_power) < 10 || (tilter_power < 0 && motor.get_position() >= bottom_position) || (tilter_power > 0 && motor.get_position() <= top_position)) tilter_power = -10;

      motor.move(tilter_power);

      if(master.get_digital_new_press(tilter_search_button)){ // toggles holding state if tilter button is pressed
        if(held){
          tilter_bottom_piston.set_value(LOW);
          tilter_top_piston.set_value(HIGH);
          held = false;
        }
        else{
          tilter_bottom_piston.set_value(HIGH);
          tilter_top_piston.set_value(LOW);
          held = true;
        }
      }

      if(master.get_digital_new_press(tilter_manual_button)){ // escapes manual control if manual button is pressed again
        bad_count = 0;  // resets the safety
        move_absolute(bottom_position);

        set_state(tilter_states::lowering);
      }
      break;

  }

}
