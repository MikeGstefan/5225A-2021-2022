#include "f_lift.hpp"

// #define master partner

// Back Lift object
F_Lift f_lift({{"F_Lift",
{
  "idle",
  "searching",
  "grabbed",
  "releasing",
  "tip",
  "platform",
  "tall_platform",
  "dropoff",
  "lowering",
  "tall_goal",
  "manual",
}
}, f_lift_m});

F_Lift::F_Lift(Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, LIFT_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = f_lift_states::searching;
  last_state = state;
  target = bottom_position;
  last_target = target;
  held = false;
}

void F_Lift::handle(){

  // joystick control
  // lift_power = master.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
  lift_power = 0;
  // lift safety handling
  if(state != f_lift_states::manual){
    if (fabs(target - motor.get_position()) > end_error && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
    else bad_count = 0;
    if(bad_count > 25 && state != f_lift_states::manual){
      motor.move(0);
      master.rumble("---");
      master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Manual      ");
      printf("LIFT SAFETY TRIGGERED %lf, %lf\n", target, motor.get_position());
      // intake.raise_and_disable();
      // intake.set_state(intake_states::raised);

      set_state(f_lift_states::manual);
    }

    // switches to manual control if lift joystick exceeds threshold
    if(fabs(lift_power) > 80){
      // // intake.raise_and_disable();
      // // intake.set_state(intake_states::raised);
      master.rumble("-");
      master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Manual      ");

      set_state(f_lift_states::manual);
    }
  }

  switch(state){

    case f_lift_states::idle:
      // switches state to searching if up button is pressed
      if(drivebase.get_lift_button(1) && master.get_digital_new_press(lift_up_button)){
        master.rumble("-");
        master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Searching    ");

        set_state(f_lift_states::searching);
      }
      // // intake.toggle();
      break;

    case f_lift_states::searching:
      // grabs goal if front touch or up button is pressed
      if(f_touch.get_value() || (drivebase.get_lift_button(1) && master.get_digital_new_press(lift_up_button))){
        master.rumble("-");
        f_claw_p.set_value(HIGH);
        held = true;
        delay(50);
        move_absolute(holding_position);
        master.clear_line(F_LIFT_STATE_LINE);

        // // intake.set_state(intake_states::on); // intake defaults to on when mogo is grabbed
        // // intake.motor.move(INTAKE_POWER);
        intk.move(127);
        set_state(f_lift_states::grabbed);
      }
      // switches state to idle if down button is pressed
      if((master.get_digital_new_press(lift_down_button)&& drivebase.get_lift_button(1))){
        master.rumble("-");
        master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Idle         ");
        f_claw_p.set_value(LOW);

        set_state(f_lift_states::idle);
      }
      // intake.toggle();
      break;

    case f_lift_states::grabbed:
      master.print(F_LIFT_STATE_LINE, 0, "grabbed");

      if((drivebase.get_lift_button(1) && master.get_digital_new_press(lift_up_button))){ // lifts goal to tall goal platform height if up button is pressed
        // // intake.raise_and_disable();
        delay(100);
        move_absolute(tall_dropoff_position);

        set_state(f_lift_states::tall_platform);
      }
      // releases goal and turns off intake if down button is pressed
      if(((master.get_digital_new_press(lift_down_button)&& drivebase.get_lift_button(1)) || (master.get_digital_new_press(lift_release_button)&& drivebase.get_lift_button(1))) && fabs(holding_position - motor.get_position()) < 80){
        f_claw_p.set_value(LOW);
        held = false;
        release_timer.reset();
        move_absolute(bottom_position);
        // master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Idle         ");

        // // intake.motor.move(0); // turns off intake
        intk.move(0);

        set_state(f_lift_states::idle);
        master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Idle         ");
      }
      // // intake.toggle();
      break;

    case f_lift_states::releasing:
      // enters searching state again only after mogo is no longer detected, or times out
      if(!f_touch.get_value() || release_timer.get_time() > 3000){
        set_state(f_lift_states::searching); 
        master.rumble("-");
        
        master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Searching      ");
      }
      break;

    case f_lift_states::tip:
      if(drivebase.get_lift_button(1) && master.get_digital_new_press(lift_up_button)){ // lifts goal to platform height if up button is pressed
        move_absolute(last_target);

        set_state(last_state);
      }
      if(master.get_digital_new_press(lift_down_button)&& drivebase.get_lift_button(1)){ // lowers goal if down button is pressed
        move_absolute(holding_position);
        intk.move(0);
        set_state(f_lift_states::lowering);
      }
      break;

    case f_lift_states::platform:
      // drops off goal if up button is pressed and has reached platform height
      if(((drivebase.get_lift_button(1) && master.get_digital_new_press(lift_up_button)) || (master.get_digital_new_press(lift_release_button)&& drivebase.get_lift_button(1))) && fabs(motor.get_position() - platform_position) < end_error){
        f_claw_p.set_value(LOW);
        held = false;

        set_state(f_lift_states::dropoff);
      }
      if(master.get_digital_new_press(lift_down_button)&& drivebase.get_lift_button(1)){ // lowers goal to tall platform dropoff height down button is pressed
        move_absolute(tall_dropoff_position);

        set_state(f_lift_states::tall_platform);
      }
      break;

    case f_lift_states::tall_platform:
      if(master.get_digital_new_press(DIGITAL_Y)){
        held = !held;
        f_claw_p.set_value(held);
      }
      // drops off goal if up button is pressed
      if(drivebase.get_lift_button(1) && master.get_digital_new_press(lift_up_button)){ // moves to platform height if up button is pressed
        move_absolute(platform_position);

        set_state(f_lift_states::platform);
      }
      // releases goal if release button is pressed
      if((master.get_digital_new_press(lift_release_button)&& drivebase.get_lift_button(1)) && fabs(motor.get_position() - tall_dropoff_position) < end_error){
        f_claw_p.set_value(LOW);
        held = false;

        set_state(f_lift_states::dropoff);
      }
      if(master.get_digital_new_press(lift_down_button)&& drivebase.get_lift_button(1)){ // lowers goal if down button is pressed
        move_absolute(raised_position);

        set_state(f_lift_states::tip);
      }
      break;

    case f_lift_states::dropoff:
      if(master.get_digital_new_press(DIGITAL_Y)){
        held = !held;
        f_claw_p.set_value(held);
      }
      // releases goal if up or down button is pressed
      if((drivebase.get_lift_button(1) && master.get_digital_new_press(lift_up_button)) || (master.get_digital_new_press(f_lift_down_button)&& drivebase.get_lift_button(1))){
        move_absolute(bottom_position);
        intk.move(0);
        set_state(f_lift_states::lowering);
      }
      break;

    case f_lift_states::lowering:
      master.print(F_LIFT_STATE_LINE, 0, "lowering");
      // moves to last position if up button is pressed
      if(drivebase.get_lift_button(1) && master.get_digital_new_press(lift_up_button)){
        switch (last_state) {
          case f_lift_states::dropoff:
            move_absolute(last_target);
            break;

          case f_lift_states::tip:
            move_absolute(raised_position);
            break;

          default:
            move_absolute(raised_position);
            break;
        }
        set_state(last_state);
      }
      // moves to next state if lift is at bottom position
      if(fabs(motor.get_position() - holding_position) < end_error){
        switch (last_state) {
          case f_lift_states::dropoff:
            // intake_piston.set_value(LOW);
            // intake.set_state(intake_states::off);
            master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Searching    ");

            set_state(f_lift_states::searching);
            break;

          case f_lift_states::tip:
            // intake_piston.set_value(LOW); // lowers the intake again and turns it on
            // // intake.motor.move(INTAKE_POWER);

            set_state(f_lift_states::grabbed);
            break;

          default:
            // intake_piston.set_value(LOW);
            // // intake.set_state(intake_states::off);
            master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Searching    ");

            set_state(f_lift_states::searching);
            break;
        }
        // move(-10);  // applies holding power
      }
      break;

    case f_lift_states::tall_goal:

      break;

    case f_lift_states::manual:

      // gives holding power if joystick is within deadzone or lift is out of range
      if (fabs(lift_power) < 10 || (lift_power < 0 && motor.get_position() <= bottom_position) || (lift_power > 0 && motor.get_position() >= top_position)) lift_power = -10;

      motor.move(lift_power);
      if(master.get_digital_new_press(lift_down_button)&& drivebase.get_lift_button(1)){
        bad_count = 0;  // resets the safety
        move_absolute(bottom_position);
        // intake_piston.set_value(LOW); // lowers the intake back
        // // intake.set_state(intake_states::off);
        if (held) set_state(f_lift_states::grabbed);
        else{
          master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Searching    ");
          set_state(f_lift_states::searching);
        }
      }

      // toggles state of lift pneumatic if lift up button is pressed
      if(drivebase.get_lift_button(1) && master.get_digital_new_press(lift_up_button)){
        held = !held;
        f_claw_p.set_value(held);
      }
      break;

  }
}

double F_Lift::pos_to_height(double pos){
  return offset_h + arm_len * sin(deg_to_rad((pos - offset_a) / gear_ratio));
}

double F_Lift::height_to_pos(double height){
  return gear_ratio * (rad_to_deg(asin((height - offset_h) / (arm_len)))) + offset_a;
}

void F_Lift::elastic_util(){
  master.clear();
  reset();
  motor.move(-10);
  master.print(0, 0, "press a to start");
  waitUntil(master.get_digital_new_press(DIGITAL_A));
  f_claw_p.set_value(HIGH);
  held = true;
  Timer move_timer{"move"};
  move_absolute(top_position);
  // // intake_piston.set_value(HIGH);  // raises intake
  waitUntil(fabs(motor.get_position() - top_position) < end_error);
  move_timer.print();
  master.print(1, 0, "up time: %d", move_timer.get_time());

  move_timer.reset();
  move_absolute(bottom_position);
  waitUntil(fabs(motor.get_position() - bottom_position) < end_error);
  move_timer.print();
  master.print(2, 0, "down time: %d", move_timer.get_time());
}
