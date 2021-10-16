#include "intake.hpp"

double intake_pos = 10;
double last_zero = intake.get_position();
double last_pos = 0;
uint32_t time_started = millis();
Intake_States Intake_State = Intake_States::Off;
Intake_States Last_Intake_State = Intake_State;
int ring_count = 0;
void setIntakeState(Intake_States state) {
	Last_Intake_State = Intake_State;
	Intake_State = state;
	//printf("Switching from %d, to %d", Last_Intake_State, Intake_State);
}

void Intake_Setup() {
	intake_pos = 0;
	while (!(intake_pos >= 1430 && intake_pos <= 1450)){
		intake_pos = intake.get_position() - last_zero;
		intake.move(127);
		if(intake_zero.get_new_press()){
			intake_pos = 0;
			last_zero = intake.get_position();
		}
	}
}

void Reset(){
	printf("Reset\n");
	intake_pos = 0;
	last_zero = 0;
	ring_count = 0;
	printf("Motor: %f\n", intake_pos);
	while (!(intake_pos >= 1430 && intake_pos <= 1450)){
		intake_pos = intake.get_position() - last_zero;
		intake.move(127);
		if(intake_zero.get_new_press()){
			intake_pos = 0;
			last_zero = intake.get_position();
		}
	}
	setIntakeState(Intake_States::Searching);
}

void Intake_debug(){
	//printf("Started Program: %u\n", time_started);
	//printf("Motor Pos: %f, Raw Motor Pos: %f, Last Pos: %f \n", intake_pos, intake.get_position(), last_pos);
	//printf("Raw Time: %d, Time: %u, Diff: %f \n", millis(), millis() - time_started, intake.get_position() -last_pos);
}

void Intake_brake(int time){
	intake.move_relative(0, 200);
	delay(time);
	intake.move(127);
	delay(100);
}

void Intake_loop(){
	intake_pos = intake.get_position() - last_zero;
	switch(Intake_State){
		case Intake_States::Jammed:
			printf("Jam Detected\n");
			intake.move(0);
			setIntakeState(Intake_States::Off);
			break;
		case Intake_States::Off:
			intake.move(0);
			break;
		case Intake_States::Intaking:
			intake.move(127);
			if(intake_zero.get_new_press()){
				intake_pos = 0;
				last_zero = intake.get_position();

			}
			if(ring_count != 5){
				if(intake_pos >= 945 && intake_pos <= 955){
					Intake_brake(200);
					printf("done\n");
				}
				if(intake_pos >= 1430 && intake_pos <= 1450){
					printf("here\n");
					intake.move_relative(0, 200);
					setIntakeState(Intake_States::Searching);
				}
			}else{
				if(intake_pos >= 500 && intake_pos <= 520){
					printf("here\n");
					intake.move_relative(0, 200);
					setIntakeState(Intake_States::Holding);
				}
			}
			// if((intake.get_position() - last_pos) <= 5 && (millis() - time_started) > 1000){
			// 	setIntakeState(Intake_States::Jammed);
			// }
			break;
		case Intake_States::Holding:
			intake.move(0);
			break;
		case Intake_States::Searching:
			intake.move(0);
			//printf("Distance: %d \n", distance_intake.get());
			if(distance_intake.get() <= 90){
				intake.move(127);
				while(distance_intake.get() <= 60){
					delay(10);
				}
				ring_count++;
				setIntakeState(Intake_States::Intaking);
			}
	}
	last_pos = intake.get_position();
}

void On_Off_Task(){
	switch (Intake_State) {
		case Intake_States::Off:
			setIntakeState(Intake_States::Intaking);
			time_started = millis();
			break;
		case Intake_States::Holding:
			setIntakeState(Intake_States::Off);
			break;
		case Intake_States::Searching:
			setIntakeState(Intake_States::Off);
			break;
		case Intake_States::Intaking:
			setIntakeState(Intake_States::Off);
			break;
		case Intake_States::Jammed:
			setIntakeState(Intake_States::Off);
			break;
	}
}
