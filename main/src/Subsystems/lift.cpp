#include "lift.hpp"

// Lift object
Lift lift({{"Lift",
{
  "idle",
  "searching",
  "grabbed",
  "raised",
  "platform",
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
}

void Lift::handle(){
  if (fabs(target - motor.get_position()) > end_error && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
  else bad_count = 0;
  if(bad_count > 25 && state != lift_states::manual){
    motor.move(0);
    master.rumble("---");
    printf("LIFT SAFETY TRIGGERED\n");
    intake_motor.move(0); // disables intake if lift safety is triggered
    intake.set_state(intake_states::off);

    set_state(lift_states::manual);
  }

  // switches to manual control if lift manual button is pressed
  if(master.get_digital_new_press(lift_manual_button)){
    master.rumble("-");
    master.print(LIFT_STATE_LINE, 0, "Lift: Manual      ");
    set_state(lift_states::manual);
  }

  switch(state){

    case lift_states::idle:
      // switches state to searching if up button is pressed
      if(master.get_digital_new_press(lift_up_button)){
        master.rumble("-");
        master.print(LIFT_STATE_LINE, 0, "Lift: Searching    ");

        set_state(lift_states::searching);
      }
      intake.toggle();
      break;

    case lift_states::searching:
      // grabs goal if limit switch is triggered or if up button is pressed
      if(lift_trigger.get_value() || master.get_digital_new_press(lift_up_button)){
        master.rumble("-");
        printf("*******************triggered\n");
        lift_piston.set_value(HIGH);
        master.clear_line(LIFT_STATE_LINE);

        intake.set_state(intake_states::on); // intake defaults to on when mogo is grabbed
        intake.motor.move(INTAKE_POWER);

        set_state(lift_states::grabbed);
      }
      // switches state to idle if down button is pressed
      if(master.get_digital_new_press(lift_down_button)){
        master.rumble("-");
        master.print(LIFT_STATE_LINE, 0, "Lift: Idle         ");

        set_state(lift_states::idle);
      }
      intake.toggle();
      break;

    case lift_states::grabbed:
      if(master.get_digital_new_press(lift_up_button)){ // lifts goal to platform height if up button is pressed
        move_absolute(raised_position);
        intake.raise_and_disable();

        set_state(lift_states::raised);
      }
      // releases goal and turns off intake if down button is pressed
      if(master.get_digital_new_press(lift_down_button) && fabs(bottom_position - motor.get_position()) < end_error){
        lift_piston.set_value(LOW);
        master.print(LIFT_STATE_LINE, 0, "Lift: Idle         ");

        intake.motor.move(0); // turns off intake

        set_state(lift_states::idle);
      }
      if(master.get_digital_new_press(level_platform_button)){  // moves to top position if level platform button is pressed
        move_absolute(top_position);
        intake.raise_and_disable();

        set_state(lift_states::level_platform);
      }
      intake.toggle();
      break;

    case lift_states::raised:
      if(master.get_digital_new_press(level_platform_button)){  // moves to top position if level platform button is pressed
        move_absolute(top_position);

        set_state(lift_states::level_platform);
      }
      if(master.get_digital_new_press(lift_up_button)){ // lifts goal to platform height if up button is pressed
        move_absolute(platform_position);

        set_state(lift_states::platform);
      }
      if(master.get_digital_new_press(lift_down_button)){ // lowers goal if down button is pressed
        move_absolute(bottom_position);
        intake_piston.set_value(LOW); // lowers the intake again and turns it on
        intake.motor.move(INTAKE_POWER);
        intake.set_state(intake_states::on);

        set_state(lift_states::grabbed);
      }
      break;

    case lift_states::platform:
      if(master.get_digital_new_press(level_platform_button)){  // moves to top position if level platform button is pressed
        move_absolute(top_position);

        set_state(lift_states::level_platform);
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
      }
      if(motor.get_position() < top_position / 2){ // waits for lift to lower past halfway to lower intake
        intake_piston.set_value(LOW); // lowers intake
        intake.set_state(intake_states::off);
        master.print(LIFT_STATE_LINE, 0, "Lift: Searching    ");

        set_state(lift_states::searching);
      }
      break;

    case lift_states::tall_goal:

      break;

    case lift_states::manual:
      lift_power = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);

      // gives holding power if joystick is within deadzone or lift is out of range
      // if (fabs(lift_power) < 10 || (lift_power < 0 && motor.get_position() <= bottom_position) || (lift_power > 0 && motor.get_position() >= top_position)) lift_power = 10;

      // motor.move(lift_power);
      move_absolute(map(lift_power, -127, 127, (int)top_position, (int)bottom_position));

      if(master.get_digital_new_press(lift_up_button) || master.get_digital_new_press(lift_down_button) || master.get_digital_new_press(lift_manual_button)){
        bad_count = 0;  // resets the safety
        lift_piston.set_value(LOW);
        move_absolute(bottom_position);

        intake_piston.set_value(LOW); // lowers the intake back
        intake.set_state(intake_states::off);

        master.print(LIFT_STATE_LINE, 0, "Lift: Searching    ");

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
