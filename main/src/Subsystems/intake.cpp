#include "intake.hpp"

// Intake object
Intake intake({{"Intake",
{
  "off",
  "on",
  "raised",
  "jammed",
}
}, intake_motor});

Intake::Intake(Motorized_subsystem<intake_states, NUM_OF_INTAKE_STATES, INTAKE_MAX_VELOCITY> motorized_subsystem):  Motorized_subsystem(motorized_subsystem){  // constructor
  // initiliazes states
  state = intake_states::on;
  last_state = state;

}

void Intake::handle(){

  switch(state){

    case intake_states::off: // is handled by lift state machine, can only turn on when lift is in grabbed state
      break;

    case intake_states::on:
      if(motor.get_actual_velocity() < 1.0 && bad_count <= 30) bad_count++;
      else bad_count = 0;

      if(bad_count > 30){ // if intake isn't moving for 30 cycles it reverses slowly
        motor.move(-30);
        set_state(intake_states::jammed);
      }

      break;

    case intake_states::raised: // is handlef by lift state machine, can only turn on when lift is in grabbed state
      break;

    case intake_states::jammed:
      if(master.get_digital_new_press(intake_button)){ // turns intake off when intake button is pressed
        motor.move(0);
        set_state(intake_states::off);
      }
      break;

  }

}

void Intake::toggle(){
  if(master.get_digital_new_press(intake_button)){  // toggles intake state if intake button is pressed
    if(state == intake_states::on){
      state = intake_states::off;
      motor.move(0);
    }
    else{
      state = intake_states::on;
      motor.move(INTAKE_POWER);
    }
  }
}

void Intake::raise_and_disable(){ // used by lift state machine to move intake out of the way
  motor.move(0);
  intake_piston.set_value(HIGH);
  set_state(intake_states::raised);
}
