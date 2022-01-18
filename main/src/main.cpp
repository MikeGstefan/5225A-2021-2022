#include "Subsystems/lift.hpp"
#include "Subsystems/hitch.hpp"
#include "drive.hpp"
#include "controller.hpp"
#include "Libraries/gui/gui.hpp"
#include "pid.hpp"
#include "Tracking.hpp"
#include "task.hpp"
#include "auton.hpp"
#include "task.hpp"

using namespace std;

pros::Task *updt = nullptr;
const GUI* GUI::current_gui = &g_main;


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	drivebase.download_curve_data();
	Data::init();
	_Controller::init();
	GUI::setup();
	delay(150);
	tracking.x_coord = 144.0 - 10.25, tracking.y_coord = 14.75, tracking.global_angle = -M_PI_2;
	// tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;

	update_t.start();
  auton_file_read(); //Hopefully works now
	master.print(2, 0, "Driver: %s", drivebase.drivers[drivebase.cur_driver].name);
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
	skills();

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

//Get rid of these once merged
int ring_count = 0;

void opcontrol() {
	/*Nathan:
	-auton file isn't being created
  -port_nums in util
	-controller btns for auton
	-enums for CHAR_HEIGHT_SMALL
	-allow button add_text to offset by 1 line
	-force convert it to centre, and move it to be on the button
	-change its color to btn lcol
	-add text to go button
	-Make the different gui's (main and util) objects of GUI and get rid of the header include system

	Make it possible to include testing page. Possibly using a constexpr bool

  alliance text persists on blue
  Try "wireless" upload via controller, but plug to V5 using tether cable

	Fill in lift movements and states
	Reset tracking by task
	convert some printfs to logs
	/**/

	while(true){
		GUI::update();
		drivebase.non_blocking_driver_practice();

		if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)) prev_auton();
		else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)) next_auton();
		if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_X)) switch_alliance();
		if (master.get_digital_new_press(cancel_button)) break;
		delay(10);
	}
}
