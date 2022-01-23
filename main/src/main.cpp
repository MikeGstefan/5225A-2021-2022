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
GUI* const GUI::current_gui = &g_util;


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
  auton_file_read();
	master.print(2, 0, "Driver: %s", drivebase.drivers[drivebase.cur_driver].name);
	GUI::go_to(3);
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
  -util motor rpm str issues
	test auton file creation with sd
	make some background stuff into page loop funcs
	talk to nikhil about controller buttons. autons and driver practice interfere
	look at //make for other things to do
	make everythhing c::
	convert string title/label to const char* or at least const std::string

	make _Text index nullptr instead of int and actually check if(index_ptr)
	have _Text explicitly specify type
	array text overload
	make text, flash, and go variadic

	Fill in lift movements and states
	Reset tracking by task
	convert some printfs to logs
	/**/

	extern std::array <std::tuple<int, Button*, Button*, Text*, int, char*>, 8> motor_ports;

	while(true){
		GUI::update();
		printf("MAIN- %s\n\n", std::get<5>(motor_ports[0]));
		// drivebase.non_blocking_driver_practice();

		if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)) prev_auton();
		else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)) next_auton();
		if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_X)) switch_alliance();
		if (master.get_digital_new_press(cancel_button)) break;
		delay(10);
	}
}
