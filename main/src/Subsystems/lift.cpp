#include "lift.hpp"

// Lift object
Lift lift({{"Lift",
{
  "searching",
  "grabbed",
  "releasing",
  "lifting",
  "platform",
  "released",
  "lowering",
  "manual",
}
}, lift_motor});

Lift::Lift(Motorized_subsystem<lift_states, NUM_OF_LIFT_STATES> motorized_subsystem): Motorized_subsystem(motorized_subsystem){ // constructor

  state = lift_states::searching;
  last_state = state;
  released_cycle_check = 0;
}

void Lift::handle(){
  if (fabs(motor.get_voltage()) > 2000 && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
  else bad_count = 0;
  if(bad_count > 50){
    printf("LIFT SAFETY TRIGGERED\n");
    motor.move(0);
    set_state(lift_states::manual);
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
      break;

    case lift_states::grabbed:
      if(master.get_digital_new_press(lift_up_button)){ // lifts goal to platform height if up button is pressed
        motor.move_absolute(platform_position, 100);
        set_state(lift_states::lifting);
      }
      if(master.get_digital_new_press(lift_down_button)){ // releases goal if down button is pressed
        lift_piston.set_value(LOW);
        set_state(lift_states::releasing);
      }
      break;

    case lift_states::releasing:
      // grabs goal if up button is pressed
      if(master.get_digital_new_press(lift_up_button)){
        lift_piston.set_value(HIGH);
        set_state(lift_states::grabbed);
      }
      // waits for goal to leave trigger to enter searching state (checks for 5 cycles)
      if(!lift_trigger.get_value()) released_cycle_check++;
      else released_cycle_check = 0;
      if(released_cycle_check >= 10)  set_state(lift_states::searching);
      break;

    case lift_states::lifting:
      if(master.get_digital_new_press(lift_down_button)){ // lowers goal if down button is pressed
        motor.move_absolute(bottom_position, 100);
        set_state(lift_states::lowering);
      }
      if(fabs(motor.get_position() - platform_position) < 25){ // waits for lift to reach platform position before setting state to platform
        set_state(lift_states::platform);
      }
      break;

    case lift_states::platform:
      if(master.get_digital_new_press(lift_up_button)){ // releases goal if up button is pressed
        lift_piston.set_value(LOW);
        set_state(lift_states::released);
      }
      if(master.get_digital_new_press(lift_down_button)){ // lowers goal if down button is pressed
        motor.move_absolute(bottom_position, 100);
        set_state(lift_states::lowering);
      }
      break;

    case lift_states::released:
      // releases goal if up or down button is pressed
      if(master.get_digital_new_press(lift_up_button) || master.get_digital_new_press(lift_down_button)){
        motor.move_absolute(bottom_position, 100);
        set_state(lift_states::lowering);
      }
      break;

    case lift_states::lowering:
      if(master.get_digital_new_press(lift_up_button)){ // lifts goal to platform height if up button is pressed
        motor.move_absolute(platform_position, 100);
        set_state(lift_states::lifting);
      }
      else if(fabs(motor.get_position() - bottom_position) < 25){  // waits for lift to reach bottom before setting state to searching
        if(last_state == lift_states::released) set_state(lift_states::searching);
        else set_state(lift_states::grabbed);
      }
      break;
    case lift_states::manual:
      int lift_power = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
      if (lift_power < 0 && motor.get_position() <= bottom_position) lift_power = 0;
      if (lift_power > 0 && motor.get_position() >= top_position) lift_power = 0;
      motor.move(lift_power);
      if(master.get_digital_new_press(lift_up_button) || master.get_digital_new_press(lift_down_button)){
        lift_piston.set_value(LOW);
        motor.move_absolute(bottom_position, 100);
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
