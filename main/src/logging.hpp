#pragma once
#include "util.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cstdarg>
#include <array>
#include <fstream>
#include "main.h"

using namespace std;
using namespace pros;

extern Task *printing;



void logging_task_start();
void logging_task_stop();


void log_print(const char* format, ...);
void queue_handle(void* params);
uintptr_t data_size();
const int queue_size = 100;
const int print_point = 80;
const int print_max_time = 2000;
