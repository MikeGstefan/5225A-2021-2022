// NEW CONFIG
#pragma once
#include "main.h"
#include "controller.hpp"

class _Controller;
class Text;

extern _Controller master;
extern pros::Motor front_l, front_r, back_l, back_r, intake_motor, lift_motor, tilter_motor, spinner_motor;
extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;
extern pros::ADIDigitalOut claw, claw_out;
extern pros::ADIDigitalIn claw_touch;
extern pros::ADIDigitalIn lift_trigger, tilter_trigger, hitch_trigger;
extern pros::ADIDigitalOut lift_piston, hitch_piston;
extern pros::Distance tilter_dist;
extern pros::ADIDigitalOut tilter_bottom_piston;
extern pros::ADIDigitalOut tilter_top_piston;

extern pros::ADIDigitalOut intake_piston;

extern pros::ADIDigitalIn spinner_trigger;
extern pros::ADIDigitalOut ring_piston; // "end effector"


extern std::array<std::tuple<pros::Motor*, int, std::string, const char*, Text*>, 8> motors;
