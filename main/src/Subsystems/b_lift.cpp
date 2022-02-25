#include "b_lift.hpp"

#define master partner

// Back Lift object
B_Lift b_lift({{"B_Lift",
{
  "bottom",
  "side_dropoff",
  "dropoff",
  "top",
  "manual",
}
}, b_lift_m});

B_Lift::B_Lift(Motorized_subsystem<b_lift_states, NUM_OF_B_LIFT_STATES, LIFT_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = b_lift_states::bottom;
  last_state = state;
  target = bottom_position;
  last_target = target;
  held = false;
}

void B_Lift::handle(){

  // joystick control
  lift_power = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);

  // resets button hold timers if the button is not pressed
  if (!master.get_digital(b_lift_up_button) && up_button_hold_timer.playing()) up_button_hold_timer.reset(false);
  if (!master.get_digital(b_lift_down_button) && down_button_hold_timer.playing()) down_button_hold_timer.reset(false);

  // goes to platform position if up button is held
  if(up_button_hold_timer.get_time() > 500){ 
    up_button_hold_timer.reset(false);  // resets and pauses timer
    move_absolute(dropoff_position);

    set_state(b_lift_states::dropoff);
  }
  // goes to platform position if up button is held
  else if(down_button_hold_timer.get_time() > 500){ 
    down_button_hold_timer.reset(false);  // resets and pauses timer
    move_absolute(bottom_position);

    set_state(b_lift_states::lowering);
  }


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

      set_state(b_lift_states::manual);
    }

    // switches to manual control if lift joystick exceeds threshold
    if(fabs(lift_power) > 60){
      // // intake.raise_and_disable();
      // // intake.set_state(intake_states::raised);
      master.rumble("-");
      master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Manual      ");

      set_state(b_lift_states::manual);
    }
  }

  switch(state){

    // intake.toggle();
    case b_lift_states::bottom:
      if(master.get_digital_new_press(b_lift_up_button)){
        up_button_hold_timer.reset();
        move_absolute(side_dropoff_position);

        set_state(b_lift_states::side_dropoff);
      }
      break;

    case b_lift_states::side_dropoff:
      if(master.get_digital_new_press(b_lift_up_button)){
        up_button_hold_timer.reset();
        move_absolute(dropoff_position);

        set_state(b_lift_states::dropoff);
      }
      if(master.get_digital_new_press(b_lift_down_button)){
        down_button_hold_timer.reset();
        move_absolute(bottom_position);

        set_state(b_lift_states::lowering);
      }
      break;

    case b_lift_states::dropoff:
      if(master.get_digital_new_press(b_lift_up_button)){
        up_button_hold_timer.reset();
        move_absolute(top_position);

        set_state(b_lift_states::top);
      }
      if(master.get_digital_new_press(b_lift_down_button)){
        down_button_hold_timer.reset();
        move_absolute(side_dropoff_position);

        set_state(b_lift_states::side_dropoff);
      }
      break;

    case b_lift_states::lowering:
      // goes to previous state if up button is pressed
      if(master.get_digital_new_press(b_lift_up_button)){
        move_absolute(last_target);

        set_state(last_state);
      }
      // changes state to bottom once target is reached
      if(fabs(motor.get_position() - bottom_position) < end_error){
        set_state(b_lift_states::bottom);

        // LOWER PISTON HERE
        // if grabbed turn on intake
      }
      break;

    case b_lift_states::manual:

      // gives holding power if joystick is within deadzone or lift is out of range
      if (fabs(lift_power) < 10 || (lift_power < 0 && motor.get_position() <= bottom_position) || (lift_power > 0 && motor.get_position() >= top_position)) motor.move_relative(0, 30);

      motor.move(lift_power);
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

// FRONT CLAW SUBSYSTEM
B_Claw b_claw({"B_Claw",
{
  "idle",
  "searching_lip",
  "searching_bowl",
  "grabbed"
}
});

B_Claw::B_Claw(Subsystem<b_claw_states, NUM_OF_B_CLAW_STATES> subsystem): Subsystem(subsystem)  // constructor
{
  // state setup
  state = b_claw_states::searching_bowl;
  last_state = state;
}

void B_Claw::handle(){
  switch(state){
    case b_claw_states::idle:
      // switches state to searching if search button is pressed
      if(b_lift.get_state() == b_lift_states::bottom && master.get_digital_new_press(b_claw_search_button)){
        master.rumble("-");
        master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Searching    ");

        set_state(b_claw_states::searching_bowl);
      }
      // grabs goal if toggle button is pressed
      if(master.get_digital_new_press(b_claw_toggle_button)){
        master.rumble("-");
        b_claw_p.set_value(HIGH);
        // intake.set_state(intake_states::on); // intake defaults to on when mogo is grabbed
        // intake.motor.move(INTAKE_POWER);
        master.clear_line(B_LIFT_STATE_LINE);

        set_state(b_claw_states::grabbed);
      }
      break;

    case b_claw_states::searching_lip:

      // search for bowl of mogo once lip is detected
      if(b_dist.get() < 70){
        search_cycle_check_count = 0; // resets search cycle count

        set_state(b_claw_states::searching_bowl);
      }
      // grabs goal if toggle button is pressed
      if(master.get_digital_new_press(b_claw_toggle_button)){
        master.rumble("-");
        b_claw_p.set_value(HIGH);
        // intake.set_state(intake_states::on); // intake defaults to on when mogo is grabbed
        // intake.motor.move(INTAKE_POWER);
        master.clear_line(B_LIFT_STATE_LINE);

        set_state(b_claw_states::grabbed);
      }
      // cancels search if search button is pressed
      if(master.get_digital_new_press(b_claw_search_button)){
        master.rumble("-");
        master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Idle         ");

        set_state(b_claw_states::idle);
      }
      break;

    case b_claw_states::searching_bowl:

      if (b_dist.get() > 70 && b_dist.get() < 95) search_cycle_check_count++;

      // grabs goal if toggle button is pressed or bowl is detected
      if(master.get_digital_new_press(b_claw_toggle_button) || search_cycle_check_count >= 2){
        master.rumble("-");
        b_claw_p.set_value(HIGH);
        // intake.set_state(intake_states::on); // intake defaults to on when mogo is grabbed
        // intake.motor.move(INTAKE_POWER);
        master.clear_line(B_LIFT_STATE_LINE);

        set_state(b_claw_states::grabbed);
      }
      // cancels search if search button is pressed
      if(master.get_digital_new_press(b_claw_search_button)){
        master.rumble("-");
        master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Idle         ");

        set_state(b_claw_states::idle);
      }
      break;

    case b_claw_states::grabbed:
      // releases goal and turns off intake if down button is pressed
      if(master.get_digital_new_press(b_claw_toggle_button)){
        b_claw_p.set_value(LOW);
        // release_timer.reset();

        // // intake.motor.move(0); // turns off intake

        set_state(b_claw_states::idle);
        master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Idle         ");
      }
      break;

  }
}

/*

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
        held = true;
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

      if(master.get_digital_new_press(b_lift_up_button) || search_cycle_check_count >= 2){
        master.rumble("-");
        b_claw_p.set_value(HIGH);
        held = true;
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
      if(master.get_digital_new_press(b_lift_up_button)){ // lifts goal to tall goal platform height if up button is pressed
        // // intake.raise_and_disable();
        delay(100);
        move_absolute(tall_dropoff_position);

        set_state(b_lift_states::tall_platform);
      }
      // releases goal and turns off intake if down button is pressed
      if((master.get_digital_new_press(b_lift_down_button) || master.get_digital_new_press(b_lift_release_button)) && fabs(bottom_position - motor.get_position()) < end_error){
        b_claw_p.set_value(LOW);
        held = false;
        // master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Idle         ");
        release_timer.reset();
        // // intake.motor.move(0); // turns off intake

        set_state(b_lift_states::releasing);

      }
      // // intake.toggle();
      break;

    case b_lift_states::releasing:
      // enters searching state again only after mogo is no longer detected, or times out
      if(b_dist.get() > 120 || release_timer.get_time() > 3000){
        set_state(b_lift_states::search_lip); 
        master.rumble("-");
        
        master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Searching      ");
      }
      break;
*/