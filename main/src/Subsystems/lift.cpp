#include "lift.hpp"

// Lift object
Lift lift({{"Lift",
{
  "searching",
  "lowered",
  "grabbed",
  "raised",
  "platform",
  "level_platform_prep",
  "level_platform",
  "dropoff",
  "manual",
}
}, lift_motor});

Lift::Lift(Motorized_subsystem<lift_states, NUM_OF_LIFT_STATES, LIFT_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = lift_states::searching;
  last_state = state;
  released_cycle_check = 0;
  target = bottom_position;
  last_target = target;
  intake_on = false;
}

void Lift::handle(){
  if (fabs(target - motor.get_position()) > end_error && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
  else bad_count = 0;
  if(bad_count > 25 && state != lift_states::manual){
    motor.move(0);
    delay(50);
    master.rumble("---");
    delay(50);
    printf("LIFT SAFETY TRIGGERED\n");
    set_state(lift_states::manual);
    intake.move(0); // disables intake lift safety is triggered
    intake_on = false;

  }

  switch(state){
    case lift_states::searching:
      // grabs goal if limit switch is triggered or if up button is pressed
      if(lift_trigger.get_value() || master.get_digital_new_press(lift_up_button)){
        delay(50);
        master.rumble("-");
        delay(50);
        printf("*******************triggered\n");
        lift_piston.set_value(HIGH);
        set_state(lift_states::grabbed);
      }
      // switches state to lowered if down button is pressed
      if(master.get_digital_new_press(lift_down_button)){
        master.print(2, 0, "lowered     ");
        // master.rumble("-");
        set_state(lift_states::lowered);
      }
      if(master.get_digital_new_press(intake_button)){  // toggles intake
        if(intake_on){
          intake.move(0);
          intake_on = false;
        }
        else{
          intake.move(100);
          intake_on = true;
        }
      }
      break;

    case lift_states::lowered:
      // grabs goal if up button is pressed
      if(master.get_digital_new_press(lift_up_button)){
        lift_piston.set_value(HIGH);
        set_state(lift_states::grabbed);
      }
      // switches state to searching if down button is pressed
      if(master.get_digital_new_press(lift_down_button)){
        master.print(2, 0, "searching     ");
        // master.rumble("-");
        set_state(lift_states::searching);
      }
      if(master.get_digital_new_press(intake_button)){  // toggles intake
        if(intake_on){
          intake.move(0);
          intake_on = false;
        }
        else{
          intake.move(100);
          intake_on = true;
        }
      }
      break;

    case lift_states::grabbed:
      if(master.get_digital_new_press(lift_up_button)){ // lifts goal to platform height if up button is pressed
        move_absolute(raised_position, 100);
        set_state(lift_states::raised);

        intake.move(0);
        intake_on = false;
        intake_piston.set_value(HIGH); // raises intake when lift is lifting

      }
      // releases goal if down button is pressed
      if(master.get_digital_new_press(lift_down_button) && fabs(bottom_position - motor.get_position()) < end_error){
        lift_piston.set_value(LOW);
        master.print(2, 0, "lowered     ");
        set_state(lift_states::lowered);
      }
      if(master.get_digital_new_press(level_platform_button)){  // moves to top position if level platform button is pressed
        move_absolute(top_position);
        set_state(lift_states::level_platform_prep);
      }
      if(master.get_digital_new_press(intake_button)){  // toggles intake
        if(intake_on){
          intake.move(0);
          intake_on = false;
        }
        else{
          intake.move(100);
          intake_on = true;
        }
      }
      break;

    case lift_states::raised:
      if(master.get_digital_new_press(level_platform_button)){  // moves to top position if level platform button is pressed
        move_absolute(top_position);
        set_state(lift_states::level_platform_prep);
      }
      if(master.get_digital_new_press(lift_up_button)){ // lifts goal to platform height if up button is pressed
        move_absolute(platform_position);
        set_state(lift_states::platform);
      }
      if(master.get_digital_new_press(lift_down_button)){ // lowers goal if down button is pressed
        move_absolute(bottom_position);
        set_state(lift_states::grabbed);

        intake_piston.set_value(LOW); // lowers intake once lift is down
      }
      break;

    case lift_states::platform:
      if(master.get_digital_new_press(level_platform_button)){  // moves to top position if level platform button is pressed
        move_absolute(top_position);
        set_state(lift_states::level_platform_prep);
      }
      // drops off goal if up button is pressed
      if(master.get_digital_new_press(lift_up_button) && fabs(motor.get_position() - platform_position) < end_error){
        lift_piston.set_value(LOW);
        set_state(lift_states::dropoff);
      }
      if(master.get_digital_new_press(lift_down_button)){ // lowers goal if down button is pressed
        move_absolute(raised_position);
        set_state(lift_states::raised);
      }
      break;

    case lift_states::level_platform_prep:
      // goes to platform height if up or down buttons are pressed
      if(master.get_digital_new_press(lift_up_button) || master.get_digital_new_press(lift_down_button)){
        move_absolute(platform_position);
        set_state(lift_states::platform);
      }
      // sets state to level platform once lift reaches top position
      if(fabs(motor.get_position() - top_position) < end_error) set_state(lift_states::level_platform);
      break;

    case lift_states::level_platform:
      // goes to platform height if any of the lift buttons are pressed
      if(master.get_digital_new_press(lift_up_button) || master.get_digital_new_press(lift_down_button) || master.get_digital_new_press(level_platform_button)){
        move_absolute(platform_position);
        set_state(lift_states::platform);
      }
      break;

    case lift_states::dropoff:
      // releases goal if up or down button is pressed
      if(master.get_digital_new_press(lift_up_button) || master.get_digital_new_press(lift_down_button)){
        move_absolute(bottom_position);
        set_state(lift_states::searching);
        intake_piston.set_value(LOW); // lowers intake
      }
      break;

    case lift_states::manual:
      lift_power = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
      // makes sure lift stays within bounds
      if (lift_power < 0 && motor.get_position() <= bottom_position) lift_power = 0;
      if (lift_power > 0 && motor.get_position() >= top_position) lift_power = 0;
      motor.move(lift_power);
      if(master.get_digital_new_press(lift_up_button) || master.get_digital_new_press(lift_down_button)){
        bad_count = 0;  // resets the safety
        lift_piston.set_value(LOW);
        move_absolute(bottom_position);
        set_state(lift_states::searching);

      }
      break;

  }
}

double Lift::pos_to_height(double pos){
  return offset_h + arm_len * sin(deg_to_rad((pos - offset_a) / gear_ratio));
}

double Lift::height_to_pos(double height){
  return gear_ratio * (rad_to_deg(asin((height - offset_h) / (arm_len)))) + offset_a;
}
