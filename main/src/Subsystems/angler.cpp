#include "angler.hpp"

angler_states angler_state = angler_states::searching;
angler_states last_angler_state = angler_state;

int angler_encoder_position;

const int angler_bottom_position = 20;
const int angler_raised_position = 200;
const int angler_top_position = 300;

std::array<const char*, NUM_OF_ANGLER_STATES> angler_state_names = {
  "lowered",
  "searching",
  "raising",
  "lowering"
};

void set_angler_state(angler_states next_state){
  printf("Angler | Going from %s to %s\n", angler_state_names[static_cast<int>(angler_state)], angler_state_names[static_cast<int>(next_state)]);
  last_angler_state = angler_state;
  angler_state = next_state;
}

void angler_handle(){
  switch(angler_state){
    case angler_states::lowered:
      if(angler_dist.get() < 100){  // throws into searching state when distance sensor get triggered
        angler_encoder_position = LeftEncoder.get_value()/360.0 *(2.75*M_PI);
        set_angler_state(angler_states::searching);
      }
      if(master.get_digital_new_press(angler_button)){  // grabs goal and raises angler when angler_button is pressed
        angler_motor.move_absolute(angler_raised_position, 100);
        angler_piston.set_value(HIGH);
        set_angler_state(angler_states::raised);
      }
      break;
    case angler_states::searching:
      // waits until robot thinks it hits the branch to grab the goal
      if(LeftEncoder.get_value()/360.0 *(2.75*M_PI) - angler_encoder_position > 2.0){
        angler_motor.move_absolute(angler_raised_position, 100);
        angler_piston.set_value(HIGH);
        set_angler_state(angler_states::raised);
      }
      if(master.get_digital_new_press(angler_button)){  // cancels search when angler_button is pressed
        set_angler_state(angler_states::lowered);
      }
      break;

    case angler_states::raised:
      if(master.get_digital_new_press(angler_button)){  // lowers angler to bottom when angler_button is pressed
        angler_motor.move_absolute(angler_bottom_position, 100);
        set_angler_state(angler_states::lowering);
      }
      break;

    case angler_states::lowering:
      if(master.get_digital_new_press(angler_button)){  // raises angler when angler_button is pressed
        angler_motor.move_absolute(angler_raised_position, 100);
        set_angler_state(angler_states::raised);
      }
      if(fabs(angler_motor.get_position() - angler_bottom_position) < 25){  // releases goal once angler reaches bottom
        angler_piston.set_value(LOW);
        set_angler_state(angler_states::searching);
      }
      break;
  }
}

void angler_reset(){
  angler_motor.move(-60);
  Timer vel_rise_timeout("vel_rise");
  // waits for motor's velocity to rise or timeout to trigger
  while(fabs(angler_motor.get_actual_velocity()) < 45.0){
    printf("Angler's velocity is (rising loop): %lf\n", angler_motor.get_actual_velocity());
    if (vel_rise_timeout.get_time() > 50){
      printf("Angler's rising loop timed out\n");
      break;
    }
    delay(10);
  }
  printf("Angler's velocity done rising\n");
  // waits until motors velocity slows down for 5 cycles
  cycleCheck(fabs(angler_motor.get_actual_velocity()) < 5.0, 5, 10)
  angler_motor.tare_position();  // resets subsystems position
  printf("%d, angler's reset %lf\n", millis(), angler_motor.get_position());
  angler_motor.move(0);
}
