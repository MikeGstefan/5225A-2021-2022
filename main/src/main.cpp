#include "main.h"
#include "logging.hpp"
#include "config.hpp"
#include "util.hpp"
#include "tracking.hpp"
// using namespace std;
// using namespace pros;


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	delay(150);
	updateStartTask();

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
void opcontrol() {
	int power_x, power_y, power_a;
	// move_to_target_sync(0.0,0.0,90.0,true, true);
	while(true){
		power_x = master.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
		power_y = master.get_analog(E_CONTROLLER_ANALOG_RIGHT_Y);
		power_a = master.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);
		if(fabs(power_x) < 15)power_x = 0;
		if(fabs(power_y) < 15)power_y = 0;
		if(fabs(power_a) < 15)power_a = 0;
		move(power_x, power_y, power_a);
		delay(10);
	}

}
