#include "gui.hpp"

//Timer stuff
Timer Flash("Flash Timer", false);
std::uint32_t flash_end; //Sets the flash's end time to max possible val

//Static Variable Declarations
Page* GUI::current_page = nullptr;
bool GUI::touched = false;
int GUI::x = 0, GUI::y = 0;

//Text Vars
char* const go_string = (char*)malloc(90*sizeof(char));

//Default pages

Page perm ("PERM BTNS", COLOUR(PINK)); //Common page objects
Button prev_page(PAGE_LEFT, PAGE_UP, 75, 20, GUI::Style::SIZE, Button::SINGLE, perm, "<-");
Button next_page(480, 0, -75, 20, GUI::Style::SIZE, Button::SINGLE, perm, "->");
Button home(100, 10, 18, 6, GUI::Style::CENTRE, Button::SINGLE, perm, "Home");

Page testing ("Testing"); //Blank page made so it already exists when quick tests are created
_Text testing_text_1 (125, 50, GUI::Style::CENTRE, TEXT_SMALL, testing, "BLANK TEXT 1");
_Text testing_text_2 (350, 50, GUI::Style::CENTRE, TEXT_SMALL, testing, "BLANK TEXT 2");
Button testing_button_1 (25, 70, 200, 80, GUI::Style::SIZE, Button::SINGLE, testing, "BLANK BUTTON 1");
Button testing_button_2 (250, 70, 200, 80, GUI::Style::SIZE, Button::SINGLE, testing, "BLANK BUTTON 2");
Slider testing_slider (MID_X, 200, 200, 20, GUI::Style::CENTRE, Slider::HORIZONTAL, -100, 100, testing, "BLANK SLIDER");

Page go_sequence ("Go Sequence");
Button go_button (300, MID_Y, 160, 90, GUI::Style::CENTRE, Button::SINGLE, go_sequence, "PRESS TO");
Button go_back_button (20, USER_UP, 100, 50, GUI::Style::SIZE, Button::SINGLE, go_sequence, "BACK");
_Text go_button_text (300, 140, GUI::Style::CENTRE, TEXT_SMALL, go_sequence, "%s", go_string, COLOUR(BLACK));

//To get coordinates for aligned objects, (buttons, sliders...) of same size
//Put in how many of buttons/sliders you want, and get properly spaced coords
void GUI::aligned_coords (int x_objects, int y_objects, int x_size, int y_size, int x_range, int y_range){
  double x_space = (x_range-x_objects*x_size)/(x_objects+1.0);
  double y_space = (y_range-y_objects*y_size)/(y_objects+1.0);

  //Recalculates until it gets a nice multiple of 5
  while (fmod(x_space, 5)) x_space = ((--x_range)-x_objects*x_size)/(x_objects+1.0);
  while (fmod(y_space, 5)) y_space = ((--y_range)-y_objects*y_size)/(y_objects+1.0);

  printf("\n\n");
  for (int y = 0; y < y_objects; y++){
    for (int x = 0; x < x_objects; x++){
      printf("(%d, %d, %d, %d, GUI::Style::SIZE)\n", int(x_space)*(x+1) + x_size*x, int(y_space)*(y+1) + y_size*y + 20, x_size, y_size);
    }
    printf("\n");
  }
  printf("\nScreen Size: [%d, %d]\n", x_range, y_range);
  if ((x_space+x_size)*(x_objects) > 480) printf("X out of bounds\n");
  if ((y_space+y_size)*(y_objects) > 220) printf("Y out of bounds\n");
  printf("\n\n");
}

//Flashing
void GUI::flash(Colour colour, std::uint32_t time, const char* fmt, ...){
  std::va_list args;
  va_start(args, fmt);
  GUI::flash(colour, time, printf_to_string(fmt, args));
  va_end(args);
}

