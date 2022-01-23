#include "spinner.hpp"

// Spinner object
Spinner spinner({{"Spinner",
{
  "idle",
  "prep",
  "release_1",
  "lowering",
  "release_2",
}
}, spinner_motor});

Spinner::Spinner(Motorized_subsystem<spinner_states, NUM_OF_SPINNER_STATES, SPINNER_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = spinner_states::idle;
  last_state = state;

}

void Spinner::handle(){

  // if (fabs(target - motor.get_position()) > end_error && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
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

  switch(state){

    case spinner_states::idle:
      break;

    case spinner_states::prep:
      // waits for lift and tilter to reach their respective positions
      if(fabs(lift.top_position - lift.motor.get_position()) < lift.end_error && fabs(tilter.top_position - tilter.motor.get_position()) < tilter.end_error){
        motor.move(127);

        set_state(spinner_states::aligning_1);
      }
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
      if(releasing_timer.get_time() > 200){  // waits 200 ms for rings to fall
        motor.move(127);
        ring_piston.set_value(HIGH); // holds remaining rings

        set_state(spinner_states::aligning_2);
      }
      break;

    case spinner_states::aligning_2:
      if(spinner_trigger.get_value()){   // waits for spinner's touch sensor to trigger
        motor.move(0);
        releasing_timer.reset();
        ring_piston.set_value(LOW); // releases 2nd batch of rings

        set_state(spinner_states::release_2);
      }
      break;

    case spinner_states::release_2:
      if(releasing_timer.get_time() > 200){  // waits 200 ms for rings to fall
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
