#pragma once
#include "drive.hpp"
#include "Tracking.hpp"
#include "util.hpp"
#include <bitset>
#include <variant>
#include <typeinfo>

#define ORANGE 0x00F36421
#define GREY 0x00202020

#define PAGE_COUNT 12
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

enum class Style{ //how the rect coords get evaluated
    CENTRE,
    CORNER,
    SIZE
};

extern int ring_count, cur_auton; //For gui.cpp to use
extern Page testing; //For use in opcontrol

void gui_setup(), gui_background();
void aligned_coords (int x_objects, int y_objects, int x_btn, int y_btn, int x_range = 480, int y_range = 220);
void flash (std::uint32_t color, std::uint32_t time, std::string = "");
void prev_driver(), next_driver();

//All constructor args are in the format points, format, page, text, color
class Page{
  friend class Text;
  friend class Button;
  friend class Slider;
  private:

    //Vars
    static last_touch_e_t touch_status;
    static int16_t x, y;
    std::function <void()> setup_func, loop_func;
    std::uint32_t b_col;
    std::string title;

  public:
    //Page num, Title, Bcolor
    Page(int, std::string, std::uint32_t = GREY);

    //Vars
    static Page* current_page;
    static std::array<Page*, PAGE_COUNT> pages;
    std::vector<Button*> buttons;
    std::vector<Slider*> sliders;
    std::vector<Text*> texts;
    int page_num;

    //Functions
    static void update_screen_status();
    static void go_to(Page*);
    static void go_to(int);
    static void clear_screen(std::uint32_t color);
    static void to_prev();
    static void to_next();
    static void update();
    void set_setup_func(std::function <void()>), set_loop_func(std::function <void()>);
    bool pressed();
};

class Button{
  friend class Page;
  public: enum pressType{
      SINGLE,
      LATCH,
      REPEAT,
      TOGGLE
    };

  private:
    //Vars
    std::uint32_t l_col, b_col, b_col_dark;
    std::string label, label1="";
    int16_t x1, y1, x2, y2, text_x, text_y, text_x1, text_y1;
    bool last_pressed=0;
    pressType form; //What type of button
    std::function <void()> func, off_func; //toggle is only for toggle type buttons

    //For latch buttons
    bool latched=0, on=0; //on is for toggle
    std::vector<Button*> options;

    //Functions
    void run_func();
    void draw();
    void draw_pressed();

  public:
    //Constructor
    //Points, Format, Page, Label, Bcolor, Lcolor
    Button (int16_t, int16_t, int16_t, int16_t, Style, pressType, Page*, std::string = "", std::uint32_t = ORANGE, std::uint32_t = COLOR_BLACK);
    Button (){};
    void construct (int16_t, int16_t, int16_t, int16_t, Style, pressType, Page*, std::string, std::uint32_t, std::uint32_t);

    //Vars
    Page* page;

    //Functions
    static Button* update();
    static void create_options(std::vector<Button*>);
    void set_func(std::function <void()>), set_off_func(std::function <void()>);
    bool pressed();
    bool new_press();
    bool new_release();
};

class Slider{
  friend class Page;
  public: enum direction{
      VERTICAL,
      HORIZONTAL
  };

  private:
    //Vars
    std::uint32_t l_col, b_col;
    std::string label;
    int16_t x1, y1, x2, y2, text_x, text_y;
    int min, max;
    Button dec, inc;
    direction dir;

    //Functions
    void draw();
    void draw_bar();

  public:
    //Points, Format, Min, Max, Page, Label, Bcolor, Lcolor
    Slider (int16_t, int16_t, int16_t, int16_t, Style, direction, int, int, Page*, std::string = "Value", std::uint32_t = COLOR_WHITE, std::uint32_t = ORANGE);

    //Vars
    Page* page;
    double val=0.0, prev_val;

    //Functions
    static Slider* update();
    void update_val();
};

class Text{
  friend class Page;

  private:
    int16_t x, y, x1, y1, x2, y2;
    text_format_e_t txt_fmt;
    std::string label;
    std::uint32_t l_col, b_col=COLOR_BLACK;
    Style type;
    Page* page;

    std::variant<std::monostate, double*, int*, std::string*> val_ptr;
    std::variant<std::monostate, double, int, std::string> prev_val;
    const std::type_info& val_type;

    void construct (int16_t, int16_t, Style, text_format_e_t, Page*, std::string, std::variant<std::monostate, double*, int*, std::string*>, std::uint32_t);
    void draw();

  public:
    //Points, Format, Page, Label, [var], Lcolor, other points for background
    Text (int16_t, int16_t, Style, text_format_e_t, Page*, std::string, std::uint32_t label_color = COLOR_WHITE);
    Text (int16_t, int16_t, Style, text_format_e_t, Page*, std::string, double*, std::uint32_t label_color = COLOR_WHITE);
    Text (int16_t, int16_t, Style, text_format_e_t, Page*, std::string, int*, std::uint32_t label_color = COLOR_WHITE);
    Text (int16_t, int16_t, Style, text_format_e_t, Page*, std::string, std::string*, std::uint32_t label_color = COLOR_WHITE);

    static void update();
    void set_background (int16_t, int16_t, std::uint32_t = GREY);
    void set_background (int16_t, int16_t, int16_t, int16_t, Style, std::uint32_t = GREY);
    void set_background (std::uint32_t);
};
