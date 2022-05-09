#include "util.hpp"

double operator "" _deg(long double degree){
  return deg_to_rad(degree);
}
double operator "" _rad(long double radians){
  return rad_to_deg(radians);
}

double operator "" _rot(long double rotations){
  return rotations * M_TWOPI;
}

double deg_to_rad(double deg){
  return deg / 180.0 * M_PI;
}

double rad_to_deg(double rad){
  return rad / M_PI * 180.0;
}

double near_angle(double angle, double reference){
	return round((reference - angle)/(M_TWOPI)) * (M_TWOPI) + angle - reference;
}

double weighted_avg(double first, double second, double first_scale){
  return first*first_scale + second*(1-first_scale);
}

int random_direction(){
  return std::rand()-RAND_MAX/2 > 0 ? 1 : -1;
}

std::function<long double (long double)> func_scale(std::function<long double(long double)> f, Point p1, Point p2, double control){
  long double f1 = f(p1.x+control);
  long double f2 = f(p2.x+control);

  return [=](long double inp){return ((p2.y-p1.y) * f(inp+control) + f2*p1.y - f1*p2.y) / (f2-f1);};
}