#include "drive.hpp"
#include "gui.hpp"
#include "controller.hpp"
#include "pid.hpp"
#include "Tracking.hpp"
#include "task.hpp"

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
	tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;
	update_t.start();
	// updt = new Task(update);
	printf("here\n");
	// gui_setup();
	// master.print(2, 0, "Driver: %s", drivebase.drivers[drivebase.cur_driver].name);
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
	f_bar.move_relative(0, 100);
	Timer move = {"move"};
	tank_move_on_arc({0.0, 0.0}, {24.0, 24.0, 90.0}, 127.0, 127.0, true);

	// move_on_arc({0.0, 0.0}, {24.0, 24.0, 0.0}, 24.0, false, 127, true, 0.3, true);
	// move_on_line({0.0, 0.0}, {12.0, 24.0, 0.0}, 127.0, false, 0.0, false);
	// move_to_point({0.0, 0.0}, {24.0, 12.0, -80.0}, 127.0, false, 0.0, true);
	move.print();

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
 int ring_count = 0;
 double cur_auton = 1;
int x = 0;
void opcontrol() {
	while(true){
		// x = master.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);
		// if(abs(x) < 20)x = 0;
		// // if(x > 50)x = 50;
		// drivebase.move(x, -25,0);
		delay(10);
		drivebase.handle_input();
	}
// 	printf("in op\n");
// 	// f_bar.move_relative(0, 100);
// 	// Timer move = {"move"};
// 	// printf("START\n");
// 	// move.print();
// 	// drivebase.driver_practice();
// 	// rush_goal2(0.0, -20.0, 0.0);
// 	// move_on_line({0.0, 0.0}, {0.0, 24.0, 00.0}, 127.0, false, 0.0, false);
// 	lift.move(-60);
 // Timer vel_rise_timeout("f_bar_vel_rise");
// // waits for f_bar velocity to rise or timeout to trigger
// while(fabs(lift.get_actual_velocity()) < 45.0){
// 	printf("vel (rising loop): %lf\n", lift.get_actual_velocity());
// 	if (vel_rise_timeout.get_time() > 50){
// 		printf("f_bar rising timeout\n");
// 		break;
// 	}
// 	delay(10);
// }
// printf("done rising\n");
// // waits until f_bar velocity slows down for 5 cycles
// cycleCheck(fabs(lift.get_actual_velocity()) < 5.0, 5, 10)
// lift.tare_position();
// printf("%d, f_bar reset %lf\n", millis(), lift.get_position());
// lift.move(-5);

// //
// //
// 	bool state  = 0;
// 	bool lift_state = 1;
// 	int count = 0;
// // 	printf("going in\n");
// 	while(true){

// 		// delay(10);
// 		// state = !state;
// 		// claw.set_value(state);
// 		// count++;
// 		// delay(1000);
// 		// if(master.get_digital(E_CONTROLLER_DIGITAL_A)){
// 		// 	printf("%d\n",count);
// 		// 	break;
// 		// }
// 		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_R2)){
// 			printf("switching state\n");
// 			state = !state;
// 			claw.set_value(state);
// 		}
// 		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_R1)){
// 			if(lift_state){ 
// 				lift.move_absolute(600, 100);
// 			}
// 			else lift.move_absolute(20, 100);
// 			lift_state = !lift_state;
			
// 			// delay(2000);
// 			// lift.move(5);
// 		}
// 		if(lift_state && !state && claw_touch.get_value()){ 
// 			state = !state;
// 			claw.set_value(state);
// 		}
// 		drivebase.handle_input();
// 		delay(10);

// 	}

}
