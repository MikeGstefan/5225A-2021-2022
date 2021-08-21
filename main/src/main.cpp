#include "main.h"
#include "logging.hpp"
#include "config.hpp"
#include "util.hpp"
#include "tracking.hpp"
#include "lift.hpp"
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
	pros::lcd::initialize();
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
	int power_x, power_y, power_a, power_l;
	int claw_state = 0, uptk_state = 0, last_state = 0;
	// fbar.move_relative(0,200);
	master.clear();
	delay(50);
	int backup_point = 0;
	while(!master.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT)){
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){
			if(backup_point == 25)backup_point = 0;
			else backup_point++;
		}
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)){
			if(backup_point == 0)backup_point = 25;
			else backup_point--;
		}
		delay(10);
		master.print(0,0,"backup: %2d",backup_point);
	}

	lift_reset();
	while(!master.get_digital_new_press(E_CONTROLLER_DIGITAL_A))delay(10);
	fbar.move_relative(0,200);
	int grab_time = millis(), backup_time = millis(), lift_done = millis();

	move_to_target_sync(0.0,-45.0,0.0);
	move(0,-50,0);

	while(!claw_touch.get_value())delay(10);
	claw_out.set_value(0);
	claw_in.set_value(1);
	grab_time = millis()-grab_time;
	claw_state = 0;
	last_state = 0;
	// fbar.move(50);
	move_to_target_async(0.0,-20.0,0.0);
	tracking.wait_for_dist(backup_point);
	fbar.move_absolute(1000,200);
	tracking.wait_for_complete();
	backup_time = millis() -backup_time;

	// fbar.move(0);

	master.print(0,0,"UP:toggle, R:vent");
	while(true){
		power_x = master.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);
		power_y = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
		power_a = master.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
		power_l = master.get_analog(E_CONTROLLER_ANALOG_RIGHT_Y);
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){
			if(claw_state !=1){
				claw_out.set_value(1);
				claw_in.set_value(0);
				claw_state = 1;
			}
			else{
				claw_out.set_value(0);
				claw_in.set_value(1);
				claw_state = 0;
			}
		}
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT)){
			claw_out.set_value(0);
			claw_in.set_value(0);
			claw_state = 0;
		}
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
			if(!uptk_state){
				fbar.move_relative(0,200);
				uptk_state = !uptk_state;
			}
			else {
				fbar.move(0);
				uptk_state = !uptk_state;
			}
		}
		if(claw_touch.get_value() && last_state == 1){
			claw_out.set_value(0);
			claw_in.set_value(1);
			claw_state = 0;
			last_state = 0;
		}
		if(!claw_touch.get_value()&& last_state ==0)last_state = 1;
		if(fabs(power_x) < 15)power_x = 0;
		if(fabs(power_y) < 15)power_y = 0;
		if(fabs(power_a) < 15)power_a = 0;
		if(fabs(power_l) < 15)power_l = 0;
		if(uptk_state)fbar.move(power_l);
		move(power_x, power_y, power_a);
		delay(10);
	}

}
