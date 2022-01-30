#include "auton_util.hpp"

Gyro gyro(imu_sensor);

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
    drivebase.handle_input();
    angular_component = tracking.g_velocity.angle * dist_b;
    linear_component = tracking.b_velo + angular_component;

    back_velocity = linear_component - angular_component * radial_scalar;
    front_velocity = linear_component + angular_component * radial_scalar;

    printf("Velocities | back: %lf, front: %lf\n", back_velocity, front_velocity);

    if (fabs(front_velocity) < velocity_threshold){
      front_done = front_count > cycle_threshold;
      if(!front_done) front_count++;
    }
    else{ // resets front counter if robot is moving
      front_done = false;
      front_count = 0;
    }

      back_done = back_count > cycle_threshold;
    if (fabs(back_velocity) < velocity_threshold){
      if(!back_done) back_count++;
    }
    else{ // resets back counter if robot is moving
      back_done = false;
      back_count = 0;
    }

    if(front_done) drivebase.move(flatten_power, 0.0, -power_diff); // turn back end to align with wall if front is aligned
    else if(back_done) drivebase.move(flatten_power, 0.0, power_diff); //  turn front end to align with wall if back is aligned
    else drivebase.move(flatten_power, 0.0, 0.0); // otherwise continune towards wall

  }
    delay(10);
}
void find_goal_lift(bool move_stop_b){
  while(!lift_trigger.get_value()){//NEED TO ADD SAFETY
  
    delay(10);

  }
	lift_piston.set_value(HIGH);
  if(move_stop_b)move_stop();
}

void find_goal_tilter(int delay_t){
    while(tilter_dist.get() > 70){
    log_d.print("tilter d: %f\n", tilter_dist.get());
    delay(33);
  }
  log_d.print("%d | grabbing goal 2 at: (%.2f, %.2f, %.2f)\n",millis(),tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));
  delay(delay_t);
  tilter_top_piston.set_value(0);
  delay(100);
  tilter_bottom_piston.set_value(1);
  delay(200);
  tilter.move_absolute(300);
  
}
double get_front_dist(){
  double avg = 0.0;
  int count = 0;
  while(true){ 
    // for(int i = 0; i < 3; i++){
    if(front_dist.get() > 10){
      avg+= front_dist.get();
      count++;
    }
    if(count > 2){
      avg/=count;
      break;
    }
    delay(34);
  }
  return ((avg/25.4) +front_dist_dist);
  
}

Gyro::Gyro(Imu& imu): inertial(imu){}

double Gyro::get_angle() {
  angle = GYRO_SIDE*inertial.GYRO_AXIS();
  return angle;
}

void Gyro::calibrate(){
  inertial.reset();
}

void Gyro::finish_calibrating(){
	while (imu_sensor.is_calibrating()){
		printf("Calibrating Gyro...\n");
		delay(10);
	}
	printf("Done Calibrating Gyro\n");
}

void Gyro::climb_ramp(){
  finish_calibrating(); //Makes sure it's calibrated before starting (should already be)
	inertial.tare_roll();
  inertial.tare_pitch();

	drivebase.move_tank(127, 0);
	waitUntil(fabs(get_angle()) > 22)
	printf("ON RAMP\n");

  //Drive forward the equivalent of 400 encoder units

  // double start_position = l1.get_position(); //Change to tracking
  // waitUntil(l1.get_position() > start_position+400)
}

void Gyro::level(double kP, double kD){
	double last_angle=0;
	std::uint32_t steady_time = 0, last_steady_time = 0;
	PID gyro_pid(kP, 0, kD, 0);
  int speed;

	while(true){
    get_angle();
    speed = gyro_pid.compute(-angle, 0);
		drivebase.move_tank(speed, 0);
		printf("Angle: %f   Speed: %d  \n", angle, speed); //Get rid of speed var

    //Use timer class
		if (fabs(angle-last_angle) > 0.6) last_steady_time = millis(); //Unsteady, Resets the last known steady time
		else if (fabs(angle) < 6 && steady_time > 450) break; //If within range to be level and steady on ramp

		if (master.interrupt(true, false)) return;

		steady_time = millis()-last_steady_time;
		last_angle = angle;
		delay(10);
	}

	printf("\nLevelled on ramp\n\n\n");
	drivebase.brake();
}