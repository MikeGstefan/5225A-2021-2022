#include "util.hpp"


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

std::string printf_to_string(const char* fmt, va_list arg){
  char buffer[150];
  vsnprintf(buffer,150,fmt,arg);
  return std::string(buffer);
}

//Returns a heap-allocated c-string. Call delete[] after usage
char* const millis_to_str(std::uint32_t milliseconds){
  char* const buffer = new char[20];
  if (milliseconds == 1) sprintf(buffer, "1 millisecond");
  else if (milliseconds < 1000) sprintf(buffer, "%d milliseconds", milliseconds);
  else if (milliseconds == 1000) sprintf(buffer, "1 second");
  else sprintf(buffer, "%d seconds", milliseconds/1000);

  return buffer;
}

std::function<long double (long double)> func_scale(std::function<long double(long double)> f, Point p1, Point p2, double control){
  double denom = f(p2.x-control)-f(p1.x-control);
  double a = (p2.y-p1.y)/(denom);
  double b = (p1.y*f(p2.x-control)-p2.y*f(p1.x-control))/denom;

  return [=](long double inp){return a*f(inp-control)+b;};
}