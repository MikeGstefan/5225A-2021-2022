
#include "drive.hpp"
#include "controller.hpp"
#include "Libraries/gui.hpp"
#include "pid.hpp"
#include "Tracking.hpp"
#include "task.hpp"
#include "auton.hpp"
#include "auton_util.hpp"
#include "Subsystem/f_lift.hpp"
#include "Subsystem/b_lift.hpp"
#include "vision_loop.hpp"

// using namespace std;
#include "task.hpp"
#include "util.hpp"

#include <fstream>
#include <sys/wait.h>
using namespace std;

pros::Task *updt = nullptr;
const GUI* GUI::current_gui = &g_main;


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
//  pros::Task *updt = nullptr;
 bool auton_run = false; // has auton run

void initialize() {
	// gyro.calibrate();
	drivebase.download_curve_data();
	Data::init();
	_Controller::init();
	GUI::init();
	delay(500);
	// tracking.x_coord = 26.0, tracking.y_coord = 11.75, tracking.global_angle = -90.0_deg;
	tracking.x_coord = 108.0, tracking.y_coord = 16.0, tracking.global_angle = 0.0_deg;
	update_t.start();
	auton_file_read();
	pos_auton_file_read();
	// master.print(2, 0, "Driver: %s", drivebase.drivers[drivebase.cur_driver].name);
	// gyro.finish_calibrating(); //Finishes calibrating gyro before program starts
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
	// skills();


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
	/*
	mix/match sizing in screen printing
	namespace for auton
	motor control in main gui similar to pneumatics_for_gui
	auton give up
	make gui a task
	create good autosizing
	*/

	while(true){
		GUI::update();
		// drivebase.non_blocking_driver_practice();

		if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)) prev_auton();
		else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)) next_auton();
		if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_X)) switch_alliance();
		if (master.get_digital_new_press(cancel_button)) break;
		delay(10);
	}
}
