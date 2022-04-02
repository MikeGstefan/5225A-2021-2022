
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
	// skills();
	

}


// extern Slider mot_speed_set;

void opcontrol() {
	while(!master.get_digital_new_press(DIGITAL_A))delay(10);
	drivebase.move(0.0, 127.0);
	Timer timer{"timer"};
	// while(!master.get_digital_new_press(DIGITAL_A) && timer.get_time() < 500){
	// 	// printf("vel: %lf\n", (tracking.l_velo + tracking.r_velo) / 2);
	// 	printf("vel:%lf\n", rad_to_deg(tracking.g_velocity.angle));
	// 	delay(10);
	// }
	//power | velocity
	// 60 | 200
	// 80 | 274
	// 100 | 370
	// 127 | 418
	// ratio is 3.29

	// move_start(move_types::tank_point, tank_point_params({0.0,60.0,0.0},false, 127.0,1.0,true,2.0,25.0));
	// move_start(move_types::turn_angle, turn_angle_params(90.0, true,true, 5.0, 0.0));
	// delay(5000);

	// move_start(move_types::turn_angle, turn_angle_params(90.0, true,true, 0.0, 0.0));
	move_start(move_types::tank_arc, tank_arc_params({0.0, 0.0}, {-12,12, -90.0}));

	drivebase.move(0.0,0.0);
	// int speed = 0;
	while(true){
		// drivebase.handle_input();
		// if(master.get_digital_new_press(DIGITAL_A)){
		// 	speed+=2;
		// 	master.print(0,0,"%d", speed);
		// }
		// drivebase.move(0.0,speed);
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