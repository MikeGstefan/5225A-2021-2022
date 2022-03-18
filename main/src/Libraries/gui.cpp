#include "gui.hpp"

//Timer stuff
Timer Flash("Flash Timer", false);
std::uint32_t flash_end; //Sets the flash's end time to max possible val
bool touched_when_flashed;

//Static Variable Declarations
const Page* GUI::current_page = nullptr;
bool GUI::touched = false;
int GUI::x = 0, GUI::y = 0;
_Task GUI::task(update, "GUI");

//Text Vars
char* const go_string = (char*)malloc(90*sizeof(char));

//Default pages

Page perm ("PERM BTNS", COLOUR(PINK)); //Common page objects
Button prev_page(PAGE_LEFT, PAGE_UP, 75, 20, GUI::Style::SIZE, Button::SINGLE, perm, "<-");
Button next_page(480, 0, -75, 20, GUI::Style::SIZE, Button::SINGLE, perm, "->");
Button home(100, 10, 18, 6, GUI::Style::CENTRE, Button::SINGLE, perm, "Home");

Page testing ("Testing"); //Blank page made so it already exists when quick tests are created
Text<> testing_text_1 (125, 50, GUI::Style::CENTRE, TEXT_SMALL, testing, "BLANK TEXT 1");
Text<> testing_text_2 (350, 50, GUI::Style::CENTRE, TEXT_SMALL, testing, "BLANK TEXT 2");
Button testing_button_1 (25, 70, 200, 80, GUI::Style::SIZE, Button::SINGLE, testing, "BLANK BUTTON 1");
Button testing_button_2 (250, 70, 200, 80, GUI::Style::SIZE, Button::SINGLE, testing, "BLANK BUTTON 2");
Slider testing_slider (MID_X, 200, 200, 20, GUI::Style::CENTRE, Slider::HORIZONTAL, -100, 100, testing, "BLANK SLIDER");

Page go_sequence ("Go Sequence");
Button go_button (300, MID_Y, 160, 90, GUI::Style::CENTRE, Button::SINGLE, go_sequence, "PRESS TO");
Button go_back_button (20, USER_UP, 100, 50, GUI::Style::SIZE, Button::SINGLE, go_sequence, "BACK");
Text go_button_text (300, 140, GUI::Style::CENTRE, TEXT_SMALL, go_sequence, "%s", go_string, COLOUR(BLACK));

Page terminal ("Screen Printing");

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
void GUI::flash(std::uint32_t time, const char* fmt, ...){
  std::va_list args;
  va_start(args, fmt);
  GUI::flash(printf_to_string(fmt, args), time, Colours::ERROR);
  va_end(args);
}

void GUI::flash(GUI::Colours colour, const char* fmt, ...){
  std::va_list args;
  va_start(args, fmt);
  GUI::flash(printf_to_string(fmt, args), 1000, colour);
  va_end(args);
}

void GUI::flash(const char* fmt, ...){
  std::va_list args;
  va_start(args, fmt);
  GUI::flash(printf_to_string(fmt, args), 1000, Colours::ERROR);
  va_end(args);
}

void GUI::flash(GUI::Colours colour, std::uint32_t time, const char* fmt, ...){
  std::va_list args;
  va_start(args, fmt);
  GUI::flash(printf_to_string(fmt, args), time, colour);
  va_end(args);
}

void GUI::flash(std::string text,  std::uint32_t time, Colour colour){
  touched_when_flashed = touched;
  clear_screen(colour);
  screen::set_pen(~colour&0xFFFFFF); //Makes text inverted colour of background so it is always visible
  screen::set_eraser(colour);

  printf("\n\n%s\n\n", text.c_str());
  events.print("\n\n%d | %s\n\n", millis(), text.c_str());

  int spaces = int(CHAR_WIDTH_LARGE*text.length()/460)+1;
  std::size_t space, last_space=0;
  std::string sub;

  master.rumble(".-");

  for(int i=1; i <= spaces; i++){ //make the printing actually look good
    space = text.find(' ', text.length()*i/spaces);
    sub = text.substr(last_space, space-last_space);
    screen::print(TEXT_LARGE, (480-sub.length()*CHAR_WIDTH_LARGE)/2, (CHAR_HEIGHT_LARGE+5)*i, sub.c_str());
    last_space = space+1;
  }

  Flash.reset(); //Starts counting
  flash_end = time;
}


