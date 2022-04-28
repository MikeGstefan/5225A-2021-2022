#pragma once
#include "main.h"
#include "printing.hpp"

//Forward-Declaration
class _Task;
class GUI;
class Page;
class Button;
class Slider;
class Text_; //parent (interface)
template <typename V = std::nullptr_t> class Text; //child (used)

//For main.cpp to switch between
extern GUI main_obj, util_obj;

//From gui.cpp
extern Page terminal, testing;
extern Button testing_button_1, testing_button_2;
extern Text<> testing_text_1, testing_text_2;
extern Slider testing_slider;

typedef std::uint32_t Colour;
#define COLOUR(NAME) (Colour) COLOR_##NAME

constexpr Colour
  ORANGE = 0x00F36421,
  GREY = 0x00202020;

constexpr int
  PAGE_LEFT = 0,
  PAGE_UP = 0,
  PAGE_RIGHT = 480,
  PAGE_DOWN = 240,
  USER_LEFT = 0,
  USER_UP = 25,
  USER_RIGHT = 479,
  USER_DOWN = 239,
  MID_X = 240,
  MID_Y = 120,
  CHAR_HEIGHT_SMALL = 12,
  CHAR_WIDTH_SMALL = 7,
  CHAR_HEIGHT_MEDIUM = 16,
  CHAR_WIDTH_MEDIUM = 9,
  CHAR_HEIGHT_LARGE = 32,
  CHAR_WIDTH_LARGE = 19;

namespace screen_flash{
  extern Timer timer;

  void
    end(),
    start(std::string, Colour, std::uint32_t = 1000), //text + col + time / text + col
    start(std::string, term_colours = term_colours::ERROR, std::uint32_t = 1000); //text + cols + time / text + cols / text
  template <typename... Params> void start(term_colours colour, std::uint32_t time, std::string fmt, Params... args); //text + cols + time
  template <typename... Params> void start(Colour colour, std::uint32_t time, std::string fmt, Params... args); //text + col + time
  template <typename... Params> void start(std::uint32_t time, std::string fmt, Params... args); //text + red + time
  template <typename... Params> void start(term_colours colour, std::string fmt, Params... args); //text + col+1000
}


//All constructor args are in the format points, format, page, Text, Colour

class GUI{
  template <typename V> friend class Text;
  friend class Page;
  friend class Button;
  friend class Slider;
  friend class Text_;
  friend void
    main_setup(),
    main_background(),
    util_setup(),
    util_background(),
    screen_flash::end(),
    screen_flash::start(std::string, Colour, std::uint32_t),
    screen_flash::start(std::string, term_colours, std::uint32_t);

  public:
    enum class Style{ //how the rect coords get evaluated
      CENTRE,
      CORNER,
      SIZE,
      CENTER=CENTRE
    };

  private:
    //Vars
      static constexpr bool
        prompt_enabled = true,
        testing_page_active = false;
      static const Page* current_page;
      static const GUI* current_gui;
      static _Task task;
      static bool touched;
      static int x, y;
      std::vector<Page*> pages;
      std::function <void()> setup, background;

    //Functions
      static void
        update(void* = nullptr),
        update_screen_status(),
        go_next(), go_prev(),
        screen_terminal_fix(),
        clear_screen(Colour=GREY),
        draw_oblong(int, int, int, int, double, double);
      static Colour get_colour(term_colours);
      static int get_height(text_format_e_t), get_width(text_format_e_t);
      static std::tuple<int, int, int, int> fix_points(int, int, int, int, Style);
      bool pressed() const;

  public:
    //Pages in the gui, init function, loop function
    GUI(std::vector<Page*>, std::function <void()>, std::function <void()>);

    //Functions
      static void
        aligned_coords (int, int, int, int, int = 480, int = 220),
        init(),
        go_to(int);
      static bool prompt(std::string, std::string = "", std::uint32_t=0); //Also prompts to controller
};

class Page{
  template <typename V> friend class Text;
  friend class GUI;
  friend class Button;
  friend class Slider;
  friend class Text_;
  friend void
    main_setup(),
    main_background(),
    util_setup(),
    util_background(),
    screen_flash::end(),
    screen_flash::start(std::string, Colour, std::uint32_t),
    screen_flash::start(std::string, term_colours, std::uint32_t);
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
      bool pressed() const;
      void
        draw() const,
        update() const,
        go_to() const,
        set_setup_func(std::function <void()>), set_loop_func(std::function <void()>),
        set_active(bool = true);

  public:
    //Title, Bcolour
    explicit Page(std::string, Colour = GREY);
};

//Text parent class
class Text_{
  template <typename V> friend class Text;
  friend class GUI;
  friend class Page;
  friend class Button;
  friend class Slider;
  friend void
    main_setup(),
    main_background(),
    util_setup(),
    util_background(),
    screen_flash::end(),
    screen_flash::start(std::string, Colour, std::uint32_t),
    screen_flash::start(std::string, term_colours, std::uint32_t);
  private:
    int x1 = USER_RIGHT, y1 = USER_DOWN, x2 = USER_LEFT, y2 = USER_UP;

