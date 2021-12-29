#include "Subsystems/lift.hpp"
#include "Subsystems/hitch.hpp"
#include "drive.hpp"
#include "gui.hpp"
#include "controller.hpp"
#include "pid.hpp"

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
	lift.reset();
	lift_piston.set_value(LOW);
	lift.motor.move_absolute(lift.bottom_position, 100);

	Timer move_timer{"move"};
	move_to_point({112.5, 14.75, -90.0});
	move_timer.print();
	// tank_move_on_arc({110.0, 14.75}, {132.0, 24.0, -180.0}, 127.0);
	// move_on_line(polar_to_vector(110.0, 14.75, 10.0, 45.0, -135.0));
	move_to_point(polar_to_vector_point(110.0, 14.75, 17.5, 45.0, -135.0), 127.0, false, 1.0, false);	// grabs alliance goal
	lift_piston.set_value(HIGH);

	tank_turn_to_target({108.0, 60.0});
	// tank_move_to_target({110.0, 40.0, 0.0}, true, 127.0, 0.0, false);
	move_to_point({110.0, 60.0, 0.0}, 127.0, false, 0.0, false);	// in front of small netural goal
	move_to_point({110.0, 80.0, 0.0}, 127.0, false, 0.0, false);	// drives through small neutral goal
	tank_move_on_arc({110.0, 84.0},{80.0, 96.0, -90.0}, 127.0);
	move_to_point({60.0, 96.0, -90.0}, 80.0, false, 0.0, true);	// drives over rings
	move_to_point({60.0, 108.0, -90.0}, 80.0, false, 0.0, true);	// drives to drop off small neutral
	lift.motor.move_absolute(lift.platform_position, 100);
	move_to_point({60.0, 108.0, -180.0}, 80.0, false, 0.0, true);	// drives to drop off small neutral
	lift_piston.set_value(LOW);



	move_timer.print();
	/*
	move_to_point({108.0, 60.0, 0.0}, 127.0, false, 0.0, false);
	move_timer.print();
	delay(1000);
	move_to_point({108.0, 84.0, 0.0}, 127.0, false, 0.0, false);
	move_timer.print();
	*/

	// move_on_arc({108.0, 0.0}, {132.0, 24.0, -180.0}, 24.0, true, 127.0, true, 1.0);
	// move_timer.print();

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
	lift_piston.set_value(LOW);	// in searching state
	lift.reset();
	lift.motor.move_absolute(35, 100);
	drivebase.driver_practice();


}
