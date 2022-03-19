
#include "config.hpp"
#include "drive.hpp"
#include "controller.hpp"
#include "Libraries/gui.hpp"
#include "pid.hpp"
#include "Tracking.hpp"
#include "task.hpp"
#include "auton.hpp"
#include "auton_util.hpp"
#include "Subsystems/f_lift.hpp"
#include "Subsystems/b_lift.hpp"
#include "distance.hpp"

// using namespace std;
#include "task.hpp"
#include "util.hpp"

#include <fstream>
#include <sys/wait.h>
using namespace std;

pros::Task *updt = nullptr;
const GUI* GUI::current_gui = &g_main;


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
//  pros::Task *updt = nullptr;
 bool auton_run = false; // has auton run

void initialize() {
	// gyro.calibrate();
	// front_l.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// front_r.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// back_l.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// back_r.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// Autons::file_read();
	autonFile_read();
	f_lift.motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	b_lift.motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	drivebase.download_curve_data();
	Data::init();
	_Controller::init();
	GUI::init();
	delay(500);
	switch(cur_auto){
		case auto1:
			tracking.x_coord = 26.0, tracking.y_coord = 11.75, tracking.global_angle = -90.0_deg;
		break;
		case auto2:
			tracking.x_coord = 108.0, tracking.y_coord = 16.0, tracking.global_angle = 0.0_deg;
		break;
		case auto3:
			tracking.x_coord = 106.0, tracking.y_coord = 16.0, tracking.global_angle = 0.0_deg;
			// tracking.x_coord = 104.0, tracking.y_coord = 12.0, tracking.global_angle = -30.0_deg;
		break;
		case auto4:
			// tracking.x_coord = 24.5, tracking.y_coord = 15.0, tracking.global_angle = 9.0_deg;
			tracking.x_coord = 26.0, tracking.y_coord = 11.75, tracking.global_angle = -90.0_deg;
		break; 
		default:
			tracking.x_coord = 26.0, tracking.y_coord = 11.75, tracking.global_angle = -90.0_deg;
		break;
	}
	
	// tracking.x_coord = 104.0, tracking.y_coord = 12.0, tracking.global_angle = -30.0_deg;
	// tracking.x_coord = 24.5, tracking.y_coord = 15.0, tracking.global_angle = 9.0_deg;
	tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0_deg;
	update_t.start();
	// master.print(2, 0, "Driver: %s", drivebase.drivers[drivebase.cur_driver].name);
	// gyro.finish_calibrating(); //Finishes calibrating gyro before program starts
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
	// start of prog state machine testing
	/*
	b_lift.reset();
	f_lift.reset();
	Task handlers([](){
		while(true){
      f_lift.handle_buttons();
      f_lift.handle(false);
      f_claw.handle();
      b_claw.handle();
      intake.handle();
			delay(10);
		}
	});
	move_start(move_types::tank_point, tank_point_params({10.0, 20.0, 0.0}, true));
	b_claw.set_state(b_claw_states::search_lip);
	move_start(move_types::tank_point, tank_point_params({00.0, 00.0, 0.0}, false));
	f_claw.set_state(f_claw_states::searching);
	waitUntil(f_claw.get_state() == f_claw_states::grabbed);
	// f_lift.set_state(f_lift_states::move_to_target, );
	delay(3000);
	for(int i = 0; i < 7; i++){
		f_lift.set_state(f_lift_states::move_to_target, i);
		delay(500);
	}

	while(true){
		
		delay(10);
	}
	*/
// END OF PROG STATE MACHINE TESTING

	// skills();
	switch(cur_auto){
		case auto1:
			// tracking.x_coord = 26.0, tracking.y_coord = 11.75, tracking.global_angle = -90.0_deg;
			skills();
			skills2();
			new_skills3();
		break;
		case auto2:
			f_claw_p.set_value(0);
			b_claw_p.set_value(0);
			blue_highside();
		break;
		case auto3:
			f_claw_p.set_value(0);
			b_claw_p.set_value(0);
			blue_highside_tall();
			
		break;
		case auto4:
			f_claw_p.set_value(0);
			b_claw_p.set_value(0);
			blue_lowside();
		break; 
		default:
			skills();
			skills2();
			new_skills3();
		break;
	}

}


// extern Slider mot_speed_set;

void opcontrol() {
	// pros::Task task{[=] {
	// 	while(true){
	// 		b_claw.handle();
	// 		delay(10);
	// 	}
	// }};
	// delay(2000);
	// b_claw.set_state(b_claw_states::grabbed);
	// delay(3000);
	// b_claw.set_state(b_claw_states::search_lip);
	// while(true) delay(10);

	master.print(2, 0, "hello");
	drivebase.driver_practice();
	move_stop();
	// while(true){
	// 	printf("%d\n", b_dist.get());
	// 	delay(10);
	// }
	// f_lift_m.move(40);
	pros::Task intk_task(intk_c);
  drivebase.driver_practice();
  
	master.clear();
	// b_lift.reset();
	// Task([](){ 
	// 	f_lift.reset();
	// });
	// f_claw_p.set_value(0);
	// b_claw_p.set_value(0);
	// skills();
	// skills2();

	//Intake Jam code
	Task([](){
		Timer intake_t ("intake jam", false);
		intk.move(127);
		while(true){
			if(intake_jam.get_new_press()) intake_t.reset(); //Start timer when pressed
			else if(!intake_jam.get_value()) intake_t.reset(false); //End timer when unpressed
			if(intake_t.get_time() > 1000){ //If pressed for more than 1 sec, reverse intk
				intk.move(-127);
				waitUntil(!intake_jam.get_value()); //Waits for unjam plus some time
				delay(150);
				intk.move(127);
			}
			delay(10);
		}
	});


	Autons::selector();

	while(true){
		GUI::update();
		// drivebase.non_blocking_driver_practice();
		
		// if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
		// 	int speed = mot_speed_set.get_value();
		// 	move_start(move_types::line, line_params({0.0, 0.0}, {0.0, 24.0, 0.0}, speed));
		// 	move_start(move_types::turn_angle, turn_angle_params(45.0, speed));
		// 	move_start(move_types::line, line_params({0.0, 0.0}, {0.0, 24.0, 0.0}, speed));
		// }
		delay(10);
	}
}