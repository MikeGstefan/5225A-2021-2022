#include "main.h"
#include "logging.hpp"
#include "config.hpp"
#include "util.hpp"
#include "Tracking.hpp"
#include "lift.hpp"
#include "drive.hpp"
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
	drivebase.download_curve_data();
	// logging_task_start();
	Data::log_init();

	delay(150);
	tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;
	updateStartTask();
	guiSetup();
	printf("here\n");
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
	// drivebase.update_lookup_table_util();
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

int ring_count = 0; //Get rid of this once merged

Page mogo_test(9,"mogo",0x00202020);

Slider mogo_power(50,150,450,200,Style::CORNER, Slider::HORIZONTAL, 0,100,&mogo_test,"POWER");
Button mogo_start(150,50,350,100, Style::CORNER, Button::SINGLE, &mogo_test, "go");
Text motor_label(50,50,Style::CORNER, TEXT_SMALL,&mogo_test, "Motor: 5");
Text sens_label(400,50,Style::CORNER, TEXT_SMALL,&mogo_test, "Sens: G");
void move_mogo(){
	printf("in ur mom \n");
	mogo.move_velocity(mogo_power.val);
	while(!mogo_limit.get_value())delay(10);
	mogo.move(0);
}
void opcontrol() {
	//Try to get rid of pair for elastic time
	//instead of manually pushing back prev and next buttons from perm, have each page just save everything from perms text, buttons, and slider vectors
	//Reset tracking by stopping task

	Page::goTo(9);

	mogo_power.val = 0;

	mogo_start.func = &move_mogo;

	while(true){

		guiBackground();

		drivebase.handle_input();


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
