#include "printing.hpp"
#include "geometry.hpp"

std::string convert_all_args(const std::string& fmt, const std::string& arg){
  if(fmt.back() == 'p') return convert_all_args(fmt, arg.data());
  return arg;
}

std::string convert_all_args(const std::string& fmt, const Position& arg){
  return '(' + convert_all_args(fmt, arg.x) + ", " + convert_all_args(fmt, arg.y) + ", " + convert_all_args(fmt, arg.angle) + ")";
}

std::string convert_all_args(const std::string& fmt, const Point& arg){
  return '(' + convert_all_args(fmt, arg.x) + ", " + convert_all_args(fmt, arg.y) + ")";
}

std::string convert_all_args(const std::string& fmt, const Vector& arg){
  return '(' + convert_all_args(fmt, arg.get_x()) + ", " + convert_all_args(fmt, arg.get_y()) + ")";
}

//Template Recursion Base case
std::string sprintf2(const std::string& fmt){
  return fmt;
}

void newline(int count){
  std::cout << std::string (count-1 , '\n') << std::endl; //-1 because endl puts the final one
}

std::string get_term_colour(term_colours colour){
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