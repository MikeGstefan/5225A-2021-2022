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
#include "Subsystems/hitch.hpp"
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
	gyro.calibrate();
	// front_l.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// front_r.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// back_l.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// back_r.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	drivebase.download_curve_data();
	Data::init();
	_Controller::init();
	GUI::init();
	delay(500);
	tracking.x_coord = 25.0, tracking.y_coord = 11.75, tracking.global_angle = -90.0_deg;
  // tracking.x_coord = 36.0, tracking.y_coord = 106.0, tracking.global_angle = 90.0_deg;

  b_lift.motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);
  f_lift.motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	
	// tracking.x_coord = 104.0, tracking.y_coord = 12.0, tracking.global_angle = -30.0_deg;
	// tracking.x_coord = 24.5, tracking.y_coord = 15.0, tracking.global_angle = 9.0_deg;
	// tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0_deg;
	update_t.start();
  lift_handle_t.start();
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
//   run_auton();
}


bool timer_state = 0; 
int timer = millis();
void opcontrol() {
  // tilt_lock.set_state(HIGH);

  // b_claw_obj.set_state(b_claw_states::managed);
  // f_claw_obj.set_state(f_claw_states::managed);
  // hitch_obj.set_state(hitch_states::managed);
  // b_lift.move_to_top();
  // // if(f_claw_o.get_state() == HIGH) f_claw(LOW);
  // f_claw(HIGH);
  // f_lift.set_state(f_lift_states::move_to_target, 2);

  // drivebase.move(0.0, 0.0); //so it's not locked when switching trans
	// drivebase.set_state(HIGH);

  // master.wait_for_press(DIGITAL_R1);
	// f_claw(HIGH);
	// b_claw.set_state(HIGH);
  // hitch.set_state(HIGH);
  // tilt_lock.set_state(LOW);
	master.clear();
  hitch.set_state(1);
  delay(500);
  b_lift.set_state(b_lift_states::move_to_target, 0);
  while(b_lift.get_target_state() != b_lift_states::bottom)delay(10);
  b_lift.Subsystem::set_state(b_lift_states::intake_on);
  b_lift.Subsystem::set_state(b_lift_states::intake_off);
  // f_lift.set_state(f_lift_states::move_to_target, 1);
  while(!master.get_digital_new_press(DIGITAL_A))delay(10);
  int time = millis();
//   b_lift.Subsystem::set_state(b_lift_states::intake_on);
//   move_start(move_types::tank_point, tank_point_params({0.0, 40.0, 0.0}, false, 60.0),false);
//   f_detect_goal();
//   move_stop();
//   drivebase.brake();
  // b_lift.set_state(b_lift_states::move_to_target, b_top);
  // hitch.set_state(0);
  // while(b_lift.get_target_state() != b_lift_states::top)delay(10);
  // // move_start(move_types::tank_point, tank_point_params({36.0,106.0, 30.0}, false, 80.0));
  // // move_start(move_types::turn_angle, turn_angle_params(angle_to_point(12.0, 107.0) + 180));
  // move_start(move_types::tank_point, tank_point_params({12.0,106.0, 30.0}, false, 80.0), false);
  // Task([](){
  //   while(true){
  //     printf("SENSOR: %d\n", hitch_dist.get());
  //     delay(33);
  //   }
  // });
  // detect_hitch();
  // move_stop();
  // drivebase.brake();
//   skills();
//   skills2();
//   skills3();
  skills4();
//   skills_park();
  master.print(0,0, "FULL : %D", millis()- time);
  while(true)delay(10);

  // master.wait_for_press(DIGITAL_R1);
  // int start = millis();

  // gyro.climb_ramp();
  // drivebase.brake();


  // printf("\n\nStart: %d\n", start);
  // printf("\n\nEnd: %d\n", millis());
  // printf("\n\nTotal: %d\n", millis()-start);
  // master.print(0, 0, "Time:%d", millis()-start);

  // delay(500);
  // hitch.set_value(LOW);
  // tilt_lock.set_state(HIGH);
  // b_claw.set_state(LOW);
  // b_lift.set_state(b_lift_states::move_to_target, 4);
  // f_lift.set_state(f_lift_states::move_to_target, 0);
  // // lift_t.toggle_state();
  // b_lift.Subsystem::set_state(b_lift_states::intake_off);
}
