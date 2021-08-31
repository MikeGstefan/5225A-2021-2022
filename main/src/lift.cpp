#include "lift.hpp"
// mecanum wheel radius = 2in, para_arm_len = 7 holes = 3.5 in, gear diameter = 3.5 in, lift_base_height = 15.5 in, arm_len = 13 in., gear ratio 21, offset_s 43 deg
// NOTE lift must reach 1336 deg to get to 35 inches
Lift lift (lift_m, 2.0, 3.5, 3.5, 15.5, 13.0, 7.0, 371.0);

Lift::Lift(pros::Motor& lift_motor, double mecanum_wheel_radius, double parallel_arm_len, double gear_diameter, double lift_base_height, double arm_len, double gear_ratio, double offset_a):
  pros::Motor(lift_motor),
  arm_len(arm_len),
  offset_a (offset_a),
  offset_h(parallel_arm_len + gear_diameter + lift_base_height + mecanum_wheel_radius), // should 23.5 inches)
  gear_ratio(gear_ratio)
  {}

void Lift::cal(void * params){
  lift.move(-60);
  Timer vel_rise_timeout {"lift_vel_rise"};
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
  Timer text_set_timer{"text_set"};
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
