#include "intake.hpp"

void full(){
  intk.move(127);
}

void half(){
  intk.move(63);
}

void dead(){
  intk.move(0);
}
