#include "auton_util.hpp"

Gyro gyro(imu_sensor);

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

	drivebase.move(127, 0);
	waitUntil(fabs(get_angle()) > 22)
	printf("ON RAMP\n");

  double start_position = l1.get_position(); //Change to tracking
  waitUntil(l1.get_position() > start_position+400)
}

void Gyro::level(double kP, double kD){
	double last_angle=0;
	std::uint32_t steady_time = 0, last_steady_time = 0;
	PID gyro_pid(kP, 0, kD, 0);
  int speed;

	while(true){
    get_angle();
    speed = gyro_pid.compute(-angle, 0);
		drivebase.move(speed, 0);
		printf("Angle: %f   Speed: %d  \n", angle, speed); //Get rid of speed var

    //Use timer class
		if (fabs(angle-last_angle) > 0.6) last_steady_time = millis(); //Unsteady, Resets the last known steady time
		else if (fabs(angle) < 6 && steady_time > 450) break; //If within range to be level and steady on ramp

		if (controller_interrupt(true, false)) return;

		steady_time = millis()-last_steady_time;
		last_angle = angle;
		delay(10);
	}

	printf("\nLevelled on ramp\n\n\n");
	drivebase.brake();
}

//make a method of _Controller
bool controller_interrupt(bool analog, bool digital){
  if (analog){
    if (fabs(master.get_analog(ANALOG_LEFT_X)) > 15) return true;
    if (fabs(master.get_analog(ANALOG_LEFT_Y)) > 15) return true;
    if (fabs(master.get_analog(ANALOG_RIGHT_X)) > 15) return true;
    if (fabs(master.get_analog(ANALOG_RIGHT_Y)) > 15) return true;
  }
  if(digital){
    if (master.get_digital(DIGITAL_RIGHT)) return true;
    if (master.get_digital(DIGITAL_DOWN)) return true;
    if (master.get_digital(DIGITAL_LEFT)) return true;
    if (master.get_digital(DIGITAL_UP)) return true;
    if (master.get_digital(DIGITAL_A)) return true;
    if (master.get_digital(DIGITAL_B)) return true;
    if (master.get_digital(DIGITAL_Y)) return true;
    if (master.get_digital(DIGITAL_X)) return true;
    if (master.get_digital(DIGITAL_R1)) return true;
    if (master.get_digital(DIGITAL_R2)) return true;
    if (master.get_digital(DIGITAL_L1)) return true;
    if (master.get_digital(DIGITAL_L2)) return true;
  }

  return false;
}
