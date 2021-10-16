#pragma once
#include "main.h"
#include "config.hpp"
#include "util.hpp"
#include "Tracking.hpp"
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include <array>

using namespace pros;

// aliases to make code more readable, used to encode vales of drive.cur_driver
// enum class drivers{Nikhil = 0, Emily = 1, Sarah = 2};

#define WAIT_FOR_SCREEN_REFRESH() {\
  delay(screen_timer.get_time() < 50 ? 50 - screen_timer.get_time() : 0);\
  screen_timer.reset();\
}

class custom_drive{
  // curve_types curve_type = curve_types::polynomial; // defaults to polynomial if not specified
  int lookup_table[255];

  // curve functions
  int polynomial(int x);
  int exponential(int x);

public:
  double curvature;
  custom_drive(double curvature); // constructor
  void fill_lookup_table();
  int lookup(int x);
};

struct driver{
  const char* name;
  // 0 is strafe, 1 is forward, 2 is turn
  std::array<controller_analog_e_t, 3> joy_sticks;
  std::array<custom_drive, 3> custom_drives;
  driver(const char* name, std::array<controller_analog_e_t, 3> joy_sticks, std::array<custom_drive, 3> custom_drives);
};


// Nikhil is 0, Emily is 1, Sarah is 2

class Drivebase {
  Timer screen_timer = {"screen_timer"};
  int cur_screen;
  int deadzone = 5;
  const char* screen_text[3] = {"LOCAL_X CURVE:", "LOCAL_Y CURVE:", "LOCAL_A CURVE:"};
  void update_screen();

public:
  int cur_driver = 0;  // driver to defaults to Nikhil
  int num_of_drivers;
  FILE* curve_file;
  std::vector<driver> drivers;  // driver profiles
  Drivebase(std::vector<driver> drivers); // constructor
  void move(double x, double y, double a);
  void brake();
  void download_curve_data();
  void update_lookup_table_util();
  void handle_input();
  void driver_practice();
};

extern Drivebase drivebase;
