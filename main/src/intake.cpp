#include "intake.hpp"

int buffer;
Task *IntakeTask = nullptr;
bool running = 0;
double intake_pos = 10;
double last_zero = intake.get_position();
double last_pos = 0;
bool have_ring;
uint32_t time_started = millis();
Intake_States Intake_State = Intake_States::Off;
Intake_States Last_Intake_State = Intake_State;
int ring_count = 0;

void StartInTask(){
  IntakeTask = new Task(Intake_loop);
}

void setIntakeState(Intake_States state) {
	Last_Intake_State = Intake_State;
	Intake_State = state;
	printf("Switching from %d, to %d\n", static_cast<int>(Last_Intake_State), static_cast<int>(Intake_State));
}

bool range(double bot_range, double top_range){
	if(intake.get_position() >= bot_range && intake_pos <= top_range){
		return true;
	}else{
		return false;
	}
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
	printf("Motor Pos: %f, Raw Motor Pos: %f, Ring: %d\n", intake_pos, intake.get_position(), ring_count);
	//printf("Raw Time: %d, Time: %u, Diff: %f \n", millis(), millis() - time_started, intake.get_position() -last_pos);
}

void Intake_brake(int time){
	intake.move_relative(0, 200);
	delay(time);
	intake.move(127);
	delay(100);
}

void Intake_loop(){
	while(true){
		intake_pos = intake.get_position() - last_zero;
		if(!running && Intake_State != Intake_States::Off){
			setIntakeState(Intake_States::Off);
		}
		switch(Intake_State){
			case Intake_States::Jammed:
				printf("Jam Detected\n");
				intake.move(0);
				running = 0;
				setIntakeState(Intake_States::Off);
				break;
			case Intake_States::Off:
				intake.move(0);
				if(running){
					buffer = 3;
					if(Last_Intake_State != Intake_States::Holding)
					setIntakeState(Intake_States::Intaking);
				}else{
					setIntakeState(Intake_States::Holding);
				}
				break;
			case Intake_States::Intaking:
				intake.move(127);
				if(intake_zero.get_new_press()){
					intake_pos = 0;
					last_zero = intake.get_position();
				}
				if(ring_count != 4 && have_ring && intake_pos >= 945 && intake_pos <= 975){ //has ring and wants to drop it offdrop offs rings 945.955
					Intake_brake(400);
					have_ring = 0;
					ring_count++;
					buffer = 3;
				}else if (ring_count == 4 &&(intake_pos >= 520 && intake_pos <= 550) && have_ring){// has rings but mag is full
					intake.move_relative(0, 200);
					setIntakeState(Intake_States::Holding);
				 }
				if(intake_pos >= 1430 && intake_pos <= 1450){ // Will never have a ring so it is waiting for a ring
					printf("here\n");
					intake.move_relative(0, 200);
					setIntakeState(Intake_States::Searching);
				}
				break;
			case Intake_States::Holding:
				intake.move(0);
				if(ring_count == 0){
					buffer = 3;
					setIntakeState(Intake_States::Intaking);
				}
				break;
			case Intake_States::Searching:
				intake.move(0);
				if(distance_intake.get() <= 90){
					intake.move(127);
					while(!(intake_pos >= 0 && intake_pos <= 100) && !have_ring){
						if(intake_zero.get_new_press()){
							intake_pos = 0;
							last_zero = intake.get_position();
						}
						delay(10);
						if(distance_intake.get() < 70){
							have_ring = 1;
							printf("picked up ring\n");
						}
					}
					buffer = 3;
					setIntakeState(Intake_States::Intaking);
				}
		}
		last_pos = intake.get_position();
		//printf("Motor Pos: %f, Raw Motor Pos: %f, Ring: %d\n", intake_pos, intake.get_position(), ring_count);
		delay(10);
	}
}
void On_Off_Task(){
	running = !running;
	// switch (Intake_State) {
	// 	case Intake_States::Off:
	// 		setIntakeState(Intake_States::Intaking);
	// 		time_started = millis();
	// 		break;
	// 	case Intake_States::Holding:
	// 		setIntakeState(Intake_States::Off);
	// 		break;
	// 	case Intake_States::Searching:
	// 		setIntakeState(Intake_States::Off);
	// 		break;
	// 	case Intake_States::Intaking:
	// 		setIntakeState(Intake_States::Off);
	// 		break;
	// 	case Intake_States::Jammed:
	// 		setIntakeState(Intake_States::Off);
	// 		break;
	// }
}
