#pragma once
#include "../Subsystems/lift.hpp"
#include "../auton.hpp"
#include "../drive.hpp"
#include "../util.hpp"
#include "../include/pros/apix.h"
#include <bitset>
// #include <cstdio>
// #include <iostream>
// #include <vector>

// Forward-Declaration
class GUI;
class Page;
class Button;
class Slider;
template <typename V=std::nullptr_t> class _Text;

//For main.cpp to switch between
extern GUI g_main, g_util;

typedef std::uint32_t Colour;
#define COLOUR(NAME) (Colour)COLOR_##NAME

#define ORANGE (Colour)0x00F36421
#define GREY (Colour)0x00202020

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

//For gui to use
extern int ring_count;
extern const char* alliance_names[];
extern const char* auton_names[];

class GUI{
  friend class Page;
  friend class Button;
  friend class Slider;
  friend class Text;
  template <typename V> friend class _Text;

  public:
    enum class Style{ //how the rect coords get evaluated
      CENTRE,
      CORNER,
      SIZE,
      CENTER=CENTRE
    };

  private:
    //Vars
    static bool touched;
    static int x, y;
    static Page* current_page;
    static GUI* const current_gui;
    static constexpr bool go_enabled=true;
    std::vector<Page*> pages;
    std::function <void()> setup, background;

    //Functions
    static void update_screen_status();
    static void go_next(), go_prev();
    static void end_flash();
    static void draw_oblong(int, int, int, int, const double, const double);
    static int get_size(text_format_e_t, std::string);
    static std::tuple<int, int, int, int> fix_points(int, int, int, int, Style);

  public:

    //Constructor
    GUI(std::vector<Page*>, std::function <void()>, std::function <void()>);

    //Functions
    static void aligned_coords (int, int, int, int, int = 480, int = 220);
    static void flash(Colour, std::uint32_t, const char*, ...);
    static void flash(Colour, std::uint32_t, std::string = "");
    static bool go(std::string, std::string, std::uint32_t=0), go_end(std::string, std::uint32_t=0);
    static void clear_screen(Colour=GREY);
    static void init(), update();
    static void go_to(int);
    const bool pressed();
};

//All constructor args are in the format points, format, page, Text, Colour
class Page{
  friend class GUI;
  friend class Button;
  friend class Slider;
  friend class Text;
  template <typename V> friend class _Text;
  private:

    //Vars
    std::function <void()> setup_func, loop_func;
    Colour b_col;
    std::string title;
    bool active=true;
    std::vector<GUI*> guis; //Parents
    std::vector<Button*> buttons; //Children
    std::vector<Slider*> sliders; //Children
    std::vector<Text*> texts; //Children

    //Functions
    static Page* const page_id(int);
    static const int page_num(Page*);
    const void draw();
    const void update();

  public:
    //Title, Bcolour
    explicit Page(std::string, Colour = GREY);

    //Functions
    const bool pressed();
    const void go_to();
    void set_setup_func(std::function <void()>), set_loop_func(std::function <void()>);
    void set_active(bool=true);
};

class Text{
  friend class GUI;
  friend class Page;
  friend class Button;

  protected:
    //Vars
    int x, y, x1=USER_RIGHT, y1=USER_DOWN, x2=USER_LEFT, y2=USER_UP;
    text_format_e_t txt_fmt;
    std::string label;
    Colour l_col, b_col=GREY;
    GUI::Style type;
    Page* page;
    bool active = true;

    //Functions
    const virtual void update() = 0;
    virtual void draw() = 0;

  public:
    //Functions
    void set_background(int, int, Colour = 0xFFFFFFFF);
    void set_background(int, int, int, int, GUI::Style, Colour = 0xFFFFFFFF);
    void set_background(Colour);
    void set_active(bool=true);
};

