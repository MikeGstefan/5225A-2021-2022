#include "util.hpp"


Timer::Timer(const char* name): name(name){ // constructor
    printf("%s's initialize time is: %d\n", name, pros::millis());
    reset();
}

uint32_t Timer::get_last_reset_time(){
    return last_reset_time;
}

uint32_t Timer::get_time(){
    return pros::millis() - last_reset_time;
}

void Timer::print(){
    printf("%s's current time is: %d\n", name, pros::millis() - last_reset_time);
}

void Timer::reset(){

    last_reset_time = millis();
    printf("%s's initialize time is: %d\n", name, pros::millis());
}
