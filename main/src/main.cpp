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

void hello(){
	 printf("HELLO\n");
	 delay(10);
 }

void opcontrol() {
	//Constructing a page does not go to it
	//Constructing a button will draw it if it is on the right page

	Page page1 (1, COLOR_RED);
	Page tester (2);

	Page::goTo(&page1);

	Button button1 (25, 25, 170, 75, Button::CORNER, &page1, "BUTTON1");
	Button button2 (150, 50, 250, 150, Button::CORNER, &page1, "BUTTON2", COLOR_YELLOW);
	Button test1 (175, 50, 300, 150, Button::CORNER, &tester, "BUTTON1", COLOR_GREEN);
	Button test2 (100, 25, 300, 80, Button::CORNER, &tester, "TEST", COLOR_PINK, COLOR_BLUE);

	button1.setTask(&hello);

	while(1){
		//These should probably be called in the background
		Button::update_press_status();
		Button::get_press();
		delay(10);
	}

}
