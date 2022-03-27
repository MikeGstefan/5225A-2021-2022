#include "timer.hpp"

Timer::Timer(const char* name, const bool& play, timing_units timing_unit, Data* data_obj_ptr):
name(name), timing_unit(timing_unit)
{
  if(data_obj_ptr == nullptr) this->data_obj = &term;
  else this->data_obj = data_obj_ptr;
  data_obj->print("%s's initialize time is: %lld\n", name, get_time_in_timing_unit());
  reset(play);
}

Timer::Timer(const char* name, Data* data_obj_ptr,const bool& play, timing_units timing_unit):
name(name), timing_unit(timing_unit)
{
  if(data_obj_ptr == nullptr) this->data_obj = &term;
  else this->data_obj = data_obj_ptr;
  data_obj->print("%s's initialize time is: %lld\n", name, get_time_in_timing_unit());
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
  else paused = true;
}

uint64_t Timer::get_time(){
  if (paused) return time;
  else return get_time_in_timing_unit() - last_play_time + time;
}

void Timer::play(){
  if (paused){
    last_play_time = get_time_in_timing_unit();
    paused = false;
  }
  else data_obj->print("Timer \"%s\" is already playing.\n", name);
}

void Timer::pause(){
  if (!paused){
    time += get_time_in_timing_unit() - last_play_time;
    paused = true;
  }
  else data_obj->print("Timer \"%s\" is already paused.\n", name);
}

void Timer::print(){
  data_obj->print("%s's current time is: %lld\n", name, get_time());
}

void Timer::print(const std::string str){
  data_obj->print("%s's current time is: %lld | %s\n", name, get_time(), str.c_str());
}

void Timer::print(const char* fmt, ...){
  va_list args;
  va_start(args, fmt);
  char buffer[100];
  vsnprintf(buffer, 100, fmt, args);
  va_end(args);
  data_obj->print("%s's current time is: %lld | %s\n", name, get_time(), buffer);
}

void Timer::print_fancy(std::string str, int long_names, bool unit_conversion){
  data_obj->print("%s's current time is: %s | %s\n", name, to_string(get_time(), timing_unit, long_names, unit_conversion).c_str(), str.c_str());
}


uint64_t Timer::get_time_in_timing_unit(){ // returns time in either millis or micros
  return pros::micros() * (timing_unit == timing_units::micros ? 1 : (timing_unit == timing_units::millis ? 0.001 : 0.000001));
}

bool Timer::playing(){
  return !paused;
}

std::string Timer::to_string(std::uint64_t time, timing_units unit, int long_names, bool unit_conversion){
  const char* millis;
  const char* micros;
  const char* sec;
  const char* min;
  const char* plural;

  if (long_names==0){
    millis = "ms";
    micros = "us";
    sec = "s";
    min = "m";
    plural = "";
  }
  else if (long_names==1){
    millis = " millis";
    micros = " micros";
    sec = " sec";
    min = " min";
    plural = "";
  }
  else if (long_names==2){
    millis = " millisecond";
    micros = " microsecond";
    sec = " second";
    min = " minute";
    plural = "s";
  }

  std::string buffer;

  //Prefixes with large unit. (e.g. 200s -> 3m 20s)
  if(unit_conversion){

    if(time >= 1000 && unit == timing_units::micros){
      std::uint64_t milliseconds = time/1000;
      time -= milliseconds*1000;

      if(milliseconds >= 1000) buffer += to_string(milliseconds, timing_units::millis, long_names, true) + ' ';
      else buffer += std::to_string(milliseconds) + millis + ' ';
    }

    else if(time >= 1000 && unit == timing_units::millis){
      std::uint64_t seconds = time/1000;
      time -= seconds*1000;

      if(seconds >= 60) buffer += to_string(seconds, timing_units::seconds, long_names, true) + ' ';
      else buffer += std::to_string(seconds) + sec + ' ';
    }

    else if(time >= 60 && unit == timing_units::seconds){
      std::uint64_t minutes = time/60;
      time -= minutes*60;

      buffer += std::to_string(minutes) + min + ' ';
    }

  }

  //Writes value
  if(time){
    buffer += std::to_string(time);
    //Writes unit
    switch(unit){
      case timing_units::micros: buffer += micros; break;
      case timing_units::millis: buffer += millis; break;
      case timing_units::seconds: buffer += sec; break;
    }
  }

  if(buffer.back() == ' ') buffer.pop_back();
  return buffer;
}