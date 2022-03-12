
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
	// switch(cur_auto){
		// case auto1:
		// 	tracking.x_coord = 26.0, tracking.y_coord = 11.75, tracking.global_angle = -90.0_deg;
		// break;
		// case auto2:
		// 	tracking.x_coord = 108.0, tracking.y_coord = 16.0, tracking.global_angle = 0.0_deg;
		// break;
		// case auto3:
			tracking.x_coord = 106.0, tracking.y_coord = 16.0, tracking.global_angle = 0.0_deg;
		// 	// tracking.x_coord = 104.0, tracking.y_coord = 12.0, tracking.global_angle = -30.0_deg;
		// break;
		// case auto4:
		// 	// tracking.x_coord = 24.5, tracking.y_coord = 15.0, tracking.global_angle = 9.0_deg;
		// 	tracking.x_coord = 26.0, tracking.y_coord = 11.75, tracking.global_angle = -90.0_deg;
		// break; 
		// default:
		// 	tracking.x_coord = 26.0, tracking.y_coord = 11.75, tracking.global_angle = -90.0_deg;
		// break;
	// }
	
	// tracking.x_coord = 104.0, tracking.y_coord = 12.0, tracking.global_angle = -30.0_deg;
	// tracking.x_coord = 24.5, tracking.y_coord = 15.0, tracking.global_angle = 9.0_deg;
	// tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0_deg;
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
	// move_stop();
	int time = millis();
	move_start(move_types::tank_rush, tank_rush_params({106.0,71.0, 0.0}, false));
	master.print(0,0,"%d",millis()-time);
	while(true){
		// if(master.get_digital_new_press(DIGITAL_A))master.print(0,0,"%.2f, %.2f",LeftEncoder.get_value(), RightEncoder.get_value());
		// drivebase.handle_input();
		delay(10);
	}
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