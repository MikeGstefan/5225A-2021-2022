#include "lift.hpp"
// mecanum wheel radius = 2in, para_arm_len = 7 holes = 3.5 in, gear diameter = 3.5 in, lift_base_height = 15.5 in, arm_len = 13 in., gear ratio 21, offset_s 43 deg
// NOTE lift must reach 1336 deg to get to 35 inches
Lift lift (lift_m, 2, 3.5, 2.5, 15.5, 13.0, 0.4*7, 43.0);

Lift::Lift(pros::Motor& lift_motor, double mecanum_wheel_radius, double parallel_arm_len, double gear_diameter, double lift_base_height, double arm_len, double gear_ratio, double offset_a):
  pros::Motor(lift_motor),
  arm_len(arm_len),
  offset_a (deg_to_rad(offset_a)),
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
double Lift::deg_to_height(double deg){
  return offset_h + 2 * arm_len * sin(deg_to_rad(deg - offset_a));
}

double Lift::height_to_deg(double height){
  // return offset_h;
  // return  (2 * arm_len);
  // return rad_to_deg(asin((height - offset_h) / (2 * arm_len)));
  // return rad_to_deg(asin((height - offset_h) / (2 * arm_len))) + offset_a;

  return gear_ratio * (rad_to_deg(asin((height - offset_h) / (2 * arm_len))) + offset_a);
}
