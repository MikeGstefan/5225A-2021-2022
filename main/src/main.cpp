
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
	drivebase.download_curve_data();
	Data::init();
	_Controller::init();
	GUI::init();
	delay(500);
	// tracking.x_coord = 26.0, tracking.y_coord = 11.75, tracking.global_angle = -90.0_deg;
	// tracking.x_coord = 104.0, tracking.y_coord = 12.0, tracking.global_angle = -30.0_deg;
	// tracking.x_coord = 24.5, tracking.y_coord = 15.0, tracking.global_angle = 9.0_deg;
	// tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0_deg;
	tracking.x_coord = 108.0, tracking.y_coord = 16.0, tracking.global_angle = 0.0_deg;
	update_t.start();
	Autons::file_read();
	Autons::pos_file_read();
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


}


void opcontrol() {
	/*
	namespace for auton
	auton give up
	make gui a task
	create good autosizing
	lvgl images
	*/


  extern Slider mot_speed_set;

	Timer intake_t ("intake", false);
	intk.move(127);


	while(true){
		GUI::update();
		// drivebase.non_blocking_driver_practice();
		
		if(intake_jam.get_new_press()){
			intake_t.reset();
			GUI::flash("Triggered", 200);
		}
		else if(!intake_jam.get_value()) intake_t.reset(false);
		if(intake_t.get_time() > 1000){
			GUI::flash("Jammed", 200);
			// intk.move(-127);
			// waitUntil(!intake_jam.get_value());
			// intk.move(127);
		}

		if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)) Autons::prev_route();
		else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)) Autons::next_route();
		if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_X)) Autons::switch_alliance();
		if (master.get_digital_new_press(cancel_button)) break;
		// if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
		// 	int speed = mot_speed_set.get_value();
		// 	move_start(move_types::line, line_params({0.0, 0.0}, {0.0, 24.0, 0.0}, speed));
		// 	move_start(move_types::turn_angle, turn_angle_params(45.0, speed));
		// 	move_start(move_types::line, line_params({0.0, 0.0}, {0.0, 24.0, 0.0}, speed));
		// }
		delay(10);
	}
}