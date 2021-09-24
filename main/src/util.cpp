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

// timing class functions

// Timer::Timer(const char* name): name(name){ // constructor
//     printf("%s's initialize time is: %d\n", name, pros::millis());
//     reset();
// }
//
// uint32_t Timer::get_last_reset_time(){
//     return last_reset_time;
// }
//
// uint32_t Timer::get_time(){
//     return pros::millis() - last_reset_time;
// }
//
// void Timer::print(){
//     printf("%s's current time is: %d\n", name, pros::millis() - last_reset_time);
// }
//
// void Timer::reset(bool print_time){
//     last_reset_time = millis();
//     if (print_time) printf("%s's reset time is: %d\n", name, pros::millis());
// }

Timer::Timer(const char* name, const bool& play, timing_units timing_unit):
name(name), timing_unit(timing_unit)
{
    printf("%s's initialize time is: %lld\n", name, get_time_in_timing_unit());
    reset(play);
}

uint64_t Timer::get_last_reset_time(){
    return last_reset_time;
}

void Timer::reset(const bool& play){
    time = 0;
    if(play){
        paused = true;
        this->play();
    }
    else    paused = true;
}

uint64_t Timer::get_time(){
    if (paused)    return time;
    else    return get_time_in_timing_unit() - last_play_time + time;
}

void Timer::play(){
    if (paused){
        last_play_time = get_time_in_timing_unit();
        paused = false;
    }
    else    printf("Timer \"%s\" is already playing.\n", name);
}

void Timer::pause(){
    if (!paused){
        time += get_time_in_timing_unit() - last_play_time;
        paused = true;
    }
    else    printf("Timer \"%s\" is already paused.\n", name);
}

void Timer::print(const char* str){
    printf("%s's current time is: %lld | %s\n", name, get_time(), str);
}

uint64_t Timer::get_time_in_timing_unit(){ // returns time in either millis or micros
    return pros::micros() * (timing_unit == timing_units::micros ? 1 : 0.001);
}


// Coord constructor definition
Coord::Coord(double x, double y, double angle): x(x), y(y), angle(angle){}
Coord::Coord(): x(0.0), y(0.0), angle(0.0){}
