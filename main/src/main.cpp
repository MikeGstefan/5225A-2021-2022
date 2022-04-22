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
	// gyro.calibrate();
	// front_l.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// front_r.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// back_l.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	// back_r.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

  load_positions();
  load_auton();

	drivebase.download_curve_data();
	Data::init();
	_Controller::init();
	GUI::init();
	delay(500);
	// tracking.x_coord = 25.0, tracking.y_coord = 11.75, tracking.global_angle = -90.0_deg;

  b_lift.motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);
  f_lift.motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	
	// tracking.x_coord = 104.0, tracking.y_coord = 12.0, tracking.global_angle = -30.0_deg;
	// tracking.x_coord = 24.5, tracking.y_coord = 15.0, tracking.global_angle = 9.0_deg;
	// tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0_deg;
	update_t.start();
    // lift_handle_t.start();

	// gyro.finish_calibrating(); //Finishes calibrating gyro before program starts
	// GUI::go_to(3);
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
	// while(true){ 
	// 	drivebase.handle_input();
	// 	delay(10);
	// }

	// Position start = distance_reset_center();
	// // update_t.kill();
	// // tracking.x_coord = start.x, tracking.y_coord = start.y, tracking.global_angle = deg_to_rad(start.angle);
	// // update_t.start();

	// // printf("x:%lf, y: %lf, a:%lf\n", start.x, start.y, start.angle);
	// // x:69.576370, y: 26.728601, a:-3.335143
	// // 81 back
	// // tracking.reset(start.x, start.y, start.angle +180);
	// flatten_against_wall(true);
	// tracking.reset(70.0, 27.0, 180.0);

	// // x: 36.47, y: 102.75, a: 105.66
	// // x: 37.86, y: 103.60, a: 102.11
	// move_start(move_types::tank_point, tank_point_params({70.0, 102.0, 0.0}, false, 80.0));
	// move_start(move_types::turn_angle, turn_angle_params(90.0));
	// move_start(move_types::tank_point, tank_point_params({36.0, 105.0, 0.0}));
	// while(true) pros::delay(10);
	lift_handle_t.start();
	hitch.set_state(1);
  tilt_lock.set_state(1);
	delay(500);
	b_lift.set_state(b_lift_states::move_to_target, 0);
	f_lift.set_state(f_lift_states::move_to_target,0);
	f_claw(0);

	int state = 0;
	master.wait_for_press(DIGITAL_R1);
	int timer = millis();

	move_start(move_types::tank_rush, tank_rush_params({102.0, 72.0, 0.0}, false, 127.0, 1.0, false));
  drivebase.random_turn(0);
  master.wait_for_press(DIGITAL_R1);

  move_start(move_types::tank_point, tank_point_params({105.0, 20.0, -45.0}, false, 127.0, 1.0, true, 6.4, 70.0, 0.0, 0, {5, 5}), false);
  wait_until(tracking.y_coord < 50.0);
  f_lift.set_state(f_lift_states::move_to_target, 1);
  wait_until(tracking.y_coord < 40.0);
  move_stop(true);
  
  master.wait_for_press(DIGITAL_R1);

  move_start(move_types::turn_angle, turn_angle_params(angle_to_point(130.0, 36.0)+180.0, true, true, 5.0, 0.0, 10.0, 20.0, 127.0, 0, 50));
  move_start(move_types::tank_point, tank_point_params({130.0, 36.0, 0.0}), false);
  b_detect_goal();
  move_stop(true);
  move_start(move_types::turn_angle, turn_angle_params(0.0, true, true, 5.0, 0.0, 10.0, 20.0, 127.0, 0, 50));
  printf2("TIME: %d", millis()-timer);

	// move_start(move_types::tank_point, tank_point_params({100.0, 40.0, 0.0}));
  // master.wait_for_press(DIGITAL_R1);
  // move_start(move_types::turn_point, turn_point_params({130.0, 36.0}), true);
  // move_start(move_types::turn_angle, turn_angle_params(angle_to_point(130.0, 36.0)+180.0));

	// skills();
	// skills2();
	// skills3();
	// skills4();
	// master.print(1,1,"total %d", millis() - timer);
	// skills3();
	// // skills();
	// move_start(move_types::tank_point, tank_point_params({5.0, 100.0,0.0},false,50));
	// move_start(move_types::turn_angle, turn_angle_params(90.0));
	// move_start(move_types::tank_point, tank_point_params({20.0, 20.0,0.0}));
	// // move_start(move_types::tank_arc, tank_arc_params({0.0,0.0}, {20.0,20.0,90.0}, 70.0));
	// flatten_against_wall();
	// // move_start(move_types::tank_arc, tank_arc_params({8.5,8.5}, {20.0,20.0,90.0}));
	// move_start(move_types::tank_point, tank_point_params({30.0, 20.0,0.0}));
	// int diff = LeftEncoder.get_value() - RightEncoder.get_value();
	// int last_diff = diff;
	while(true){
	// 	diff = LeftEncoder.get_value() - RightEncoder.get_value();
	// 	if(diff != last_diff){
	// 		printf("%d|| diff: %d\n", millis(), diff);
	// 		last_diff = diff;
	// 	}
		delay(10);
		// drivebase.handle_input();
	}
	while(true){
		// printf("b:%d, f:%d\n", b_lift_pot.get_value(), f_lift_pot.get_value());
		master.update_buttons();
		partner.update_buttons();
		// printf("%d\n", get_lift());
		// drivebase handlers
		drivebase.handle_input();
		drivebase.handle_trans();

		// intake handlers
		handle_intake_buttons();

		// lift handlers
		handle_lift_buttons();
		b_lift.handle(true);
		f_lift.handle(true);

		// claw handlers
		handle_claw_buttons();
		b_claw_obj.handle();
		f_claw_obj.handle();
		hitch_obj.handle();


		
		// if(print_timer.get_time() > 100){
		// 	printf("b_lift_pot_val:%d, f_lift_pot_val:%d\n", b_lift_pot.get_value(), f_lift_pot.get_value());
		// 	// printf("b_dist:%d\n", b_dist.get());
		// 	print_timer.reset();
		// }

		if(partner.is_rising(timer_btn)){ 
			timer_state = !timer_state;
			if(timer_state)timer = millis();
			else{
				partner.print(0,0,"time: %d", millis() - timer);
			}
		}

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
