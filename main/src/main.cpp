#include "config.hpp"
#include "drive.hpp"
#include "controller.hpp"
#include "Libraries/gui.hpp"
#include "Libraries/printing.hpp"
#include "pid.hpp"
#include "Tracking.hpp"
#include "task.hpp"
#include "auton.hpp"
#include "auton_util.hpp"
#include "Subsystems/f_lift.hpp"
#include "Subsystems/b_lift.hpp"
#include "distance.hpp"
#include "util.hpp"
using namespace std;

pros::Task *updt = nullptr; //What's this for
const GUI* GUI::current_gui = &main_obj;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

bool auton_run = false; // has auton run

void initialize() {
	// gyro.calibrate();

	drivebase.download_curve_data();

	Data::init();
	_Controller::init();
	GUI::init();

	delay(500);

	b_lift.motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	f_lift.motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);

	// tracking.x_coord = 104.0, tracking.y_coord = 12.0, tracking.global_angle = -30.0_deg;
	// tracking.x_coord = 24.5, tracking.y_coord = 15.0, tracking.global_angle = 9.0_deg;
	tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0_deg;
	update_t.start();
  // lift_handle_t.start();

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
  load_positions();
  load_auton();
  run_auton();
}

void opcontrol() {
  /* Nathan
  -distance sensor check
  lift movement
  misc: check if what's plugged matches what's registered to that port
  jam code for main motors (will have to be a page func) (not needed if motor class)

  auton give up func - ask mike

  After Worlds:
  make flash its own page
  make flash queue. (if multiple things try to flash, wait for the first one to finish)
  motor subclass
  adaptable fll menu selector
  file writing functions
  proper text splitter for flash
  allow multiple text variables through tuple
  print stack trace
  remove unnecessary 0s from str func. (03.4200 -> 3.42)
  get rid of add_text. just change the buttons text directly (will have to deal with changeing text length)
  controller button class
  skills recorder. Have the program record movements and write the skills route.
  lvgl images
  2d sliders
  */
}
