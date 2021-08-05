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
        void print(const char* format, ...);
        Data(Levels data_level){
            level = data_level;
        }
        void logging(string output);
};
void print(const char* format, ...);
const int queue_size = 10240;
