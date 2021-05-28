#pragma once
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


enum class Levels{
    both,
    card,
    wire,
    none
};

class Data{
    public:
        Levels level;
        string file;
        void print(const char* format, ...);
        Data(Levels data_level, string file_path){
            level = data_level;
            file = file_path;
        }
        void logging(string output);
};

void print_task();


void L_print();
void printStopTask();

const int queue_size = 10;



extern Data test;
