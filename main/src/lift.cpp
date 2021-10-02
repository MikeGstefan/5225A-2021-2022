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

void f_bar_cal(){
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
  delay(50);
  f_bar.move(0);
}

void c_bar_cal(){
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
  delay(50);
  c_bar.move(0);
}

void f_bar_elastic_util(){

  // up time should be around 750 and down time should be around 820
  c_bar_cal();
  f_bar_cal();
  Timer f_bar_timer("f_bar_timer");
  f_bar.move_absolute(800, 200);
  waitUntil(f_bar.get_position() > 750);
  master.print(0, 0, "f_bar up time: %d", f_bar_timer.get_time());
  f_bar_timer.print();
  f_bar_timer.reset();
  f_bar.move_absolute(0, 200);
  waitUntil(f_bar.get_position() < 50);
  master.print(1, 0, "f_bar down time: %d", f_bar_timer.get_time());
  f_bar_timer.print();
  f_bar.move(0);
}

/*
void find_arm_angles(Vector2D target, lift_position_types lift_position_type){
    double pos_dist, neg_dist;
    double bottom_arm_speed, top_arm_speed; // should be in pros velocity units
    double bottom_arm_lower_limit = 0, bottom_arm_upper_limit, top_arm_lower_limit = 0, top_arm_upper_limit;
    double bottom_arm_pos_angle, top_arm_pos_angle, bottom_arm_neg_angle, top_arm_neg_angle;
    double bottom_arm_gear_ratio = 7.0, top_arm_ratio = 5.0/3.0;
    double bottom_arm_angle, top_arm_angle;
    bool pos_position_valid, neg_position_valid;
    double bottom_arm_len = 14.5, top_arm_len = 11.5;
    double dist_between_axles = 10.5;
    double bottom_arm_offset_a = deg_to_rad(53.0); // this will be in encoder degrees but needs to be converted to arm degrees
    double top_arm_offset_a = deg_to_rad(54.0); // this will be in encoder degrees but needs to be converted to arm degrees
    double offset_h = 17.0 + dist_between_axles; // height of arm + wheel_radius + dist_between_axles
    printf("offset_h: %lf\n", offset_h);
    target.y -= offset_h;
    double point_dist = sqrt(pow(target.x, 2.0) + pow(target.y, 2.0));
    printf("point_dist: %lf\n", point_dist);
    // use law of cosines to find arm angles
    // angle 'C' is the angle formed between the bottom and top arm
    // in the law of cosines, 'a' is the bottom_arm_len, 'b' is the top_arm_len, and 'c' is the point_dist
    // this is the angle formed between the line of the target and the horizontal
    double target_angle = atan2(target.y, target.x);
    printf("target_angle: %lf\n", rad_to_deg(target_angle));
    // angle 'B' is the angle formed between the bottom arm and the line of the target, calculated using sine law
    // beware of the ambiguous case of the sine law
    // correct
    double C = acos((pow(bottom_arm_len, 2) + pow(top_arm_len, 2) - pow(point_dist, 2)) / (2 * bottom_arm_len * top_arm_len));
    double B = asin(top_arm_len * sin(C) / point_dist);
    bottom_arm_pos_angle = target_angle - B;
    top_arm_pos_angle = C - bottom_arm_pos_angle + top_arm_offset_a;
    bottom_arm_pos_angle += bottom_arm_offset_a;
    printf("C: %lf\n", rad_to_deg(C));
    printf("B: %lf\n", rad_to_deg(B));

    printf("\nPos POSITION:\n\n");

    printf("bottom_arm_angle: %lf\n", rad_to_deg(bottom_arm_pos_angle));
    printf("top_arm_angle: %lf\n", rad_to_deg(top_arm_pos_angle));

    printf("\nNeg POSITION:\n\n");

    bottom_arm_neg_angle = target_angle + B;
    top_arm_neg_angle = 2 * M_PI - C - bottom_arm_neg_angle + top_arm_offset_a;
    bottom_arm_neg_angle += bottom_arm_offset_a;
    printf("bottom_arm_angle: %lf\n", rad_to_deg(bottom_arm_neg_angle));
    printf("top_arm_angle: %lf\n", rad_to_deg(top_arm_neg_angle));

    printf("AFTER scaling by gear ratio:\n");
    bottom_arm_pos_angle *= bottom_arm_gear_ratio;
    top_arm_pos_angle *= top_arm_ratio;

    bottom_arm_neg_angle *= bottom_arm_gear_ratio;
    top_arm_neg_angle *= top_arm_ratio;
    // code to figure out lift position
    // if positive lift positions is valid
    // pos_position_valid = bottom_arm_pos_angle < bottom_arm_upper_limit && bottom_arm_pos_angle > bottom_arm_lower_limit && top_arm_pos_angle < top_arm_upper_limit && top_arm_pos_angle;
    // // if negative lift position is valid
    // neg_position_valid = bottom_arm_pos_angle < bottom_arm_upper_limit && bottom_arm_pos_angle > bottom_arm_lower_limit && top_arm_pos_angle < top_arm_upper_limit && top_arm_pos_angle;
    // switch(lift_position_type){
    // case lift_position_types::positive:
    //   if (pos_position_valid){
    //     bottom_arm_angle = bottom_arm_pos_angle;
    //     top_arm_angle = top_arm_pos_angle;
    //   }
    //   else printf("POSITIVE POSITION IS INVALID, TOP:%lf, BOTTOM:%lf\n", top_arm_angle, bottom_arm_angle);
    //   break;
    // case lift_position_types::negative:
    //   if (neg_position_valid){
    //     bottom_arm_angle = bottom_arm_neg_angle;
    //     top_arm_angle = top_arm_neg_angle;
    //   }
    //   else printf("POSITIVE POSITION IS INVALID, TOP:%lf, BOTTOM:%lf\n", top_arm_angle, bottom_arm_angle);
    //   break;
    // case lift_position_types::fastest:
    //
    //   if (pos_position_valid){
    //     pos_dist = max((f_bar.get_position() - rad_to_deg(bottom_arm_pos_angle)) / bottom_arm_speed, (c_bar.get_position() - rad_to_deg(top_arm_pos_angle)) / top_arm_speed);
    //   }
    //   // else
    //   if (neg_position_valid){
    //     neg_dist = max((f_bar.get_position() - rad_to_deg(bottom_arm_neg_angle)) / bottom_arm_speed, (c_bar.get_position() - rad_to_deg(top_arm_neg_angle)) / top_arm_speed);
    //
    //   }
    //
    //   break;
    //
    // }

    // defaults to positive rn
    printf("bottom_arm_angle: %lf\n", rad_to_deg(bottom_arm_pos_angle));
    printf("top_arm_angle: %lf\n", rad_to_deg(top_arm_pos_angle));

    f_bar.move_absolute(rad_to_deg(bottom_arm_pos_angle), 80);
    waitUntil(f_bar.get_position() / 7.0 > 50);
    c_bar.move_absolute(rad_to_deg(top_arm_pos_angle), 80);
}
*/





