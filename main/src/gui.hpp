#pragma once
#include <vector>
#include <string>
#include "main.h"
using namespace pros;

class Page; //So they can access each other
class Button;

class Page{
  //create permanent buttons that let you scroll between pages.

  private:
    std::uint32_t page_bcolor;

  public:
    enum disp{ //Display Constants
      left = 0,
      up = 0,
      right = 479,
      down = 239, //480 and 240 are the max but offscreen
      mid_x = 240,
      mid_y = 120,
      char_height = 12,
      char_width = 7
    };

    Page(int num, std::uint32_t background_color = COLOR_BLACK);

    std::vector<Button*> buttons;
    static Page* currentPage;
    int pageNum;

    static void goTo(Page* page);
    static void clearScreen(std::uint32_t color);

};

class Button{

  private:
    friend class Page; //Only so Page class can access Button::draw()

    static last_touch_e_t touch_status;
    static int16_t x, y;

    int16_t x1=0, y1=0, x2=0, y2=0;
    std::uint32_t bcolor=0, lcolor;
    std::string label;

    void draw();
    void (*funcPtr)()=0;
    void runTask();

  public:
    enum style{ //how the rect coords get evaluated
      CENTRE,
      CORNER,
      SIZE
    };

    Page* page;

    Button (int16_t point1, int16_t point2, int16_t point3, int16_t point4, style type, Page* page_number, std::string text = "", std::uint32_t background_color = COLOR_WHITE, std::uint32_t label_color = COLOR_BLACK);

    static void update_press_status ();
    static Button* get_press();
    bool pressed();
    bool new_press();
    void del();
    void setTask(void (*pointer_to_function)());
};
