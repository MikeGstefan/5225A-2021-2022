#include "Subsystems/lift.hpp"
#include "Subsystems/hitch.hpp"
#include "Subsystems/angler.hpp"
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
	// tracking.x_coord = 144.0 - 10.25, tracking.y_coord = 14.75, tracking.global_angle = -M_PI_2;
	tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;

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
	// lift.reset();
	// lift_piston.set_value(LOW);
	// lift.angler_motor.move_absolute(lift.bottom_position, 100);

	// Timer move_timer{"move"};
	// move_to_point({112.5, 14.75, -90.0});
	// move_timer.print();
	// // tank_move_on_arc({110.0, 14.75}, {132.0, 24.0, -180.0}, 127.0);
	// // move_on_line(polar_to_vector(110.0, 14.75, 10.0, 45.0, -135.0));
	// move_to_point(polar_to_vector_point(110.0, 14.75, 17.5, 45.0, -135.0), 127.0, false, 1.0, false);	// grabs alliance goal
	// lift_piston.set_value(HIGH);

	// tank_turn_to_target({108.0, 60.0});
	// // tank_move_to_target({110.0, 40.0, 0.0}, true, 127.0, 0.0, false);
	// move_to_point({110.0, 60.0, 0.0}, 127.0, false, 0.0, false);	// in front of small netural goal
	// move_to_point({110.0, 80.0, 0.0}, 127.0, false, 0.0, false);	// drives through small neutral goal
	// tank_move_on_arc({110.0, 84.0},{80.0, 96.0, -90.0}, 127.0);
	// move_to_point({60.0, 96.0, -90.0}, 80.0, false, 0.0, true);	// drives over rings
	// move_to_point({60.0, 108.0, -90.0}, 80.0, false, 0.0, true);	// drives to drop off small neutral
	// lift.angler_motor.move_absolute(lift.platform_position, 100);
	// move_to_point({60.0, 108.0, -180.0}, 80.0, false, 0.0, true);	// drives to drop off small neutral
	// lift_piston.set_value(LOW);



	// move_timer.print();
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
	angler.reset();
	top_claw.set_value(0);
	btm_claw.set_value(0);
	Timer angler_out{"timer"};
	pros::Task([&](){
		while(angler.motor.get_position() < 480)delay(10);
		master.print(0,0,"%d",angler_out.get_time());
	});
	angler.motor.move_absolute(500, 100);


	while(!master.get_digital(E_CONTROLLER_DIGITAL_A)){
		// if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
		// 	drivebase.move(0,127,0);
		// 	if(angler_dist.get() <= 70){
		// 		top_claw.set_value(1);
		// 		btm_claw.set_value(1);
		// 	}	
		// }
		// // if(angler_dist.get() <= 190){
		// // 	top_claw.set_value(1);
		// // 	btm_claw.set_value(1);
		// // }
		// printf("%d\n",angler_dist.get());
		delay(10);
	}
	Timer grab_time{"time"};
	rush_goal2(0.0,50.0,0.0);
	master.print(0,0,"%d",grab_time.get_time());
	while(!master.get_digital(E_CONTROLLER_DIGITAL_A)){
		// if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
		// 	drivebase.move(0,127,0);
		// 	if(angler_dist.get() <= 70){
		// 		top_claw.set_value(1);
		// 		btm_claw.set_value(1);
		// 	}	
		// }
		// // if(angler_dist.get() <= 190){
		// // 	top_claw.set_value(1);
		// // 	btm_claw.set_value(1);
		// // }
		// printf("%d\n",angler_dist.get());
		delay(10);
	}
	top_claw.set_value(0);
	btm_claw.set_value(0);

	// claw_in.set_value(1); //Open
	// drivebase.move(0, -127, 0);
	// 		waitUntil(dist.get() <= 190);
	// 		claw_in.set_value(0); //Close
	// 		drivebase.brake();
	// angler_motor.move(-60);


	// while(true)delay(10);
    // Timer vel_rise_timeout("vel_rise");
    // // waits for angler_motor's velocity to rise or timeout to trigger
    // while(fabs(angler_motor.get_actual_velocity()) < 45.0){
    // //   printf("%s's velocity is (rising loop): %lf\n", this->name, angler_motor.get_actual_velocity());
    //   if (vel_rise_timeout.get_time() > 50){
    //     // printf("%s's rising loop timed out\n", this->name);
    //     break;
    //   }
    //   delay(10);
    // }
    // // printf("%s's velocity done rising\n", this->name);
    // // waits until angler_motors velocity slows down for 5 cycles
    // cycleCheck(fabs(angler_motor.get_actual_velocity()) < 5.0, 5, 10)
    // angler_motor.tare_position();  // resets subsystems position
    // // printf("%d, %s's reset %lf\n", millis(), this->name, angler_motor.get_position());
    // angler_motor.move(0);
	// angler.angler_motor.move_absolute(100, 100);
	// move_start(move_types::line, line_params({0.0, 0.0}, {0.0, 24.0, 00.0}, 127.0, false, 0.0, false), true);

	// move_on_arc({0.0, 0.0}, {-24.0, 0.0, -180.0}, 12.0, false, 127, true, 1.0, true, 20.0, 127.0);
	// tank_move_on_arc({0.0, 0.0}, {-24.0, 24.0, -90.0}, 127.0, 127.0, true);
	// move_on_arc({0.0, 0.0}, {10.0, 10.0, 0.0}, 10.0, false, 127, false, 0.3, 0.0, true);
	// move.print();
	printf("DONE\n");
	// while(true){
	// 	gui_background();
	// 	drivebase.handle_input();

	// 	if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_B)){ //Update expo util
	// 		drivebase.update_lookup_table_util();
	// 		master.clear();
	// 	}
	// 	else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)) next_driver();
	// 	else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)) prev_driver();
	// 	delay(10);
	// }
}
