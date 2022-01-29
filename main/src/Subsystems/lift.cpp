#include "lift.hpp"

// Lift object
Lift lift({{"Lift",
{
  "idle",
  "searching",
  "grabbed",
  "tip",
  "platform",
  "dropoff",
  "tall_goal",
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
bool intake_on = false;
void Lift::handle(){
  // if (fabs(target - motor.get_position()) > end_error && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
  // else bad_count = 0;
  // if(bad_count > 25 && state != lift_states::manual){
  //   motor.move(0);
  //   master.rumble("---");
  //   printf("LIFT SAFETY TRIGGERED\n");
  //   intake.raise_and_disable();
  //   intake.set_state(intake_states::raised);

  //   set_state(lift_states::manual);
  // }

  // switches to manual control if lift manual button is pressed
  if(master.get_digital_new_press(lift_manual_button)){
    intake.raise_and_disable();
    intake.set_state(intake_states::raised);
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
      if(!intake_on && motor.get_position() < bottom_position + 100){
        intake_piston.set_value(LOW); // lowers the intake again and turns it on
        intake.motor.move(INTAKE_POWER);
        intake_on = true;
      }
      if(master.get_digital_new_press(lift_up_button)){ // lifts goal to platform height if up button is pressed
        intake.raise_and_disable();
        delay(100);
        move_absolute(platform_position);

        set_state(lift_states::platform);
      }
      // releases goal and turns off intake if down button is pressed
      if(master.get_digital_new_press(lift_down_button) && fabs(bottom_position - motor.get_position()) < end_error){
        lift_piston.set_value(LOW);
        master.print(LIFT_STATE_LINE, 0, "Lift: Idle         ");

        intake.motor.move(0); // turns off intake

        set_state(lift_states::idle);
      }
      if(master.get_digital_new_press(mogo_tip_button)){  // moves to top position if level platform button is pressed
        move_absolute(raised_position);
        intake.raise_and_disable();

        set_state(lift_states::tip);
      }
      intake.toggle();
      break;

    case lift_states::tip:
      if(master.get_digital_new_press(lift_up_button)){ // lifts goal to platform height if up button is pressed
        move_absolute(platform_position);

        set_state(lift_states::platform);
      }
      if(master.get_digital_new_press(lift_down_button)){ // lowers goal if down button is pressed
        move_absolute(bottom_position);
        intake.set_state(intake_states::on);
        intake_on = false;

        set_state(lift_states::grabbed);
      }
      break;

    case lift_states::platform:
      // drops off goal if up button is pressed
      if(master.get_digital_new_press(lift_up_button) && fabs(motor.get_position() - platform_position) < end_error){
        lift_piston.set_value(LOW);

        set_state(lift_states::dropoff);
      }
      if(master.get_digital_new_press(lift_down_button)){ // lowers goal if down button is pressed
        move_absolute(raised_position);

        set_state(lift_states::tip);
      }
      break;

    case lift_states::dropoff:
      // releases goal if up or down button is pressed
      if(master.get_digital_new_press(lift_up_button) || master.get_digital_new_press(lift_down_button)){
        move_absolute(bottom_position);
      }
      if(motor.get_position() < bottom_position + 100){ // waits for lift to lower past halfway to lower intake
        intake_piston.set_value(LOW);
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
      if (fabs(lift_power) < 10 || (lift_power < 0 && motor.get_position() <= bottom_position) || (lift_power > 0 && motor.get_position() >= top_position)) lift_power = 10;

      motor.move(lift_power);
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

void Lift::elastic_util(){
  master.clear();
  lift.reset();
  lift.motor.move(-10);
  master.print(0, 0, "press a to start");
  waitUntil(master.get_digital_new_press(DIGITAL_A));
  lift_piston.set_value(HIGH);
  Timer move_timer{"move"};
  lift.move_absolute(lift.top_position);
  intake_piston.set_value(HIGH);  // raises intake
  waitUntil(fabs(lift.motor.get_position() - lift.top_position) < lift.end_error);
  move_timer.print();
  master.print(1, 0, "up time: %d", move_timer.get_time());

  move_timer.reset();
  lift.move_absolute(lift.bottom_position);
  waitUntil(fabs(lift.motor.get_position() - lift.bottom_position) < lift.end_error);
  move_timer.print();
  master.print(2, 0, "down time: %d", move_timer.get_time());
}
