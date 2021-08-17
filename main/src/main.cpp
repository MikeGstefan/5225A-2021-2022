#include "main.h"
#include "logging.hpp"
#include "config.hpp"
#include "util.hpp"
#include "tracking.hpp"
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
void opcontrol() {
  // int speed = 0;
  // while(true){
  //   if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
  //     speed++;
  //     master.print(0,0,"speed: %d", speed);
  //   }
  //   //y = 12
  //   // x = 28
  //   //a = 20
  //   move(speed,0,0);
  // }
	while(!master.get_digital(E_CONTROLLER_DIGITAL_A))delay(10);
	// move_to_target_sync(0.0,0.0,180.0);
  move_on_line_sync(40,0,0.0,true,true,127,false,0.5,5.0,15.0,0);
  // move_to_target_sync(0,40,0);
	// int power_x, power_y, power_a;
	// while(true){
	// 	power_x = master.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);
	// 	power_y = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
	// 	power_a = master.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
	// 	if(fabs(power_x) < 15)power_x = 0;
	// 	if(fabs(power_y) < 15)power_y = 0;
	// 	if(fabs(power_a) < 15)power_a = 0;
	// 	move(power_x, power_y, power_a);
	// 	delay(10);
	// }

}