  protected:
    //Vars
      int x, y;
      text_format_e_t txt_size;
      std::string label, text;
      Colour l_col, b_col = GREY;
      GUI::Style type;
      Page* page;
      bool active = true;

    //Functions
      virtual void
        update() = 0,
        update_val() = 0;
      void
        draw(),
        set_background(int, int, Colour), //Centre
        set_background(int, int), //Centre
        set_background(int, int, int, int, GUI::Style, Colour),
        set_background(int, int, int, int, GUI::Style),
        set_background(Colour),
        set_active(bool = true);
};

template <typename V>
class Text: public Text_{
  friend class GUI;
  friend class Page;
  friend class Button;
  friend class Slider;
  friend class Text_;
  friend void
    main_setup(),
    main_background(),
    util_setup(),
    util_background(),
    screen_flash::end(),
    screen_flash::start(std::string, Colour, std::uint32_t),
    screen_flash::start(std::string, term_colours, std::uint32_t);
  private:
    Text(){};

    //Vars
      std::remove_const_t<V> prev_value;
      std::function<V() > value;

    //Functions
      void update() override;
      void update_val() override;
      void construct (int, int, GUI::Style, text_format_e_t, Page*, std::string, std::function<V()>, Colour);

  public:
    //Constructors
      //Points, Format, Page, Label, [var info], Lcolour

      /*Terminal (var - no format)*/ Text (std::string, V&, Colour = COLOUR(WHITE));
      /*Terminal (var - format)*/ Text (std::string, V&, text_format_e_t, Colour = COLOUR(WHITE));
      /*Terminal (array - no format)*/ template <typename I> Text (std::string, V*, I&, Colour = COLOUR(WHITE));
      /*Terminal (array - format)*/ template <typename I> Text (std::string, V*, I&, text_format_e_t, Colour = COLOUR(WHITE));
      /*Terminal (function - no format)*/ Text (std::string, const std::function<V()>&, Colour = COLOUR(WHITE));
      /*Terminal (function - format)*/ Text (std::string, const std::function<V()>&, text_format_e_t, Colour = COLOUR(WHITE));
      /*No var*/ Text (int, int, GUI::Style, text_format_e_t, Page&, std::string, Colour = COLOUR(WHITE));
      /*Variable*/ Text (int, int, GUI::Style, text_format_e_t, Page&, std::string, V&, Colour = COLOUR(WHITE));
      /*Array*/ template <typename I> Text (int, int, GUI::Style, text_format_e_t, Page&, std::string, V*, I&, Colour = COLOUR(WHITE));
      /*Function*/ Text (int, int, GUI::Style, text_format_e_t, Page&, std::string, const std::function<V() >&, Colour = COLOUR(WHITE));
};

class Button{
  template <typename V> friend class Text;
  friend class GUI;
  friend class Page;
  friend class Slider;
  friend class Text_;
  friend void
    main_setup(),
    main_background(),
    util_setup(),
    util_background(),
    screen_flash::end(),
    screen_flash::start(std::string, Colour, std::uint32_t),
    screen_flash::start(std::string, term_colours, std::uint32_t);
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
      std::string label, label1 = "";
      int x1, y1, x2, y2, text_x, text_y, text_x1, text_y1;
      bool last_pressed = 0;
      press_type form; //What type of button
      std::function<void()> func, off_func;
      Text_* title = nullptr;
      bool active=true;
      Page* page;

      //For latch buttons
      bool on = 0; //on is for toggle
      std::vector<Button*> options;

    //Functions
      static void create_options(std::vector<Button*>);
      bool
        pressed() const,
        new_press(),
        new_release();
      void
        run_func() const,
        run_off_func() const,
        draw() const,
        draw_pressed()  const,
        construct (int, int, int, int, GUI::Style, press_type, Page*, std::string, Colour, Colour),
        update(),
        add_text (Text_&, bool = true),
        set_func(std::function <void()>), set_off_func(std::function <void()>),
        set_active(bool = true),
        set_background (Colour);

  public:
    //Points, Format, Page, Label, Bcolour, Lcolour
    Button (int, int, int, int, GUI::Style, press_type, Page&, std::string = "", Colour = ORANGE, Colour = COLOUR(BLACK));

    //Functions
      void select(), deselect();
};

class Slider{
  template <typename V> friend class Text;
  friend class GUI;
  friend class Page;
  friend class Button;
  friend class Text_;
  friend void
    main_setup(),
    main_background(),
    util_setup(),
    util_background(),
    screen_flash::end(),
    screen_flash::start(std::string, Colour, std::uint32_t),
    screen_flash::start(std::string, term_colours, std::uint32_t);
  public: enum direction{
    VERTICAL,
    HORIZONTAL
  };

