#include "lift.hpp"

Lift lift;

void Lift::f_bar_cal(){
  f_bar.move(-60);
  Timer vel_rise_timeout("f_bar_vel_rise");
  // waits for f_bar velocity to rise or timeout to trigger
  while(fabs(f_bar.get_actual_velocity()) < 45.0){
    printf("vel (rising loop): %lf\n", f_bar.get_actual_velocity());
    if (vel_rise_timeout.get_time() > 50){
      printf("f_bar rising timeout\n");
      break;
    }
    delay(10);
  }
  printf("done rising\n");
  // waits until f_bar velocity slows down for 5 cycles
  cycleCheck(fabs(f_bar.get_actual_velocity()) < 5.0, 5, 10)
  f_bar.tare_position();
  printf("%d, f_bar reset %lf\n", millis(), f_bar.get_position());
  f_bar.move(0);
}

void Lift::c_bar_cal(){
  c_bar.move(-60);
  Timer vel_rise_timeout("c_bar_vel_rise");
  // waits for c_bar velocity to rise or timeout to trigger
  while(fabs(c_bar.get_actual_velocity()) < 45.0){
    printf("vel (rising loop): %lf\n", c_bar.get_actual_velocity());
    if (vel_rise_timeout.get_time() > 50){
      printf("c_bar rising timeout\n");
      break;
    }
    delay(10);
  }
  printf("done rising\n");
  // waits until c_bar velocity slows down for 5 cycles
  cycleCheck(fabs(c_bar.get_actual_velocity()) < 5.0, 5, 10)
  c_bar.tare_position();
  printf("%d, c_bar reset %lf\n", millis(), c_bar.get_position());
  c_bar.move(0);
}

void Lift::f_bar_elastic_util(){

  // up time should be around 750 and down time should be around 820
  c_bar_cal();
  f_bar_cal();
  Timer f_bar_timer("f_bar_timer");
  f_bar.move_absolute(730, 200);
  waitUntil(f_bar.get_position() > 700);
  master.print(0, 0, "f_bar up time: %d", f_bar_timer.get_time());
  f_bar_timer.print();
  f_bar_timer.reset();
  f_bar.move_absolute(0, 200);
  waitUntil(f_bar.get_position() < 50);
  master.print(1, 0, "f_bar down time: %d", f_bar_timer.get_time());
  f_bar_timer.print();
  f_bar.move(0);
}


