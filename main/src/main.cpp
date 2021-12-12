#include "main.h"
#include "logging.hpp"
#include "config.hpp"
#include "util.hpp"
#include "Tracking.hpp"
#include "lift.hpp"
#include "drive.hpp"
#include "gui.hpp"
#include "controller.hpp"
#include "pid.hpp"
#include "auton_util.hpp"

// using namespace std;
#include "task.hpp"
using namespace std;
// using namespace pros;


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */


void initialize() {
	gyro.calibrate();
	drivebase.download_curve_data();
	Data::log_init();
	_Controller::init();
	delay(150);
	tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;
	updateStartTask();
	guiSetup();
	WAIT_FOR_SCREEN_REFRESH();
	master.print(2, 0, "Driver: %s", drivebase.drivers[drivebase.cur_driver].name);
	gyro.finish_calibrating(); //Finishes calibrating gyro before program starts
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
void autonomous() {}

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

int ring_count = 0; //Get rid of this once merged
double cur_auton = 1;

void opcontrol() {
	gyro.climb_ramp();
	gyro.level(1.4, 0); //(2, 250) for 6 motor

	delay(2000); //To make sure robot is longer moving from momentum. May be taken out if non-drive commands are called.
	while(true){
		guiBackground();
		drivebase.handle_input();
		printf("%f\n", fabs(gyro.get_angle()));
		if (fabs(gyro.get_angle()) > 10) gyro.level(2.2, 250); //Strayed off balance

		if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_B)){ //Update expo util
			drivebase.update_lookup_table_util();
			delay(50);
			master.clear();
			drivebase.screen_timer.reset();
		}
		else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)) nextDriver();
		else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)) prevDriver();
		delay(10);
	}
}
