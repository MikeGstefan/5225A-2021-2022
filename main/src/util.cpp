#include "util.hpp"

Timer screen_timer{"Screen Timer"};

int sgn(int n){
  if (n > 0)return 1;
  else if(n < 0) return -1;
  else return 0;
}
int sgn(double n){
  if (n > 0)return 1;
  else if(n < 0) return -1;
  else return 0;
}

double operator "" _deg(long double degree){
  return degree/180 *M_PI;
}
double operator "" _rad(long double radians){
  return radians/M_PI *180;
}

double deg_to_rad(double deg){
  return deg / 180 * M_PI;
}

double rad_to_deg(double rad){
  return rad / M_PI * 180;
}

double near_angle(double angle, double reference){
	return round((reference - angle) / (2 * M_PI)) * (2 * M_PI) + angle - reference;
}