tuple<double, double, double, double, double, double, bool> Lift::find_arm_angles(double target_y, double target_z, lift_position_types lift_position_type){
  // variables to determine postion type for lift
  double top_arm_speed = 100, bottom_arm_speed = 200; // in pros velocity units
  double pos_time, neg_time;
  double bottom_arm_pos_angle, top_arm_pos_angle, bottom_arm_neg_angle, top_arm_neg_angle;
  bool pos_position_valid, neg_position_valid, move_valid = true; // flags to determine if move is possible

  double bottom_arm_angle, top_arm_angle;

  target_z -= offset_h;
  target_y *= -1 ;  // reverses y axis

  double point_dist = sqrt(pow(target_y, 2.0) + pow(target_z, 2.0));
  printf("point_dist: %lf\n", point_dist);
  // this is the angle formed between the line of the target and the horizontal
  double target_angle = atan2(target_z, target_y);
  printf("target_angle: %lf\n", rad_to_deg(target_angle));
  // use law of cosines to find arm angles
  // angle 'C' is the angle formed between the bottom and top arm
  // in the law of cosines, 'a' is the bottom_arm_len, 'b' is the top_arm_len, and 'c' is the point_dist
  // angle 'B' is the angle formed between the bottom arm and the line of the target, calculated using sine law
  // remember the ambiguous case of the sine law
  double C = acos((pow(bottom_arm_len, 2) + pow(top_arm_len, 2) - pow(point_dist, 2)) / (2 * bottom_arm_len * top_arm_len));
  double B = asin(top_arm_len * sin(C) / point_dist);
  printf("C: %lf\n", rad_to_deg(C));
  printf("B: %lf\n", rad_to_deg(B));

  bottom_arm_pos_angle = rad_to_deg(target_angle - B);
  top_arm_pos_angle = top_arm_offset_a + rad_to_deg(C) - bottom_arm_pos_angle;
  if (top_arm_pos_angle < 0) top_arm_pos_angle += 360; // compensates for 360 degree wraparound
  bottom_arm_pos_angle += bottom_arm_offset_a;
  printf("\nPositive position:\n");
  printf("bottom_arm_pos_angle: %lf\n", bottom_arm_pos_angle);
  printf("top_arm_pos_angle: %lf\n", top_arm_pos_angle);

  bottom_arm_neg_angle = rad_to_deg(target_angle + B);
  top_arm_neg_angle = top_arm_offset_a - rad_to_deg(C) - bottom_arm_neg_angle;
  if (top_arm_neg_angle < 0) top_arm_neg_angle += 360; // compensates for 360 degree wraparound
  bottom_arm_neg_angle += bottom_arm_offset_a;
  printf("\nNegative position:\n");
  printf("bottom_arm_neg_angle: %lf\n", bottom_arm_neg_angle);
  printf("top_arm_neg_angle: %lf\n", top_arm_neg_angle);

  // Note: All ARM angles are in degrees from this point on

  // multiplying arm angles by gear ratio
  printf("\nAFTER scaling by gear ratio:\n");
  bottom_arm_pos_angle *= bottom_arm_gear_ratio;
  top_arm_pos_angle *= top_arm_gear_ratio;
  printf("bottom_arm_pos_angle: %lf\n", bottom_arm_pos_angle);
  printf("top_arm_pos_angle: %lf\n", top_arm_pos_angle);

  bottom_arm_neg_angle *= bottom_arm_gear_ratio;
  top_arm_neg_angle *= top_arm_gear_ratio;
  printf("bottom_arm_neg_angle: %lf\n", bottom_arm_neg_angle);
  printf("top_arm_neg_angle: %lf\n\n", top_arm_neg_angle);

  // code to figure out lift position type
  pos_position_valid = bottom_arm_pos_angle < bottom_arm_upper_limit && bottom_arm_pos_angle > bottom_arm_lower_limit && top_arm_pos_angle < top_arm_upper_limit && top_arm_pos_angle > top_arm_lower_limit;
  neg_position_valid = bottom_arm_neg_angle < bottom_arm_upper_limit && bottom_arm_neg_angle > bottom_arm_lower_limit && top_arm_neg_angle < top_arm_upper_limit && top_arm_neg_angle > top_arm_lower_limit;

  switch(lift_position_type){
  case lift_position_types::positive:
    bottom_arm_angle = bottom_arm_pos_angle;
    top_arm_angle = top_arm_pos_angle;
    move_valid = pos_position_valid;
    break;
  case lift_position_types::negative:
    bottom_arm_angle = bottom_arm_neg_angle;
    top_arm_angle = top_arm_neg_angle;
    move_valid = neg_position_valid;
    break;
  case lift_position_types::fastest:
    if (pos_position_valid && neg_position_valid){
      pos_time = max((f_bar.get_position() - bottom_arm_pos_angle) / bottom_arm_speed, (c_bar.get_position() - top_arm_pos_angle) / top_arm_speed);
      neg_time = max((f_bar.get_position() - bottom_arm_neg_angle) / bottom_arm_speed, (c_bar.get_position() - top_arm_neg_angle) / top_arm_speed);
      if(neg_time < pos_time){
        bottom_arm_angle = bottom_arm_neg_angle;
        top_arm_angle = top_arm_neg_angle;
      }
      else {
        bottom_arm_angle = bottom_arm_pos_angle;
        top_arm_angle = top_arm_pos_angle;
      }
    }
    else if (pos_position_valid){
      bottom_arm_angle = bottom_arm_pos_angle;
      top_arm_angle = top_arm_pos_angle;
    }
    else if (neg_position_valid){
      bottom_arm_angle = bottom_arm_neg_angle;
      top_arm_angle = top_arm_neg_angle;
    }
    else  move_valid = false;
    break;
  }

  // defaults to positive for testing only
  // bottom_arm_angle = bottom_arm_pos_angle;
  // top_arm_angle = top_arm_pos_angle;

  printf("Final bottom_arm_angle: %lf\n", bottom_arm_angle);
  printf("Final top_arm_angle: %lf\n", top_arm_angle);
  return {top_arm_angle, bottom_arm_angle, top_arm_pos_angle, bottom_arm_pos_angle, top_arm_neg_angle, bottom_arm_neg_angle, move_valid};
}