void GUI::flash(std::string text, std::uint32_t time, GUI::Colours colour){
  touched_when_flashed = touched;
  clear_screen(get_gui_colour(colour));
  screen::set_pen(~get_gui_colour(colour)&0xFFFFFF); //Makes text inverted colour of background so it is always visible
  screen::set_eraser(get_gui_colour(colour));

  std::string pretext;
  switch(colour){
    case Colours::ERROR: pretext = "ERROR: "; break;
    case Colours::WARNING: pretext = "WARNING: "; break;
    case Colours::GOOD: pretext = "NOTIF: "; break;
    default: pretext = ""; break;
  }

  printf("\n\n%s%s%s%s\n\n", get_term_colour(colour), pretext.c_str(), text.c_str(), get_term_colour(Colours::NONE));
  events.print("\n\n%d | %s\n\n", millis(), text.c_str());

  int spaces = int(CHAR_WIDTH_LARGE*text.length()/460)+1;
  std::size_t space, last_space=0;
  std::string sub;

  master.rumble(".-");

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
  if (Flash.playing() && (Flash.get_time() >= flash_end || (!touched_when_flashed && touched))){
    Flash.reset(false);
    GUI::current_page->go_to();
  }
}

bool GUI::go(std::string short_msg, std::string long_msg, std::uint32_t delay_time){
 if(!go_enabled) return true;
  master.clear();
  master.print(0, 0, "Press OK btn");
  printf("\n\n%s\nPress the screen big button or the controller OK button when ready.\n", long_msg.c_str());
  snprintf(go_string, 90, "%s", short_msg.c_str());

  bool pressed = false, interrupted = false;
  const Page* page = GUI::current_page;
  go_sequence.go_to();

  //Wait for Release
  wait_until(!(go_button.pressed() || master.get_digital(ok_button) || master.interrupt(false, true, false)) || interrupted){ //checks that no button is being pressed
    GUI::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
  }

  //Wait for Press
  wait_until((go_button.pressed() || master.get_digital(ok_button)) || interrupted){ //waits for a press from go btn or ok btn. Interrupts with any controller digital btn
    GUI::update_screen_status();
    if (go_back_button.pressed() || master.interrupt(false, true, true)) interrupted = true;
  }
  
  //Wait for Release
  wait_until(!(go_button.pressed() || master.get_digital(ok_button) || master.interrupt(false, true, false)) || interrupted){ //checks that no button is being pressed
    GUI::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
  }

  if (!interrupted){
    if(delay_time){
      const char* time = millis_to_str(delay_time);
      printf("\nWaiting for %s before running.\n\n", time);
      delete[] time;
      delay(delay_time);
    }
    printf("\nRunning\n\n");
  }
  else printf("\n%sInterrupted%s\n\n", get_term_colour(Colours::ERROR), get_term_colour(Colours::NONE));

  page->go_to();
  master.clear();
  return !interrupted;
}

