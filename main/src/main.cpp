#include "main.h"
#include "logging.hpp"
#include "config.hpp"
#include "util.hpp"
#include "Tracking.hpp"
#include "lift.hpp"
#include "drive.hpp"

#include "gui.hpp"
// using namespace std;
// using namespace pros;


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	// drivebase.download_curve_data();
	//
		printf("start");

	// tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;
	// updateStartTask();
	//
	// pros::lcd::initialize();
	// drivebase.download_curve_data();
	//
	// delay(150);
	// updateStartTask();
	//pros::lcd::initialize();
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
	// drivebase.update_lookup_table_util();
	printf("here\n");
	//rush_goal2(0.0, -45.0, 0.0);
	// drivebase.move(0,-127, 0);
	// while(!claw_touch.get_value()) delay(10);
	// master.print(0, 0, "inches: %lf", tracking.y_coord);
/*
	Timer move_timer{"move_timer"};
	// move_to_target_async(0.0, -45.0, 0.0);	// got to goal
	// while(tracking.y_coord > -45.0)	delay(10);
	master.print(0, 0, "time: %d", move_timer.get_time());
	rush_goal(0.0, -20.0, 0.0);
*/
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

void hello(){
	 printf("HELLO\n");
	 if(intake.get_current_draw() == 0){
		 intake.move(127);
	 }else{
		 intake.move(0);
	 }
	 delay(10);
 }

void opcontrol() {
	bool toggle = 0;
	Page page1 (1,COLOR_GREY);
	Page::goTo(&page1);
	Button Text(0,10,0,100, Button::CORNER,&page1, "Intake in Port 6", COLOR_GREY, COLOR_WHITE);
	Button Intake_Button(150,50,300,150, Button::CORNER,&page1, "Intake Switch", COLOR_WHITE);
	Intake_Button.setTask(&hello);
	while (1) {
		Button::update_press_status();
		Button::get_press();
		delay(10);
		/* code */
	}
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
	// update_lookup_table_util();
	/*
	int power_x, power_y, power_a, power_l;
	int claw_state = 0, uptk_state = 0, last_state = 0;
	lift.cal();
	lift.move_absolute(120, 100);
	int timer = millis();
	claw_out.set_value(0);
	claw_in.set_value(0);
	rush_goal2(0,-40,0);
	brake();
	printf("timer: %d\n\n",millis()-timer);
	move_to_target_async(0.0,-20.0,0.0);
	lift.move_absolute(400,200);
	tracking.wait_for_complete();
	printf("timer: %d\n\n",millis()-timer);
	return;
	// lift.move_relative(0,200);
//	master.clear();
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
		master.print(0, 0, "backup: %2d", backup_point);
	}

	lift.cal();
	while(!master.get_digital_new_press(E_CONTROLLER_DIGITAL_A))delay(10);
	lift.move_relative(0, 200);
	int grab_time = millis(), backup_time = millis(), lift_done = millis();

	move_to_target_sync(0.0, -45.0, 0.0);	// got to goal
	move(0, -50, 0);

	while(!claw_touch.get_value())	delay(10);
	claw_out.set_value(0);
	claw_in.set_value(1);	// grabbed goal
	grab_time = millis() - grab_time;
	claw_state = 0;
	last_state = 0;
	// lift.move(50);
	move_to_target_async(0.0, -20.0, 0.0);
	tracking.wait_for_dist(backup_point);
	lift.move_absolute(1000, 200);
	tracking.wait_for_complete();
	backup_time = millis() - backup_time;
	master.print(1, 0, "grab: %d", grab_time);
	master.print(2, 0, "back: %d", backup_time);
	// lift.move(0);

	// master.print(0,0,"UP:toggle, R:vent");

	while(true){
		power_l = master.get_analog(JOY_LIFT);
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){	// unlocks claw
			if(claw_state != 1){
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
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT)){	// vents claw pneumatic
			claw_out.set_value(0);
			claw_in.set_value(0);
			claw_state = 0;
		}
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
			if(uptk_state)	lift.move(0);	// releases lift
			else lift.move_relative(0, 200);	// holds lift
			uptk_state = !uptk_state;
		}
		if(claw_touch.get_value() && last_state == 1){	// locks claw
			claw_out.set_value(0);
			claw_in.set_value(1);
			claw_state = 0;
			last_state = 0;
		}
		if(!claw_touch.get_value() && last_state == 0)	last_state = 1;

		drive_input();
		if(fabs(power_l) < 7)	power_l = 0;
		// prevents lift from going out of bounds
		if(lift.pos_to_height(lift.get_position()) > 40.0 && power_l > 0) power_l = 0;
		if(lift.pos_to_height(lift.get_position()) < 10.0 && power_l < 0) power_l = 0;
		printf("height: %lf\n", lift.pos_to_height(lift.get_position()));
		if(uptk_state)	lift.move(power_l);
		delay(10);
	}
*/
}
