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

// void Lift::handle(){
//   switch(state){
//     case lift_states::searching:
//       // grabs goal if limit switch is triggered or if up button is pressed
//       if(lift_trigger.get_value() || master.get_digital_new_press(lift_up_button)){
//         lift_piston.set_value(HIGH);
//         set_state(lift_states::grabbed);
//       }
//       break;
//
//     case lift_states::grabbed:
//       if(master.get_digital_new_press(lift_up_button)){ // lifts goal to platform height if up button is pressed
//         move_absolute(lift_platform_position, 100);
//         set_state(lift_states::lifting);
//       }
//       if(master.get_digital_new_press(lift_down_button)){ // releases goal if down button is pressed
//         lift_piston.set_value(LOW);
//         set_state(lift_states::releasing);
//       }
//       break;
//
//     case lift_states::releasing:
//       // grabs goal if up button is pressed
//       if(lift_trigger.get_value() || master.get_digital_new_press(lift_up_button)){
//         lift_piston.set_value(HIGH);
//         set_state(lift_states::grabbed);
//       }
//
//       // waits for goal to leave trigger to enter searching state (checks for 5 cycles)
//       if(!lift_trigger.get_value()) released_cycle_check++;
//       else released_cycle_check = 0;
//       if(released_cycle_check >= 5)  set_state(lift_states::searching);
//       break;
//
//     case lift_states::lifting:
//       if(master.get_digital_new_press(lift_down_button)){ // lowers goal if down button is pressed
//         move_absolute(lift_platform_position, 100);
//         set_state(lift_states::lowering);
//       }
//       if(fabs(get_position() - lift_platform_position) < 25){ // waits for lift to reach platform position before setting state to platform
//         set_state(lift_states::platform);
//       }
//       break;
//
//     case lift_states::platform:
//       if(master.get_digital_new_press(lift_up_button)){ // releases goal if up button is pressed
//         lift_piston.set_value(LOW);
//         set_state(lift_states::released);
//       }
//       if(master.get_digital_new_press(lift_down_button)){ // lowers goal if down button is pressed
//         move_absolute(lift_bottom_position, 100);
//         set_state(lift_states::lowering);
//       }
//       break;
//
//     case lift_states::released:
//       if(master.get_digital_new_press(lift_up_button)){ // releases goal if up button is pressed
//         move_absolute(lift_bottom_position, 100);
//         set_state(lift_states::lowering);
//       }
//       if(master.get_digital_new_press(lift_down_button)){ // lowers goal if down button is pressed
//         move_absolute(lift_bottom_position, 100);
//         set_state(lift_states::lowering);
//       }
//       break;
//
//     case lift_states::lowering:
//       if(master.get_digital_new_press(lift_up_button)){ // lifts goal to platform height if up button is pressed
//         move_absolute(lift_platform_position, 100);
//         set_state(lift_states::lifting);
//       }
//       if(fabs(get_position() - lift_bottom_position) < 25){  // waits for lift to reach bottom before setting state to searching
//         if(lift_trigger.get_value())  set_state(lift_states::grabbed);
//         else set_state(lift_states::searching);
//       }
//       break;
//
//   }
// }
// non-class stuff

lift_states lift_state = lift_states::searching;
lift_states last_lift_state = lift_state;

int lift_released_cycle_check = 0;
int lift_bad_count = 0; // cycle checkfor safeties

const int lift_bottom_position = 30;
const int lift_platform_position = 600;
const int lift_top_position = 650;

std::array<const char*, NUM_OF_LIFT_STATES> lift_state_names = {
  "searching",
  "grabbed",
  "releasing",
  "lifting",
  "platform",
  "released",
  "lowering",
  "manual",
};

void set_lift_state(lift_states next_state){
  printf("Lift | Going from %s to %s\n", lift_state_names[static_cast<int>(lift_state)], lift_state_names[static_cast<int>(next_state)]);
  last_lift_state = lift_state;
  lift_state = next_state;
}

