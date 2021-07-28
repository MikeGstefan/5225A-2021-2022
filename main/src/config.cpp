#include "config.hpp"

pros::Controller master(pros::E_CONTROLLER_MASTER);
pros::Motor front_l(1,true), front_r(2,false), back_l(3,true),back_r(4,false);
pros::Motor fbar(5,false);
pros::ADIEncoder LeftEncoder(1,2,false), RightEncoder(3,4,true), BackEncoder(5,6,false);
pros::ADIDigitalIn top(7), btm(8);
