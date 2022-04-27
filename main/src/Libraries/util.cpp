#include "util.hpp"

bool random_bool(int ratio){
  return millis() % (ratio + 1);
}

int random_direction(int ratio){
  return bool2sgn(random_bool());
}

std::function<long double (long double) > func_scale(std::function<long double(long double) > f, Point p1, Point p2, double control){
  long double f1 = f(p1.x + control);
  long double f2 = f(p2.x + control);

  return [=](long double inp){return ((p2.y-p1.y) * f(inp + control) + f2 * p1.y - f1 * p2.y) / (f2-f1);};
}