void GUI::flash(Colour colour, std::uint32_t time, std::string text){
  clear_screen(colour);
  screen::set_pen(~colour&0xFFFFFF); //Makes text inverted colour of background so it is always visible
  screen::set_eraser(colour);

  printf("\n\n\033[31mWARNING: %s\033[0m\n\n", text.c_str()); //Mike, Convert to log

  int spaces = int(CHAR_WIDTH_LARGE*text.length()/460)+1;
  std::size_t space, last_space=0;
  std::string sub;

  // master.rumble(".-");

  for(int i=1; i <= spaces; i++){ //make the printing actually look good
    space = text.find(' ', text.length()*i/spaces);
    sub = text.substr(last_space, space-last_space);
    screen::print(TEXT_LARGE, (480-sub.length()*CHAR_WIDTH_LARGE)/2, (CHAR_HEIGHT_LARGE+5)*i, sub.c_str());
    last_space = space+1;
  }

  Flash.reset(); //Starts counting
  flash_end = time;
}

void GUI::end_flash (){
  if (Flash.playing() && (Flash.get_time() >= flash_end || touched)){
    Flash.reset(false);
    GUI::current_page->go_to();
  }
}

bool GUI::go(std::string short_msg, std::string long_msg, std::uint32_t delay_time){
 if(!go_enabled) return true;
  printf("\n\n%s\nPress the big button when ready.\n", long_msg.c_str());
  snprintf(go_string, 90, "%s", short_msg.c_str());

  bool pressed = false, interrupted = false;
  Page* page = GUI::current_page;
  go_sequence.go_to();

  while(go_button.pressed() && !interrupted){ //Wait for Release
    GUI::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }
  while(!go_button.pressed() && !interrupted){ //Wait for Press
    drivebase.handle_input();
    GUI::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }
  while(go_button.pressed() && !interrupted){ //Wait for Release
    GUI::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }

  page->go_to();

  if (!interrupted){
    if(delay_time){
      printf("Waiting for %s before running.\n", millis_to_str(delay_time).c_str());
      delay(delay_time);
    }
    printf("Running\n");
  }
  else printf("\033[31mInterrupted\033[0m\n");
  return !interrupted;
}

bool GUI::go_end(std::string msg, std::uint32_t delay_time){
  if(!go_enabled) return true;
  printf("Press Again when done.\n");
  snprintf(go_string, 90, "END %s", msg.c_str());

  bool pressed = false, interrupted = false;
  Page* page = GUI::current_page;
  go_sequence.go_to();

  while(go_button.pressed() && !interrupted){ //Wait for Release
    GUI::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }
  while(!go_button.pressed() && !interrupted){ //Wait for Press
    GUI::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }
  while(go_button.pressed() && !interrupted){ //Wait for Release
    GUI::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }

  page->go_to();

  if (!interrupted){
    if(delay_time){
      printf("Waiting for %s before running.\n", millis_to_str(delay_time).c_str());
      delay(delay_time);
    }
    printf("Running\n");
  }
  else printf("\033[31mInterrupted\033[0m\n");
  return !interrupted;
}

void GUI::draw_oblong(int x1, int y1, int x2, int y2, double kS, double kR){ //ks and kr and scale values for shrink and radius
  int s = std::min(x2-x1, y2-y1)*kS; //Scale for shrinking button (pressed look)
  int r = std::min(x2-x1, y2-y1)*kR; //Scale for how rounded the button edges should be
  screen::fill_rect(x1+s, y1+s, x2-s, y2-s);
  screen::erase_rect(x1+s, y1+s, x1+s+r, y1+s+r);
  screen::erase_rect(x2-s, y1+s, x2-s-r, y1+s+r);
  screen::erase_rect(x1+s, y2-s, x1+s+r, y2-s-r);
  screen::erase_rect(x2-s, y2-s, x2-s-r, y2-s-r);
  screen::fill_circle(x1+s+r, y1+s+r, r);
  screen::fill_circle(x2-s-r, y1+s+r, r);
  screen::fill_circle(x1+s+r, y2-s-r, r);
  screen::fill_circle(x2-s-r, y2-s-r, r);
}