bool GUI::go_end(std::string msg, std::uint32_t delay_time){
  if(!go_enabled) return true;
  master.clear();
  master.print(0, 0, "Press OK btn");
  printf("\nPress Again when done.\n\n");
  snprintf(go_string, 90, "END %s", msg.c_str());

  bool pressed = false, interrupted = false;
  const Page* page = GUI::current_page;
  go_sequence.go_to();

  wait_until(!(go_button.pressed() || master.get_digital(ok_button) || master.interrupt(false, true, false)) || interrupted){ //Wait for Release
    if (go_back_button.pressed()) interrupted = true;
  }
  wait_until((go_button.pressed() || master.get_digital(ok_button)) || interrupted){ //Wait for Press
    if (go_back_button.pressed() || master.interrupt(false)) interrupted = true;
  }
  wait_until(!(go_button.pressed() || master.get_digital(ok_button) || master.interrupt(false, true, false)) || interrupted){ //Wait for Release
    if (go_back_button.pressed()) interrupted = true;
  }

  page->go_to();

  if (!interrupted){
    if(delay_time){
      const char* time = millis_to_str(delay_time);
      printf("\nWaiting for %s before running.\n\n", time);
      delete[] time;
      delay(delay_time);
    }
    printf("\nRunning\n\n");
  }
  else printf("\n%sInterrupted%s\n\n", get_term_colour(Colours::ERROR), get_term_colour(Colours::NONE));
  master.clear();
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

int GUI::get_height(text_format_e_t size){
  switch(size){
    case TEXT_SMALL: return CHAR_HEIGHT_SMALL; break;
    case TEXT_MEDIUM: return CHAR_HEIGHT_MEDIUM; break;
    case TEXT_LARGE: return CHAR_HEIGHT_LARGE; break;
  }
  return 0;
}

int GUI::get_width(text_format_e_t size){
  switch(size){
    case TEXT_SMALL: return CHAR_WIDTH_SMALL; break;
    case TEXT_MEDIUM: return CHAR_WIDTH_MEDIUM; break;
    case TEXT_LARGE: return CHAR_WIDTH_LARGE; break;
  }
  return 0;
}

const char* GUI::get_term_colour(GUI::Colours colour){
  switch(colour){
    case Colours::BLACK: return "\033[30m"; break;
    case Colours::RED: return "\033[31m"; break;
    case Colours::GREEN: return "\033[32m"; break;
    case Colours::YELLOW: return "\033[33m"; break;
    case Colours::BLUE: return "\033[34m"; break;
    case Colours::MAGENTA: return "\033[35m"; break;
    case Colours::CYAN: return "\033[36m"; break;
    case Colours::WHITE: return "\033[37m"; break;
    case Colours::NONE: return "\033[0m"; break;
  }
  return "";
}

Colour GUI::get_gui_colour(GUI::Colours colour){
  switch(colour){
    case Colours::BLACK: return COLOUR(BLACK); break;
    case Colours::RED: return COLOUR(RED); break;
    case Colours::GREEN: return COLOUR(GREEN); break;
    case Colours::YELLOW: return COLOUR(YELLOW); break;
    case Colours::BLUE: return COLOUR(BLUE); break;
    case Colours::MAGENTA: return COLOUR(MAGENTA); break;
    case Colours::CYAN: return COLOUR(CYAN); break;
    case Colours::WHITE: return COLOUR(WHITE); break;
    case Colours::NONE: return GREY; break;
  }
  return GREY;
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
    this->text_y = (this->y1+this->y2-CHAR_HEIGHT_SMALL)/2 - CHAR_HEIGHT_SMALL;
    this->text_y1 = (this->y1+this->y2-CHAR_HEIGHT_SMALL)/2 + CHAR_HEIGHT_SMALL;
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
  this->pages.push_back(&terminal);
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
Page* Page::page_id(int page_num){
  return GUI::current_gui->pages[page_num];
}

int Page::page_num(const Page* page_id){
  std::vector<Page*>::const_iterator it = std::find(GUI::current_gui->pages.begin(), GUI::current_gui->pages.end(), page_id);
    if (it == GUI::current_gui->pages.end()){
    char error [35];
    snprintf(error, 35, "Page %p does not exist!\n", page_id);
    throw std::domain_error(error);
    return 0;
  }
  return it-GUI::current_gui->pages.begin();
}

void GUI::go_next(){
  std::vector<Page*>::const_iterator it = std::find(GUI::current_gui->pages.begin(), GUI::current_gui->pages.end(), current_page);
  do{
    if (it == current_gui->pages.end()-2) it = current_gui->pages.begin();
    it++;
  } while(!(Page::page_num(*it) && (*it)->active));
  (*it)->go_to();
}

void GUI::go_prev(){
  std::vector<Page*>::const_iterator it = std::find(GUI::current_gui->pages.begin(), GUI::current_gui->pages.end(), current_page);
  do{
    if (it == current_gui->pages.begin()+1) it = current_gui->pages.end()-1;
    it--;
  } while(!(Page::page_num(*it) && (*it)->active));
  (*it)->go_to();
}

void GUI::go_to(int page_num) {Page::page_id(page_num)->go_to();}

void Page::go_to() const{
  if(!page_num(this)) return;
  GUI::current_page = this; //Saves new page then draws all the buttons on the page
  draw();
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

void Button::add_text(Text_& text_ref, bool overwrite) {
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
    text_ref.y = (y1+y2)/2 + GUI::get_height(text_ref.txt_size);
    text_y -= GUI::get_height(text_ref.txt_size);
  }
  text_ref.y = (y1+y2)/2;
  text_ref.x1=USER_RIGHT;
  text_ref.y1=USER_DOWN;
  text_ref.x2=USER_LEFT;
  text_ref.y2=USER_UP;
}

void Button::set_background (Colour colour){
  if (title) title->set_background(colour);
  if (b_col != colour){
    b_col = colour;
    b_col_dark = RGB2COLOR(int(COLOR2R(b_col)*0.8), int(COLOR2G(b_col)*0.8), int(COLOR2B(b_col)*0.8));
    draw();
  }
}

void Text_::set_background (int x1, int y1, Colour colour){ //Centre
  std::tie(this->x1, this->y1, this->x2, this->y2) = GUI::fix_points(this->x, this->y, x1, y1, GUI::Style::CENTRE);
  set_background(colour);
}

void Text_::set_background (int x1, int y1, int x2, int y2, GUI::Style type, Colour colour){
  std::tie(this->x1, this->y1, this->x2, this->y2) = GUI::fix_points(x1, y1, x2, y2, type);
  set_background(colour);
}

void Text_::set_background (int x1, int y1){ //Centre
  std::tie(this->x1, this->y1, this->x2, this->y2) = GUI::fix_points(this->x, this->y, x1, y1, GUI::Style::CENTRE);
}

void Text_::set_background (int x1, int y1, int x2, int y2, GUI::Style type){
  std::tie(this->x1, this->y1, this->x2, this->y2) = GUI::fix_points(x1, y1, x2, y2, type);
}

void Text_::set_background (Colour colour){
  if (b_col != colour){
    b_col = colour;
    draw();
  }
}

void Page::set_active(bool active) {
  this->active = active;
  if (active) go_to();
  else if (this == GUI::current_page) GUI::go_next();
}

void Button::set_active(bool active) {
  this->active = active;
  if (title) title->set_active(active);
  if (active) draw();
  else if (page == GUI::current_page){
    screen::set_pen(page->b_col);
    screen::fill_rect(x1, y1, x2, y2);
  }
}

void Text_::set_active(bool active) {
  this->active = active;
  if (active) draw();
  else if (page == GUI::current_page){
    screen::set_pen(page->b_col);
    screen::fill_rect(x1, y1, x2, y2);
  }
}

void Slider::set_active(bool active) {
  this->active = active;
  if (active) draw();
  else if (page == GUI::current_page){
    screen::set_pen(page->b_col);
    screen::fill_rect(x1, y1, x2, y2);
  }
}


//Drawing

void Page::draw() const{
  GUI::clear_screen(b_col);
  screen::set_pen(b_col);
  screen::set_eraser(b_col);
  screen::fill_rect(PAGE_LEFT, PAGE_UP, PAGE_RIGHT, 20);
  screen::set_pen(COLOUR(WHITE));
  screen::print(TEXT_SMALL, MID_X-(title.length()+3+std::to_string(page_num(this)).length())*CHAR_WIDTH_SMALL/2, 5, "%s - %d", title, page_num(this));
  for (std::vector<Button*>::const_iterator it = buttons.begin(); it != buttons.end(); it++) (*it)->draw();
  for (std::vector<Slider*>::const_iterator it = sliders.begin(); it != sliders.end(); it++) (*it)->draw();
  for (std::vector<Text_*>::const_iterator it = texts.begin(); it != texts.end(); it++) (*it)->draw();
  if(setup_func) setup_func();
}

void Button::draw() const{
  if (!(active && (page == GUI::current_page || page == &perm))) return;
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

void Button::draw_pressed() const{
  if (!(active && (page == GUI::current_page || page == &perm))) return;
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

void Slider::draw() const{
  if (!(active && (page == GUI::current_page || page == &perm))) return;
  screen::set_pen(b_col);
  screen::fill_rect(x1, y1, x2, y2);
  screen::set_pen(l_col);
  if(dir == HORIZONTAL) screen::fill_rect(x1+1, y1+1, map(val, min, max, x1, x2), y2-1);
  else screen::fill_rect(x1+1, map(val, min, max, y2, y1), x2-1, y2-1);
}

void Text_::draw(){
  if (!(active && (page == GUI::current_page || page == &perm))) return;
  char buffer [100];
  int length = update_val(buffer);

  if (x2 != 0 && y2 != 0){ //If background box exists. Should be able to get rid of this as now it always exists
    screen::set_eraser(page->b_col);
    screen::erase_rect(x1, y1, x2, y2);

    screen::set_pen(b_col);
    GUI::draw_oblong(x1, y1, x2, y2, 0, 0.15);
  }
  screen::set_pen(l_col);
  screen::set_eraser(b_col);

  int x_coord = x, y_coord = y;
  if (type == GUI::Style::CENTRE){
    x_coord -= GUI::get_width(txt_size)/2.0*length;
    y_coord -= GUI::get_height(txt_size)/2.0;
  }
  
  //Resizes the background so it won't have overwriting issues
  x1 = min(x1, x_coord);
  x2 = max(x2, x_coord + (GUI::get_width(txt_size)+1)*length);
  y1 = min(y1, y_coord);
  y2 = max(y2, y_coord + GUI::get_height(txt_size));

  screen::print(txt_size, x_coord, y_coord, "%s", buffer);
}


//Function Handling

void Page::set_setup_func(std::function <void()> function) {setup_func = function;}

void Page::set_loop_func(std::function <void()> function) {loop_func = function;}

void Button::set_func(std::function <void()> function) {func = function;}

void Button::set_off_func(std::function <void()> function) {off_func = function;}

void Button::run_func() const {if (func) func();}

void Button::run_off_func() const {if (off_func) off_func();}

int Slider::get_value() const {return val;}

//Data Updates
void GUI::update_screen_status(){
  screen_touch_status_s_t status = screen::touch_status();
  touched = status.touch_status == TOUCH_PRESSED || status.touch_status == TOUCH_HELD;
  x = status.x;
  y = status.y;
  printf("Updated\n");
}

bool GUI::pressed() const{
  return (touched && this == GUI::current_gui);
}

bool Page::pressed() const{
  if (this == GUI::current_page || this == &perm){
    for (std::vector<GUI*>::const_iterator it = guis.begin(); it != guis.end(); it++){
      if ((*it)->pressed()) return true; //If any of it's owning gui's are pressed
    }
  }
  return false;
}

bool Slider::pressed() const{
  return (page->pressed() && active && inRange(GUI::x, x1, x2) && inRange(GUI::y, y1, y2));
}

bool Button::pressed() const{
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

void Page::update() const{
  if (loop_func) loop_func();
}

void Button::select(){
  switch(form){
    case Button::LATCH:
    case Button::TOGGLE:
      on = true;

      for (std::vector<Button*>::const_iterator option_it = options.begin(); option_it != options.end(); option_it++) (*option_it)->deselect();

    case Button::SINGLE:
    case Button::REPEAT:
      draw_pressed();
      run_func();
      break;
  }
}

void Button::deselect(){
  switch(form){
    case Button::LATCH:
    case Button::TOGGLE:
      on = false;
    case Button::SINGLE:
    case Button::REPEAT:
      draw();
      run_off_func();
      break;
  }
}

void Button::update(){
  if (!active) return;

  switch(form){
    case Button::LATCH:
      if (new_press()){
        on = !on; //Toggles the latch
        if (on) select();
        else deselect();
      }

      if(on) run_func();
      else run_off_func();
      break;
    
    case Button::TOGGLE:
      if (new_press()){
        on = !on; //Toggles the latch
        if (on) select();
        else deselect();
      }
      break;

    case Button::SINGLE:
      if (new_press()) select();
      else if (new_release()) deselect();

      break;

    case Button::REPEAT:
      if (new_press()) select();
      else if (new_release()) deselect();

      if (pressed()) run_func();

      break;
  }
}

void Slider::update(){
  if (!active) return;
  if (pressed()){
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

void GUI::screen_terminal_fix(){
  //Will only run if things are actually being printed
  if(terminal.texts.empty()){
    terminal.set_active(false);
    return;
  }

  //Sees how much space is user-requested
  int y = USER_UP;
  for (std::vector<Text_*>::iterator it = terminal.texts.begin(); it != terminal.texts.end(); it++){
    if ((*it)->txt_size != 4) y += get_height((*it)->txt_size) + 5;
  }

  int size = (PAGE_DOWN - y)/(std::count_if(terminal.texts.begin(), terminal.texts.end(), [](Text_* text){return text->txt_size == 4;})) - 5;
  text_format_e_t fmt;

  if(CHAR_HEIGHT_LARGE <= size) fmt = TEXT_LARGE;
  else if(CHAR_HEIGHT_MEDIUM <= size) fmt = TEXT_MEDIUM;
  else if(CHAR_HEIGHT_SMALL <= size) fmt = TEXT_SMALL;
  else{
    throw std::length_error("Too Many items being printed to screen\n");
    return;
  }

  y = USER_UP;

  //Saves y-pos and txt_size
  for (std::vector<Text_*>::iterator it = terminal.texts.begin(); it != terminal.texts.end(); it++){
    (*it)->y = y;
    if ((*it)->txt_size == 4) (*it)->txt_size = fmt;
    y += get_height((*it)->txt_size) + 5;
  }
}

void GUI::init(){
  screen_terminal_fix();
  go_to(0);

  prev_page.set_func(&go_prev);
  next_page.set_func(&go_next);
  testing.set_active(testing_page_active);

  home.set_func([](){go_to(1);});
  go_button.add_text(go_button_text);

  current_gui->setup();
  if(terminal.active) terminal.go_to();
  else go_to(1); //Sets it to page 1 for program start. Don't delete this. If you want to change the starting page, call GUI::go_to(Page Number) in initialize()
  // update();
  GUI::task.start();
}

void GUI::update(void* params){
  _Task* ptr = _Task::get_obj(params);
  while(true){
    printf("Loop\n");
    current_gui->background();
    update_screen_status();
    const Page& cur_p = *current_page;
    /*Page*/cur_p.update();
    /*Button*/for (std::vector<Button*>::const_iterator it = cur_p.buttons.begin(); it != cur_p.buttons.end(); it++) {(*it)->update(); if(&cur_p != current_page) continue;}
    /*Slider*/for (std::vector<Slider*>::const_iterator it = cur_p.sliders.begin(); it != cur_p.sliders.end(); it++) (*it)->update();
    /*Text*/for (std::vector<Text_*>::const_iterator it = cur_p.texts.begin(); it != cur_p.texts.end(); it++) (*it)->update();
    /*Flash*/end_flash();

    delay(10);
    if(ptr->notify_handle()) break;
  }
  printf("\nGUI Task Ended\n\n");
}