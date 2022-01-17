#pragma once
#include "../../auton.hpp"
#include "../../drive.hpp"
#include "../../Tracking.hpp"
#include "../../util.hpp"

// #define GUI_UTIL

#if defined(GUI_MAIN)
#define PAGE_COUNT 12 //The number for testing if not included. Otherwise +1

#elif defined(GUI_UTIL)
#undef GUI_MAIN
#define PAGE_COUNT 5

#else
#define GUI_MAIN
#define PAGE_COUNT 12

#endif //MAIN

#define ORANGE 0x00F36421
#define GREY 0x00202020

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

static const int zero = 0;
typedef std::uint32_t Color;
#define COLOR(NAME) (Color)COLOR_##NAME

// Forward-Declaration
class GUI;
class Page;
class Button;
class Slider;
template <typename V=int, typename I=int> class _Text;

//For other gui hpp files
extern int ring_count; //For gui_objects to use
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
  template <typename V, typename I> friend class _Text;

  private:
    static last_touch_e_t touch_status;
    static int x, y;

    static void update_screen_status();
    static void setup(), update();
    static void end_flash();
    static void draw_oblong(int, int, int, int, double, double);
    static int get_size(text_format_e_t, std::string);
  public:
    static void init(), background();
    static void aligned_coords (int, int, int, int, int = 480, int = 220);
    static void flash (Color, std::uint32_t, std::string = "");
    static bool go(std::string, std::string, std::uint32_t=0), go(std::string, std::uint32_t=0);
    static bool pressed();
};

//All constructor args are in the format points, format, page, Text, Color
class Page{
  friend class GUI;
  friend class Button;
  friend class Slider;
  template <typename V, typename I> friend class _Text;
  private:

    //Vars
    std::function <void()> setup_func, loop_func;
    Color b_col;
    std::string title;

  public:
    //Page num, Title, Bcolor
    Page(int, std::string, Color = GREY);

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
    static void clear_screen(Color=GREY);
    void set_setup_func(std::function <void()>), set_loop_func(std::function <void()>);
    bool pressed();
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
    Color l_col, b_col=GREY;
    Style type;
    Page* page;
    bool active = true;

    //Functions
    virtual void update() = 0;
    virtual void draw() = 0;

  public:
    //Functions
    void set_background (int, int, Color = 0xFFFFFFFF);
    void set_background (int, int, int, int, Style, Color = 0xFFFFFFFF);
    void set_background (Color);
    void set_active(bool=true);
};

template <typename V, typename I>
class _Text: public Text{
  friend class Button;
  friend class Slider;
  private:
    _Text(){};

    //Vars
    const V* value_ptr;
    V prev_value=0;
    const I* index_ptr;

    //Functions
    void construct (int x, int y, Style type, text_format_e_t txt_fmt, Page* page, std::string label, const V* value_ptr, const I* index_ptr, Color l_col){
      static_assert(!std::is_same_v<V, std::string>, "Text variable cannot be std::string, it causes unkown failures");
      static_assert(std::is_scalar_v<I>, "Text Index type must be int-convertible");
      this->x = x;
      this->y = y;
      this->type = type;
      this->txt_fmt = txt_fmt;
      this->page = page;
      this->label = label;
      this->value_ptr = value_ptr;
      this->index_ptr = index_ptr;
      this->l_col = l_col;
      this->b_col = page->b_col;
      page->texts.push_back(this);
    }
    void update(){
      if(value_ptr && active && prev_value != *value_ptr) draw();
    }
    void draw(){
      if (!active) return;
      std::cout << label << typeid(V).name() << std::endl;
      char buffer [100];
      int length;

      if(value_ptr){
        prev_value = value_ptr[static_cast<int>(*index_ptr)];
        length = sprintf(buffer, label.c_str(), prev_value);

      }
      else length = sprintf(buffer, label.c_str());

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
      if (type == Style::CENTRE){
        x_coord -= (length*GUI::get_size(txt_fmt, "width"))/2;
        y_coord -= GUI::get_size(txt_fmt, "height")/2;
      }
      x1 = min(x1, x_coord); //Resizes the background so it won't have overwriting issues
      x2 = max(x2, x_coord + length*(GUI::get_size(txt_fmt, "width")+1));
      y1 = min(y1, y_coord);
      y2 = max(y2, y_coord + GUI::get_size(txt_fmt, "height"));

      screen::print(txt_fmt, x_coord, y_coord, "%s", buffer);
    }

  public:
    //Points, Format, Page, Label, [var info], Lcolor
    _Text (int x, int y, Style rect_type, text_format_e_t size, Page& page, std::string text, Color label_color = COLOR_WHITE){
      construct (x, y, rect_type, size, &page, text, nullptr, &zero, label_color);
    }
    _Text (int x, int y, Style rect_type, text_format_e_t size, Page& page, std::string text, const V& value_ref, Color label_color = COLOR_WHITE){
      construct (x, y, rect_type, size, &page, text, &value_ref, &zero, label_color);
    }
    _Text (int x, int y, Style rect_type, text_format_e_t size, Page& page, std::string text, const V& value_ref, const I& index_ref, Color label_color = COLOR_WHITE){
      construct (x, y, rect_type, size, &page, text, &value_ref, &index_ref, label_color);
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
    Color l_col, b_col, b_col_dark;
    std::string label, label1="";
    int x1, y1, x2, y2, text_x, text_y, text_x1, text_y1;
    bool last_pressed=0;
    press_type form; //What type of button
    std::function <void()> func, off_func; //toggle is only for toggle type buttons
    Text* title = nullptr;
    bool active=true;

    //For latch buttons
    bool on=0; //on is for toggle
    std::vector<Button*> options;

    //Functions
    void update();
    void construct (int, int, int, int, Style, press_type, Page*, std::string, Color, Color);
    void run_func(), run_off_func();
    void draw();
    void draw_pressed();

  public:
    //Points, Format, Page, Label, Bcolor, Lcolor
    Button (int, int, int, int, Style, press_type, Page&, std::string = "", Color = ORANGE, Color = COLOR(BLACK));

    //Vars
    Page* page;

    //Functions
    static void create_options(std::vector<Button*>);
    void set_func(std::function <void()>), set_off_func(std::function <void()>);
    void set_active(bool=true);
    void set_background (Color);
    void add_text (Text&, bool=true);
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
    Color l_col, b_col;
    int x1, y1, x2, y2, text_x, text_y;
    int min, max;
    int val=0, prev_val;
    direction dir;
    Page* page;
    Button dec, inc;
    _Text<int, int> title, min_title, max_title;

    //Functions
    void update();
    void draw();

  public:
    //Points, Format, Min, Max, Page, Label, Bcolor, Lcolor
    Slider (int, int, int, int, Style, direction, int, int, Page&, std::string = "Value", Color = COLOR_WHITE, Color = ORANGE);

    //Functions
    int get_val();
};

//For auton.hpp
extern Button alliance;
