#include "main.h"
#include "Libraries/gui.hpp"
#include "auton.hpp"
#include "auton_util.hpp"
#include "config.hpp"
#include "drive.hpp"
#include "tracking.hpp"
#include "Subsystems/f_lift.hpp"
#include "Subsystems/b_lift.hpp"

Task* updt = nullptr; //What's this for
const GUI* GUI::current_gui = &main_obj;

bool robot_setup = false;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

bool auton_run = false; //has auton run

void initialize() {
	// gyro.calibrate();
  // front_l.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	// front_r.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	 //back_l.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	 //back_r.set_brake_mode(E_MOTOR_BRAKE_HOLD);

  load_positions();
  load_auton();

	drivebase.download_curve_data();
	Data::init();
	_Controller::init();
	GUI::init();
	delay(500);

  b_lift.motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);
  f_lift.motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	update_t.start();
  lift_handle_t.start();

	 //gyro.finish_calibrating(); //Finishes calibrating gyro before program starts
  tracking.reset(107, 15, 0);
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
  run_auton();
}

bool timer_state = 0; 
int timer = millis();
void opcontrol() {
  /*
    Move all template / constexpr definitions to the bottom of the file
    make distance.hpp functions static class memebers
  */
}