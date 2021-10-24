#pragma once
#include <vector>
#include <string>
#include "main.h"
#include "tracking.hpp"
#include "config.hpp"
#include "intake.hpp"
using namespace pros;
#define PAGE_COUNT 9
#define PAGE_LEFT 0
#define PAGE_UP 0
#define PAGE_RIGHT 480
#define PAGE_DOWN 240
#define USER_LEFT 0
#define USER_UP 25
#define USER_RIGHT 479
#define USER_DOWN 239
#define MID_X 240
#define MID_Y 120
#define CHAR_HEIGHT_SMALL 12
#define CHAR_WIDTH_SMALL 7
#define CHAR_HEIGHT_MEDIUM 16
#define CHAR_WIDTH_MEDIUM 9
#define CHAR_HEIGHT_LARGE 32
#define CHAR_WIDTH_LARGE 19


//Forward-Declaration
class Page;
class Button;
class Slider;
class Text;

namespace Style{
  enum style{ //how the rect coords get evaluated
    CENTRE,
    CORNER,
    SIZE
  };
};

extern int ring_count, cur_driver;
extern Page liftElastic, liftMove, track, moving, autoSel, driverCurve, intkTest, temps, mContr;
// extern std::array<driver, num_of_drivers> drivers;

void guiSetup();
void guiBackground();
void alignedCoords (int x_objects, int y_objects, int x_btn, int y_btn, int x_range = 480, int y_range = 220);
void flash(std::uint32_t color, std::uint32_t time);

//All constructor args are in the format points, format, page, text, color
class Page{
  friend class Text;
  friend class Button;
  friend class Slider;
  private:

    //Vars
    static last_touch_e_t touch_status;
    static int16_t x, y;
    std::uint32_t bcol, inv_bcol;
    std::string title;

  public:
    //Page num, Title, Bcolor
    Page(int, std::string, std::uint32_t = COLOR_BLACK);

    //Vars
    static Page* currentPage;
    static std::array<Page*, PAGE_COUNT> pages;
    std::function <void()> func;
    std::vector<Button*> buttons;
    std::vector<Slider*> sliders;
    std::vector<Text*> texts;
    int pageNum;

    //Functions
    static void updateScreenStatus ();
    static void goTo(Page*);
    static void goTo(int);
    static void clearScreen(std::uint32_t color);
    static void toPrev();
    static void toNext();
    static void update();
    bool pressed();

};

class Button{
  friend class Page;
  public: enum press_type{
      SINGLE,
      LATCH,
      HOLD
    };

  private:
    //Vars
    std::uint32_t lcol, bcol, dark_bcol;
    std::string label, label1="";
    int16_t x1, y1, x2, y2, text_x, text_y, text_x1, text_y1;
    // void (*funcPtr)(); //This is a var because it is a pointer to a void function, not a void function in itself
    bool lastPressed=0;
    press_type form;

    //For latch buttons
    bool latched=0;
    std::vector<Button*> options;

    //Functions
    void runTask();
    void draw();
    void drawPressed();

  public:
    //Constructor
    //Points, Format, Page, Label, Bcolor, Lcolor
    Button (int16_t, int16_t, int16_t, int16_t, Style::style, press_type, Page*, std::string = "", std::uint32_t = COLOR_WHITE, std::uint32_t = COLOR_BLACK);
    Button (){};
    void construct (int16_t, int16_t, int16_t, int16_t, Style::style, press_type, Page*, std::string, std::uint32_t, std::uint32_t);

    //Vars
    Page* page;
    std::function <void()> func;

    //Functions
    static Button* update();
    static void createOptions(std::vector<Button*>);
    bool pressed();
    bool newPress();
    bool newRelease();
    void del(); //Unused
};

class Slider{
  friend class Page;
  public: enum direction{
      VERTICAL,
      HORIZONTAL
  };

  private:
    //Vars
    std::uint32_t lcol, bcol;
    std::string label;
    int16_t x1, y1, x2, y2, text_x, text_y;
    int min, max;
    Button dec, inc;
    direction dir;

    //Functions
    void draw();

  public:
    //Points, Format, Min, Max, Page, Label, Bcolor, Lcolor
    Slider (int16_t, int16_t, int16_t, int16_t, Style::style, direction, int, int, Page*, std::string = "", std::uint32_t = COLOR_WHITE, std::uint32_t = COLOR_YELLOW);

    //Vars
    Page* page;
    double val=0.0, prevVal;

    //Functions
    static Slider* update();
    void updateVal();
};

class Text{
  friend class Page;

  private:
    int16_t x, y;
    text_format_e_t txt_fmt;
    std::string label;
    std::uint32_t lcol;
    Page* page;
    double* val_ptr=nullptr;
    double prevVal;

    void construct (int16_t, int16_t, Style::style, text_format_e_t, Page*, std::string, std::uint32_t label_color);
    void draw();

  public:
    //Points, Format, Page, Label, [var], Lcolor
    Text (int16_t, int16_t, Style::style, text_format_e_t, Page*, std::string, std::uint32_t label_color = COLOR_WHITE);
    Text (int16_t, int16_t, Style::style, text_format_e_t, Page*, std::string, double*, std::uint32_t label_color = COLOR_WHITE);

    static void update();
};
