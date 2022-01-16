#include "Subsystems/lift.hpp"
#include "Subsystems/hitch.hpp"
#include "drive.hpp"
#include "controller.hpp"
#include "gui/gui_main.hpp"
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
	// tracking.x_coord = 144.0 - 10.25, tracking.y_coord = 14.75, tracking.global_angle = -M_PI_2;
	tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;

	update_t.start();
	GUI::setup();
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
  flatten_against_wall(false);

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
 int cur_auton = 1;

//Get rid of these once merged
void prev_driver(){
	if (drivebase.cur_driver == 0) drivebase.cur_driver = drivebase.num_of_drivers - 1;
	else drivebase.cur_driver--;
	WAIT_FOR_SCREEN_REFRESH();
	master.print(2, 0, "Driver: %s          ", drivebase.drivers[drivebase.cur_driver].name);
}
void next_driver(){
	drivebase.cur_driver++;
	drivebase.cur_driver %= drivebase.num_of_drivers;
	WAIT_FOR_SCREEN_REFRESH();
	master.print(2, 0, "Driver: %s          ", drivebase.drivers[drivebase.cur_driver].name);
}

// int ring_count = 0, cur_auton = 1;

void opcontrol() {
  // flatten_against_wall(false);
  /*
  Timer rush_timer{"rush_timer"};
  drivebase.move(0.0, 127.0, 0.0);
  waitUntil(tracking.y_coord > 35.0);
  master.print(0, 0, "time: %d", rush_timer.get_time());
  rush_timer.print();
  drivebase.move(0.0, -10.0, 0.0);
  delay(500);
  */

  // tilter.reset();
  // tilter.move_absolute(tilter.bottom_position);
  // while(true){
  //   printf("dist: %d\n", tilter_dist.get());
  //   delay(10);
  // }

	// drivebase.driver_practice();
  while(true){
    drivebase.handle_input();
    delay(10);
  }
}
