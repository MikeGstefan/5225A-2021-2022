#include "subsystem.hpp"

/*
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
*/
