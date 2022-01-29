#include "Subsystems/lift.hpp"
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
#include <fstream>
using namespace std;
// using namespace pros;


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
 pros::Task *updt = nullptr;
 bool auton_run = false; // has auton run

void initialize() {
	drivebase.download_curve_data();
	Data::init();
	_Controller::init();
	delay(150);
	// tracking.x_coord = 144.0 - 10.25, tracking.y_coord = 14.75, tracking.global_angle = -M_PI_2;
	// tracking.x_coord = 0.0, tracking.y_coord = 0.0, tracking.global_angle = 0.0;
	// tracking.x_coord = 79.0, tracking.y_coord = 106.0, tracking.global_angle = deg_to_rad(-270.0);
	tracking.x_coord = 23.5, tracking.y_coord = 5.75, tracking.global_angle = 1.0_deg;
	update_t.start();
	GUI::setup();
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
	lrt_auton();

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
  move_stop();
  printf("STOPPING00\n\n");
  // while(true){
  //   ring_piston.set_value(master.get_digital(DIGITAL_A));
  //   delay(10);
  // }
  if(auton_run){
    drivebase.driver_practice();
  }
  else{
    // initializes pneumatics in appropriate state
    intake_piston.set_value(HIGH);
    delay(50); // wait for intake to rise
    lift_piston.set_value(LOW);

    tilter_bottom_piston.set_value(HIGH);
    tilter_top_piston.set_value(HIGH);

    intake.motor.move(0);

    // resets subsystems
    lift.reset();
    intake_piston.set_value(LOW);

    tilter.reset();

    while(true){
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
			Data::log_t.data_update();
			// file =
			// ofstream file;
			// file.open("/usd/init.tx t");
			// file.write("%f, %f, %f")
			FILE * fp;
			fp = fopen("/usd/init.txt", "w");
			fprintf(fp, "%lf, %lf, %lf", tracking.x_coord, tracking.y_coord, tracking.global_angle);
			fclose(fp);
			Data::log_t.done_update();
		}
      delay(10);
    }
  }

  /*
	master.clear();
	// while(true){
	// 	if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
	// 		master.print(0,0,"%.2f, %.2f, %.2f",tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
	// 	}
	// 	drivebase.handle_input();
	// 	delay(10);
	// }
	intake_piston.set_value(0);
	tilter_top_piston.set_value(1);
	tilter_bottom_piston.set_value(1);
	while(!master.get_digital(E_CONTROLLER_DIGITAL_A))delay(10);

	lrt_auton();


  */
	// tilter_reset2();
	// // tilter_top_piston.set_value(0);
	// // tilter_bottom_piston.
	// // tilter_reset2();
	// Timer timer{"timer"};
	// move_start(move_types::point, point_params({5.0,8.0, 45.0}));
	// intake.move(100);
	// move_start(move_types::point, point_params(polar_to_vector_point(5.0,8.0,17.5,45.0,45.0),127.0,false,0.0,true,0.0,0.0,{18.0,0.0,20.0},{12.0,0.0001,1000.0},{125.0,0.0,1000.0}),false);
	// // move_wait_for_error(2.0);

	// move_wait_for_complete();
	// move_start(move_types::point, point_params(polar_to_vector_point(5.0,8.0,13.0,45.0,45.0),127.0,false,0.0,true,0.0,0.0,{18.0,0.0,100.0},{12.0,0.0001,1000.0},{125.0,0.0,1000.0}),false);
	// tilter.move_absolute(tilter.bottom_position);
	// while(tilter.motor.get_position() < tilter.bottom_position - 10)delay(10);
	// // move_wait_for_complete();
	// // move_wait_for_error(2.0);
	// delay(100);
	// tilter_bottom_piston.set_value(0);
	// delay(100);
	// move_wait_for_complete();
	// move_start(move_types::point, point_params(polar_to_vector_point(5.0,8.0,10.0,45.0,45.0),127.0,false,0.0,true,0.0,0.0,{18.0,0.0,100.0},{12.0,0.0001,1000.0},{125.0,0.0,1000.0}),true);
	// // move_start(move_types::point, point_params({-3.0, 18.0,0.0}));
	// // move_start(move_types::turn_point, turn_point_params({-2.5, 92.5}));
	// move_start(move_types::turn_angle, turn_angle_params(-15.0));
	// move_start(move_types::line_old, line_old_params(12.75, 15.75, -5.5, 92.5,-15.0,false,true,127, false,2.5,10.0,15.0,0.6,0.2),false); // drives through 1st small neutral mogo
	// // move_start(move_types::point, point_params({-2.5, 92.5, rad_to_deg(atan2(-2.5 - tracking.x_coord, 92.5 - tracking.y_coord))}),false);
	// find_goal_tilter(75);
	// move_wait_for_complete();
	// move_start(move_types::point, point_params(polar_to_vector_point(-3.5, 92.5, -14.0, -20,-20))); // backs up from goal
	// move_wait_for_error(2.0);
	// tilter.move_absolute(tilter.bottom_position);
  	// while(tilter.motor.get_position() < tilter.bottom_position - 30)delay(10);
  	// tilter_top_piston.set_value(1);
  	// tilter_bottom_piston.set_value(0);
	// tilter.move_absolute(300);
	// while(tilter.motor.get_position() > 310)delay(10);
	// move_wait_for_complete();
	// move_start(move_types::point, point_params({3.0,75.0,-105.0}));


	// //going for center goal now
	// tilter.move_absolute(tilter.bottom_position);
	// // move_start(move_types::point, point_params(polar_to_vector_point(3.0,75.0,32.0,-110.0,-110.0)),false);
	// move_start(move_types::line_old, line_old_params(3.0, 75.0, -27.0, 64.5, -105, false,true,127, false,2.5,10.0,15.0,0.6,0.2), false);
	// find_goal_tilter();
	// move_stop();
	// // drivebase.move(0,0,0);
	// move_start(move_types::tank_arc, tank_arc_params({-27.0, 64.0}, {-55.0, 41.0, -135.0}, 127.0, 127.0, false));
	// move_start(move_types::point, point_params({-59.0, 36.0, -135.0}));
	// master.print(0,0,"%d",timer.get_time());
	// // const double max_power = 127.0, const bool overshoot = false, const double min_angle_percent = 0.0, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0);


}
