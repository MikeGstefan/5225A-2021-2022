#include "b_lift.hpp"

// Back Lift object
B_Lift b_lift({{"B_Lift",
{
  "idle",
  "search_lip",
  "search_bowl",
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
}, b_lift_m});

B_Lift::B_Lift(Motorized_subsystem<b_lift_states, NUM_OF_B_LIFT_STATES, LIFT_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = b_lift_states::search_lip;
  last_state = state;
  target = bottom_position;
  last_target = target;
}

void B_Lift::handle(){

  // joystick control
  lift_power = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);

  // lift safety handling
  if(state != b_lift_states::manual){
    if (fabs(target - motor.get_position()) > end_error && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
    else bad_count = 0;
    if(bad_count > 25 && state != b_lift_states::manual){
      motor.move(0);
      master.rumble("---");
      master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Manual      ");
      printf("LIFT SAFETY TRIGGERED %lf, %lf\n", target, motor.get_position());
      // intake.raise_and_disable();
      // intake.set_state(intake_states::raised);
      held = false;

      set_state(b_lift_states::manual);
    }

    // switches to manual control if lift joystick exceeds threshold
    if(fabs(lift_power) > 80){
      // // intake.raise_and_disable();
      // // intake.set_state(intake_states::raised);
      master.rumble("-");
      master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Manual      ");
      held = false;

      set_state(b_lift_states::manual);
    }
  }

  switch(state){

    case b_lift_states::idle:
      // switches state to searching if up button is pressed
      if(master.get_digital_new_press(b_lift_up_button)){
        master.rumble("-");
        master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Searching    ");

        set_state(b_lift_states::search_lip);
      }
      // // intake.toggle();
      break;

    case b_lift_states::search_lip:
      // search for bowl of mogo once lip is detected
      if(b_dist.get() < 70){
        search_cycle_check_count = 0; // resets search cycle count

        set_state(b_lift_states::search_bowl);
      }

      // grabs goal if up button is pressed
      if(master.get_digital_new_press(b_lift_up_button)){
        master.rumble("-");
        b_claw_p.set_value(HIGH);
        master.clear_line(B_LIFT_STATE_LINE);

        // // intake.set_state(intake_states::on); // intake defaults to on when mogo is grabbed
        // // intake.motor.move(INTAKE_POWER);

        set_state(b_lift_states::grabbed);
      }
      // switches state to idle if down button is pressed
      if(master.get_digital_new_press(b_lift_down_button)){
        master.rumble("-");
        master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Idle         ");
        
        set_state(b_lift_states::idle);
      }
      // intake.toggle();
      break;

    case b_lift_states::search_bowl:
      // grabs goal if up button is pressed or bowl is detected

      if (b_dist.get() > 70 && b_dist.get() < 95) search_cycle_check_count++;

      if(master.get_digital_new_press(b_lift_up_button) || search_cycle_check_count > 2){
        master.rumble("-");
        b_claw_p.set_value(HIGH);
        master.clear_line(B_LIFT_STATE_LINE);

        // // intake.set_state(intake_states::on); // intake defaults to on when mogo is grabbed
        // // intake.motor.move(INTAKE_POWER);

        set_state(b_lift_states::grabbed);
      }
      // switches state to idle if down button is pressed
      if(master.get_digital_new_press(b_lift_down_button)){
        master.rumble("-");
        master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Idle         ");

        set_state(b_lift_states::idle);
      }
      // // intake.toggle();
      break;

    case b_lift_states::grabbed:
      if(master.get_digital_new_press(b_lift_up_button)){ // lifts goal to platform height if up button is pressed
        // // intake.raise_and_disable();
        delay(100);
        move_absolute(platform_position);

        set_state(b_lift_states::platform);
      }
      // releases goal and turns off intake if down button is pressed
      if(master.get_digital_new_press(b_lift_down_button) && fabs(bottom_position - motor.get_position()) < end_error){
        b_claw_p.set_value(LOW);
        // master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Idle         ");
        release_timer.reset();
        // // intake.motor.move(0); // turns off intake

        set_state(b_lift_states::releasing);

      }
      if(master.get_digital_new_press(b_tall_goal_dropoff_button)){  // moves to top position if level platform button is pressed
        move_absolute(tall_dropoff_position);
        // // intake.raise_and_disable();

        set_state(b_lift_states::tall_platform);
      }
      // // intake.toggle();
      break;

    case b_lift_states::releasing:
      if(release_timer.get_time() > 2000){
        set_state(b_lift_states::search_lip); 
        
        master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Searching      ");
      }
      break;

    case b_lift_states::tip:
      if(master.get_digital_new_press(b_lift_up_button)){ // lifts goal to platform height if up button is pressed
        move_absolute(platform_position);

        set_state(b_lift_states::platform);
      }
      if(master.get_digital_new_press(b_lift_down_button)){ // lowers goal if down button is pressed
        move_absolute(bottom_position);

        set_state(b_lift_states::lowering);
      }
      break;

    case b_lift_states::platform:
      // drops off goal if up button is pressed and has reached paltform height
      if(master.get_digital_new_press(b_lift_up_button) && fabs(motor.get_position() - platform_position) < end_error){
        b_claw_p.set_value(LOW);

        set_state(b_lift_states::dropoff);
      }
      if(master.get_digital_new_press(b_lift_down_button)){ // lowers goal if down button is pressed
        move_absolute(raised_position);

        set_state(b_lift_states::tip);
      }
      break;

    case b_lift_states::tall_platform:
      // drops off goal if up button is pressed
      if(master.get_digital_new_press(b_lift_up_button) && fabs(motor.get_position() - tall_dropoff_position) < end_error){
        b_claw_p.set_value(LOW);

        set_state(b_lift_states::dropoff);
      }
      if(master.get_digital_new_press(b_lift_down_button)){ // lowers goal if down button is pressed
        move_absolute(raised_position);

        set_state(b_lift_states::tip);
      }
      break;

    case b_lift_states::dropoff:
      // releases goal if up or down button is pressed
      if(master.get_digital_new_press(b_lift_up_button) || master.get_digital_new_press(b_lift_down_button)){
        move_absolute(bottom_position);

        set_state(b_lift_states::lowering);
      }

      break;

    case b_lift_states::lowering:
      // moves to last position if up button is pressed
      if(master.get_digital_new_press(b_lift_up_button)){
        switch (last_state) {
          case b_lift_states::dropoff:
            move_absolute(platform_position);
            break;

          case b_lift_states::tip:
            move_absolute(raised_position);
            break;

          default:
            move_absolute(raised_position);
            break;
        }
        set_state(last_state);
      }
      // moves to next state if lift is at bottom position
      if(fabs(motor.get_position() - bottom_position) < end_error){
        switch (last_state) {
          case b_lift_states::dropoff:
            // intake_piston.set_value(LOW);
            // intake.set_state(intake_states::off);
            master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Searching    ");

            set_state(b_lift_states::search_lip);
            break;

          case b_lift_states::tip:
            // intake_piston.set_value(LOW); // lowers the intake again and turns it on
            // // intake.motor.move(INTAKE_POWER);

            set_state(b_lift_states::grabbed);
            break;

          default:
            // intake_piston.set_value(LOW);
            // // intake.set_state(intake_states::off);
            master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Searching    ");

            set_state(b_lift_states::search_lip);
            break;
        }
        // move(-10);  // applies holding power
      }
      break;

    case b_lift_states::tall_goal:

      break;

    case b_lift_states::manual:

      // gives holding power if joystick is within deadzone or lift is out of range
      if (fabs(lift_power) < 10 || (lift_power < 0 && motor.get_position() <= bottom_position) || (lift_power > 0 && motor.get_position() >= top_position)) lift_power = -10;

      motor.move(lift_power);
      if(master.get_digital_new_press(b_lift_down_button)){
        bad_count = 0;  // resets the safety
        b_claw_p.set_value(LOW);
        move_absolute(bottom_position);

        // intake_piston.set_value(LOW); // lowers the intake back
        // // intake.set_state(intake_states::off);

        master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Searching    ");

        set_state(b_lift_states::search_lip);
      }

      // toggles state of lift pneumatic if lift up button is pressed
      if(master.get_digital_new_press(b_lift_up_button)) held = !held;
      b_claw_p.set_value(held);

      break;

  }
}

double B_Lift::pos_to_height(double pos){
  return offset_h + arm_len * sin(deg_to_rad((pos - offset_a) / gear_ratio));
}

double B_Lift::height_to_pos(double height){
  return gear_ratio * (rad_to_deg(asin((height - offset_h) / (arm_len)))) + offset_a;
}

void B_Lift::elastic_util(){
  master.clear();
  reset();
  motor.move(-10);
  master.print(0, 0, "press a to start");
  waitUntil(master.get_digital_new_press(DIGITAL_A));
  b_claw_p.set_value(HIGH);
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
