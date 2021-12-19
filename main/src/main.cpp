#include "drive.hpp"
#include "controller.hpp"
#include "gui/gui_main.hpp"

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
	drivebase.download_curve_data();
	Data::init();
	_Controller::init();
	delay(150);
	tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;
	update_t.start();
	gui_setup();
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

int ring_count = 0, cur_auton = 1; //Get rid of this once merged

void opcontrol() {
	/*Gui:
	Reset tracking by task
	/**/

	// while(true){
	// 	printf("%d\n", dist.get());
	// 	delay(10);
	// }

	// claw_in.set_value(1); //Open
	// rush_goal2(0, 100, 0);


	while(true){
		gui_background();
		drivebase.handle_input();

		if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_B)){ //Update expo util
			drivebase.update_lookup_table_util();
			master.clear();
		}
		else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)) next_driver();
		else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)) prev_driver();
		else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_X)) claw_in.set_value(1); //Open
		else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_Y)){
			claw_in.set_value(1); //Open
			drivebase.move(0, 127, 0);
			waitUntil(dist.get() <= 100);
			printf("V:%f\n", front_l.get_actual_velocity());
			claw_in.set_value(0); //Close
			drivebase.brake();
		}
		delay(10);
	}
}
