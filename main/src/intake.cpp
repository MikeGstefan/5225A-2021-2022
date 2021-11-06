#include "intake.hpp"

Task *IntakeTask = nullptr;
double intake_pos = 10;
double last_zero = intk.get_position();
double last_pos = 0;
uint32_t time_started = millis();
Intake_States Intake_State = Intake_States::Off;
Intake_States Last_Intake_State = Intake_State;
int error_range = 20;
Intake intake;
double error;
double target;

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
	while (!(fabs(intake_pos-1440) <= error_range)){
		intake_pos = intk.get_position() - last_zero;
		intk.move_velocity(600);
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
	while (!(intake_pos > 1440)){
    printf("Motor: %f\n", intake_pos);
    intk.move(127);
    printf("Power: %f\n", intk.get_power());
		intake_pos = intk.get_position() - last_zero;
		if(intake_zero.get_new_press()){
			intake_pos = 0;
			last_zero = intk.get_position();
		}
    delay(20);
	}
  intake.running = 1;
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
    // if(intk.get_power() > 30 && intake_pos-last_pos <= 2){
    //   setIntakeState(Intake_States::Jammed);
    // }
    if(intk.get_current_draw() > 3300){
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
            target = 700;
          }else{
            setIntakeState(Intake_States::Holding);
          }
        }
			  break;
			case Intake_States::Fast:
        error = target - intake_pos;
        if((1/200)*error*error+50 < 500){
          intk.move_velocity(500);
        }else{
          intk.move_velocity((1/200)*error*error+50);
        }
				if(intake_zero.get_new_press()){
					intake_pos = 0;
					last_zero = intk.get_position();
				}
        if(Last_Intake_State == Intake_States::Searching || Last_Intake_State == Intake_States::Off){
          if(intake_pos >= 900){
            intk.move_velocity(75);
            delay(50);
            setIntakeState(Intake_States::DropOff);
          }
        }
        if(Last_Intake_State  == Intake_States::DropOff){
          if(intake_pos > 1300){
            intk.move_velocity(200);
            setIntakeState(Intake_States::Searching);
          }
        }
        if (intake.ring_count == 4 && intake_pos > 500){// has rings but mag is full
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
        if(intake.ring_count < 4 && intake_pos >= 950){ //has ring and wants to drop it offdrop offs rings 945.955
          Intake_brake(200);
          intake.ring_count++;
          intake.have_ring = 0;
          setIntakeState(Intake_States::Fast);
          target = 1300;
        }
        break;
			case Intake_States::Searching:
        if(intake_pos >= 1350){
          intk.move(0);
  				if(distance_intake.get() <= 24){
  					intk.move_velocity(400);
  					while(!(intake_pos < 10)){
  						if(intake_zero.get_new_press()){
  							intake_pos = 0;
  							last_zero = intk.get_position();
  						}
  						delay(10);
  					}
  					setIntakeState(Intake_States::Fast);
            target = 700;
  				}
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