void find_arm_angles(Vector2D target, lift_position_types lift_position_type){
  double bottom_arm_gear_ratio = 7.0, top_arm_gear_ratio = 5.0/3.0;
  double bottom_arm_angle, top_arm_angle;
  double bottom_arm_len = 14.5, top_arm_len = 11.5;
  double dist_between_axles = 10.5;
  double bottom_arm_offset_a = deg_to_rad(55.0); // this will be in encoder degrees but needs to be converted to arm degrees
  double top_arm_offset_a = deg_to_rad(60.0); // this will be in encoder degrees but needs to be converted to arm degrees
  double offset_h = 17.0 + dist_between_axles; // height of arm + wheel_radius + dist_between_axles
  target.y -= offset_h;
  double point_dist = sqrt(pow(target.x, 2.0) + pow(target.y, 2.0));
  printf("point_dist: %lf\n", point_dist);
  // use law of cosines to find arm angles
  // angle 'C' is the angle formed between the bottom and top arm
  // in the law of cosines, 'a' is the bottom_arm_len, 'b' is the top_arm_len, and 'c' is the point_dist
  // this is the angle formed between the line of the target and the horizontal
  double target_angle = atan2(target.y, target.x);
  printf("target_angle: %lf\n", rad_to_deg(target_angle));
  // angle 'B' is the angle formed between the bottom arm and the line of the target, calculated using sine law
  // beware of the ambiguous case of the sine law
  // correct
  double C = acos((pow(bottom_arm_len, 2) + pow(top_arm_len, 2) - pow(point_dist, 2)) / (2 * bottom_arm_len * top_arm_len));
  double B = asin(top_arm_len * sin(C) / point_dist);
  bottom_arm_angle = target_angle - B;
  top_arm_angle = C - bottom_arm_angle + top_arm_offset_a;
  bottom_arm_angle += bottom_arm_offset_a;
  printf("C: %lf\n", rad_to_deg(C));
  printf("B: %lf\n", rad_to_deg(B));

  printf("bottom_arm_angle: %lf\n", rad_to_deg(bottom_arm_angle));
  printf("top_arm_angle: %lf\n", rad_to_deg(top_arm_angle));

  printf("\nNegative POSITION:\n\n");
  //
  bottom_arm_angle = target_angle + B;
  top_arm_angle = top_arm_offset_a - C - bottom_arm_angle;
  if (top_arm_angle < 0) top_arm_angle += 2 * M_PI; // compensates for 360 degree wraparound
  bottom_arm_angle += bottom_arm_offset_a;
  printf("bottom_arm_angle: %lf\n", rad_to_deg(bottom_arm_angle));
  printf("top_arm_angle: %lf\n", rad_to_deg(top_arm_angle));

  bottom_arm_angle *= bottom_arm_gear_ratio;
  top_arm_angle *= top_arm_gear_ratio;
  printf("AFTER scaling by gear ratio:\n");
  printf("bottom_arm_angle: %lf\n", rad_to_deg(bottom_arm_angle));
  printf("top_arm_angle: %lf\n", rad_to_deg(top_arm_angle));

  f_bar.move_absolute(rad_to_deg(bottom_arm_angle), 150);
  if (rad_to_deg(bottom_arm_angle) / bottom_arm_gear_ratio > 50){
    waitUntil(f_bar.get_position() / bottom_arm_gear_ratio > 50);
  }
  c_bar.move_absolute(rad_to_deg(top_arm_angle), 100);
  // while(!master.get_digital_new_press(E_CONTROLLER_DIGITAL_X)){
  //   printf("f_bar: %lf c_bar: %lf\n", f_bar.get_position(), c_bar.get_position());
  //   delay(10);
  // }
}
