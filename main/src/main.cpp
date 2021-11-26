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
	Timer move = {"move"};
	printf("START\n");
	move.print();
	move_on_arc({0.0, 0.0}, {-24.0, 24.0, 0.0}, 24.0, true, 127, true, 1.0, 0.0, true);
	// move_on_arc({0.0, 0.0}, {10.0, 10.0, 0.0}, 10.0, false, 127, false, 0.3, 0.0, true);
	move.print();
	printf("DONE\n");

	// move_on_arc({0.0, 0.0}, {10.0, 10.0, 0.0}, 10.0, true, 127, true, 0.0, 0.0, true);
	// move_on_arc(const Vector2D start, Coord target, const double radius, const bool positive, const double max_power, const bool angle_relative_to_arc, const double min_angle_percent, const double min_x_line_percent, const bool brake = true);
	// double error, target = 40.0, kp = 30.0, min_power_y = 40;
	// Timer pid_timer{"pid_timer"};
	// pid_timer.print();
	// while(true){
	//
	// 	error = target - tracking.x_coord;
	// 	tracking.power_y = kp * error;
	//
	// 	if (fabs(tracking.power_y) < min_power_a) tracking.power_y = sgn(error) * min_power_y;
	//
	// 	drivebase.move(tracking.power_y, 0, 0);
	//
	// 	printf("cur_x: %lf, error: %lf, power_a: %lf\n", tracking.x_coord, error, tracking.power_y);
	//
	// 	if(error < 0.5)	break;
	// 		// drivebase.move(0, 17, 0);
	// 	delay(10);
	// }
	// // drivebase.move(0, 0, 0);
	// drivebase.brake();
	// printf("DONE.");
	// pid_timer.print();
	// while(true){
	// 	printf("cur_x: %lf, error: %lf, power_a: %lf\n", tracking.x_coord, error, tracking.power_y);
	// 	delay(10);
	// }

	// move_on_arc({0.0, 0.0}, {-10.0, 10.0, 0.0}, 10.0, true, 127.0, true, 0.9, 0.0, true);


	/*
	lift.c_bar_cal();
	lift.f_bar_cal();

	lift.move_to_target({-6.0, 20.0});	// moves lift out of the way

	master.clear();
	delay(50);	// waits for screen to clear
	master.print(0, 0, "Dropoff: %s", lift.is_dropoff_front() ? "front" : "back");
	delay(50);
	// lift.move_to_target(lift.get_dropoff_coords()[1][2]);


	// lower_f_bar(nullptr);
	// lift.raise_f_bar_to_platform();
	//
		lift.move_to_neutral();
		while(true){
			lift.handle();
			delay(10);
		}
	// */


	// f_bar_move_to_height testing
	/*
	double target_z = 15.0;
	master.clear();
	delay(10);
	master.print(0, 0, "z: %lf", target_z);
	lift.move_f_bar_to_height(target_z);

	while(true){
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){
			target_z++;
			lift.move_f_bar_to_height(target_z);
			master.print(0, 0, "z: %lf", target_z);
		}
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)){
			target_z--;
			lift.move_f_bar_to_height(target_z);
			master.print(0, 0, "z: %lf", target_z);
		}
		delay(10);
	}
	*/



}
