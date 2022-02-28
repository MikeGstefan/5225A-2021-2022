#pragma once
#include <array>
#include "drive.hpp"
#include "config.hpp"
#include "util.hpp"
#include "main.h"

using namespace std;
using namespace pros;

bool get_lift();

void f_lift_inc();
void f_lift_dec();
void b_lift_inc();
void b_lift_dec();

void handle_lifts();

void lift_pid_set_target(int target);
void lift_pid(void* params);

extern _Task lift;