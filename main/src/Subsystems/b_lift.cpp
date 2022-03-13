#include "b_lift.hpp"

// #define master partner

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
  held = false;
}

void B_Lift::handle(){
}

//   // joystick control
//   // lift_power = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
//   lift_power = 0;

//   // lift safety handling
//   if(state != b_lift_states::manual){
//     if (fabs(target - motor.get_position()) > end_error && fabs(motor.get_actual_velocity()) < 5.0) bad_count++;
//     else bad_count = 0;
//     if(bad_count > 25 && state != b_lift_states::manual){
//       motor.move(0);
//       master.rumble("---");
//       master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Manual      ");
//       printf("LIFT SAFETY TRIGGERED %lf, %lf\n", target, motor.get_position());
//       // intake.raise_and_disable();
//       // intake.set_state(intake_states::raised);

//       set_state(b_lift_states::manual);
//     }

//     // switches to manual control if lift joystick exceeds threshold
//     if(fabs(lift_power) > 80){
//       // // intake.raise_and_disable();
//       // // intake.set_state(intake_states::raised);
//       master.rumble("-");
//       master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Manual      ");

//       set_state(b_lift_states::manual);
//     }
//   }

//   switch(state){

//     case b_lift_states::idle:
//       // switches state to searching if up button is pressed
//       if(master.get_digital_new_press(lift_up_button) && drivebase.get_lift_button(0)){
//         master.rumble("-");
//         master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Searching    ");

//         set_state(b_lift_states::search_lip);
//       }
//       // // intake.toggle();
//       break;

//     case b_lift_states::search_lip:
//       // search for bowl of mogo once lip is detected
//       if(b_dist.get() < 70){
//         search_cycle_check_count = 0; // resets search cycle count

//         set_state(b_lift_states::search_bowl);
//       }

//       // grabs goal if up button is pressed
//       if(master.get_digital_new_press(lift_up_button) && drivebase.get_lift_button(0)){
//         master.rumble("-");
//         b_claw_p.set_value(HIGH);
//         held = true;
//         master.clear_line(B_LIFT_STATE_LINE);

//         // // intake.set_state(intake_states::on); // intake defaults to on when mogo is grabbed
//         // // intake.motor.move(INTAKE_POWER);

//         set_state(b_lift_states::grabbed);
//       }
//       // switches state to idle if down button is pressed
//       if(master.get_digital_new_press(lift_down_button)&& drivebase.get_lift_button(0)){
//         master.rumble("-");
//         master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Idle         ");
        
//         set_state(b_lift_states::idle);
//       }
//       // intake.toggle();
//       break;

//     case b_lift_states::search_bowl:
//       // grabs goal if up button is pressed or bowl is detected

//       if (b_dist.get() > 70 && b_dist.get() < 95) search_cycle_check_count++;

//       if((drivebase.get_lift_button(0) && master.get_digital_new_press(lift_up_button)) || search_cycle_check_count >= 2){
//         master.rumble("-");
//         b_claw_p.set_value(HIGH);
//         held = true;
//         master.clear_line(B_LIFT_STATE_LINE);

//         // // intake.set_state(intake_states::on); // intake defaults to on when mogo is grabbed
//         // // intake.motor.move(INTAKE_POWER);

//         set_state(b_lift_states::grabbed);
//       }
//       // switches state to idle if down button is pressed
//       if(master.get_digital_new_press(lift_down_button)&& drivebase.get_lift_button(0)){
//         master.rumble("-");
//         master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Idle         ");

//         set_state(b_lift_states::idle);
//       }
//       // // intake.toggle();
//       break;

//     case b_lift_states::grabbed:
//       if(drivebase.get_lift_button(0) && master.get_digital_new_press(lift_up_button)){ // lifts goal to tall goal platform height if up button is pressed
//         // // intake.raise_and_disable();
//         delay(100);
//         move_absolute(tall_dropoff_position);

//         set_state(b_lift_states::tall_platform);
//       }
//       // releases goal and turns off intake if down button is pressed
//       if(((master.get_digital_new_press(lift_down_button)&& drivebase.get_lift_button(0)) || (master.get_digital_new_press(lift_release_button)&& drivebase.get_lift_button(0))) && fabs(bottom_position - motor.get_position()) < end_error){
//         b_claw_p.set_value(LOW);
//         held = false;
//         // master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Idle         ");
//         release_timer.reset();
//         // // intake.motor.move(0); // turns off intake

//         set_state(b_lift_states::releasing);

//       }
//       // // intake.toggle();
//       break;

//     case b_lift_states::releasing:
//       // enters searching state again only after mogo is no longer detected, or times out
//       if(b_dist.get() > 120 || release_timer.get_time() > 3000){
//         set_state(b_lift_states::search_lip); 
//         master.rumble("-");
        
//         master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Searching      ");
//       }
//       break;

//     case b_lift_states::tip:
//       if(drivebase.get_lift_button(0) && master.get_digital_new_press(lift_up_button)){ // lifts goal to platform height if up button is pressed
//         move_absolute(last_target);

//         set_state(last_state);
//       }
//       if(master.get_digital_new_press(lift_down_button)&& drivebase.get_lift_button(0)){ // lowers goal if down button is pressed
//         move_absolute(bottom_position);

//         set_state(b_lift_states::lowering);
//       }
//       break;