int GUI::get_size(text_format_e_t size, std::string type){
  switch(size){
    case TEXT_SMALL:
      if(type == "height") return CHAR_HEIGHT_SMALL;
      else if(type == "width") return CHAR_WIDTH_SMALL;
      break;
    case TEXT_MEDIUM:
      if(type == "height") return CHAR_HEIGHT_MEDIUM;
      else if(type == "width") return CHAR_WIDTH_MEDIUM;
      break;
    case TEXT_LARGE:
      if(type == "height") return CHAR_HEIGHT_LARGE;
      else if(type == "width") return CHAR_WIDTH_LARGE;
      break;
  }
  return 0;
}

//Formats coordinates based on a GUI::Style (always in x1, y1, x2, y2)
std::tuple<int, int, int, int> GUI::fix_points (int x1, int y1,int x2, int y2, Style type){
  switch(type){
    case GUI::Style::CENTRE:
      x1 -= x2;
      y1 -= y2;
      x2 += x1+x2;
      y2 += y1+y2;
      break;

    case GUI::Style::SIZE:
      x2 += x1;
      y2 += y1;
      break;
  }

  int temp; //Putting coordinates in a left-right up-down order in case it was accidentally reversed or given negatives. Makes checking presses easier

  temp = std::max(x1, x2);
  x1 = std::min(x1, x2);
  x2 = temp;

  temp = std::max(y1, y2);
  y1 = std::min(y1, y2);
  y2 = temp;
  return {x1,y1,x2,y2}; //Gives back the fixed points as a tuple
}


//Constructors

void Button::construct(int x1, int y1, int x2, int y2, GUI::Style type, press_type form, Page* page, std::string text, Colour b_col, Colour l_col){

    //Saves params to class private vars
    this->b_col = b_col;
    this->b_col_dark = RGB2COLOR(int(COLOR2R(b_col)*0.8), int(COLOR2G(b_col)*0.8), int(COLOR2B(b_col)*0.8));
    this->l_col = l_col;
    this->form = form;

    //Saves the buttons owning page
    this->page = page;
    (this->page->buttons).push_back(this);
    std::tie(this->x1, this->y1, this->x2, this->y2) = GUI::fix_points(x1, y1, x2, y2, type);


    std::size_t next_space = text.find(' ', text.length()/2);
    std::size_t prev_space = text.find_last_of(' ', text.length()/2);
    std::size_t space = (next_space-text.length()/2 < text.length()/2-prev_space) ? next_space : prev_space;

    if (space != std::string::npos && 8*text.length()+5 > this->x2-this->x1){ //Spaces it if it's too long for one line
      this->label = text.substr(0, space);
      this->label1 = text.substr(space+1);

      this->text_x = (this->x1+this->x2-(this->label.length()*CHAR_WIDTH_SMALL))/2;
      this->text_x1 = (this->x1+this->x2-(this->label1.length()*CHAR_WIDTH_SMALL))/2;
      this->text_y = (this->y1+this->y2-CHAR_HEIGHT_SMALL)/2-CHAR_HEIGHT_SMALL;
      this->text_y1 = (this->y1+this->y2-CHAR_HEIGHT_SMALL)/2+CHAR_HEIGHT_SMALL;
    }
    else{
      this->text_x = (this->x1+this->x2-(text.length()*CHAR_WIDTH_SMALL))/2;
      this->text_y = (this->y1+this->y2-CHAR_HEIGHT_SMALL)/2;
      this->label = text;
    }
}

GUI::GUI(std::vector<Page*> pages, std::function <void()> setup, std::function <void()> background){
  //Saves pages to gui
  this->pages.push_back(&perm);
  for (std::vector<Page*>::const_iterator it = pages.begin(); it != pages.end(); it++) this->pages.push_back(*it);
  this->pages.push_back(&testing);
  this->pages.push_back(&go_sequence);

  //Saves gui to pages
  for (std::vector<Page*>::const_iterator it = this->pages.begin(); it != this->pages.end(); it++) (*it)->guis.push_back(this);

  this->setup = setup;
  this->background = background;
}