void Lift::move_to_target(const double target_y, const double target_z, const lift_position_types lift_position_type, const bool wait_for_complete, const double bottom_arm_speed, const double top_arm_speed){
  auto[top_arm_angle, bottom_arm_angle, top_arm_pos_angle, bottom_arm_pos_angle, top_arm_neg_angle, bottom_arm_neg_angle, move_valid] = find_arm_angles(target_y, target_z, lift_position_type);
  if (move_valid){
    f_bar.move_absolute(bottom_arm_angle, bottom_arm_speed);
    if(bottom_arm_angle / bottom_arm_gear_ratio > bottom_arm_offset_a) waitUntil(f_bar.get_position() / bottom_arm_gear_ratio > bottom_arm_offset_a);
    c_bar.move_absolute(top_arm_angle, top_arm_speed);
    if (wait_for_complete){
      waitUntil(fabs(f_bar.get_position() - bottom_arm_angle) < bottom_arm_end_error && fabs(c_bar.get_position() - top_arm_angle) < top_arm_end_error);
    }
  }
  else printf("POSITION IS INVALID | POS: TOP: %lf, BOTTOM: %lf | NEG: TOP: %lf, BOTTOM: %lf\n", top_arm_pos_angle, bottom_arm_pos_angle, top_arm_neg_angle, bottom_arm_neg_angle);
}

void Lift::move_to_target_util(){
  double target_y = -6.0, target_z = 13.0;

  master.clear();
  delay(50);
  master.print(0, 0, "z: %lf", target_z);
  delay(50);
  master.print(1, 0, "y: %lf", target_y);
  move_to_target(target_y, target_z);

	while(true){
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT)){
			target_y++;
			delay(50);
			master.print(1, 0, "y: %lf", target_y);
      move_to_target(target_y, target_z, lift_position_types::fastest, false);
		}
		else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_LEFT)){  // decrease height
			target_y--;
			delay(50);
			master.print(1, 0, "y: %lf", target_y);
      move_to_target(target_y, target_z, lift_position_types::fastest, false);
		}
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){  // increase height
			target_z++;
			delay(50);
			master.print(0, 0, "z: %lf", target_z);
      move_to_target(target_y, target_z, lift_position_types::fastest, false);
		}
		else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)){  // decrease height
			target_z--;
			delay(50);
			master.print(0, 0, "z: %lf", target_z);
      move_to_target(target_y, target_z, lift_position_types::fastest, false);
		}
		delay(10);
	}
}

double Lift::find_y_pos(){
    return top_arm_len * cos(deg_to_rad(c_bar.get_position() / top_arm_gear_ratio - top_arm_offset_a)) - bottom_arm_len * cos(deg_to_rad(f_bar.get_position() / bottom_arm_gear_ratio - bottom_arm_offset_a));
}

double Lift::find_z_pos(){
    return offset_h + top_arm_len * sin(deg_to_rad(c_bar.get_position() / top_arm_gear_ratio - top_arm_offset_a)) + bottom_arm_len * sin(deg_to_rad(f_bar.get_position() / bottom_arm_gear_ratio - bottom_arm_offset_a));
}

double Lift::find_top_arm_angle(const double target_y){
  return rad_to_deg(deg_to_rad(top_arm_offset_a) - acos((cos(deg_to_rad(f_bar.get_position() / bottom_arm_gear_ratio) - deg_to_rad(bottom_arm_offset_a)) * bottom_arm_len + target_y) / top_arm_len)) * top_arm_gear_ratio;
}

void Lift::touch_line(const double target_y, double speed){ // should be in motor degrees
  double top_arm_angle = find_top_arm_angle(target_y);
  if (top_arm_angle > top_arm_upper_limit || top_arm_angle < top_arm_lower_limit) // move is invalid
    printf("MOVE INVALID: top arm position attempt: %lf\n", top_arm_angle);
  else
    c_bar.move_absolute(top_arm_angle, speed);
}

