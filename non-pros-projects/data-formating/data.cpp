#include "data.hpp"

Data tracking(Levels::wire);
Data pos(Levels::both);

void Data::print(const char* format, ...){
    char output[256];
    va_list args;
    va_start(args, format);
    vsprintf(output, format, args);
    switch(this->level){
        case Levels::both:
            printf("%s\n",output);
        break;
        case Levels::wire:
            printf("%s\n",output);
        break;
        case Levels::card:

        break;
        case Levels::none:

        break;
    }
}

int main()
{   
    int pain = 36;
    tracking.print("testing %d",pain);
    // pos.print();
}