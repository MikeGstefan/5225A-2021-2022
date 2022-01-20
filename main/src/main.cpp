#include "Subsystems/lift.hpp"
#include "Subsystems/hitch.hpp"
#include "drive.hpp"
#include "controller.hpp"
#include "gui/gui_main.hpp"
#include "pid.hpp"
#include "Tracking.hpp"
#include "task.hpp"
#include "auton.hpp"
#include "auton_util.hpp"

// using namespace std;
#include "task.hpp"
using namespace std;
// using namespace pros;


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
 pros::Task *updt = nullptr;

void initialize() {
	drivebase.download_curve_data();
	Data::init();
	_Controller::init();
	delay(150);
	tracking.x_coord = 144.0 - 10.25, tracking.y_coord = 14.75, tracking.global_angle = -M_PI_2;
	// tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;
	// tracking.x_coord = 79.0, tracking.y_coord = 106.0, tracking.global_angle = deg_to_rad(-270.0);
	update_t.start();
	GUI::setup();
	master.print(2, 0, "Driver: %s", drivebase.drivers[drivebase.cur_driver].name);
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
	skills_pt2();

}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
 int ring_count = 0;
 int cur_auton = 1;

//Get rid of these once merged
void prev_driver(){
	if (drivebase.cur_driver == 0) drivebase.cur_driver = drivebase.num_of_drivers - 1;
	else drivebase.cur_driver--;
	WAIT_FOR_SCREEN_REFRESH();
	master.print(2, 0, "Driver: %s          ", drivebase.drivers[drivebase.cur_driver].name);
}
void next_driver(){
	drivebase.cur_driver++;
	drivebase.cur_driver %= drivebase.num_of_drivers;
	WAIT_FOR_SCREEN_REFRESH();
	master.print(2, 0, "Driver: %s          ", drivebase.drivers[drivebase.cur_driver].name);
}


void tilter_reset2(){
    tilter_motor.move(-40);
    Timer vel_rise_timeout("vel_rise");
    // waits for motor's velocity to rise or timeout to trigger
    while(fabs(tilter_motor.get_actual_velocity()) < 45.0){
    //   printf("%s's velocity is (rising loop): %lf\n", this->name, motor.get_actual_velocity());
      if (vel_rise_timeout.get_time() > 50){
        // printf("%s's rising loop timed out\n", this->name);
        break;
      }
      delay(10);
    }
    // printf("%s's velocity done rising\n", this->name);
    // waits until motors velocity slows down for 5 cycles
    cycleCheck(fabs(tilter_motor.get_actual_velocity()) < 5.0, 5, 10)
    tilter_motor.tare_position();  // resets subsystems position
    // printf("%d, %s's reset %lf\n", millis(), this->name, motor.get_position());
    tilter_motor.move(0);
}

