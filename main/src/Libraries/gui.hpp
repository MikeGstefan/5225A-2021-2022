#pragma once
#include "printing.hpp"
#include "../auton.hpp"
#include "../drive.hpp"
#include "../piston.hpp"
#include "../util.hpp"
#include "../include/pros/apix.h"
#include <bitset>

// Forward-Declaration
class GUI;
class Page;
class Button;
class Slider;
class Text_; //parent (interface)
template <typename V=std::nullptr_t> class Text; //child (used)

//For main.cpp to switch between
extern GUI g_main, g_util;

//From gui.cpp
extern Page terminal, testing;
extern Button testing_button_1, testing_button_2;
extern Text<> testing_text_1, testing_text_2;
extern Slider testing_slider;

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

//All constructor args are in the format points, format, page, Text, Colour

class GUI{
  friend class Page;
  friend class Button;
  friend class Slider;
  friend class Text_;
  template <typename V> friend class Text;

  public:
    enum class Style{ //how the rect coords get evaluated
      CENTRE,
      CORNER,
      SIZE,
      CENTER=CENTRE
    };

  private:
    //Vars
    static _Task task;
    static bool touched;
    static int x, y;
    static const Page* current_page;
    static const GUI* current_gui;
    static constexpr bool prompt_enabled = true;
    static constexpr bool testing_page_active = false;
    std::vector<Page*> pages;
    std::function <void()> setup, background;

    //Functions
    static void update_screen_status();
    static void go_next(), go_prev();
    static void screen_terminal_fix();
    static void draw_oblong(int, int, int, int, double, double);
    static int get_height(text_format_e_t), get_width(text_format_e_t);
    static std::tuple<int, int, int, int> fix_points(int, int, int, int, Style);

  public:
    //Pages in the gui, init function, loop function
    GUI(std::vector<Page*>, std::function <void()>, std::function <void()>);

    //Functions
    static Colour get_colour(term_colours);

    static void aligned_coords (int, int, int, int, int = 480, int = 220);
    static void clear_screen(Colour=GREY);
    static void init(), update(void* = nullptr);
    static void go_to(int);
    static bool prompt(std::string, std::string="", std::uint32_t=0); //Also prompts to controller
    static bool is_touched();
    static const Page* const get_current_page();
    static const GUI* const get_current_gui();
    bool pressed() const;
};

class Page{
  friend class GUI;
  friend class Button;
  friend class Slider;
  friend class Text_;
  template <typename V> friend class Text;
  private:

    //Vars
    std::function <void()> setup_func, loop_func;
    Colour b_col;
    std::string title;
    bool active=true;
    std::vector<GUI*> guis; //Parents
    std::vector<Button*> buttons; //Children
    std::vector<Slider*> sliders; //Children
    std::vector<Text_*> texts; //Children

    //Functions
    static Page* page_id(int);
    static int page_num(const Page*);
    void draw() const;
    void update() const;

  public:
    //Title, Bcolour
    explicit Page(std::string, Colour = GREY);

    //Functions
    bool pressed() const;
    void go_to() const;
    void set_setup_func(std::function <void()>), set_loop_func(std::function <void()>);
    void set_active(bool=true);
};

//Text parent class
class Text_{
  friend class GUI;
  friend class Page;
  friend class Button;
  private:
  int x1=USER_RIGHT, y1=USER_DOWN, x2=USER_LEFT, y2=USER_UP;

  protected:
    //Vars
    int x, y;
    text_format_e_t txt_size;
    std::string label;
    Colour l_col, b_col=GREY;
    GUI::Style type;
    Page* page;
    bool active = true;

    //Functions
    virtual void update() = 0;
    virtual int update_val(char* const buffer) = 0;
    void draw();

  public:
    //Functions
    void set_background(int, int, Colour); //Centre
    void set_background(int, int); //Centre
    void set_background(int, int, int, int, GUI::Style, Colour);
    void set_background(int, int, int, int, GUI::Style);
    void set_background(Colour);
    void set_active(bool=true);
};

template <typename V>
class Text: public Text_{
  friend class Button;
  friend class Slider;
  private:
    Text(){};

    //Vars
    std::remove_const_t<V> prev_value;
    std::function<V()> value;

    //Functions
    void update() override {
      if (active && value && prev_value != value()) draw();
    }
    int update_val(char* const buffer) override {
      if(value){
        prev_value = value();
        return snprintf(buffer, 100, "%s", convert_all_args(label, prev_value));
      }
      return snprintf(buffer, 100, label.c_str());
    }
    void construct (int x, int y, GUI::Style type, text_format_e_t txt_size, Page* page, std::string label, std::function<V()> value, Colour l_col){
      // static_assert(!std::is_same_v<V, std::string>, "Text variable cannot be std::string, it causes unknown failures"); //Keep this for memories
      this->x = x;
      this->y = y;
      this->type = type;
      this->txt_size = txt_size;
      this->page = page;
      this->label = label;
      this->value = value;
      this->l_col = l_col;
      this->b_col = page->b_col;
      page->texts.push_back(this);
    } 

  public:
    //Points, Format, Page, Label, [var info], Lcolour

