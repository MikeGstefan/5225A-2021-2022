#include "subsystem.hpp"

Subsystem::Subsystem(Motor& motor, const char* name): Motor(motor), name(name){}

void Subsystem::handle(){}

void Subsystem::reset(){
  move(-60);
  Timer vel_rise_timeout("vel_rise");
  // waits for motor's velocity to rise or timeout to trigger
  while(fabs(get_actual_velocity()) < 45.0){
    printf("%s's velocity is (rising loop): %lf\n", name, get_actual_velocity());
    if (vel_rise_timeout.get_time() > 50){
      printf("%s's rising loop timed out\n", name);
      break;
    }
    delay(10);
  }
  printf("%s's velocity done rising\n", name);
  // waits until motors velocity slows down for 5 cycles
  cycleCheck(fabs(get_actual_velocity()) < 5.0, 5, 10)
  tare_position();  // resets subsystems position
  printf("%d, %s's reset %lf\n", millis(), name, get_position());
  move(0);

}
