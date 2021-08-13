#pragma once
#include "main.h"
#include "config.hpp"
#include <cmath>

void move(int x, int y, int a);
void move(double x, double y, double a);
void brake();

int sgn(int n);
int sgn(double n);

double operator "" _deg(long double degree);
double operator "" _rad(long double radians);

double rad_to_deg(double rad);
double deg_to_rad(double deg);
