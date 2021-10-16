#include "main.h"
#include "logging.hpp"
#include "config.hpp"
#include "util.hpp"
#include "Tracking.hpp"
#include "lift.hpp"
#include "drive.hpp"
#include "pid.hpp"

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

	delay(150);
	tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;
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
void autonomous() {
	f_bar.move(50);
	c_bar.move(60);
	// drivebase.update_lookup_table_util();
	// printf("here\n");
	// rush_goal2(0.0, -45.0, 0.0);
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
void opcontrol() {
	lift.c_bar_cal();
	lift.f_bar_cal();

	// lift.move_to_target(-6.0, 25.0); // goes to position above the rings

/*
	PID bottom_arm(0.6, 0.002, 0.0, 0.0, true, 0.0, 5.0);
	while(true){
		// if
		bottom_arm.compute(f_bar.get_position(), 400);
		f_bar.move(bottom_arm.get_output());
		// printf("%lf, %lf\n", f_bar.get_position(), bottom_arm.get_output());
		delay(10);
	}
*/
	lift.move_to_target(-6.0, 18.0);
	delay(500);

	lift.pickup_rings();

	// lift.new_move_on_line();

	// while(true){
	// 	printf("y: %lf\n", lift.find_y_pos());
	// 	delay(10);
	// }

/*
	lift.move_to_target(-6.0, 25.0); // goes to position above the rings
  // grabs position for f_bar to reach when at bottom of ring stack
  auto[top_arm_angle, bottom_arm_angle, top_arm_pos_angle, bottom_arm_pos_angle, top_arm_neg_angle, bottom_arm_neg_angle, move_valid] = lift.find_arm_angles(-6.0, 13.0);
  f_bar.move(bottom_arm_angle); // moves f_bar to bottom of ring stack
  while(fabs(f_bar.get_position() - bottom_arm_angle) > 15){  // moves the chain bar to maintain the horizontal distance
      lift.touch_line(-6.0, f_bar.get_position());
			delay(10);
  }
*/
	// f_bar.move_absolute(136.776786, 150);
	// c_bar.move_absolute(24.558264, 80);
	// lift.move_to_target(-6.0, 13.0);
	// lift.move_to_target_util();
	// lift.move_on_line(-6.0, 25.0, 12.0);

	// lift.move_to_target(-6.0, 25);
	// delay(2000);
	// lift.move_to_target(-6.0, 13);

		// f_bar.move_velocity(-30);
		// c_bar.move_velocity(-30);
		// waitUntil(master.get_digital_new_press(E_CONTROLLER_DIGITAL_B));
		//
		// f_bar.move(0);
		// c_bar.move(0);


/*
	auto[top_arm_angle, bottom_arm_angle, top_arm_pos_angle, bottom_arm_pos_angle, top_arm_neg_angle, bottom_arm_neg_angle, move_valid] = lift.find_arm_angles(-6.0, 13.0);
	double target_y = -6.0;
	while(true){
		// z_speed = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
		f_bar.move(master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y));
		lift.touch_line(target_y, f_bar.get_position());
		// printf("GOING TO:%lf", ((rad_to_deg(top_arm_offset_a - acos((cos(deg_to_rad(bottom_arm_angle / bottom_arm_gear_ratio) - bottom_arm_offset_a) * bottom_arm_len - target_y) / top_arm_len))) * top_arm_gear_ratio);
		delay(10);
	}
*/

	// f_bar.move_absolute(54 * 7.0, 100);
	// waitUntil(f_bar.get_position() / 7 > 50);
	// while(f_bar.get_position() / 7 < 54) delay(10);
	// printf("DONE %lf\n", f_bar.get_position() / 7);
	// c_bar.move_absolute(54 * 5.0 / 3.0, 100);
	// find_arm_angles(Vector2D(4.0, 17.0 + 24.0));

// VELOCITY TESTING

	/*

	double low = 1000, high = -1000, mid;
	c_bar.move_absolute(500, 100);
  while(c_bar.get_position() < 450){
		printf("f_bar_vel: %lf, pos:%lf\n", c_bar.get_actual_velocity(), f_bar.get_position());
		if(c_bar.get_actual_velocity() > high) high = c_bar.get_actual_velocity();
		if(c_bar.get_actual_velocity() < low) low = c_bar.get_actual_velocity();
		delay(10);
	}
	c_bar.move(0);
	mid = (high + low) / 2;
	printf("high: %lf, low: %lf, mid: %lf\n", high, low, mid);
	*/

	// find_arm_angles(target_x, target_y);
	// f_bar.move_absolute(7.0 * 42.0, 150);
	// c_bar.move_absolute(65.0 * 5.0 / 3.0, 80);
	// waitUntil(f_bar.get_position() > 7.0 * 40.0 - 20);
	// f_bar.move(0);
	// c_bar.move(0);
	// while(true){
	// 	printf("f_bar_pos: %lf, c_bar_pos:%lf\n", f_bar.get_position(), c_bar.get_position());
	// 	delay(10);
	// }


	//
	// while(true){
	// // 	// f_bar.move_absolute(45 *7.0, 50);
	// // 	// c_bar.move_absolute(45 * 5.0 / 3.0, 50)
	// 	printf("c_bar: %lf | f_bar: %lf\n", c_bar.get_position() * 3.0 / 5.0, f_bar.get_position() / 7.0);
	// 	delay(10);
	// }


	// drivebase.driver_practice();	// this method has self-contained while loop
	// drivebase.update_lookup_table_util();
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