Button::Button(int x1, int y1, int x2, int y2, GUI::Style type, press_type form, Page& page, std::string text, Colour background_colour, Colour label_colour){
  construct(x1, y1, x2, y2, type, form, &page, text, background_colour, label_colour);
}

Slider::Slider (int x1, int y1, int x2, int y2, GUI::Style type, direction dir, int min, int max, Page& page, std::string label, int increment, Colour background_colour, Colour label_colour){
  //Saves params to class private vars
  this->dir = dir;
  this->min = min;
  this->max = max;
  this->page = &page;
  this->val = inRange(0, min, max) ? 0 : (min+max)/2; //0 if that's the min, otherwise the average
  this->b_col = background_colour;
  this->l_col = label_colour;
  this->page->sliders.push_back(this);

  std::tie(this->x1, this->y1, this->x2, this->y2) = GUI::fix_points(x1, y1, x2, y2, type);

  switch(this->dir){
    case HORIZONTAL:
      text_x = (this->x1+this->x2)/2;
      text_y = this->y1-CHAR_HEIGHT_SMALL/2-2;
      inc.construct(this->x2+5, this->y1, this->x2+25, this->y2, GUI::Style::CORNER, Button::SINGLE, this->page, ">", l_col, b_col);
      dec.construct(this->x1-25, this->y1, this->x1-5, this->y2, GUI::Style::CORNER, Button::SINGLE, this->page, "<", l_col, b_col);
      title.construct(text_x, text_y, GUI::Style::CENTRE, TEXT_SMALL, this->page, label + ":%d", [&](){return val;}, l_col);
      min_title.construct(this->x1, text_y, GUI::Style::CENTRE, TEXT_SMALL, this->page, std::to_string(min), nullptr, l_col);
      max_title.construct(this->x2, text_y, GUI::Style::CENTRE, TEXT_SMALL, this->page, std::to_string(max), nullptr, l_col);
      break;

    case VERTICAL:
      text_x = (this->x1+this->x2)/2;
      text_y = (this->y1+this->y2)/2;
      inc.construct(this->x1, this->y1-25, this->x2, this->y1-5, GUI::Style::CORNER, Button::SINGLE, this->page, "▲", l_col, b_col);
      dec.construct(this->x1, this->y2+5, this->x2, this->y2+25, GUI::Style::CORNER, Button::SINGLE, this->page, "▼", l_col, b_col);
      title.construct(text_x, this->y1-27-CHAR_HEIGHT_SMALL, GUI::Style::CENTRE, TEXT_SMALL, this->page, label + ":%d", [&](){return val;}, l_col);
      min_title.construct(this->x1-25, this->y2, GUI::Style::CENTRE, TEXT_SMALL, this->page, std::to_string(min), nullptr, l_col);
      max_title.construct(this->x1-25, this->y1, GUI::Style::CENTRE, TEXT_SMALL, this->page, std::to_string(max), nullptr, l_col);
      break;
  }

  //Buttons
  if(min > max) increment = -increment;
  dec.set_func([&, increment](){this->val-=increment; if(!inRange(this->val, this->min, this->max)) this->val = this->min;});
  inc.set_func([&, increment](){this->val+=increment; if(!inRange(this->val, this->min, this->max)) this->val = this->max;});
}

Page::Page(std::string title, Colour background_colour){
  this->b_col = background_colour;
  this->title = title;
  if (!(title == "PERM BTNS" || title == "Go Sequence")){
    for (std::vector<Button*>::const_iterator it = perm.buttons.begin(); it != perm.buttons.end(); it++) buttons.push_back(*it);
  }
}


//Methods

void GUI::go_to(int page) {current_gui->pages[page]->go_to();}

