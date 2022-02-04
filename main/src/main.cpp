
#include "drive.hpp"
#include "controller.hpp"
#include "Libraries/gui.hpp"
#include "pid.hpp"
#include "Tracking.hpp"
#include "task.hpp"
#include "auton.hpp"
#include "auton_util.hpp"
#include "Subsystem/f_lift.hpp"
#include "Subsystem/b_lift.hpp"

// using namespace std;
#include "task.hpp"
#include <fstream>
using namespace std;

pros::Task *updt = nullptr;
GUI* const GUI::current_gui = &g_main;


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
	drivebase.download_curve_data();
	Data::init();
	_Controller::init();
	GUI::init();
	// delay(150);
	tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;
	update_t.start();
	// // auton_file_read();
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
	auton_run = true;
	double x;
	double y;
	double a;

	// FILE * fp;
	// fp = fopen("/usd/init.txt", "r");
	// fscanf(fp,"%lf %lf %lf",&x,&y,&a);

	// tracking.reset(x,y,rad_to_deg(a));
	// ring_piston.set_value(0);
	// lrt_auton();

	// skills();
	// skills_pt2();


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

//Get rid of these once merged

int ring_count = 0;
// int ring_count = 0, cur_auton = 1;
bool claw_state = false, lift_state = false, drive_state = false;
int lift_speed = 0;
void opcontrol() {
	
	master.clear();
	b_lift.reset();
	b_lift.move(0);

	while(true){
		if(master.get_digital_new_press(DIGITAL_Y)){ 
			master.print(0,0,"%d",BackEncoder.get_value());
		}
		if(master.get_digital_new_press(DIGITAL_A)){
			printf("switching state to %d\n", !claw_state);
			claw_state = !claw_state;
			b_claw_p.set_value(claw_state);
		}
		if(master.get_digital_new_press(DIGITAL_B)){ 
			b_lift.move_absolute(10);
			detect_goal();
			claw_state = 1;
			
		}
		if(master.get_digital_new_press(DIGITAL_L1)){ 
			b_lift.move_absolute(300);
		}
		if(master.get_digital_new_press(DIGITAL_L2)){ 
			b_lift.move_absolute(0);
		}
		// printf("dist: %d\n", b_dist.get());
		drivebase.handle_trans();
		drivebase.handle_input();
		if(master.get_digital_new_press(DIGITAL_R1)){ 
			// b_lift.move_absolute(10);
			b_lift.move(-15);
			move_start(move_types::tank_point, tank_point_params({-2.0,-72.0,0.0}, false),true);
			// detect_goal();
			// move_stop();
			// drivebase.brake();
			move_start(move_types::tank_arc, tank_arc_params({-4.0,-72.0}, {-22.0,-69.0,45.0},127.0,127.0));
			move_start(move_types::tank_point, tank_point_params({-32.0,-85.0,-135.0}, false));
			move_start(move_types::turn_angle, turn_angle_params(-180.0));
			delay(1000);

		}

		if(abs(master.get_analog(ANALOG_LEFT_Y))> 20)b_lift.move(master.get_analog(ANALOG_LEFT_Y));
		else b_lift.move(10);
		
		delay(33);
	}
		
}
