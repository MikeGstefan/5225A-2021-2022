#include "lift.hpp"

void lift_reset(void * params){
  fbar.move(-60);
  while(fabs(fbar.get_actual_velocity()) < 5.0 )delay(10);
  int cycle_count = 0;
  while(cycle_count < 5){
    if(fabs(fbar.get_actual_velocity()) < 5.0)cycle_count++;
    else cycle_count = 0;
  }
  fbar.tare_position();
  printf("%d, fbar reset %0.2lf", millis(),fbar.get_position());
  delay(50);
  fbar.move(0);
}
