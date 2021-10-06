#include "lift.hpp"
// mecanum wheel radius = 2in, para_arm_len = 7 holes = 3.5 in, gear diameter = 3.5 in, lift_base_height = 15.5 in, arm_len = 13 in., gear ratio 21, offset_s 43 deg
// NOTE lift must reach 1336 deg to get to 35 inches



// Lift lift (lift_m, 2.0, 3.5, 3.5, 15.5, 13.0, 7.0, 371.0);
/*
Lift::Lift(pros::Motor& lift_motor, double mecanum_wheel_radius, double parallel_arm_len, double gear_diameter, double lift_base_height, double arm_len, double gear_ratio, double offset_a):
  pros::Motor(lift_motor),
  arm_len(arm_len),
  offset_a (offset_a),
  offset_h(parallel_arm_len + gear_diameter + lift_base_height + mecanum_wheel_radius), // should 23.5 inches)
  gear_ratio(gear_ratio)
  {}

void Lift::cal(void * params){
  lift.move(-60);
  Timer vel_rise_timeout("lift_vel_rise");
  // waits for lift velocity to rise or timeout to trigger
  while(fabs(lift.get_actual_velocity()) < 45.0){
    printf("vel (rising loop): %lf\n", lift.get_actual_velocity());
    if (vel_rise_timeout.get_time() > 50){
      printf("lift rising timeout\n");
      break;
    }
    delay(10);
  }
  printf("done rising\n");
  // waits until lift velocity slows down for 5 cycles
  cycleCheck(fabs(lift.get_actual_velocity()) < 5.0, 5, 10)
  lift.tare_position();
  printf("%d, lift reset %lf\n", millis(), lift.get_position());
  delay(50);
  lift.move(0);
}

// NOTE: these do not work yet
double Lift::pos_to_height(double pos){
  return offset_h + 2 * arm_len * sin(deg_to_rad((pos - offset_a) / gear_ratio));
}
double Lift::height_to_pos(double height){
  return gear_ratio * (rad_to_deg(asin((height - offset_h) / (2 * arm_len)))) + offset_a;
}

void Lift::lift_height_util(){
  lift.cal();
	double target_height = 5.0; // default target height is 5 inches (bottom height)
  master.clear();
  delay(150);
  Timer text_set_timer("text_set");
  int text_set_line = 0;  // which line is next to be set
	while (true){
		printf("pos:%lf, height:%lf, target_height: %lf\n", lift.get_position(), lift.pos_to_height(lift.get_position()), target_height);

    // delays allow for text-setting
    if (text_set_timer.get_time() >= 50 && text_set_line == 0){
      master.print(0, 0, "pos: %.1lf", lift.get_position());
      text_set_timer.reset();
      text_set_line++;
    }
    if (text_set_timer.get_time() >= 50 && text_set_line == 1){
      master.print(1, 0, "height: %.1lf", lift.get_position());
      text_set_timer.reset();
      text_set_line++;
    }
    if (text_set_timer.get_time() >= 50 && text_set_line == 2){
      master.print(2, 0, "target_height: %.1lf", target_height);
      text_set_timer.reset();
      text_set_line = 0;
    }
    // only allows alteration of height is within lift range
    if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){
      if(target_height < 48.0)  target_height += 1.0;
    }
    if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)){
      if(target_height > 5.0)  target_height -= 1.0;
    }

    lift.move_absolute(lift.height_to_pos(target_height), 100); // default speed is 100 rpm
		// delay(60);
	}

}
*/
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
  double bottom_arm_lower_limit = 0, bottom_arm_upper_limit = 700, top_arm_lower_limit = 0, top_arm_upper_limit = 500;
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

void Lift::move_to_target(const double target_y, const double target_z, const lift_position_types lift_position_type, const double bottom_arm_speed, const double top_arm_speed){
  auto[top_arm_angle, bottom_arm_angle, top_arm_pos_angle, bottom_arm_pos_angle, top_arm_neg_angle, bottom_arm_neg_angle, move_valid] = find_arm_angles(target_y, target_z, lift_position_type);
  if (move_valid){
    f_bar.move_absolute(bottom_arm_angle, top_arm_speed);
    if(bottom_arm_angle / bottom_arm_gear_ratio > bottom_arm_offset_a) waitUntil(f_bar.get_position() / bottom_arm_gear_ratio > bottom_arm_offset_a);
    c_bar.move_absolute(top_arm_angle, bottom_arm_speed);
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
      move_to_target(target_y, target_z);
		}
		else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_LEFT)){  // decrease height
			target_y--;
			delay(50);
			master.print(1, 0, "y: %lf", target_y);
      move_to_target(target_y, target_z);
		}
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){  // increase height
			target_z++;
			delay(50);
			master.print(0, 0, "z: %lf", target_z);
      move_to_target(target_y, target_z);
		}
		else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)){  // decrease height
			target_z--;
			delay(50);
			master.print(0, 0, "z: %lf", target_z);
      move_to_target(target_y, target_z);
		}
		delay(10);
	}
}
