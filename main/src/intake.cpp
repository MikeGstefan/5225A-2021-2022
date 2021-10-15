#include "intake.hpp"

Task *InTask = nullptr;


void Intake_loop(){
	double intake_pos = 0;
	double last_zero = intake.get_position();
	double last_pos = 0;
	uint32_t time_started = millis();
	printf("Started Program: %u\n", time_started);
	intake.move(127);
	while(true){
		//printf("Motor Pos: %f, Raw Motor Pos: %f, Last Pos: %f \n", intake_pos, intake.get_position(), last_pos);
		printf("Raw Time: %d, Time: %u, Diff: %f \n", millis(), millis() - time_started, intake.get_position() -intake_pos);
		if((intake.get_position() - intake_pos) <= 5 && (millis() - time_started) > 1000){
			printf("Jam Detected\n");
			intake.move(0);
			InTask = nullptr;
			break;
		}
		intake_pos = intake.get_position();
		delay(10);
	}
}

void Start_Task(){
	printf("here\n");
  if(InTask != nullptr){
    InTask->remove();
    delete InTask;
    InTask = nullptr;
    delay(50);
    intake.move(0);
		printf("killed Task\n");
  }else{
		printf("Started Task\n");
    InTask = new Task(&Intake_loop);
  }
}
