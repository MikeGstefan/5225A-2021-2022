#include "main.h"
#include "logging.hpp"
#include "config.hpp"
#include "util.hpp"
#include "tracking.hpp"
#include "gui.hpp"
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
	guiSetup();

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
	Page liftElastic (1, "Lift - Elastic Test"); //Testing the elastics on the lift
	Page liftMove (2, "Lift - Moving"); //Moving the lift to an xyz position
	Page tracking (3, "Tracking"); //Display tracking vals and reset btns
	Page autoSel (4, "Auton"); //Select auton routes
	Page driverCurve (5, "Drivers"); //Select a driver and their exp curve
	Page intkTest (6, "Intake"); //Test for intake with rings
	Page temps (7, "Temperature"); //Motor temps


	Button tester (50, 75, 100, 50, Button::SIZE, true, &liftMove, "TESTING", COLOR_RED, COLOR_CYAN);
	Text (225, 30, &autoSel, "HELLO", COLOR_BLUE);

	while(1){
		//These should probably be called in the background
		Page::updateScreenStatus();
		Button::getPress();
		delay(10);
	}

}