    // Terminal (var - no format)
    Text (std::string text, V& value_obj, Colour label_colour = COLOUR(WHITE)){
      construct (5, 0, GUI::Style::CORNER, E_TEXT_LARGE_CENTER, &terminal, text, [&](){return value_obj;}, label_colour);
    }

    // Terminal (var - format)
    Text (std::string text, V& value_obj, text_format_e_t size, Colour label_colour = COLOUR(WHITE)){
      construct (5, 0, GUI::Style::CORNER, size, &terminal, text, [&](){return value_obj;}, label_colour);
    }

    // Terminal (array - no format)
    template <typename I>
    Text (std::string text, V* value_arr, I& index, Colour label_colour = COLOUR(WHITE)){
      construct (5, 0, GUI::Style::CORNER, E_TEXT_LARGE_CENTER, &terminal, text, [value_arr, &index](){return value_arr[static_cast<int>(index)];}, label_colour);
    }

    // Terminal (array - format)
    template <typename I>
    Text (std::string text, V* value_arr, I& index, text_format_e_t size, Colour label_colour = COLOUR(WHITE)){
      construct (5, 0, GUI::Style::CORNER, size, &terminal, text, [value_arr, &index](){return value_arr[static_cast<int>(index)];}, label_colour);
    }

    // Terminal (function - no format)
    Text (std::string text, const std::function<V()>& func, Colour label_colour = COLOUR(WHITE)){
      construct (5, 0, GUI::Style::CORNER, E_TEXT_LARGE_CENTER, &terminal, text, func, label_colour);
    }

    // Terminal (function - format)
    Text (std::string text, const std::function<V()>& func, text_format_e_t size, Colour label_colour = COLOUR(WHITE)){
      construct (5, 0, GUI::Style::CORNER, size, &terminal, text, func, label_colour);
    }



    //No var
    Text (int x, int y, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, Colour label_colour = COLOUR(WHITE)){
      construct (x, y, rect_type, size, &page, text, nullptr, label_colour);
    }

    //Variable
    Text (int x, int y, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, V& value_obj, Colour label_colour = COLOUR(WHITE)){
      construct (x, y, rect_type, size, &page, text, [&](){return value_obj;}, label_colour);
    }

    //Array
    template <typename I>
    Text (int x, int y, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, V* value_arr, I& index, Colour label_colour = COLOUR(WHITE)){
      construct (x, y, rect_type, size, &page, text, [value_arr, &index](){return value_arr[static_cast<int>(index)];}, label_colour);
    }

    //Function
    Text (int x, int y, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, const std::function<V()>& func, Colour label_colour = COLOUR(WHITE)){
      construct (x, y, rect_type, size, &page, text, func, label_colour);
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
    std::function<void()> func, off_func;
    Text_* title = nullptr;
    bool active=true;
    Page* page;

    //For latch buttons
    bool on=0; //on is for toggle
    std::vector<Button*> options;

    //Functions
    void update();
    void construct (int, int, int, int, GUI::Style, press_type, Page*, std::string, Colour, Colour);
    void run_func() const;
    void run_off_func() const;
    void draw() const;
    void draw_pressed()  const;
    bool new_press();
    bool new_release();

  public:
    //Points, Format, Page, Label, Bcolour, Lcolour
    Button (int, int, int, int, GUI::Style, press_type, Page&, std::string = "", Colour = ORANGE, Colour = COLOUR(BLACK));

    //Functions
    static void create_options(std::vector<Button*>);
    bool pressed() const;
    void set_func(std::function <void()>), set_off_func(std::function <void()>);
    void set_active(bool=true);
    void set_background (Colour);
    void add_text (Text_&, bool=true);
    void select(), deselect();
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
    Text<int> title;
    Text<> min_title, max_title;

    //Functions
    void update();
    void draw() const;

  public:
    //Points, Format, Min, Max, Page, Label, Bcolour, Lcolour
    Slider (int, int, int, int, GUI::Style, direction, int, int, Page&, std::string = "Value", int = 1, Colour = COLOUR(WHITE), Colour = ORANGE);

    //Functions
    void set_active(bool=true);
    int get_value() const;
    bool pressed() const;

};

namespace screen_flash{
  extern Timer timer;

  void end();
  void start(std::string, Colour, std::uint32_t = 1000); //text+col+time / text+col
  void start(std::string, term_colours = term_colours::ERROR, std::uint32_t = 1000); //text+cols+time / text+cols / text

  template <typename... Params> //text+cols+time
  void start(term_colours colour, std::uint32_t time, const char* fmt, Params... args){
    start(printf_to_string(fmt, args...), colour, time);
  }

  template <typename... Params> //text+col+time
  void start(Colour colour, std::uint32_t time, const char* fmt, Params... args){
    start(printf_to_string(fmt, args...), colour, 1000);
  }

  template <typename... Params> //text+red+time
  void start(std::uint32_t time, const char* fmt, Params... args){
    start(printf_to_string(fmt, args...), term_colours::ERROR, time);
  }

  template <typename... Params> //text+col+1000
  void start(term_colours colour, const char* fmt, Params... args){
    start(printf_to_string(fmt, args...), colour, 1000);
  }
}