template <typename V> class _Text: public Text{
  friend class Button;
  friend class Slider;
  private:
    _Text(){};

    //Vars
    V prev_value=0;
    Value<V> value;

    //Functions
    const void update() override {
      if (value && active && prev_value != value()) draw();
    }
    void draw() override {
      if (!active) return;
      char buffer [100];
      int length;

      if(value){
        prev_value = value();
        length = snprintf(buffer, 100, label.c_str(), prev_value);
      }
      else length = snprintf(buffer, 100, label.c_str());

      if (x2 != 0 && y2 != 0){
        screen::set_eraser(page->b_col); //Erases previous box
        screen::erase_rect(x1, y1, x2, y2);

        screen::set_pen(b_col);
        GUI::draw_oblong(x1, y1, x2, y2, 0, 0.15);
        screen::set_pen(l_col);
        screen::set_eraser(b_col);
      }
      else{
        screen::set_pen(l_col);
        screen::set_eraser(b_col);
      }

      int x_coord = x, y_coord = y;
      if (type == GUI::Style::CENTRE){
        x_coord -= (length*GUI::get_size(txt_fmt, "width"))/2;
        y_coord -= GUI::get_size(txt_fmt, "height")/2;
      }
      x1 = min(x1, x_coord); //Resizes the background so it won't have overwriting issues
      x2 = max(x2, x_coord + length*(GUI::get_size(txt_fmt, "width")+1));
      y1 = min(y1, y_coord);
      y2 = max(y2, y_coord + GUI::get_size(txt_fmt, "height"));

      screen::print(txt_fmt, x_coord, y_coord, "%s", buffer);
    }
    void construct (int x, int y, GUI::Style type, text_format_e_t txt_fmt, Page* page, std::string label, Value<V> value, Colour l_col){
      static_assert(!std::is_same_v<V, std::string>, "Text variable cannot be std::string, it causes unknown failures");
      this->x = x;
      this->y = y;
      this->type = type;
      this->txt_fmt = txt_fmt;
      this->page = page;
      this->label = label;
      this->value = value;
      this->l_col = l_col;
      this->b_col = page->b_col;
      page->texts.push_back(this);
    }

  public:
    //Points, Format, Page, Label, [var info], Lcolour

    //No var
    _Text (int x, int y, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, Colour label_colour = COLOUR(WHITE))
    {
      construct (x, y, rect_type, size, &page, text, nullptr, label_colour);
    }

    //Variable
    _Text (int x, int y, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, Value<V> value_obj, Colour label_colour = COLOUR(WHITE))
    {
      construct (x, y, rect_type, size, &page, text, value_obj, label_colour);
    }

};

class Button{
  friend class GUI;
  friend class Page;
  friend class Slider;
  public: enum press_type{
      SINGLE,
      LATCH,
      REPEAT,
      TOGGLE
    };

  private:
    Button (){};

    //Vars
    Colour l_col, b_col, b_col_dark;
    std::string label, label1="";
    int x1, y1, x2, y2, text_x, text_y, text_x1, text_y1;
    bool last_pressed=0;
    press_type form; //What type of button
    Value<void> func, off_func;
    Text* title = nullptr;
    bool active=true;
    Page* page;

    //For latch buttons
    bool on=0; //on is for toggle
    std::vector<Button*> options;

    //Functions
    void update();
    void construct (int, int, int, int, GUI::Style, press_type, Page*, std::string, Colour, Colour);
    const void run_func(), run_off_func();
    const void draw();
    const void draw_pressed();

  public:
    //Points, Format, Page, Label, Bcolour, Lcolour
    Button (int, int, int, int, GUI::Style, press_type, Page&, std::string = "", Colour = ORANGE, Colour = COLOUR(BLACK));

    //Functions
    static void create_options(std::vector<Button*>);
    const bool pressed();
    void set_func(std::function <void()>), set_off_func(std::function <void()>);
    void set_func(Value<void>), set_off_func(Value<void>);
    void set_active(bool=true);
    void set_background (Colour);
    void add_text (Text&, bool=true);
    bool new_press();
    bool new_release();
    void wait_for_press(); //Blocking
};

class Slider{
  friend class GUI;
  friend class Page;
  public: enum direction{
      VERTICAL,
      HORIZONTAL
    };

  private:
    //Vars
    Colour l_col, b_col;
    int x1, y1, x2, y2, text_x, text_y;
    int min, max;
    int val, prev_val;
    direction dir;
    bool active=true;
    Page* page;
    Button dec, inc;
    _Text<int> title;
    _Text<std::nullptr_t> min_title, max_title;

    //Functions
    void update();
    const void draw();

  public:
    //Points, Format, Min, Max, Page, Label, Bcolour, Lcolour
    Slider (int, int, int, int, GUI::Style, direction, int, int, Page&, std::string = "Value", int = 1, Colour = COLOUR(WHITE), Colour = ORANGE);

    //Functions
    void set_active(bool=true);
    const int get_value();
    const bool pressed();

};
