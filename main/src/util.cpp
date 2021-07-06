#include "util.hpp"

void move(int x, int y, int a){
  front_l.move(-x + y + a);
  front_r.move(-x+y-a);
  back_l.move(x+y+a);
  back_l.move(x+y-a);
}
void move(double x, double y, double a){
  front_l.move(-x + y + a);
  front_r.move(-x+y-a);
  back_l.move(x+y+a);
  back_l.move(x+y-a);
}