void Page::go_to(){
  std::vector<Page*>::const_iterator it = std::find(GUI::current_gui->pages.begin(), GUI::current_gui->pages.end(), this); //Makes sure this page exists
  if (it == GUI::current_gui->pages.end()){
    char error [35];
    snprintf(error, 35, "Page %p does not exist!\n", this);
    throw std::invalid_argument(error);
    return;
  }
  GUI::current_page = this; //Saves new page then draws all the buttons on the page
  GUI::clear_screen(b_col);
  screen::set_pen(b_col);
  screen::set_eraser(b_col);
  screen::fill_rect(PAGE_LEFT, PAGE_UP, PAGE_RIGHT, 20);
  screen::set_pen(COLOUR(WHITE));
  std::string page_num = " - " + std::to_string(it-GUI::current_gui->pages.begin());
  screen::print(TEXT_SMALL, MID_X-(title.length()+page_num.length())*CHAR_WIDTH_SMALL/2, 5, "%s%s", title, page_num);
  for (std::vector <Button*>::const_iterator it = buttons.begin(); it != buttons.end(); it++) (*it)->draw();
  for (std::vector <Slider*>::const_iterator it = sliders.begin(); it != sliders.end(); it++) (*it)->draw();
  for (std::vector<Text*>::const_iterator it = texts.begin(); it != texts.end(); it++) (*it)->draw();
  if(setup_func) setup_func();
}

void GUI::clear_screen(Colour colour){
  screen::set_pen(colour);
  screen::fill_rect(PAGE_LEFT, PAGE_UP, PAGE_RIGHT, PAGE_DOWN);
}

void Button::create_options(std::vector<Button*> buttons){
  std::vector<Button*>::const_iterator it, it2; //For clarity

  for (it = buttons.begin(); it != buttons.end(); it++){
    Button* btn_id= *it;
    if (btn_id->form != LATCH && btn_id->form != TOGGLE){
      char error [100];
      snprintf(error, 100, "Option Feature is only available for latch and toggle buttons! Failed on \"%s\" button.\n", btn_id->label.c_str());
      throw std::invalid_argument(error);
      return;
    }
  }

  for (it = buttons.begin(); it != buttons.end(); it++){//For each button in the list
    for (it2 = buttons.begin(); it2 != buttons.end(); it2++){//Go through the list and save each button
      if (*it != *it2) (*it)->options.push_back(*it2);
    }
  }
}

void Button::add_text(Text& text_ref, bool overwrite) {
  if(page != text_ref.page){
    char error [120];
    snprintf(error, 120, "Text can only be linked to a button on the same page! Failed on \"%s\" button and \"%s\" text.\n", label.c_str(), text_ref.label.c_str());
    throw std::invalid_argument(error);
    return;
  }
  title = &text_ref;
  text_ref.l_col = l_col;
  text_ref.b_col = b_col;
  text_ref.active = active;
  text_ref.type = GUI::Style::CENTRE;
  text_ref.x = (x1+x2)/2;
  if (overwrite){
    text_ref.y = (y1+y2)/2;
    label = "";
    label1 = "";
  }
  else{
    text_ref.y = (y1+y2+GUI::get_size(text_ref.txt_fmt, "height"))/2;
    text_y -= GUI::get_size(text_ref.txt_fmt, "height");
  }
  text_ref.y = (y1+y2)/2;
  text_ref.x1=USER_RIGHT;
  text_ref.y1=USER_DOWN;
  text_ref.x2=USER_LEFT;
  text_ref.y2=USER_UP;
  // GUI::clear_screen();
  // GUI::current_page->go_to();
}

void Button::set_background (Colour colour){
  if (title) title->set_background(colour);
  if (b_col != colour){
    b_col = colour;
    b_col_dark = RGB2COLOR(int(COLOR2R(b_col)*0.8), int(COLOR2G(b_col)*0.8), int(COLOR2B(b_col)*0.8));
    if (page == GUI::current_page) draw();
  }
}

