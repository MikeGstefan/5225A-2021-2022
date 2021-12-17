#include "../Libraries/subsystem.hpp"

enum class hitch_states{
  searching,  // released but waiting for goal
  released, // released but not waiting for goal
  grabbed // has goal
};

class Hitch: public Subsystem<hitch_states, 3> {

public:
  Hitch(Subsystem<hitch_states, 3> subsystem);  // constructor
  void handle();  // contains state machine code

};

extern Hitch hitch;