//     case b_lift_states::platform:
//       // drops off goal if up button is pressed and has reached platform height
//       if(((drivebase.get_lift_button(0) && master.get_digital_new_press(lift_up_button)) || (master.get_digital_new_press(lift_release_button)&& drivebase.get_lift_button(0))) && fabs(motor.get_position() - platform_position) < end_error){
//         b_claw_p.set_value(LOW);
//         held = false;

//         set_state(b_lift_states::dropoff);
//       }
//       if(master.get_digital_new_press(b_lift_down_button)){ // lowers goal to tall platform dropoff height down button is pressed
//         move_absolute(tall_dropoff_position);

//         set_state(b_lift_states::tall_platform);
//       }
//       break;

//     case b_lift_states::tall_platform:
//       // drops off goal if up button is pressed
//       if(drivebase.get_lift_button(0) && master.get_digital_new_press(lift_up_button)){ // moves to platform height if up button is pressed
//         move_absolute(platform_position);

//         set_state(b_lift_states::platform);
//       }
//       // releases goal if release button is pressed
//       if((master.get_digital_new_press(lift_release_button)&& drivebase.get_lift_button(0)) && fabs(motor.get_position() - tall_dropoff_position) < end_error){
//         b_claw_p.set_value(LOW);
//         held = false;

//         set_state(b_lift_states::dropoff);
//       }
//       if(master.get_digital_new_press(lift_down_button) && drivebase.get_lift_button(0)){ // lowers goal if down button is pressed
//         move_absolute(raised_position);

//         set_state(b_lift_states::tip);
//       }
//       break;

//     case b_lift_states::dropoff:
//       // releases goal if up or down button is pressed
//       if((master.get_digital_new_press(lift_up_button) && drivebase.get_lift_button(0))|| (master.get_digital_new_press(lift_down_button)&& drivebase.get_lift_button(0))){
//         move_absolute(bottom_position);

//         set_state(b_lift_states::lowering);
//       }

//       break;

//     case b_lift_states::lowering:
//       // moves to last position if up button is pressed
//       if(drivebase.get_lift_button(0) && master.get_digital_new_press(lift_up_button)){
//         switch (last_state) {
//           case b_lift_states::dropoff:
//             move_absolute(last_target);
//             break;

//           case b_lift_states::tip:
//             move_absolute(raised_position);
//             break;

//           default:
//             move_absolute(raised_position);
//             break;
//         }
//         set_state(last_state);
//       }
//       // moves to next state if lift is at bottom position
//       if(fabs(motor.get_position() - bottom_position) < end_error){
//         switch (last_state) {
//           case b_lift_states::dropoff:
//             // intake_piston.set_value(LOW);
//             // intake.set_state(intake_states::off);
//             master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Searching    ");

//             set_state(b_lift_states::search_lip);
//             break;

//           case b_lift_states::tip:
//             // intake_piston.set_value(LOW); // lowers the intake again and turns it on
//             // // intake.motor.move(INTAKE_POWER);

//             set_state(b_lift_states::grabbed);
//             break;

//           default:
//             // intake_piston.set_value(LOW);
//             // // intake.set_state(intake_states::off);
//             master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Searching    ");

//             set_state(b_lift_states::search_lip);
//             break;
//         }
//         // move(-10);  // applies holding power
//       }
//       break;

//     case b_lift_states::tall_goal:

//       break;

//     case b_lift_states::manual:

//       // gives holding power if joystick is within deadzone or lift is out of range
//       if (fabs(lift_power) < 10 || (lift_power < 0 && motor.get_position() <= bottom_position) || (lift_power > 0 && motor.get_position() >= top_position)) lift_power = -10;

//       motor.move(lift_power);
//       if(master.get_digital_new_press(lift_down_button)&& drivebase.get_lift_button(0)){
//         bad_count = 0;  // resets the safety
//         move_absolute(bottom_position);
//         // intake_piston.set_value(LOW); // lowers the intake back
//         // // intake.set_state(intake_states::off);
//         if (held) set_state(b_lift_states::grabbed);
//         else{
//           master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Searching    ");
//           set_state(b_lift_states::search_lip);
//         }
//       }

//       // toggles state of lift pneumatic if lift up button is pressed
//       if(drivebase.get_lift_button(0) && master.get_digital_new_press(lift_up_button)){
//         held = !held;
//         b_claw_p.set_value(held);
//       }
//       break;

//   }
// }

double B_Lift::pos_to_height(double pos){
  return offset_h + arm_len * sin(deg_to_rad((pos - offset_a) / gear_ratio));
}

double B_Lift::height_to_pos(double height){
  return gear_ratio * (rad_to_deg(asin((height - offset_h) / (arm_len)))) + offset_a;
}

int elastic_b_up_time, elastic_b_down_time;

void B_Lift::elastic_util(){
  reset();
  motor.move(-10);
  GUI::go("Start Elastic Utility", "Press to start the elastic utility.", 500);
  b_claw_p.set_value(HIGH);
  held = true;
  Timer move_timer{"move"};
  move_absolute(top_position);
  // // intake_piston.set_value(HIGH);  // raises intake
  wait_until(fabs(motor.get_position() - top_position) < end_error);
  move_timer.print();
  elastic_b_up_time = move_timer.get_time();
  master.print(1, 0, "up time: %d", elastic_b_up_time);

  move_timer.reset();
  move_absolute(bottom_position);
  wait_until(fabs(motor.get_position() - bottom_position) < end_error);
  move_timer.print();
  elastic_b_down_time = move_timer.get_time();
  master.print(2, 0, "down time: %d", elastic_b_down_time);
}