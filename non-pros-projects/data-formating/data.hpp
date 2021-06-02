#include <iostream>
#include <vector>
#include <string>
#include <cstdarg>

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
};
