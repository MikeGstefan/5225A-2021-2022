#include "Subsystems/lift.hpp"
#include "Subsystems/hitch.hpp"
#include "drive.hpp"
#include "gui.hpp"
#include "controller.hpp"
#include "pid.hpp"
#include "Tracking.hpp"
#include "task.hpp"
#include "auton.hpp"

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
 pros::Task *updt = nullptr;

void initialize() {
	drivebase.download_curve_data();
	Data::init();
	_Controller::init();
	delay(150);
	tracking.x_coord = 144.0 - 10.25, tracking.y_coord = 14.75, tracking.global_angle = -M_PI_2;
	// tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;

	update_t.start();
	// gui_setup();
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
 int ring_count = 0;
 double cur_auton = 1;

void opcontrol() {
	// driver setup

	lift_piston.set_value(LOW);	// in searching state
	lift.reset();
	tilter.reset();
	lift.motor.move_absolute(35, 100);
	tilter.move_absolute(500, 100);
	tilter_bottom_piston.set_value(LOW);
	tilter_top_piston.set_value(LOW);


	drivebase.driver_practice();
}
