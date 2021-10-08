#pragma once
#include <vector>
#include <string>
#include "main.h"
using namespace pros;

void guiSetup();

class Page; //So they can access each other
class Button;
struct Text;

class Page{
  friend struct Text;
  friend class Button;
  private:

    //Vars
    static last_touch_e_t touch_status;
    static int16_t x, y;
    std::uint32_t bcol;
    std::string title;

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

    Page(int page_number, std::string name, std::uint32_t background_color = COLOR_BLACK);

    //Vars
    static Page* currentPage;
    static std::array<Page*, 11> pages;
    std::vector<Button*> buttons;
    std::vector<Text*> texts;
    int pageNum;

    //Functions
    static void updateScreenStatus ();
    static void goTo(Page* page);
    static void clearScreen(std::uint32_t color);
    static void toPrev();
    static void toNext();

};

class Button{
  friend class Page;

  private:
    //Vars
    std::uint32_t lcol, bcol, dark_bcol;
    std::string label;
    int16_t x1, y1, x2, y2, text_x, text_y;
    void (*funcPtr)(); //This is a var because it is a pointer to a void function, not a void function in itself
    bool lastPressed=0;
    bool latch, latched=0;

    //Functions
    void runTask();
    void draw();
    void drawPressed();

  public:
    //Vars
    enum style{ //how the rect coords get evaluated
      CENTRE,
      CORNER,
      SIZE
    };

    Page* page;

    //Constructor
    Button (int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, style type, bool toggle, Page* page_number, std::string text = "", std::uint32_t background_color = COLOR_WHITE, std::uint32_t label_color = COLOR_BLACK);

    //Functions
    static Button* getPress();
    void setTask(void (*pointer_to_function)());
    void del();
    bool pressed();
    bool newPress();
};

struct Text{
  int16_t x, y;
  std::string label;
  std::uint32_t lcol;
  Page* page;

  Text (int16_t point1, int16_t point2, Page* page_ptr, std::string text = "", std::uint32_t label_color = COLOR_BLACK);

  void draw();
};

//test if creating btn and text objects cause issues
//Maybe allow page functions so they can display [moving] things on the screen (like robot cur pos on the field)
//Check for variadic text in constructor for screen printing
