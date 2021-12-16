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


// template <typename state_type, int num_of_states>
// Motorized_subsystem::Motorized_subsystem(Subsystem<state_type, num_of_states> subsystem, Motor motor):
//   Subsystem(subsystem), Motor(motor){}



// template <typename state_type, int num_of_states>
// void Motorized_subsystem::reset(){
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
// }

// template <typename state_type, int num_of_states>
// Motorized_subsystem::Motorized_subsystem(Subsystem<state_type, num_of_states> subsystem, Motor motor):
//   Subsystem(subsystem), Motor(motor){}


// Example of Subsystem instantiation
enum class example{
  example1,
  example2,
  example3,
  example4,
  example5
};

Subsystem<example, 5> example_subsystem("example", {
  "example1",
  "example2",
  "example3",
  "example4",
  "example5"
});

// Example of Motorized_subsystem instantiation

Motorized_subsystem<example, 5> example_motorized_subsystem(example_subsystem, c_bar);
Motorized_subsystem<example, 6> example_motorized_subsystem_2(Subsystem<example, 6>("example", {
  "example1",
  "example2",
  "example3",
  "example4",
  "example5",
  "example6"
}), c_bar);

// Example of Six_bar object(child of motorized subsystem class)

// constructor
Six_bar::Six_bar(Motorized_subsystem<six_bar_states, 6> motorized_subsystem): Motorized_subsystem(motorized_subsystem)
{}

// example of polymorphism
void Six_bar::handle(){
  // state_machine stuff

}
// example of instantiation of child class of motorized subsystem
Six_bar six_bar(Motorized_subsystem<six_bar_states, 6>(Subsystem<six_bar_states, 6>("Six_bar", {  // this line is causing the error
"searching",
"lowered",
"raised",
"platform",
"released",
"lowering"}
), f_bar));
