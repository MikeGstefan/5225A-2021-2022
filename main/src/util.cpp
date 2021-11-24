#include "util.hpp"

// void set_claw_state(claw_state state){
//   switch(state){
//     case claw_state::in:
//       claw_out.set_value(0);
//       claw_in.set_value(1);
//     break;
//     case claw_state::out:
//       claw_out.set_value(1);
//       claw_in.set_value(0);
//     break;
//     case claw_state::neut:
//       claw_out.set_value(0);
//       claw_in.set_value(0);
//     break;
//   }
// }
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
