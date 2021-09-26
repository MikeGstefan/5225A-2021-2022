#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstdarg>
#include <array>
#include <fstream>
#include <cmath>
#include <memory>
#include <vector>
#include <map>
#include "main.h"
#include "config.hpp"
#include "util.hpp"
#include "logging.hpp"
#include "config.hpp"

class Button{

  private:
    static last_touch_e_t touch_status;
    static int16_t x, y;
    static std::map <Button*, std::string> buttons;

  public:
    static void update_press_status ();

    enum disp{  //display constants
      left = 0,
      up = 0,
      right = 479,
      down = 239,
      mid_x = 240,
      mid_y = 120, //480 and 240 are the max but offscreen
      char_height = 12,
      char_width = 7
    };

    enum style{ //how the rect coords get evaluated
      CENTRE,
      CORNER,
      SIZE
    };

    int16_t x1=0, y1=0, x2=0, y2=0;

    Button (int16_t point1, int16_t point2, int16_t point3, int16_t point4, style type, std::string text = "", std::uint32_t background_color = COLOR_WHITE, std::uint32_t label_color = COLOR_BLACK);
    void del();

    bool pressed();
    bool new_press();

    static Button* get_press();
};
