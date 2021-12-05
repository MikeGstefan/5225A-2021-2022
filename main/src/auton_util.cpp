#include "auton_util.hpp"

void calibrate_gyro(){
	while (gyro.is_calibrating()){
		printf("Calibrating Gyro...\n");
		delay(10);
	}
	printf("Done Calibrating Gyro\n");
}

void climb_ramp(){
	calibrate_gyro(); //Makes sure it's calibrated before starting (should already be)

	gyro.tare_roll();
	drivebase.move(127, 0);
	waitUntil(gyro.get_roll() < -22)
	printf("ON RAMP\n");
	double start_position = l1.get_position();
	waitUntil(l1.get_position()-start_position > 400)

	double angle, last_angle=0;
	std::uint32_t steady_time = 0, last_steady_time = 0;
	PID gyro_pid(2, 0, 250, 0);

	while(true){
		angle = gyro.get_roll();
		drivebase.move(gyro_pid.compute(angle, 0), 0);
		// printf("Angle: %f time: %d da: %f    \n", angle, steady_time, fabs(angle-last_angle));

		if (fabs(angle-last_angle) > 0.6) last_steady_time = millis(); //Unsteady
		else if (fabs(angle) < 6 && steady_time > 450) break; //If within range to be level and steady on ramp

		if (fabs(master.get_analog(ANALOG_LEFT_X)) > 15) return;
		if (fabs(master.get_analog(ANALOG_LEFT_Y)) > 15) return;
		if (fabs(master.get_analog(ANALOG_RIGHT_X)) > 15) return;
		if (fabs(master.get_analog(ANALOG_RIGHT_Y)) > 15) return;

		steady_time = millis()-last_steady_time;
		last_angle = angle;
		delay(10);
	}

	printf("\n\nDONE\n");
	drivebase.brake();
}
