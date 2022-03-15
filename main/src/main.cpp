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

#include "task.hpp"
#include "util.hpp"
using namespace std;

pros::Task *updt = nullptr;
const GUI* GUI::current_gui = &g_main;


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

bool auton_run = false; // has auton run

void initialize() {
	gyro.calibrate();
	// front_l.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// front_r.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// back_l.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// back_r.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// Autons::file_read();
	// autonFile_read();
	f_lift.motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	b_lift.motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	drivebase.download_curve_data();
	Data::init();
	_Controller::init();
	GUI::init();
	f_lift.reset();
	b_lift.reset();
	delay(500);
	
	// tracking.x_coord = 104.0, tracking.y_coord = 12.0, tracking.global_angle = -30.0_deg;
	// tracking.x_coord = 24.5, tracking.y_coord = 15.0, tracking.global_angle = 9.0_deg;
	// tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0_deg;
	update_t.start();
	// master.print(2, 0, "Driver: %s", drivebase.drivers[drivebase.cur_driver].name);
	gyro.finish_calibrating(); //Finishes calibrating gyro before program starts
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
}

void opcontrol() {

	wait_until(master.get_digital_new_press(DIGITAL_R1));
	b_claw_p.set_value(HIGH);//locks in goals and raises lift
	f_claw_p.set_value(HIGH);
	// f_lift.move_absolute(600, 100, true, 100);
	// b_lift.move_absolute(100);

	// flatten_against_wall(true); //resets, change to dist sensor
	// tracking.reset();
  int start = millis();

	// move_start(move_types::turn_angle, turn_angle_params(85.0)); //turns to goal
	drivebase.set_state(HIGH);
	b_lift.move_absolute(100); //lowers lifts
	f_lift.move_absolute(0, 100, true, 50);
	// f_lift.move(-10); //holds lift down

	// // drivebase.move(0.0, 25, 0.0);
	// // move_start(move_types::tank_point, tank_point_params({-5.0, 0.0, -90.0}, false, 127.0, 1.0, true, 6.5, 150.0, 0.0, 0, {6.0, 0.5}), false);
	// // f_detect_goal(false);

	// move_start(move_types::turn_angle, turn_angle_params(90.0)); //aligns to ramp

	// wait_until(master.get_digital_new_press(DIGITAL_R1));
	gyro.climb_ramp();
  gyro.level(1.8, 650);
	b_claw_p.set_value(LOW);
	f_claw_p.set_value(LOW);
  b_lift.move(-10);
  f_lift.move(-10);

  printf("\n\nStart: %d\n", start);
  printf("\n\nEnd: %d\n", millis());
  printf("\n\nTotal: %d\n", millis()-start);
  master.clear();
  master.print(0, 0, "Time:%d", millis()-start);

	while(true){
		GUI::update();
		delay(10);
	}
}