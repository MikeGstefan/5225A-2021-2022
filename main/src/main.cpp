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
	// tracking.x_coord = 144.0 - 10.25, tracking.y_coord = 14.75, tracking.global_angle = -M_PI_2;
	tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;
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
	intk_pnue.set_value(0);
	tilter_top_piston.set_value(1);
	tilter_bottom_piston.set_value(1);
	while(!master.get_digital(E_CONTROLLER_DIGITAL_A))delay(10);
	tilter_reset2();
	// tilter_top_piston.set_value(0);
	// tilter_bottom_piston.
	// tilter_reset2();
	Timer timer{"timer"};
	move_start(move_types::point, point_params({5.0,8.0, 45.0}));
	
	move_start(move_types::point, point_params(polar_to_vector_point(5.0,8.0,17.5,45.0,45.0),127.0,false,0.0,true,0.0,0.0,{18.0,0.0,20.0},{12.0,0.0001,1000.0},{125.0,0.0,1000.0}),false);
	// move_wait_for_error(2.0);
	
	move_wait_for_complete();
	move_start(move_types::point, point_params(polar_to_vector_point(5.0,8.0,13.0,45.0,45.0),127.0,false,0.0,true,0.0,0.0,{18.0,0.0,100.0},{12.0,0.0001,1000.0},{125.0,0.0,1000.0}),false);
	tilter.move_absolute(tilter.bottom_position);
	while(tilter.motor.get_position() < tilter.bottom_position - 10)delay(10);
	// move_wait_for_complete();
	// move_wait_for_error(2.0);
	delay(100);
	tilter_bottom_piston.set_value(0);
	delay(100);
	move_wait_for_complete();
	move_start(move_types::point, point_params(polar_to_vector_point(5.0,8.0,11.0,45.0,45.0),127.0,false,0.0,true,0.0,0.0,{18.0,0.0,100.0},{12.0,0.0001,1000.0},{125.0,0.0,1000.0}),true);
	// move_start(move_types::point, point_params({-3.0, 18.0,0.0}));
	move_start(move_types::turn_point, turn_point_params({-5.5, 92.5}));
	move_start(move_types::line_old, line_old_params(12.75, 15.75, -5.5, 92.5,-20.0,false,true,127, false,1.5,7.0,15.0,0.6,0.2),false); // drives through 1st small neutral mogo
	find_goal_tilter();
	move_wait_for_complete();
	move_start(move_types::point, point_params(polar_to_vector_point(-3.5, 92.5, -12.0, -20,-20))); // backs up from goal
	tilter.move_absolute(tilter.bottom_position);
  	while(tilter.motor.get_position() < tilter.bottom_position - 30)delay(10);
  	tilter_top_piston.set_value(1);
  	tilter_bottom_piston.set_value(0);
	tilter.move_absolute(300);
	while(tilter.motor.get_position() > 310)delay(10);
	master.print(0,0,"%d",timer.get_time());
	// const double max_power = 127.0, const bool overshoot = false, const double min_angle_percent = 0.0, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0);


}