void lift_handle(){
  if (fabs(lift_motor.get_actual_velocity()) < 5.0 && fabs(lift_motor.get_power()) > 25.0) lift_bad_count++;
  else lift_bad_count = 0;
  if(lift_bad_count > 50){
    printf("OH SHIT SAFETY TRIGGERED\n");
    lift_motor.move(0);
    set_lift_state(lift_states::manual);
  }

  switch(lift_state){
    case lift_states::searching:
      // grabs goal if limit switch is triggered or if up button is pressed
      if(lift_trigger.get_value() || master.get_digital_new_press(lift_up_button)){
        delay(50);
        master.rumble("-");
        delay(50);
        printf("*******************triggered\n");
        lift_piston.set_value(HIGH);
        set_lift_state(lift_states::grabbed);
      }
      break;

    case lift_states::grabbed:
      if(master.get_digital_new_press(lift_up_button)){ // lifts goal to platform height if up button is pressed
        lift_motor.move_absolute(lift_platform_position, 100);
        set_lift_state(lift_states::lifting);
      }
      if(master.get_digital_new_press(lift_down_button)){ // releases goal if down button is pressed
        lift_piston.set_value(LOW);
        set_lift_state(lift_states::releasing);
      }
      break;

    case lift_states::releasing:
      // grabs goal if up button is pressed
      if(master.get_digital_new_press(lift_up_button)){
        lift_piston.set_value(HIGH);
        set_lift_state(lift_states::grabbed);
      }
      // waits for goal to leave trigger to enter searching state (checks for 5 cycles)
      if(!lift_trigger.get_value()) lift_released_cycle_check++;
      else lift_released_cycle_check = 0;
      if(lift_released_cycle_check >= 5)  set_lift_state(lift_states::searching);
      break;

    case lift_states::lifting:
      if(master.get_digital_new_press(lift_down_button)){ // lowers goal if down button is pressed
        lift_motor.move_absolute(lift_bottom_position, 100);
        set_lift_state(lift_states::lowering);
      }
      if(fabs(lift_motor.get_position() - lift_platform_position) < 25){ // waits for lift to reach platform position before setting state to platform
        set_lift_state(lift_states::platform);
      }
      break;

    case lift_states::platform:
      if(master.get_digital_new_press(lift_up_button)){ // releases goal if up button is pressed
        lift_piston.set_value(LOW);
        set_lift_state(lift_states::released);
      }
      if(master.get_digital_new_press(lift_down_button)){ // lowers goal if down button is pressed
        lift_motor.move_absolute(lift_bottom_position, 100);
        set_lift_state(lift_states::lowering);
      }
      break;

    case lift_states::released:
      // releases goal if up or down button is pressed
      if(master.get_digital_new_press(lift_up_button) || master.get_digital_new_press(lift_down_button)){
        lift_motor.move_absolute(lift_bottom_position, 100);
        set_lift_state(lift_states::lowering);
      }
      break;

    case lift_states::lowering:
      if(master.get_digital_new_press(lift_up_button)){ // lifts goal to platform height if up button is pressed
        lift_motor.move_absolute(lift_platform_position, 100);
        printf("YOOOOOO");
        set_lift_state(lift_states::lifting);
      }
      else if(fabs(lift_motor.get_position() - lift_bottom_position) < 25){  // waits for lift to reach bottom before setting state to searching
        if(last_lift_state == lift_states::released) set_lift_state(lift_states::searching);
        else set_lift_state(lift_states::grabbed);
      }
      break;
    case lift_states::manual:
      int lift_power = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
      if (lift_power < 0 && lift_motor.get_position() <= lift_bottom_position) lift_power = 0;
      if (lift_power > 0 && lift_motor.get_position() >= lift_top_position) lift_power = 0;
      lift_motor.move(lift_power);
      if(master.get_digital_new_press(lift_up_button) || master.get_digital_new_press(lift_down_button)){
        lift_motor.move_absolute(lift_bottom_position, 100);
        set_lift_state(lift_states::searching);
      }
      break;

  }
}
void lift_reset(){
  lift_motor.move(-60);
  Timer vel_rise_timeout("vel_rise");
  // waits for motor's velocity to rise or timeout to trigger
  while(fabs(lift_motor.get_actual_velocity()) < 45.0){
    printf("Lift's velocity is (rising loop): %lf\n", lift_motor.get_actual_velocity());
    if (vel_rise_timeout.get_time() > 50){
      printf("Lift's rising loop timed out\n");
      break;
    }
    delay(10);
  }
  printf("Lift's velocity done rising\n");
  // waits until motors velocity slows down for 5 cycles
  cycleCheck(fabs(lift_motor.get_actual_velocity()) < 5.0, 5, 10)
  lift_motor.tare_position();  // resets subsystems position
  printf("%d, lift's reset %lf\n", millis(), lift_motor.get_position());
  lift_motor.move(0);
}
