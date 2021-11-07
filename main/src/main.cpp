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
}

void disabled() {}

void competition_initialize() {}

void autonomous() {}

int cur_driver = 1; //Get rid of this once merged

void opcontrol() {
	Page::goTo(&intkTest);
	intake.StartInTask();
	while(true){
		//printf("Pos: %f\n", intk.get_position());
		guiBackground();
		delay(50);
	}
}
