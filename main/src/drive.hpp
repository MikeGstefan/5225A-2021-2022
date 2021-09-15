#pragma once
#include "main.h"
#include "config.hpp"
#include "util.hpp"
#include "Tracking.hpp"
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>

// Emily  & Sarah: forward and strafing on left, and right is turning

// #define JOY_FORWARD E_CONTROLLER_ANALOG_LEFT_Y
// #define JOY_STRAFE E_CONTROLLER_ANALOG_LEFT_X
// #define JOY_TURN E_CONTROLLER_ANALOG_RIGHT_X
// #define JOY_LIFT E_CONTROLLER_ANALOG_RIGHT_Y
// #define JOY_FORWARD E_CONTROLLER_ANALOG_LEFT_Y
// #define JOY_STRAFE E_CONTROLLER_ANALOG_RIGHT_X
// #define JOY_TURN E_CONTROLLER_ANALOG_LEFT_X
// #define JOY_LIFT E_CONTROLLER_ANALOG_RIGHT_Y
// #define DEADZONE 7

struct driver{
  // controller_digital_e_t  strafe_stick = ;
  controller_analog_e_t  JOY_STRAFE, JOY_FORWARD, JOY_TURN;

};


void drive_input();
void update_lookup_table_util();

enum class curve_types {polynomial, exponential};
class custom_drive{
  curve_types curve_type = curve_types::polynomial; // deafults to polynomial if not specified
  double curvature;
  int lookup_table[255];

  // curve functions
  int polynomial(int x);
  int exponential(int x);

public:
  static void custom_curve_init();
  void update_curve_values(curve_types curve_type, double curvature);
  void fill_lookup_table();
  int lookup(int x);
};

extern custom_drive custom_drives[3];
