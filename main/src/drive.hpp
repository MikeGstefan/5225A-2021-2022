#pragma once
#include "main.h"
#include "config.hpp"
#include "util.hpp"
#include "tracking.hpp"
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include <array>

using namespace pros;

// aliases to make code more readable, used to encode vales of drive.cur_driver
// enum class drivers{Nikhil = 0, Emily = 1, Sarah = 2};

// wait until at least 50 ms since the last reset
#define WAIT_FOR_SCREEN_REFRESH() {\
  delay(drivebase.screen_timer.get_time() < 50 ? 50 - drivebase.screen_timer.get_time() : 0);\
  drivebase.screen_timer.reset();\
}

class custom_drive{
  int lookup_table[255];
  // curve functions
  int polynomial(int x);  // transforms a linear input to a polynomially tranformed output
  int exponential(int x);  // transforms a linear input to a exponentially tranformed output

public:
  double curvature;
  custom_drive(double curvature); // constructor
  void fill_lookup_table(); // transforms linear mapping to exponential mapping for a singular axis
  int lookup(int x);  // returns transformed value from lookup table, taking in an x input
};

struct driver{
  const char* name;
  // 0 is strafe, 1 is forward, 2 is turn
  std::array<controller_analog_e_t, 3> joy_sticks;
  std::array<custom_drive, 3> custom_drives;
  driver(const char* name, std::array<controller_analog_e_t, 3> joy_sticks, std::array<custom_drive, 3> custom_drives); // constructor
};


// Nikhil is 0, Emily is 1, Sarah is 2

class Drivebase {
  int cur_screen;
  int deadzone = 5;
  const char* screen_text[3] = {"LOCAL_X CURVE:", "LOCAL_Y CURVE:", "LOCAL_A CURVE:"};
  void update_screen();

public:
    Timer screen_timer = {"screen_timer"};
  int cur_driver = 0;  // driver to defaults to Nikhil
  static constexpr int num_of_drivers = 5;
  FILE* curve_file;
  bool curve_file_exists;
  std::array<driver, num_of_drivers> drivers;  // driver profiles
  Drivebase(std::array<driver, num_of_drivers> drivers); // constructor
  void move(double x, double y, double a);  // sets the power for each drive motor based on x, y and angular power
  void brake();
  void download_curve_data(); // grabs data from SD card and copies to driver arrays
  void update_lookup_table_util();  // utility to alter expo curves for any driver
  void handle_input();  // move the drivebase according to lookup tables from a joystick input
  void driver_practice(); // method to let drivers drive and change driver profiles
};

extern Drivebase drivebase;
