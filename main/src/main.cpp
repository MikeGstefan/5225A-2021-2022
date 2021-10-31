#include "main.h"
#include "logging.hpp"
#include "config.hpp"
#include "util.hpp"
#include "tracking.hpp"
#include "task.hpp"
// using namespace std;
// using namespace pros;


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	delay(150);
	updateStartTask();
	// logging_task_start();
	Data::log_init();

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
void autonomous() {}

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
Task *tester = nullptr;
int test = 0;


void task_testing(void* params){
	Tasks* task_ptr = Tasks::get_obj(params);
	while(true){
		printf("ur mom\n");
		if(task_ptr->notify_handle())return;
		printf("data: %d\n",test);
		delay(1000);
	}
}

void task_testing2(void* params){
	Tasks* task_ptr = Tasks::get_obj(params);
	while(true){
		printf("in rebound\n");
		if(task_ptr->notify_handle())return;
		delay(1000);
	}
}





void opcontrol() {
	printf("here1\n");
	Tasks test_task(task_testing);
	printf("here2\n");
	test_task.task_start();
	printf("here4\n");
	delay(300);
	test_task.task_rebind(task_testing2);
	delay(3000);
	test_task.task_kill();

	// test_task.data_update();
	delay(100);
	while(true){
		// std::cout<< test_task.get_task_ptr()->get_state()<<"\n";
		printf("in loop \n");
		delay(100);
	}

	// test = 1;
	// test_task.done_update();

	// tester = new Task(task_testing);
	// int timer  = micros();
	// while(tester->get_state() ==1)delay(10);
	// std::cout<<micros()-timer<<" "<< tester->get_state()<<"\n";
	// tester->notify();
	// tester->notify();
	// tester->notify();
	// std::cout<< tester->get_state()<<"\n";
	while(true){
		// std::cout<<"not in ur mom"<< tester->get_state()<<"\n";
		delay(50);
	}
}
