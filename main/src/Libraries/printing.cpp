#include "printing.hpp"

std::string printf_to_string(const char* fmt, ...){
  char buffer[150];

  std::va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, 150, fmt, args);
  va_end(args);
  
  return buffer;
}

void newline(int count){
  std::cout << std::string (count , '\n');
}

const char* get_term_colour(term_colours colour){
  switch(colour){
    case term_colours::BLACK: return "\033[30m"; break;
    case term_colours::RED: return "\033[31m"; break;
    case term_colours::GREEN: return "\033[32m"; break;
    case term_colours::YELLOW: return "\033[33m"; break;
    case term_colours::BLUE: return "\033[34m"; break;
    case term_colours::MAGENTA: return "\033[35m"; break;
    case term_colours::CYAN: return "\033[36m"; break;
    case term_colours::WHITE: return "\033[37m"; break;
    case term_colours::NONE: return "\033[0m"; break;
    case term_colours::ERROR: return "\033[31mERROR: "; break;
    case term_colours::WARNING: return "\033[33mWARNING: "; break;
    case term_colours::NOTIF: return "\033[32mNOTIF: "; break;
  }
  return "";
}