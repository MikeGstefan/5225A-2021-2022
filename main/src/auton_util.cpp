#include "auton_util.hpp"
#include "logging.hpp"

Gyro gyro(ramp_imu);

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
	while (inertial.is_calibrating()){
		motion_d.print("Calibrating Gyro...\n");
		delay(10);
	}
  motion_i.print("Done Calibrating Gyro\n");
}

void Gyro::climb_ramp(){
  finish_calibrating(); //Makes sure it's calibrated before starting (should already be)
	inertial.tare_roll();
  inertial.tare_pitch();

	drivebase.move_tank(127, 0);
	waitUntil(fabs(get_angle()) > 22)
	motion_i.print("ON RAMP\n");

  tracking.wait_for_dist(400); //But it's 400 encoder units. You'll have to find the equivalent in inches
}

void Gyro::level(double kP, double kD){
	double last_angle=0;
	std::uint32_t steady_time = 0, last_steady_time = 0;
	PID gyro_p(kP, 0, kD, 0);
  Timer gyro_t ("Gyro", &motion_d);
  int speed;

	while(true){
    get_angle();
    gyro_p.compute(-angle, 0);
		drivebase.move_tank(gyro_p.get_output(), 0);
    gyro_t.print("Angle: %f  |  Speed: %d", angle, gyro_p.get_output());
    
		if (fabs(angle-last_angle) > 0.6) gyro_t.reset(); //Unsteady, Resets the last known steady time
		else if (fabs(angle) < 6 && gyro_t.get_time() > 450) break; //If within range to be level and steady on ramp

		if (master.interrupt(true, false)) return;

		last_angle = angle;
		delay(10);
	}

	motion_d.print("\nLevelled on ramp\n\n");
	drivebase.brake();
}