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



// double get_front_dist(){
//   double avg = 0.0;
//   int count = 0;
//   while(true){ 
//     // for(int i = 0; i < 3; i++){
//     if(front_dist.get() > 10){
//       avg+= front_dist.get();
//       count++;
//     }
//     if(count > 2){
//       avg/=count;
//       break;
//     }
//     delay(34);
//   }
//   return ((avg/25.4) +front_dist_dist);
  
// }

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