double Lift::get_arm_velocity_ratio(const double target_y){

  // system delay is 61.111111 ms
  // or 0.00101851851667 minutes
  double system_delay = 0.00102;

  double bottom_arm_angle = deg_to_rad(f_bar.get_position() + system_delay * f_bar.get_actual_velocity());
  // printf("acc f_bar_position_rad: %lf\t", deg_to_rad(f_bar.get_position()));
  // is returning derivative of find_top_arm_angle function
  return (bottom_arm_len * top_arm_gear_ratio * sin(deg_to_rad(bottom_arm_offset_a) - bottom_arm_angle / bottom_arm_gear_ratio)) / (top_arm_len * bottom_arm_gear_ratio * sqrt(1 - pow(bottom_arm_len * cos(deg_to_rad(bottom_arm_offset_a) - bottom_arm_angle / bottom_arm_gear_ratio) + target_y, 2) / pow(top_arm_len, 2)));

}

void Lift::move_on_line(double target_y, double target_z_start, double target_z_end, const double speed){
  move_to_target(target_y, target_z_start, lift_position_types::fastest, true, 80, 80); // goes to position above the rings
  // delay(300);
  // grabs position for f_bar to reach when at bottom of ring stack
  auto[top_arm_angle, bottom_arm_angle, top_arm_pos_angle, bottom_arm_pos_angle, top_arm_neg_angle, bottom_arm_neg_angle, move_valid] = lift.find_arm_angles(target_y, target_z_end);
  f_bar.move_absolute(bottom_arm_angle, speed); // moves f_bar to bottom of ring stack
  double top_arm_target, open_loop_velocity, closed_loop_velocity;
  double ratio;
  PID top_arm(0.5, 0.0, 0.3, 0.0, true, 0.0, 100.0);

  while(fabs(target_z_end - find_z_pos()) > 0.5 || fabs(target_y - find_y_pos()) > 0.5){  // moves the chain bar to maintain the horizontal distance
      top_arm_target = find_top_arm_angle(target_y);
      // printf("target: %lf, actual: %lf, diff: %lf, vel: %lf\n", top_arm_target, c_bar.get_position(), find_top_arm_angle(target_y) - c_bar.get_position(), (find_top_arm_angle(target_y) - c_bar.get_position()) * top_arm_kp);
      ratio = get_arm_velocity_ratio(target_y);
      // printf("ratio: %lf, f_bar_pos: %lf, f_bar_pos_rad: %lf, f_bar_vel: %lf, acc_vel: %lf\n", ratio, f_bar.get_position(), deg_to_rad(f_bar.get_position()), f_bar.get_actual_velocity(), ratio * f_bar.get_actual_velocity());
      open_loop_velocity = ratio * f_bar.get_actual_velocity();
      closed_loop_velocity = top_arm.compute(c_bar.get_position(), find_top_arm_angle(target_y));

      c_bar.move_velocity(open_loop_velocity + closed_loop_velocity);
      // c_bar.move_velocity(open_loop_velocity + closed_loop_velocity);
      printf("%lf, %lf\n", find_y_pos(), find_z_pos());
      delay(10);
  }
  c_bar.move_relative(0, 100); // stops motor
}

void Lift::pickup_rings(){
  stop_pickup_task();
  task = new Task(::pickup_rings);  // calls global function pickup rings asynchronously
}

void Lift::stop_pickup_task(){
  if(task != nullptr){
    task->remove();
    delete task;
    task = nullptr;
  }
}

void Lift::set_state(const states next_state){
  printf("Lift | Going from %d to %d", state, next_state);
  last_state = state;
  state = next_state;
}

void Lift::handle(){
  switch(state){
    case idle:
      /*
      if (intake.state == intake.states::full){ // pickup rings if the intake is full
        pickup_rings();
        set_state(full);
        intake.set_state(intake.states::searching);
      }
      */
      if (master.get_digital_new_press(f_bar_down_button)){ // lowers f_bar to pickup mogos with forks if fbar down is pressed
        lower_f_bar();
        set_state(down);
      }
      if (master.get_digital_new_press(cancel_dropoff_button)){ // brings to neutral position
        move_to_neutral_position();
      }


      break;
    case full:
      if (master.get_digital_new_press(ring_dropoff_button)){
        if (dropoff_front){

        }
        else{

        }
      }
      break;
    case down:

      break;
    case raised:

      break;
    case platform:

      break;
    case tip:

      break;
    case dropoff:

      break;
  }

}


void pickup_rings(void* params){  // async overload of Lift::pickup_rings
  // close stabber
  lift.move_on_line(-3.25, 20.0, 9.5, 70);
  // open stabber
  lift.move_on_line(-2.75, 9.5, 20.0, 70);
}