void Text::set_background (int x1, int y1, Colour colour){
  if (colour == 0xFFFFFFFF) colour = b_col;
  set_background(x-x1, y-y1, x+x1, y+y1, GUI::Style::CORNER, colour);
}

void Text::set_background (int x1, int y1, int x2, int y2, GUI::Style type, Colour colour){
    if (colour == 0xFFFFFFFF) colour = b_col;
    std::tie(this->x1, this->y1, this->x2, this->y2) = GUI::fix_points(x1, y1, x2, y2, type);
    set_background(colour);
}

void Text::set_background (Colour colour){
  if (b_col != colour){
    b_col = colour;
    if (page == GUI::current_page) draw();
  }
}

void Button::set_active(bool active) {
  this->active = active;
  if (title) title->set_active(active);
  if (active) draw();
  else{
    screen::set_pen(page->b_col);
    screen::fill_rect(x1, y1, x2, y2);
  }
}

void Text::set_active(bool active) {
  this->active = active;
  if (active) draw();
  // else if (page == GUI::current_pge) //Find a good way to "erase" the text;
}


//Drawing

const void Button::draw(){
  if (!active) return;
  if (on) { //on buttons must be drawn in a pressed state
    draw_pressed();
    return;
  }
  screen::set_pen(b_col);
  screen::set_eraser(GUI::current_page->b_col); //at some point figure out why this isn't (page->b_col)
  screen::fill_rect(x1, y1, x2, y2);
  GUI::draw_oblong(x1, y1, x2, y2, 0, 0.15);
  screen::set_pen(l_col);
  screen::set_eraser(b_col);
  screen::print(TEXT_SMALL, text_x, text_y, label.c_str());
  screen::print(TEXT_SMALL, text_x1, text_y1, label1.c_str());
  if(title) title->draw();
}

const void Button::draw_pressed(){
  if (!active) return;
  screen::set_eraser(page->b_col); //Erases button
  screen::erase_rect(x1, y1, x2, y2);

  screen::set_pen(b_col_dark);
  screen::set_eraser(GUI::current_page->b_col);
  GUI::draw_oblong(x1, y1, x2, y2, 0.04, 0.2);

  screen::set_pen(l_col);
  screen::set_eraser(b_col_dark);
  screen::print(TEXT_SMALL, text_x, text_y, label.c_str());
  screen::print(TEXT_SMALL, text_x1, text_y1, label1.c_str());
  if(title) title->draw();
}

const void Slider::draw(){
  screen::set_pen(b_col);
  screen::fill_rect(x1, y1, x2, y2);
  screen::set_pen(l_col);
  if(dir == HORIZONTAL) screen::fill_rect(x1+1, y1+1, map(val, min, max, x1, x2), y2-1);
  else screen::fill_rect(x1+1, map(val, min, max, y2, y1), x2-1, y2-1);
}


//Function Handling

void Page::set_setup_func(std::function <void()> function) {setup_func = function;}

void Page::set_loop_func(std::function <void()> function) {loop_func = function;}

void Button::set_func(std::function <void()> function) {func = function;}

void Button::set_off_func(std::function <void()> function) {off_func = function;}

const void Button::run_func() {if (func) func();}

const void Button::run_off_func() {if (off_func) off_func();}

const int Slider::get_value() {return val;}

//Data Updates
void GUI::update_screen_status(){
  screen_touch_status_s_t status = screen::touch_status();
  touched = status.touch_status == TOUCH_PRESSED || status.touch_status == TOUCH_HELD;
  x = status.x;
  y = status.y;
}

const bool GUI::pressed(){
  return (touched && this == GUI::current_gui);
}

const bool Page::pressed(){
  if (this == GUI::current_page || this == &perm){
    for (std::vector <GUI*>::const_iterator it = guis.begin(); it != guis.end(); it++){
      if ((*it)->pressed()) return true; //If any of it's owning gui's are pressed
    }
  }
  return false;
}

