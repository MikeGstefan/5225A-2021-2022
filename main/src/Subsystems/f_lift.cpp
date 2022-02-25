#include "f_lift.hpp"

#define master partner

// Back Lift object
F_Lift f_lift({{"F_Lift",
{
  "bottom",
  "holding",
  "side_dropoff",
  "dropoff",
  "top",
  "manual",
}
}, f_lift_m});

F_Lift::F_Lift(Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, LIFT_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = f_lift_states::bottom;
  last_state = state;
  target = bottom_position;
  last_target = target;
  held = false;
}

void F_Lift::handle(){

  // joystick control
  lift_power = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);

  // resets button hold timers if the button is not pressed
  if (!master.get_digital(f_lift_up_button) && up_button_hold_timer.playing()) up_button_hold_timer.reset(false);
  if (!master.get_digital(f_lift_down_button) && down_button_hold_timer.playing()) down_button_hold_timer.reset(false);

  // goes to platform position if up button is held
  if(up_button_hold_timer.get_time() > 500){ 
    up_button_hold_timer.reset(false);  // resets and pauses timer
    move_absolute(dropoff_position);

    set_state(f_lift_states::dropoff);
  }
  // goes to platform position if up button is held
  else if(down_button_hold_timer.get_time() > 500){ 
    down_button_hold_timer.reset(false);  // resets and pauses timer
    move_absolute(bottom_position);

    set_state(f_lift_states::bottom);
  }


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
    if(fabs(lift_power) > 60){
      // // intake.raise_and_disable();
      // // intake.set_state(intake_states::raised);
      master.rumble("-");
      master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Manual      ");

      set_state(f_lift_states::manual);
    }
  }

  switch(state){

    // intake.toggle();
    case f_lift_states::bottom:
      if(master.get_digital_new_press(f_lift_up_button)){
        up_button_hold_timer.reset();
        move_absolute(holding_position);

        set_state(f_lift_states::holding);
      }
      break;

    case f_lift_states::holding:
      if(master.get_digital_new_press(f_lift_up_button)){
        up_button_hold_timer.reset();
        move_absolute(side_dropoff_position);

        set_state(f_lift_states::side_dropoff);
      }
      if(master.get_digital_new_press(f_lift_down_button)){
        down_button_hold_timer.reset();
        move_absolute(bottom_position);

        set_state(f_lift_states::bottom);
      }
      break;

    case f_lift_states::side_dropoff:
      if(master.get_digital_new_press(f_lift_up_button)){
        up_button_hold_timer.reset();
        move_absolute(dropoff_position);

        set_state(f_lift_states::dropoff);
      }
      if(master.get_digital_new_press(f_lift_down_button)){
        down_button_hold_timer.reset();
        move_absolute(holding_position);

        set_state(f_lift_states::holding);
      }
      break;

    case f_lift_states::dropoff:
      if(master.get_digital_new_press(f_lift_up_button)){
        up_button_hold_timer.reset();
        move_absolute(top_position);

        set_state(f_lift_states::top);
      }
      if(master.get_digital_new_press(f_lift_down_button)){
        down_button_hold_timer.reset();
        move_absolute(side_dropoff_position);

        set_state(f_lift_states::side_dropoff);
      }
      break;

    case f_lift_states::manual:

      // gives holding power if joystick is within deadzone or lift is out of range
      if (fabs(lift_power) < 10 || (lift_power < 0 && motor.get_position() <= bottom_position) || (lift_power > 0 && motor.get_position() >= top_position)) motor.move_relative(0, 30);

      motor.move(lift_power);
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

// FRONT CLAW SUBSYSTEM
F_Claw f_claw({"F_Claw",
{
  "idle",
  "searching",
  "grabbed"
}
});

F_Claw::F_Claw(Subsystem<f_claw_states, NUM_OF_F_CLAW_STATES> subsystem): Subsystem(subsystem)  // constructor
{
  // state setup
  state = f_claw_states::searching;
  last_state = state;
}

void F_Claw::handle(){
  switch(state){
    case f_claw_states::idle:
      // switches state to searching if search button is pressed
      if(f_lift.get_state() == f_lift_states::bottom && master.get_digital_new_press(f_claw_search_button)){
        master.rumble("-");
        master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Searching    ");

        set_state(f_claw_states::searching);
      }
      // grabs goal if toggle button is pressed
      if(master.get_digital_new_press(f_claw_toggle_button)){
        master.rumble("-");
        f_claw_p.set_value(HIGH);

        // raises mogo above rings automatically if lift is in bottom state
        if(f_lift.get_state() == f_lift_states::bottom){
          delay(50);
          f_lift.move_absolute(f_lift.holding_position);
          f_lift.set_state(f_lift_states::holding);
          // intake.set_state(intake_states::on); // intake defaults to on when mogo is grabbed
          // intake.motor.move(INTAKE_POWER);
        }
        master.clear_line(F_LIFT_STATE_LINE);


        set_state(f_claw_states::grabbed);
      }
      break;

    case f_claw_states::searching:
      // grabs goal if toggle button is pressed or mogo is detected
      if(master.get_digital_new_press(f_claw_toggle_button) || f_touch.get_value()){
        master.rumble("-");
        f_claw_p.set_value(HIGH);

        // raises mogo above rings automatically if lift is in bottom state
        if(f_lift.get_state() == f_lift_states::bottom){
          delay(50);
          f_lift.move_absolute(f_lift.holding_position);
          f_lift.set_state(f_lift_states::holding);
          // intake.set_state(intake_states::on); // intake defaults to on when mogo is grabbed
          // intake.motor.move(INTAKE_POWER);
        }
        master.clear_line(F_LIFT_STATE_LINE);

        set_state(f_claw_states::grabbed);
      }    
      // cancels search if search button is pressed
      if(master.get_digital_new_press(f_claw_search_button)){
        master.rumble("-");
        master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Idle         ");

        set_state(f_claw_states::idle);
      }
      break;

    case f_claw_states::grabbed:
      // releases goal and turns off intake if down button is pressed
      if(master.get_digital_new_press(f_claw_toggle_button)){
        f_claw_p.set_value(LOW);
        // release_timer.reset();

        // // intake.motor.move(0); // turns off intake

        set_state(f_claw_states::idle);
        master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Idle         ");
      }
      // // intake.toggle();
      break;

  }
}

/*

    case f_lift_states::idle:
      // switches state to searching if up button is pressed
      if(master.get_digital_new_press(f_lift_up_button)){
        master.rumble("-");
        master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Searching    ");

        set_state(f_lift_states::searching);
      }
      // // intake.toggle();
      break;

    case f_lift_states::searching:
      // grabs goal if front touch or up button is pressed
      if(f_touch.get_value() || master.get_digital_new_press(f_lift_up_button)){
        master.rumble("-");
        f_claw_p.set_value(HIGH);
        held = true;
        delay(50);
        move_absolute(holding_position);
        master.clear_line(F_LIFT_STATE_LINE);

        // // intake.set_state(intake_states::on); // intake defaults to on when mogo is grabbed
        // // intake.motor.move(INTAKE_POWER);

        set_state(f_lift_states::grabbed);
      }
      // switches state to idle if down button is pressed
      if(master.get_digital_new_press(f_lift_down_button)){
        master.rumble("-");
        master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Idle         ");
        f_claw_p.set_value(LOW);

        set_state(f_lift_states::idle);
      }
      // intake.toggle();
      break;

    case f_lift_states::grabbed:
      master.print(F_LIFT_STATE_LINE, 0, "grabbed");

      if(master.get_digital_new_press(f_lift_up_button)){ // lifts goal to tall goal platform height if up button is pressed
        // // intake.raise_and_disable();
        delay(100);
        move_absolute(tall_dropoff_position);

        set_state(f_lift_states::tall_platform);
      }
      // releases goal and turns off intake if down button is pressed
      if((master.get_digital_new_press(f_lift_down_button) || master.get_digital_new_press(f_lift_release_button)) && fabs(holding_position - motor.get_position()) < 80){
        f_claw_p.set_value(LOW);
        held = false;
        release_timer.reset();
        move_absolute(bottom_position);
        // master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Idle         ");

        // // intake.motor.move(0); // turns off intake

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
*/