// int ring_count = 0, cur_auton = 1;
bool claw_state = false, intk_state = false;
void opcontrol() {
	while(true){
		printf("%f\n", get_front_dist());
		delay(2000);
	}
	while(true){
		drivebase.handle_input();
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_A))master.print(0,0,"%.2f, %.2f, %.2f",tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
		delay(10);
	}

	// while(true){ 
	// 	if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
	// 		claw_state = !claw_state;
	// 		tilter_bottom_piston.set_value(claw_state);
	// 	}
	// 	delay(10);
	// }

	delay(1000);

	drivebase.move(-70, 0, -10);
	// cycleCheck(fabs(rad_to_deg(tracking.g_velocity.angle)) >  0.01, 15,10);
	int bad_count = 0;
	int good_count = 0;
	while(good_count < 15 && bad_count < 30){ 
		if(fabs(rad_to_deg(tracking.g_velocity.angle)) >  0.01)good_count++;
		else{
			good_count = 0;
			bad_count++;
		}
		printf("good: %d bad: %d, v: %f\n", good_count, bad_count, fabs(rad_to_deg(tracking.g_velocity.angle)));
		delay(10);
	}
	cycleCheck(fabs(rad_to_deg(tracking.g_velocity.angle)) <  0.0001, 10,10);

	drivebase.move(-40, 0, -40);
	// cycleCheck(fabs(rad_to_deg(tracking.g_velocity.angle)) >  0.01, 15,10);
	bad_count = 0;
	good_count = 0;
	while(good_count < 10 && bad_count < 30){ 
		if(fabs(rad_to_deg(tracking.g_velocity.angle)) >  0.01)good_count++;
		else{
			good_count = 0;
			bad_count++;
		}
		printf("good: %d bad: %d, v: %f\n", good_count, bad_count, fabs(rad_to_deg(tracking.g_velocity.angle)));
		delay(10);
	}
	cycleCheck(fabs(rad_to_deg(tracking.g_velocity.angle)) <  0.0001, 10,10);
	drivebase.move(-60, 0, 30);
	bad_count = 0;
	good_count = 0;
	while(good_count < 10 && bad_count < 30){ 
		if(fabs(rad_to_deg(tracking.g_velocity.angle)) >  0.01)good_count++;
		else{
			good_count = 0;
			bad_count++;
		}
		delay(10);
	}
	cycleCheck(fabs(rad_to_deg(tracking.g_velocity.angle)) <   0.0001, 10,10);
	drivebase.move(0,0,0);
	while(true){ 
		
		printf("%f\n", rad_to_deg(tracking.g_velocity.angle));
		delay(10);
	}
	// intk_pnue.set_value(0);
  	// lift.reset();
  	// tilter_reset2();
	// tilter.move_absolute(150);
	// // move_start(move_types::point, point_params({79.0, 106.0, -270.0}));
  	// // lift.move_absolute(lift.bottom_position);
  	// move_start(move_types::point, point_params({100.0,108.0,-270.0},127.0,true,0.0,false));
	// // move_start(move_types::point, point_params({130.0,127.0,-285.0}));
  	// move_start(move_types::tank_arc, tank_arc_params({79.0, 106.0},{125.0,127.0,-285.0},127.0,127.0,false));
	// move_start(move_types::point, point_params({125.0,133.0,-270.0}));

	while(true){
		// drivebase.handle_input();
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
			claw_state = !claw_state;
			intk_pnue.set_value(claw_state);
			master.print(0,0,"state: %d",claw_state);
			// master.print(0,0,"%.2f, %.2f, %.2f",tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
		}
		delay(10);
	}

	// while(true){ 
	// 	if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
	// 		claw_state = !claw_state;
	// 		tilter_top_piston.set_value(claw_state);
	// 	}
	// 	delay(10);
	// }
	// intk_pnue.set_value(0);
	// lift_piston.set_value(0);
	// lift.reset();
	// tilter.reset();
	// tilter.move_absolute(tilter.bottom_position);
	// while(!master.get_digital_new_press(E_CONTROLLER_DIGITAL_A))delay(10);
	// lift_piston.set_value(1);
	// move_start(move_types::line_old, line_old_params(0.0,0.0,0.0,40.0,0.0,true,true,127, false,0.5,5.0,15.0,1.0,0.0));
	// move_start(move_types::line, line_params({0.0, 0.0}, {0.0, 40.0, 0.0}),true);
	//
	// driver setup
	// move_start(move_types::point, point_params{{0.0,40.0,0.0}},true);
	// move_start(move_types::point, point_params{{0.0,0.0,90.0}},true);
	// move_start(move_types::tank_arc, tank_arc_params({0.0, 0.0},{24.0, 24.0, 90.0}, 127.0, 1.0,true), true);  // arcs to first patch of rings
	lift_piston.set_value(LOW);	// in searching state
	lift.reset();
	tilter.reset();
	lift.motor.move_absolute(35, 100);
	tilter.move_absolute(500, 100);
	tilter_bottom_piston.set_value(1);
	tilter_top_piston.set_value(LOW);


	drivebase.driver_practice();
}
