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
  int front_count = 0, back_count = 0;
  double front_velocity, back_velocity;
  const double dist_b = 3.0;  // make sure to keep this same as in tracking
  const double simulated_radius = 8.0;
  const double radial_scalar = simulated_radius / dist_b; // how much to multiples the actual radius by to get the simulated radius
  double angular_component, linear_component;
  const int cycle_threshold = 20;
  const double velocity_threshold = 0.5;  // in inches/sec
  int flatten_power, power_diff;

  if(right) flatten_power = 60, power_diff = 20;
  else  flatten_power = -60, power_diff = -20;

  delay(100); // waits for robots velocity to rise
  drivebase.move(flatten_power, 0.0, 0.0);

  // while(!(front_done && back_done)){
  while(true){
    angular_component = tracking.g_velocity.angle * dist_b * radial_scalar;
    linear_component = tracking.b_velo - angular_component;

    back_velocity = linear_component + angular_component;
    front_velocity = linear_component - angular_component;

    printf("Velocities | back: %lf, front: %lf\n", back_velocity, front_velocity);

    if (fabs(front_velocity) < velocity_threshold){
      front_done = front_count > cycle_threshold;
      if(!front_done) front_count++;
    }
    else{ // resets front counter if robot is moving
      front_done = false;
      front_count = 0;
    }

    if (fabs(back_velocity) < velocity_threshold){
      back_done = back_count > cycle_threshold;
      if(!back_done) back_count++;
    }
    else{ // resets back counter if robot is moving
      back_count = 0;
      back_done = false;
    }

    if(front_done) drivebase.move(flatten_power, 0.0, -power_diff); // turn back end to align with wall if front is aligned
    else if(back_done) drivebase.move(flatten_power, 0.0, power_diff); //  turn front end to align with wall if back is aligned
    else drivebase.move(flatten_power, 0.0, 0.0); // otherwise continune towards wall

    delay(10);
  }
}
