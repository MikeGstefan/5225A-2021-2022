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
	int speed = 30;
	int time = 0;
	fbar.move(10);
	while(true){
		printf("%d temp: %f\n",millis(),fbar.get_temperature());
		delay(100);
	}

	while(true){
		fbar.move(-60);
		time = millis();
		while(!btm.get_value())delay(10);

		master.print(0,0,"Time: %d",millis()-time);
		fbar.move_relative(0,200);
		while(!master.get_digital_new_press(E_CONTROLLER_DIGITAL_A))delay(10);
		fbar.move(127);
		time = millis();
		while(!top.get_value())delay(10);
		master.print(0,0,"Time: %d",millis()-time);
		fbar.move_relative(0,200);
		while(!master.get_digital_new_press(E_CONTROLLER_DIGITAL_A))delay(10);
		lcd::print(0,"temp: %f", fbar.get_temperature());
	}

}
