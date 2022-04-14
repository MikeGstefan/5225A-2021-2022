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
  load_positions();
  load_auton();

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
  lift_handle_t.start();

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
  run_auton();
}

void opcontrol() {
	// while (true) {
	// 	printf("B: %f\n", b_lift_m.get_position());
	// 	printf("F: %f\n", f_lift_m.get_position());
	// 	delay(10);
	// }
	// b)
	// b_lift.Subsystem::set_state(b_lift_states::move_to_target);
	// lift_handle_t.start();
	// /*
	// b_lift.elastic_util(1029);
	// wait_until(false);
	// while(true){
	// 	printf("b_pot_val: %d\n", b_lift_pot.get_value());
	// 	delay(10);
	// }
	/*
	tilt_lock.set_value(HIGH);
	b_claw.set_value(HIGH);
	wait_until(master.get_digital_new_press(DIGITAL_A));
	tilt_lock.set_value(LOW);
	delay(100);
	b_claw.set_value(LOW);
	wait_until(false);
	*/
	// 1035, 1820, 2000, 2760

	// 1182, 2070, 1840
	// bool b_claw_state = false;
	// while(true){
	// 	printf("%d\n", f_lift_pot.get_value());
	// 	if(master.get_digital_new_press(DIGITAL_A)){
	// 		b_claw_state = !b_claw_state;
	// 		f_claw(b_claw_state);
	// 	}
	// 	delay(10);
	// }
	// bool transmission = false;
	// while(true){
	// 	if(master.get_digital_new_press(DIGITAL_X)){

	// 		transmission = !transmission;
	// 		trans_p.set_state();
	// 	}
	// 	delay(10);
	// }
	master.clear();
	master.clear();

	master.print(0,0, "auto      ");
	partner.print(0,0, "auto     ");

	while(true){
		// printf("b:%d, f:%d\n", b_lift_pot.get_value(), f_lift_pot.get_value());
		master.update_buttons();
		partner.update_buttons();
		// printf("%d\n", get_lift());
		// drivebase handlers
		drivebase.handle_input();
		drivebase.handle_trans();

		// intake handlers
		intake.handle_buttons();
		intake.handle();

		// lift handlers
		handle_lift_buttons();
		b_lift.handle(true);
		f_lift.handle(true);

		// claw handlers
		handle_claw_buttons();
		b_claw_obj.handle();
		f_claw_obj.handle();
		delay(10);
	}
	// */
	master.clear();
	f_lift.set_state(f_lift_states::move_to_target, 3);
	wait_until(f_lift.get_state() == f_lift_states::idle);
	master.rumble("-");
	master.print(0,0,"reached 4");
	delay(1000);

	f_lift.set_state(f_lift_states::move_to_target, 1);
	wait_until(f_lift.get_state() == f_lift_states::idle);
	master.rumble("-");
	master.print(0,0,"reached 1");
	delay(1000);

	f_lift.set_state(f_lift_states::move_to_target, 2, 50);
	wait_until(f_lift.get_state() == f_lift_states::idle);
	master.rumble("-");
	master.print(0,0,"reached 1");
	delay(1000);


	while(true) delay(10); // don't exucute anything beyond this line

	b_lift.set_state(b_lift_states::move_to_target, 3);
	b_lift.Subsystem::set_state(b_lift_states::idle);


	lift_t.set_state(HIGH);
	b_lift_m.move_relative(30, 100);
	while(fabs(b_lift_m.get_position() - b_lift_m.get_position()) > 15) delay(10);
	b_lift_m.move_relative(-30, 100);
	while(fabs(b_lift_m.get_position() - b_lift_m.get_position()) > 15) delay(10);
	// b_lift.

	// while(true){
	// 	b_lift.handle_buttons();
	// 	b_lift.handle(true);
	// 	f_lift.handle_buttons();
	// 	f_lift.handle(true);
	// 	delay(10);
	// }

	// while(true){
	pros::Task task{[=] {
		while(true){
			// f_lift.handle_buttons();
			f_lift.handle(true);
			delay(10);
		}
	}};
	delay(5000);
	f_lift.set_state(f_lift_states::move_to_target, 0);
	delay(1000);
	f_lift.set_state(f_lift_states::move_to_target, 1);



	// delay(1000);
	// f_lift.set_state(f_lift_states::move_to_target, 2);
	// delay(1000);
	// f_lift.set_state(f_lift_states::move_to_target, 3);
	// delay(10);
	// f_lift.set_state(f_lift_states::move_to_target, 0);

	// drivebase.driver_practice();
	// pros::Task task{[=] {
	// 	while(true){
	// 		f_lift.handle(true);
	// 		b_lift.handle(true);
	// 		delay(10);
	// 	}
	// }};
  // f_lift.set_state(f_lift_states::move_to_target, 2); // moves to top

	// f_lift.elastic_util();
  // BACK bottom 1030 top 2754
  // FRONT bottom 1190 top 2778

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

	// drivebase.driver_practice();
}
