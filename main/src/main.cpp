#include "main.h"
#include "logging.hpp"
#include "config.hpp"
#include "util.hpp"
// using namespace std;
// using namespace pros;

// Data test(Levels::both, "/usd/test.txt");

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	// logging_task_start();
	Data::log_init();

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
	uint64_t timer, total_time = 0;
	for(int x = 0; x<100;x++){
		timer = micros();
		test_log.log_print("t:%d ",x);
		total_time += micros() - timer;
		delay(10);
	}
	printf("%llu\n",total_time);
  // queue_handle();
  // print("helk");
	// print("helk");
	// print("helk");
	// print("helk");
	// print("helk");
	// print("helk");
	// print("helk");
	// print("helk");
	// print("hi");
	// print("hello world");




	// // test.logging("fdsjsfdu");
}
