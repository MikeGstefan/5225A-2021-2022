#pragma once
#include "main.h"
#include "config.hpp"
#include "pid.hpp"
#include "drive.hpp"

bool get_lift();

void f_lift_inc();
void f_lift_dec();
void b_lift_inc();
void b_lift_dec();

void handle_lifts();
void intk_c(void* params);
