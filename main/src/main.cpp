#include "main.h"
#include "logging.hpp"
#include "config.hpp"
#include "util.hpp"
#include "Tracking.hpp"
#include "lift.hpp"
#include "drive.hpp"
#include "intake.hpp"
#include "gui.hpp"
// using namespace std;
// using namespace pros;

void initialize() {
	delay(150);
	guiSetup();
	intk.set_brake_mode(E_MOTOR_BRAKE_HOLD);
}

void disabled() {}

void competition_initialize() {}

void autonomous() {}

int cur_driver = 1; //Get rid of this once merged

void opcontrol() {
	Page::goTo(&intkTest);
	intake.StartInTask();
	while(true){
		guiBackground();
		delay(50);
	}
}
