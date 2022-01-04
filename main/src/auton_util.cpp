#include "auton_util.hpp"

double get_filtered_output(ADIUltrasonic sensor, int check_count, uint16_t lower_bound, uint16_t upper_bound, int timeout){
  Timer timer{"Timer"};
  int success_count = 0;

  long total_input;
  uint16_t input;
  double filtered_output;
  while(timer.get_time() < timeout && success_count < check_count){
    input = sensor.get_value();
    if (lower_bound <= input <= upper_bound){

      total_input += input;
      printf("input: %d\n", input);
      success_count++;
    }
    else printf("FAILED! input: %d\n", input);
    delay(10);
  }
  filtered_output = total_input / success_count;
  printf("filtered output: %lf\n", filtered_output);
  return filtered_output;
}

void flatten_against_wall(bool right){
  bool front_done, back_done;
  int flatten_power = 50, power_diff = 15;
  double front_velocity, back_velocity;
  const double dist_b = 3.0;  // make sure to keep this same as in tracking
  const double simulated_radius = 8.0;
  const double radial_scalar = simulated_radius / dist_b; // how much to multiples the actual radius by to get the simulated radius
  double angular_component;


  if(right){
    const double velocity_threshold = 0.5;  // in inches/sec
    drivebase.move(flatten_power, 0.0, 0.0);
    delay(100); // waits for robots velocity to rise

    while(!(front_done && back_done)){
      angular_component = tracking.g_velocity.angle * dist_b;
      back_velocity = tracking.b_velo * angular_component * radial_scalar;
      front_velocity = back_velocity - 2 * simulated_radius * tracking.g_velocity.angle;

      front_done = front_velocity < velocity_threshold;
      back_done = back_velocity < velocity_threshold;

      if(front_done) drivebase.move(30.0, 0.0, -power_diff);
      else if(back_done) drivebase.move(30.0, 0.0, power_diff);
      else drivebase.move(flatten_power, 0.0, 0.0);

    }
  }
  else{
    const double velocity_threshold = -0.5;  // in inches/sec
    drivebase.move(-flatten_power, 0.0, 0.0);
    delay(100); // waits for robots velocity to rise


    while(!(front_done && back_done)){
      back_velocity = tracking.b_velo * radial_scalar;
      front_velocity = back_velocity - 2 * simulated_radius * tracking.g_velocity.angle;

      front_done = front_velocity > velocity_threshold;
      back_done = tracking.b_velo > velocity_threshold;

      if(front_done) drivebase.move(30.0, 0.0, power_diff);
      else if(back_done) drivebase.move(30.0, 0.0, -power_diff);
      else drivebase.move(flatten_power, 0.0, 0.0);
    }
  }
}
