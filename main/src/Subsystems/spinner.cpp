#include "spinner.hpp"

// Spinner object
Spinner spinner({{"Spinner",
{
  "idle",
  "prep",
  "aligning_1",
  "release_1",
  "wait_for_arms",
  "aligning_2",
  "wait_for_arms_2",
  "release_2",
}
}, spinner_motor});

// intake position 250

Spinner::Spinner(Motorized_subsystem<spinner_states, NUM_OF_SPINNER_STATES, SPINNER_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = spinner_states::idle;
  last_state = state;

}

void Spinner::handle(){

  // if (fabs(target - motor.get_position()) > 10 && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
  // else bad_count = 0;
  // if(bad_count > 25 && state != tilter_states::manual){
  //   motor.move(0);
  //   master.rumble("---");
  //   printf("TILTER SAFETY TRIGGERED\n");
  //   set_state(tilter_states::manual);
  // }


  if(state != spinner_states::idle && master.get_digital_new_press(fill_top_goal_button)){  // cancels ring dropoff if dropoff button is pressed
    // resets all subsystems to last states
    lift.move_absolute(lift.platform_position);
    lift.set_state(lift_states::platform);
    tilter.move_absolute(tilter.raised_position);
    tilter.set_state(tilter_states::raised);
    intake.set_state(intake_states::off);
    motor.move(0);
    master.rumble("-");

    set_state(spinner_states::idle);

  }
  bool open = false;
  switch(state){

    case spinner_states::idle:
      break;

    case spinner_states::prep:
      // waits for lift and tilter to reach their respective positions
      printf("lift: %lf, tilter: %lf \n", lift.motor.get_position(), tilter.motor.get_position());
      if(fabs(lift.tall_goal_position - lift.motor.get_position()) < 10 && fabs(tilter.tall_goal_position - tilter.motor.get_position()) < 10){
        // waitUntil(false);
        // motor.move(127);
        // tilter_bottom_piston.set_value(0);
        // motor.move(50);
        while(true){ 
          if(spinner_trigger.get_value())motor.move(0);
          else motor.move(50);
          if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_X) && !open){
            open = true;
            ring_piston.set_value(LOW);
          }
          if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_X) && open){
            open = false;
            ring_piston.set_value(0);
            motor.move(50);
            delay(200);
          }
          delay(10);
        }

        set_state(spinner_states::aligning_1);
      }
      break;
    case spinner_states::next_side:
    break;
    case spinner_states::aligning_1:
      if(spinner_trigger.get_value()){   // waits for spinner's touch sensor to trigger
        motor.move(0);
        releasing_timer.reset();
        ring_piston.set_value(LOW); // releases 2st batch of rings

        set_state(spinner_states::release_1);
      }
      break;

    case spinner_states::release_1:
      if(releasing_timer.get_time() > 500){  // waits 200 ms for rings to fall
        ring_piston.set_value(HIGH); // holds remaining rings
        lift.move_absolute(lift.tall_goal_position - 50);
        tilter.move_absolute(tilter.tall_goal_position + 50);

        set_state(spinner_states::wait_for_arms);
      }
      break;

    case spinner_states::wait_for_arms:
      if(fabs(lift.motor.get_position() - (lift.tall_goal_position - 50)) < 10){
        if(fabs(tilter.motor.get_position() - (tilter.tall_goal_position + 50)) < 10){
          motor.move(127);

          set_state(spinner_states::aligning_2);
        }
      }
      break;

    case spinner_states::aligning_2:
      if(spinner_trigger.get_value()){   // waits for spinner's touch sensor to trigger
        lift.move_absolute(lift.tall_goal_position);
        tilter.move_absolute(tilter.tall_goal_position);

        set_state(spinner_states::wait_for_arms_2);
      }
      break;

    case spinner_states::wait_for_arms_2:
      if(fabs(lift.motor.get_position() - lift.tall_goal_position) < 10){
        if(fabs(tilter.motor.get_position() - tilter.tall_goal_position) < 10){
          motor.move(0);
          releasing_timer.reset();
          ring_piston.set_value(LOW); // releases 2nd batch of rings

          set_state(spinner_states::release_2);
        }
      }
      break;

    case spinner_states::release_2:
      if(releasing_timer.get_time() > 500){  // waits 200 ms for rings to fall
        // resets all subsystems to last states
        lift.move_absolute(lift.platform_position);
        lift.set_state(lift_states::platform);
        tilter.move_absolute(tilter.raised_position);
        tilter.set_state(tilter_states::raised);
        intake.set_state(intake_states::off);
        motor.move(0);
        master.rumble("-");

        set_state(spinner_states::idle);
      }
      break;
  }

}
