#include "intake.hpp"

Task *IntakeTask = nullptr;
double intake_pos = 10;
double last_zero = intk.get_position();
uint32_t time_started = millis();
Intake_States Intake_State = Intake_States::Off;
Intake_States Last_Intake_State = Intake_State;
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


//Top Left: 700 Top Right: 900, Bottom: 1600
void Reset(){
  IntakeTask->remove();
  delete IntakeTask;
  IntakeTask = nullptr;
	printf("Reset\n");
	last_zero = 0;
	intake.ring_count = 0;
  while(!(intake_zero.get_new_press())){
    printf("Pos: %f,Raw Pos: %f\n", intake_pos, intk.get_position());
    intk.move(80);
    intake_pos = intk.get_position() - last_zero;
    delay(10);
  } //0.8s for one
  intake_pos = 0;
  last_zero = intk.get_position();
	while (intake_pos < 1000){
    intk.move(80);
		intake_pos = intk.get_position() - last_zero;
    if(intake_zero.get_new_press()){
      printf("Here Part 2\n");
      intake_pos = 0;
      last_zero = intk.get_position();
    }
    delay(10);
	}
  intake.running = 1;
  intake.StartInTask();
	setIntakeState(Intake_States::Searching);
}

void Intake_brake(int time){
	intk.move_relative(0, 600);
	delay(time);
	intk.move_velocity(600);
}

void Intake_loop(){
	while(true){
		intake_pos = intk.get_position() - last_zero;
    //printf("Pos: %f Speed: %f\n", intake_pos, intk.get_actual_velocity());
		if(!intake.running && Intake_State != Intake_States::Off){
			setIntakeState(Intake_States::Off);
		}
    if(intk.get_current_draw() > 3600){
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
				//intk.move(0);
				if(intake.running){
					if(Last_Intake_State != Intake_States::Holding){
            setIntakeState(Intake_States::Fast);
          }else{
            setIntakeState(Intake_States::Holding);
          }
        }
			  break;
			case Intake_States::Fast:
        intk.move(75);
				if(intake_zero.get_new_press()){
					intake_pos = 0;
					last_zero = intk.get_position();
				}
        if(intake_pos > 1200){
            setIntakeState(Intake_States::Searching);
          }
        if (intake.ring_count == 5 && intake_pos > 500){// has rings but mag is full
					setIntakeState(Intake_States::Holding);
        }
				break;
			case Intake_States::Holding:
				intk.move(0);
				if(intake.ring_count == 0){
					setIntakeState(Intake_States::DropOff);
				}
				break;
      case Intake_States::DropOff:
        if(intake.ring_count < 4 && intake_pos >= 900){ //has ring and wants to drop it offdrop offs rings 945.955
          Intake_brake(200);
          intake.ring_count++;
          intake.have_ring = 0;
          setIntakeState(Intake_States::Fast);
        }
        break;
			case Intake_States::Searching:
        if(intake_pos >= 1350){
          intk.move(0);
  				if(distance_intake.get() <= 29){
  					intk.move(75);
  					while(!(intake_pos < 10)){
  						if(intake_zero.get_new_press()){
  							intake_pos = 0;
  							last_zero = intk.get_position();
  						}
  						delay(10);
  					}
  					setIntakeState(Intake_States::Fast);
  				}
        }
        break;
		}
		//printf("Motor Pos: %f, Raw Motor Pos: %f, Ring: %d\n", intake_pos, intake.get_position(), intake.ring_count);
		delay(10);
	}
}
void On_Off_Task(){
	intake.running = !intake.running;
}
