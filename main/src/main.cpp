#include "main.h"
#include "logging.hpp"
#include "config.hpp"
#include "util.hpp"
#include "Tracking.hpp"
#include "lift.hpp"
#include "drive.hpp"
#include "pid.hpp"

// using namespace std;
// using namespace pros;


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	// drivebase.download_curve_data();

	delay(150);
	tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;
	updateStartTask();

	pros::lcd::initialize();
	// gyro.reset();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
	f_bar.move(50);
	c_bar.move(60);
	// drivebase.update_lookup_table_util();
	// printf("here\n");
	// rush_goal2(0.0, -45.0, 0.0);
	// drivebase.move(0,-127, 0);
	// while(!claw_touch.get_value()) delay(10);
	// master.print(0, 0, "inches: %lf", tracking.y_coord);
/*
	Timer move_timer{"move_timer"};
	// move_to_target_async(0.0, -45.0, 0.0);	// got to goal
	// while(tracking.y_coord > -45.0)	delay(10);
	master.print(0, 0, "time: %d", move_timer.get_time());
	rush_goal(0.0, -20.0, 0.0);
*/
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	Timer cal_timer("cal_timer");
	// while(gyro.is_calibrating()){
	// 	delay(10);
	// }
	waitUntil(!gyro.is_calibrating());
	printf("DONE CALIBRATION");
	cal_timer.print();

	while(true){
		pros::c::euler_s_t eu = gyro.get_euler();
    printf("IMU euler angles: {pitch: %f, roll: %f, yaw: %f}\n", eu.pitch, eu.roll, eu.yaw);
		// printf("%lf", );
		// pros::c::quaternion_s_t qt = gyro.get_quaternion();
		// printf("IMU quaternion: {x: %f, y: %f, z: %f, w: %f}\n", qt.x, qt.y, qt.z, qt.w);
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
			gyro.reset();
			waitUntil(!gyro.is_calibrating());
			printf("DONE CALIBRATION");
		}
		delay(10);
	}

}
