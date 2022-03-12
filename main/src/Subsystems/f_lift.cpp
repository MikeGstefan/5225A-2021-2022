#include "f_lift.hpp"

// #define master partner

// Back Lift object
F_Lift f_lift({{"F_Lift",
{
  "bottom",
  "idle",
  "move_to_target",
  "manual",
}
}, f_lift_m});


F_Lift::F_Lift(Motorized_subsystem<f_lift_states, NUM_OF_F_LIFT_STATES, LIFT_MAX_VELOCITY> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = f_lift_states::idle;
  last_state = state;
  target = bottom_position;
  last_target = target;

  index = 0;
  last_index = index;

  up_press.pause();
  down_press.pause();
}

void F_Lift::handle(){

  // index incrementing and decrementing
  if(master.get_digital_new_press(lift_up_button) && index < positions.size() - 1){
    index++;
    up_press.reset();
  }
  if(master.get_digital_new_press(lift_down_button) && index > 0){
    down_press.reset();
    index--;
  }
  // resets and pauses the timers if driver release button
  if(!master.get_digital(lift_up_button)) up_press.reset(false);
  if(!master.get_digital(lift_down_button)) down_press.reset(false);

  // goes to top position of top button is held
  if(up_press.get_time() > 300) index = positions.size() - 1;
  if(down_press.get_time() > 300) index = 0;


  // moves to target if index has changed since last loop cycle
  if(index != last_index){
    last_index = index;
    set_state(f_lift_states::move_to_target);
  }

  // joystick control
  lift_power = master.get_analog(ANALOG_RIGHT_Y);
  if(state != f_lift_states::manual){
    // switches to manual control if lift joystick exceeds threshold
    if(fabs(lift_power) > 80){
      master.rumble("-");
      master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Manual      ");

      set_state(f_lift_states::manual);
    }
  }

  switch(state){
    case f_lift_states::bottom: // at lowest position:
      break;
    case f_lift_states::idle: // not doing anything
      break;

    case f_lift_states::move_to_target: // not doing anything
      motor.move(pid.compute(motor.get_position(), positions[index]));
      
      // moves to next state if the lift has reached its target
      if(fabs(pid.get_error()) < end_error){
        motor.move_velocity(0); // holds motor
        // switches to idle by default or special case depending on current target
        switch(index){
          case 0: // lift is at bottom position
            set_state(f_lift_states::bottom);
            break;
          default:
            set_state(f_lift_states::idle);
            break;
        }
      }

      // UNCOMMENT FOR LIFT SAFETY Handling
      // if the motor is drawing more than 2.3 amps for more than 100 ms safety out
      /*
      if (motor.get_current_draw() > 2300 && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
      else bad_count = 0;
      if(bad_count > 10 && state != f_lift_states::manual){
        motor.move(0);
        master.rumble("---");
        master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Manual      ");
        printf("LIFT SAFETY TRIGGERED %lf, %lf\n", target, motor.get_position());

        set_state(f_lift_states::manual);
      }
      */
      break;

    case f_lift_states::manual:

      // holds motor if joystick is within deadzone or lift is out of range
      if (fabs(lift_power) < 10 || (lift_power < 0 && motor.get_position() <= bottom_position) || (lift_power > 0 && motor.get_position() >= top_position)) motor.move_velocity(0);
      else motor.move(lift_power);
      // exits manual state if up or down button is pressed or held
      break;

  }
}

double F_Lift::pos_to_height(double pos){
  return offset_h + arm_len * sin(deg_to_rad((pos - offset_a) / gear_ratio));
}

double F_Lift::height_to_pos(double height){
  return gear_ratio * (rad_to_deg(asin((height - offset_h) / (arm_len)))) + offset_a;
}

extern int elastic_f_up_time, elastic_f_down_time; //from gui_construction.cpp

void F_Lift::elastic_util(){
  reset();
  motor.move(-10);
  GUI::go("Start Elastic Utility", "Press to start the elastic utility.", 500);
  f_claw_p.set_value(HIGH);
  Timer move_timer{"move"};
  move_absolute(top_position);
  // // intake_piston.set_value(HIGH);  // raises intake
  waitUntil(fabs(motor.get_position() - top_position) < end_error);
  move_timer.print();
  elastic_f_up_time = move_timer.get_time();
  master.print(1, 0, "up time: %d", elastic_f_up_time);

  move_timer.reset();
  move_absolute(bottom_position);
  waitUntil(fabs(motor.get_position() - bottom_position) < end_error);
  move_timer.print();
  elastic_f_down_time = move_timer.get_time();
  master.print(2, 0, "down time: %d", elastic_f_up_time);
}



// FRONT CLAW SUBSYSTEM
F_Claw f_claw({"F_Claw",
{
  "idle",
  "searching",
  "grabbed",
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
      // enters search mode if the lift is at the bottom and it's been 2 seconds since the mogo was released
      printf("time:%d\n", release_timer.get_time());
      if(f_lift.get_state() == f_lift_states::bottom && release_timer.get_time() > 2000){
        set_state(f_claw_states::searching);
      }
      // grabs goal if toggle button is pressed
      if(master.get_digital_new_press(lift_claw_toggle_button)){
        master.rumble("-");
        f_claw_p.set_value(HIGH);

        // raises mogo above rings automatically if lift is in bottom state
        if(f_lift.get_state() == f_lift_states::bottom){
          f_lift.index = 1; // sends f_lift to raised position
        }

        set_state(f_claw_states::grabbed);
      }
      break;

    case f_claw_states::searching:
      // grabs goal if toggle button is pressed or mogo is detected
      if(master.get_digital_new_press(lift_claw_toggle_button) || f_touch.get_value()){
        master.rumble("-");
        f_claw_p.set_value(HIGH);

        // raises mogo above rings automatically if lift is in bottom state
        if(f_lift.get_state() == f_lift_states::bottom){
          f_lift.index = 1; // sends f_lift to raised position
        }

        set_state(f_claw_states::grabbed);
      }
      // doesn't let driver search if lift is not at bottom
      if(f_lift.motor.get_position() > f_lift.positions[1] - 50){
        set_state(f_claw_states::idle);
      }
      break;

    case f_claw_states::grabbed:
      // releases goal if down button is pressed
      if(master.get_digital_new_press(lift_claw_toggle_button)){
        f_claw_p.set_value(LOW);
        release_timer.reset();

        set_state(f_claw_states::idle);
      }
      break;

  }
}