const bool Button::pressed(){
  // returns true if the button is currently being pressed
  return (page->pressed() && active && inRange(GUI::x, x1, x2) && inRange(GUI::y, y1, y2));
}

bool Button::new_press(){
  if(!last_pressed){
    last_pressed = pressed();
    return last_pressed;
  }
  return false;
}

bool Button::new_release(){
  if(last_pressed){
    last_pressed = pressed();
    return !last_pressed;
  }
  return false;
}

void Button::update(){
  if (!active) return;

  switch(form){
    case Button::SINGLE:
      if (new_press()){
        draw_pressed();
        run_func();

      }

      else if (new_release()){
        draw();
        run_off_func();
      }

      break;

    case Button::LATCH:
      if (new_press()){
        on = !on; //Toggles the latch

        //Draws button's new state
        if(on){
          //Deselects connected buttons
          for (std::vector <Button*>::const_iterator option_it = options.begin(); option_it != options.end(); option_it++){
            (*option_it)->on = false;
            (*option_it)->draw();
          }
        }

        draw();
      }

      else if (new_release()){
        draw();
      }

      if(on) run_func();
      else run_off_func();

      break;

    case Button::REPEAT:
      if (new_press()){
        draw_pressed();
        run_func();
      }

      else if (new_release()){
        draw();
        run_off_func();
      }

      if (pressed()) run_func();

      break;

    case Button::TOGGLE:
      if (new_press()){
        on = !on;
        //Draws button's new state
        if(on){
          //Deselects connected buttons
          for (std::vector <Button*>::const_iterator option_it = options.begin(); option_it != options.end(); option_it++){
            (*option_it)->on = false;
            (*option_it)->draw();
          }
          draw_pressed();
          run_func();
        }
        else{ //Just turned off
          run_off_func();
          draw();
        }
      }

      else if (new_release()){
        draw();
      }
      break;
  } //switch
}

void Slider::update(){
  if (page->pressed() && inRange(GUI::x, x1, x2) && inRange(GUI::y, y1, y2)){
    switch (dir){
      case HORIZONTAL:
        val = map(GUI::x, x1, x2, min, max); //Gets val based on press location
        break;

      case VERTICAL:
        val = map(GUI::y, y2, y1, min, max);
        break;
    }
  }
  if (val != prev_val){
    draw();
    prev_val = val;
  }
}

void GUI::update(){
  current_gui->background();
  GUI::update_screen_status();
  Page& cur_p = *current_page;
  /*Page*/if (cur_p.loop_func) cur_p.loop_func();
  /*Button*/for (std::vector <Button*>::const_iterator it = cur_p.buttons.begin(); it != cur_p.buttons.end(); it++) {(*it)->update(); if(&cur_p != current_page) break;}
  /*Slider*/for (std::vector <Slider*>::const_iterator it = cur_p.sliders.begin(); it != cur_p.sliders.end(); it++) (*it)->update();
  /*_Text*/for (std::vector<Text*>::const_iterator it = cur_p.texts.begin(); it != cur_p.texts.end(); it++) (*it)->update();
  /*Flash*/end_flash();
}

void GUI::init(){
  GUI::go_to(0);

  prev_page.set_func([&](){
    if (current_page == current_gui->pages[1]) go_to(current_gui->pages.size()-(test_page ? 2 : 3));
    else (*std::prev(std::find(current_gui->pages.begin(), current_gui->pages.end(), current_page)))->go_to();
  });
  next_page.set_func([&](){
    if (current_page == current_gui->pages[current_gui->pages.size()-(test_page ? 2 : 3)]) go_to(1);
    else (*std::next(std::find(current_gui->pages.begin(), current_gui->pages.end(), current_page)))->go_to();
  });
  home.set_func([](){go_to(1);});
  go_button.add_text(go_button_text);
  current_gui->setup();
  go_to(1); //Sets it to page 1 for program start. Don't delete this. If you want to change the starting page, re-call this in initialize()
  update();
}
