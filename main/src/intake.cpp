#include "intake.hpp"

Task *InTask = nullptr;
Task *StartTask = nullptr;

void Intake_loop(){
	printf("Motor Pos: %f \n", intake.get_position());
	double intake_pos = 0;
	double last_zero = 0;
	if(intake.get_current_draw() == 0){
		intake.move(127);
		while(true){
			intake_pos = intake.get_position() - last_zero;
				if(intake_zero.get_new_press()){
					intake.set_zero_position(intake.get_position());
					intake_pos = 0;
					last_zero = intake.get_position();
				}
				if(intake_pos >= 770 && intake_pos < 950){
					intake.move(50);
				}else if(intake_pos >= 690 && intake_pos < 770){
					intake.move(80);
				}else{
					intake.move(127);
				}
				delay(10);
		}
	}else{
		intake.move(0);
	}
}

void Start_Task(){
  if(InTask != nullptr){
    InTask->remove();
    delete InTask;
    InTask = nullptr;
    delay(50);
    intake.move(0);
  }else{
    InTask = new Task(&Intake_loop);
  }
}
