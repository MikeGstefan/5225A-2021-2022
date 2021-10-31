#include "intake.hpp"

Task *IntakeTask = nullptr;
double intake_pos = 10;
double last_zero = intk.get_position();
double last_pos = 0;
uint32_t time_started = millis();
Intake_States Intake_State = Intake_States::Off;
Intake_States Last_Intake_State = Intake_State;
int error_range = 10;
Intake intake;

void ringup(){
  if(intake.ring_count < 4){
    intake.ring_count++;
  }
}
void ringdown(){
  if(intake.ring_count > 0){
    intake.ring_count--;
  }
}

void Intake::StartInTask(){
  IntakeTask = new Task(Intake_loop);
}

void setIntakeState(Intake_States state) {
	Last_Intake_State = Intake_State;
	Intake_State = state;
	printf("Switching from %d, to %d\n", static_cast<int>(Last_Intake_State), static_cast<int>(Intake_State));
}


void Intake_Setup() {
	intake_pos = 0;
	while (!(intake_pos >= 1440-error_range && intake_pos <= 1440+error_range)){
		intake_pos = intk.get_position() - last_zero;
		intk.move_velocity(300);
		if(intake_zero.get_new_press()){
			intake_pos = 0;
			last_zero = intk.get_position();
		}
	}
}

void Reset(){
	printf("Reset\n");
	intake_pos = 0;
	last_zero = 0;
	intake.ring_count = 0;
	printf("Motor: %f\n", intake_pos);
	while (!(intake_pos >= 1440-error_range && intake_pos <= 1440+error_range)){
		intake_pos = intk.get_position() - last_zero;
		intk.move_velocity(300);
		if(intake_zero.get_new_press()){
			intake_pos = 0;
			last_zero = intk.get_position();
		}
	}
	setIntakeState(Intake_States::Searching);
}


void Intake_brake(int time){
	intk.move_relative(0, 300);
	delay(time);
	intk.move_velocity(300);
	delay(100);
}

void Intake_loop(){
	while(true){
		intake_pos = intk.get_position() - last_zero;
		if(!intake.running && Intake_State != Intake_States::Off){
			setIntakeState(Intake_States::Off);
		}
    if(intk.get_power() > 30 && intake_pos-last_pos <= 2){
      setIntakeState(Intake_States::Jammed);
    }
		switch(Intake_State){
			case Intake_States::Jammed:
				printf("Jam Detected\n");
				intk.move(0);
				intake.running = 0;
				setIntakeState(Intake_States::Off);
				break;
			case Intake_States::Off:
				intk.move(0);
				if(intake.running){
					if(Last_Intake_State != Intake_States::Holding){
            setIntakeState(Intake_States::Intaking);
          }else{
					setIntakeState(Intake_States::Holding);
          }
        }
			  break;
			case Intake_States::Intaking:
        intk.move_velocity(300);
				if(intake_zero.get_new_press()){
					intake_pos = 0;
					last_zero = intk.get_position();
				}
				if(intake.ring_count != 4 && intake.have_ring && intake_pos >= 955-error_range && intake_pos <= 955+error_range){ //has ring and wants to drop it offdrop offs rings 945.955
					Intake_brake(400);
          intake.ring_count++;
					intake.have_ring = 0;
          intake.target = 1450;
				}
        else if (intake.ring_count == 4 &&(intake_pos >= 530-error_range && intake_pos <= 530+error_range) && intake.have_ring){// has rings but mag is full
					setIntakeState(Intake_States::Holding);
				 }
				if(intake_pos >= 1440-error_range && intake_pos <= 1440+error_range){ // Will never have a ring so it is waiting for a ring
					intk.move_relative(0, 300);
					setIntakeState(Intake_States::Searching);
				}
				break;
			case Intake_States::Holding:
				intk.move(0);
				if(intake.ring_count == 0){
          intake.target = 965;
					setIntakeState(Intake_States::Intaking);
				}
				break;
			case Intake_States::Searching:
				intk.move(0);
				if(distance_intake.get() <= 25){
					intk.move_velocity(300);
					while(!(intake_pos >= 10-error_range && intake_pos <= 10+error_range) && !intake.have_ring){
						if(intake_zero.get_new_press()){
							intake_pos = 0;
							last_zero = intk.get_position();
						}
						delay(10);
					}
          intake.have_ring = 1;
          if(intake.ring_count < 4){
            intake.target = 965;
          }else{
            intake.target = 540;
          }
					setIntakeState(Intake_States::Intaking);
				}
        break;
		}
		last_pos = intk.get_position();
		//printf("Motor Pos: %f, Raw Motor Pos: %f, Ring: %d\n", intake_pos, intake.get_position(), intake.ring_count);
		delay(10);
	}
}
void On_Off_Task(){
	intake.running = !intake.running;
}
