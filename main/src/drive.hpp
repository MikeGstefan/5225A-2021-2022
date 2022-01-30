#pragma once
#include "Tracking.hpp"
#include "util.hpp"
#include "logging.hpp"
#include "Subsystems/lift.hpp"
#include "Subsystems/tilter.hpp"
#include "auton_util.hpp"

using namespace pros;

// aliases to make code more readable, used to encode vales of drive.cur_driver
// enum class drivers{Nikhil = 0, Emily = 1, Sarah = 2};

// wait until at least 50 ms since the last reset
// #define WAIT_FOR_SCREEN_REFRESH() {\
//   delay(drivebase.screen_timer.get_time() < 50 ? 50 - drivebase.screen_timer.get_time() : 0);\
//   drivebase.screen_timer.reset();\
// }

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

class Drivebase{
  bool reversed; // if false forwards is the intake side
  int cur_screen;
  int deadzone = 5;
  const char* screen_text[3] = {"LOCAL_X CURVE:", "LOCAL_Y CURVE:", "LOCAL_A CURVE:"};
  void update_screen();

public:
  int cur_driver = 0;  // driver defaults to Nikhil rn
  Timer screen_timer = {"screen_timer"};
  static constexpr int num_of_drivers = 5;
  FILE* curve_file;
  bool curve_file_exists;
  std::array<driver, num_of_drivers> drivers;  // driver profiles
  Drivebase(std::array<driver, num_of_drivers> drivers); // constructor

  // 'set-drive' methods
  void move(double x, double y, double a);
  void move(double y, double a);
  void move_tank(double y, double a);
  void move_side(double l, double r);

  void brake();
  void download_curve_data(); // grabs data from SD card and copies to driver arrays
  void update_lookup_table_util();  // utility to alter expo curves for any driver
  void handle_input();  // move the drivebase according to lookup tables from a joystick input
  void driver_practice(); // method to let drivers drive and change driver profiles
  void non_blocking_driver_practice(); // method to let drivers drive and change driver profiles to be called in loop
  void next_driver(); //Goes to next driver. Called on drivebase object.
  void prev_driver(); //Goes to previous driver. Called on drivebase object.
  const char* driver_name(); //Returns the current driver's name
};

extern Drivebase drivebase;
