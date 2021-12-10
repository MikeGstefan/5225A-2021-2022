#include "util.hpp"
#include "timer.hpp"

using namespace pros;

class Subsystem: public Motor {
public:
  const char* name;
  Subsystem(Motor& motor, const char* name);  // constructor
  void handle();  // has a switch containg the state machine for a given subsystem
  void reset();
};
