#include "subsystem.hpp"

// Subsystem::Subsystem(Motor& motor, const char* name, const char* state_names[]): Motor(motor), name(name), state_names(state_names){}
//
// void Subsystem::handle(){}
//
// void Subsystem::set_state(const states next_state){
//   printf("Lift | Going from %s to %s\n", state_names[state], state_names[next_state]);
//   last_state = state;
//   state = next_state;
// }
//
//
// void Subsystem::reset(){
//   move(-60);
//   Timer vel_rise_timeout("vel_rise");
//   // waits for motor's velocity to rise or timeout to trigger
//   while(fabs(get_actual_velocity()) < 45.0){
//     printf("%s's velocity is (rising loop): %lf\n", name, get_actual_velocity());
//     if (vel_rise_timeout.get_time() > 50){
//       printf("%s's rising loop timed out\n", name);
//       break;
//     }
//     delay(10);
//   }
//   printf("%s's velocity done rising\n", name);
//   // waits until motors velocity slows down for 5 cycles
//   cycleCheck(fabs(get_actual_velocity()) < 5.0, 5, 10)
//   tare_position();  // resets subsystems position
//   printf("%d, %s's reset %lf\n", millis(), name, get_position());
//   move(0);
//
// }


// example of Subsystem object
  /*
  Subsystem<six_bar_states, true, 6> example(f_bar, "Six_bar", {
  "searching",
  "lowered",
  "raised",
  "platform",
  "released",
  "lowering"}
  );
  */

// Example of Six_bar object(child of subsystem class)

  // constructor
  Six_bar::Six_bar(Subsystem<six_bar_states, true, 6> subsystem): Subsystem(subsystem)
  {}

  // example of polymorphism
  void Six_bar::handle(){


  }
  // actual object
  /*
  Six_bar six_bar = Six_bar(Subsystem<six_bar_states, true, 6>(f_bar, "six_bar",
  std::array<const char*, 6>{
  "searching",
  "lowered",
  "raised",
  "platform",
  "released",
  "lowering"}
  ));
  */


  Six_bar six_bar(Subsystem<six_bar_states, true, 6>(f_bar, "Six_bar", {  // this line is causing the error
  "searching",
  "lowered",
  "raised",
  "platform",
  "released",
  "lowering"}
  ));
