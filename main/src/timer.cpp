#include "timer.hpp"

Timer::Timer(const char* name, const bool& play, timing_units timing_unit, Data* data_obj_ptr):
name(name), timing_unit(timing_unit)
{
    if(data_obj_ptr == nullptr)this->data_obj = &term;
    else this->data_obj = data_obj_ptr;
    data_obj->print("%s's initialize time is: %lld\n", name, get_time_in_timing_unit());
    reset(play);
}

Timer::Timer(const char* name, Data* data_obj_ptr,const bool& play, timing_units timing_unit):
name(name), timing_unit(timing_unit)
{
    if(data_obj_ptr == nullptr)this->data_obj = &term;
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
    else    data_obj->print("Timer \"%s\" is already playing.\n", name);
}

void Timer::pause(){
    if (!paused){
        time += get_time_in_timing_unit() - last_play_time;
        paused = true;
    }
    else    data_obj->print("Timer \"%s\" is already paused.\n", name);
}

void Timer::print(const char* str){
    data_obj->print("%s's current time is: %lld | %s\n", name, get_time(), str);
}

uint64_t Timer::get_time_in_timing_unit(){ // returns time in either millis or micros
    return pros::micros() * (timing_unit == timing_units::micros ? 1 : 0.001);
}

bool Timer::playing(){
  return !paused;
}
