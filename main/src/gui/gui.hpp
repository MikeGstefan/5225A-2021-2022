#pragma once
#include "../drive.hpp"
#include "../Tracking.hpp"
#include "../util.hpp"
#include <variant>
#include <typeinfo>

#define ORANGE 0x00F36421
#define GREY 0x00202020

#define PAGE_COUNT 12 //The number for testing if not included. Otherwise +1
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

// Forward-Declaration
class GUI;
class Page;
class Button;
class Slider;
class Text;

//For other gui hpp files
extern int ring_count, cur_auton; //For gui_objects to use
extern void prev_driver(), next_driver(); //For gui_main to use
extern Timer Flash;

enum class Style{ //how the rect coords get evaluated
    CENTRE,
    CORNER,
    SIZE
};

class GUI{ //Do not make a GUI object ever. To make different GUI's, create a new .hpp file
  friend class Page;
  friend class Button;
  friend class Slider;
  friend class Text;
  
  private:
    static last_touch_e_t touch_status;
    static int16_t x, y;
    
    static void update_screen_status(), update();
    static void general_setup(), general_background();
    static void end_flash();
  public:
    static void setup(), background();
    static void aligned_coords (int x_objects, int y_objects, int x_btn, int y_btn, int x_range = 480, int y_range = 220);
    static void flash (std::uint32_t color, std::uint32_t time, std::string = "");
    static bool go(std::string, std::string, std::uint32_t=0), go(std::string, std::uint32_t=0);
};

//All constructor args are in the format points, format, page, text, color
class Page{
  friend class GUI;
  friend class Text;
  friend class Button;
  friend class Slider;
  private:

    //Vars
    std::function <void()> setup_func, loop_func;
    std::uint32_t b_col;
    std::string title;
    
    //Functions
    static void update();

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
    static void go_to(Page*);
    static void go_to(int);
    static void clear_screen(std::uint32_t color);
    void set_setup_func(std::function <void()>), set_loop_func(std::function <void()>);
    bool pressed();
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
    std::uint32_t l_col, b_col, b_col_dark;
    std::string label, label1="";
    int16_t x1, y1, x2, y2, text_x, text_y, text_x1, text_y1;
    bool last_pressed=0;
    press_type form; //What type of button
    std::function <void()> func, off_func; //toggle is only for toggle type buttons
    bool active = true;

    //For latch buttons
    bool latched=0, on=0; //on is for toggle
    std::vector<Button*> options;

    //Functions
    static Button* update();
    void construct (int16_t, int16_t, int16_t, int16_t, Style, press_type, Page*, std::string, std::uint32_t, std::uint32_t);
    void run_func();
    void draw();
    void draw_pressed();

  public:
    //Constructor
    //Points, Format, Page, Label, Bcolor, Lcolor
    Button (int16_t, int16_t, int16_t, int16_t, Style, press_type, Page&, std::string = "", std::uint32_t = ORANGE, std::uint32_t = COLOR_BLACK);

    //Vars
    Page* page;

    //Functions
    static void create_options(std::vector<Button*>);
    void set_func(std::function <void()>), set_off_func(std::function <void()>);
    void set_active(bool=true);
    bool pressed();
    bool new_press();
    bool new_release();
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
    std::uint32_t l_col, b_col;
    std::string label;
    int16_t x1, y1, x2, y2, text_x, text_y;
    int min, max;
    int val=0, prev_val;
    direction dir;
    Page* page;
    Button dec, inc;

    //Functions
    static Slider* update();
    void draw();
    void draw_bar();
    void update_val();

  public:
    //Points, Format, Min, Max, Page, Label, Bcolor, Lcolor
    Slider (int16_t, int16_t, int16_t, int16_t, Style, direction, int, int, Page&, std::string = "Value", std::uint32_t = COLOR_WHITE, std::uint32_t = ORANGE);
    
    //Functions
    int get_val();
};

class Text{
  friend class GUI;
  friend class Page;

  private:
    int16_t x, y, x1, y1, x2, y2;
    text_format_e_t txt_fmt;
    std::string label;
    std::uint32_t l_col, b_col=GREY;
    Style type;
    Page* page;
    std::variant<std::monostate, double*, int*, std::string*> val_ptr;
    std::variant<std::monostate, double, int, std::string> prev_val;
    const std::type_info& val_type;

    //Functions
    static void update();
    void construct (int16_t, int16_t, Style, text_format_e_t, Page*, std::string, std::variant<std::monostate, double*, int*, std::string*>, std::uint32_t);
    void draw();

  public:
    //Points, Format, Page, Label, [var], Lcolor
    Text (int16_t, int16_t, Style, text_format_e_t, Page&, std::string, std::uint32_t label_color = COLOR_WHITE);
    Text (int16_t, int16_t, Style, text_format_e_t, Page&, std::string, double*, std::uint32_t label_color = COLOR_WHITE);
    Text (int16_t, int16_t, Style, text_format_e_t, Page&, std::string, int*, std::uint32_t label_color = COLOR_WHITE);
    Text (int16_t, int16_t, Style, text_format_e_t, Page&, std::string, std::string*, std::uint32_t label_color = COLOR_WHITE);
    
    //Functions
    void set_background (int16_t, int16_t, std::uint32_t = 0xFFFFFFFF);
    void set_background (int16_t, int16_t, int16_t, int16_t, Style, std::uint32_t = 0xFFFFFFFF);
    void set_background (std::uint32_t);
};