  private:
    //Vars
      int
        x1, y1, x2, y2, text_x, text_y,
        min, max,
        val, prev_val;
      Colour l_col, b_col;
      direction dir;
      bool active=true;
      Page* page;
      Button dec, inc;
      Text<int> title, min_title, max_title;

    //Functions
      void
        update(),
        set_active(bool = true),
        draw() const;
      bool pressed() const;

  public:
    //Points, Format, Min, Max, Page, Label, Bcolour, Lcolour
    Slider (int, int, int, int, GUI::Style, direction, int, int, Page&, std::string = "Value", int = 1, Colour = COLOUR(WHITE), Colour = ORANGE);

    //Functions
      int get_value() const;
      void set_value(int);
};

//Text Methods
  template <typename V>
  void Text<V>::update() {
    if (active && value && prev_value != value()) draw();
  }

  template <typename V>
  void Text<V>::update_val() {
    if(value){
      prev_value = value();
      text = sprintf2(label, prev_value);
    }
    else text = label;
  }

  template <typename V>
  void Text<V>::construct (int x, int y, GUI::Style type, text_format_e_t txt_size, Page* page, std::string label, std::function<V() > value, Colour l_col){
    //static_assert(!std::is_same_v<V, std::string>, "Text variable cannot be std::string, it causes unknown failures"); //Keep this for memories
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

//Screen Flash Definitions
  namespace screen_flash{
    template <typename... Params>
    void start(term_colours colour, std::uint32_t time, std::string fmt, Params... args){
      start(sprintf2(fmt, args...), colour, time);
    }
    template <typename... Params>
    void start(Colour colour, std::uint32_t time, std::string fmt, Params... args){
      start(sprintf2(fmt, args...), colour, 1000);
    }
    template <typename... Params>
    void start(std::uint32_t time, std::string fmt, Params... args){
      start(sprintf2(fmt, args...), term_colours::ERROR, time);
    }
    template <typename... Params>
    void start(term_colours colour, std::string fmt, Params... args){
      start(sprintf2(fmt, args...), colour, 1000);
    }
  }

//Text Constructors

  //Terminal (var - no format)
  template <typename V>
  Text<V>::Text (std::string text, V& value_obj, Colour label_colour){
    construct (5, 0, GUI::Style::CORNER, E_TEXT_LARGE_CENTER, &terminal, text, [&](){return value_obj;}, label_colour);
  }

  //Terminal (var - format)
  template <typename V>
  Text<V>::Text (std::string text, V& value_obj, text_format_e_t size, Colour label_colour){
    construct (5, 0, GUI::Style::CORNER, size, &terminal, text, [&](){return value_obj;}, label_colour);
  }

  //Terminal (array - no format)
  template <typename V>
  template <typename I>
  Text<V>::Text (std::string text, V* value_arr, I& index, Colour label_colour){
    construct (5, 0, GUI::Style::CORNER, E_TEXT_LARGE_CENTER, &terminal, text, [value_arr, &index](){return value_arr[static_cast<int>(index) ];}, label_colour);
  }

  //Terminal (array - format)
  template <typename V>
  template <typename I>
  Text<V>::Text (std::string text, V* value_arr, I& index, text_format_e_t size, Colour label_colour){
    construct (5, 0, GUI::Style::CORNER, size, &terminal, text, [value_arr, &index](){return value_arr[static_cast<int>(index) ];}, label_colour);
  }

  //Terminal (function - no format)
  template <typename V>
  Text<V>::Text (std::string text, const std::function<V()>& func, Colour label_colour){
    construct (5, 0, GUI::Style::CORNER, E_TEXT_LARGE_CENTER, &terminal, text, func, label_colour);
  }

  //Terminal (function - format)
  template <typename V>
  Text<V>::Text (std::string text, const std::function<V()>& func, text_format_e_t size, Colour label_colour){
    construct (5, 0, GUI::Style::CORNER, size, &terminal, text, func, label_colour);
  }



  //No var
  template <typename V>
  Text<V>::Text (int x, int y, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, Colour label_colour){
    construct (x, y, rect_type, size, &page, text, nullptr, label_colour);
  }

  //Variable
  template <typename V>
  Text<V>::Text (int x, int y, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, V& value_obj, Colour label_colour){
    construct (x, y, rect_type, size, &page, text, [&](){return value_obj;}, label_colour);
  }

  //Array
  template <typename V>
  template <typename I>
  Text<V>::Text (int x, int y, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, V* value_arr, I& index, Colour label_colour){
    construct (x, y, rect_type, size, &page, text, [value_arr, &index](){return value_arr[static_cast<int>(index) ];}, label_colour);
  }

  //Function
  template <typename V>
  Text<V>::Text (int x, int y, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, const std::function<V() >& func, Colour label_colour){
    construct (x, y, rect_type, size, &page, text, func, label_colour);
  }
