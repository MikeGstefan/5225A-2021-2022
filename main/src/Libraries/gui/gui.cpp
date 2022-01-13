#include "gui.hpp"

//Text Vars
const char* go_string;

//Timer stuff
Timer Flash("Flash Timer", false);
std::uint32_t flash_end = std::numeric_limits<std::uint32_t>::max(); //Sets the flash's end time to max possible val

//Static Variable Declarations
Page* Page::current_page = 0;
std::array<Page*, PAGE_COUNT> Page::pages = {};
last_touch_e_t GUI::touch_status = E_TOUCH_RELEASED;
int GUI::x = 0, GUI::y = 0;

//Default pages
Page perm (0, "PERM BTNS", COLOR_PINK); //Page perm only exists because its a problem if permanent buttons think they belong to every page.
//So they think they belong to page perm, but every page knows they own these buttons.
Button prev_page(PAGE_LEFT, PAGE_UP, 75, 20, Style::SIZE, Button::SINGLE, perm, "<-");
Button next_page(480, 0, -75, 20, Style::SIZE, Button::SINGLE, perm, "->");

//Go sequence
Page go_sequence (PAGE_COUNT+1, "GO SEQUENCE");
Button go_button (300, MID_Y, 160, 90, Style::CENTRE, Button::SINGLE, go_sequence, "PRESS TO");// Hack that works for some reason. Note that all emoji's don't work
Button go_back_button (20, USER_UP, 100, 50, Style::SIZE, Button::SINGLE, go_sequence, "BACK");
_Text go_button_text (300, 140, Style::CENTRE, TEXT_SMALL, go_sequence, "%s", go_string, COLOR(BLACK));

bool GUI::go(std::string name, std::string message, std::uint32_t delay_time){ //Start
  printf("\n\n%s\n", message.c_str());
  printf("Press \"Continue\" when ready.\n");
  go_string = name.c_str();

  bool pressed = false, interrupted = false;
  Page* page = Page::current_page;
  Page::go_to(&go_sequence);

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

  if (!interrupted){
    delay(delay_time);
    printf("Running\n");
  }
  else printf("\033[31mInterrupted\033[0m\n");
  Page::go_to(page);
  return !interrupted;
}

bool GUI::go(std::string name, std::uint32_t delay_time){ //End
  printf("Press Again when done.\n");
  go_string = ("END " + name).c_str();

  bool pressed = false, interrupted = false;
  Page* page = Page::current_page;
  Page::go_to(&go_sequence);

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

  if (!interrupted){
    delay(delay_time);
    printf("Running\n");
  }
  else printf("\033[31mInterrupted\033[0m\n");
  Page::go_to(page);
  return !interrupted;
}

//Utility to get coordinates for aligned objects, (buttons, sliders...) of same size
//Put in how many of buttons/sliders you want, and get properly spaced coords
void GUI::aligned_coords (int x_objects, int y_objects, int x_btn, int y_btn, int x_range, int y_range){

  double x_space = (x_range-x_objects*x_btn)/(x_objects+1.0);
  double y_space = (y_range-y_objects*y_btn)/(y_objects+1.0);

  while (fmod(x_space, 5)) //Recalculates until it gets a nice multiple of 5
    x_space = ((--x_range)-x_objects*x_btn)/(x_objects+1.0);

  while (fmod(y_space, 5))
    y_space = ((--y_range)-y_objects*y_btn)/(y_objects+1.0);

  for (int y = 0; y < y_objects; y++){
    for (int x = 0; x < x_objects; x++){
      printf("(%d, %d, %d, %d, Style::SIZE)\n", int(x_space)*(x+1) + x_btn*x, int(y_space)*(y+1) + y_btn*y + 20, x_btn, y_btn);
    }
    printf("\n");
  }
  printf("\nScreen Size: [%d, %d]\n", x_range, y_range);
  if ((x_space+x_btn)*(x_objects) > 480) printf("X out of bounds\n");
  if ((y_space+y_btn)*(y_objects) > 220) printf("Y out of bounds\n");
  printf("\n\n");
}

//Flashing
void GUI::flash(Color color, std::uint32_t time, std::string text){
  Page::clear_screen(color);
  screen::set_pen(~color&0xFFFFFF); //Makes text inverted color of background so it is always visible
  screen::set_eraser(color);

  printf("\n\n\033[31mWARNING: %s\033[0m\n\n", text.c_str()); //Mike if you see this, convert it to a log.

  int spaces = int(CHAR_WIDTH_LARGE*text.length()/460)+1;
  std::size_t space, last_space=0;
  std::string sub;

  master.rumble("-.-.-.-.");

  for(int i=1; i <= spaces; i++){
    space = text.find(' ', text.length()*i/spaces);
    sub = text.substr(last_space, space-last_space);
    screen::print(TEXT_LARGE, (480-sub.length()*CHAR_WIDTH_LARGE)/2, (CHAR_HEIGHT_LARGE+5)*i, sub.c_str());
    last_space = space+1;
  }

  Flash.reset(); //Starts counting
  flash_end = time;
}

void GUI::end_flash (){
  Flash.reset(false);
  Page::go_to(Page::current_page);
  std::uint32_t flash_end = std::numeric_limits<std::uint32_t>::max(); //Sets it to max val so it'll never flash at weird times like 0
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

//Formats coordinates based on a style (always in x1, y1, x2, y2)
std::tuple<int, int, int, int> fixPoints (int x1, int y1,int x2, int y2, Style type){
  switch(type){
    case Style::CENTRE:
      x1 -= x2;
      y1 -= y2;
      x2 += x1+x2;
      y2 += y1+y2;
      break;

    case Style::SIZE:
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

//Constructors
// _Text::construct();

void Button::construct(int x1, int y1, int x2, int y2, Style type, press_type form, Page* page, std::string text, Color b_col, Color l_col){

    //Saves params to class private vars
    this->b_col = b_col;
    this->b_col_dark = RGB2COLOR(int(COLOR2R(b_col)*0.8), int(COLOR2G(b_col)*0.8), int(COLOR2B(b_col)*0.8));
    this->l_col = l_col;
    this->form = form;

    //Saves the buttons owning page
    this->page = page;
    (this->page->buttons).push_back(this);
    std::tie(this->x1, this->y1, this->x2, this->y2) = fixPoints(x1, y1, x2, y2, type);


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

// _Text::constructor1();
// _Text::constructor2();
// _Text::constructor3();

Button::Button(int x1, int y1, int x2, int y2, Style type, press_type form, Page& page, std::string text, Color background_color, Color label_color){
  construct(x1, y1, x2, y2, type, form, &page, text, background_color, label_color);
}

Slider::Slider (int x1, int y1, int x2, int y2, Style type, direction dir, int min, int max, Page& page, std::string label, Color b_col, Color l_col){
  //Saves params to class private vars
  this->max = max;
  this->min = min;
  this->b_col = b_col;
  this->l_col = l_col;
  this->dir = dir;

  //Saves the buttons owning page
  this->page = &page;
  (this->page->sliders).push_back(this);

  std::tie(this->x1, this->y1, this->x2, this->y2) = fixPoints(x1, y1, x2, y2, type);

  switch(this->dir){
    case HORIZONTAL:
      text_x = (this->x1+this->x2-(label.length()*CHAR_WIDTH_SMALL))/2;
      text_y = this->y1-CHAR_HEIGHT_SMALL/2-2;
      inc.construct(this->x2+5, this->y1, this->x2+25, this->y2, Style::CORNER, Button::SINGLE, this->page, ">", l_col, b_col);
      dec.construct(this->x1-25, this->y1, this->x1-5, this->y2, Style::CORNER, Button::SINGLE, this->page, "<", l_col, b_col);
      title.construct(text_x, text_y, Style::CENTRE, TEXT_SMALL, this->page, label + ":%d", &val, &zero, l_col);
      min_title.construct(this->x1, text_y, Style::CENTRE, TEXT_SMALL, this->page, std::to_string(min), nullptr, &zero, l_col);
      max_title.construct(this->x2, text_y, Style::CENTRE, TEXT_SMALL, this->page, std::to_string(max), nullptr, &zero, l_col);
      break;

    case VERTICAL:
      text_x = (this->x1+this->x2-(label.length()*CHAR_WIDTH_SMALL))/2;
      text_y = (this->y1+this->y2)/2;
      inc.construct(this->x1, this->y1-25, this->x2, this->y1-5, Style::CORNER, Button::SINGLE, this->page, "▲", l_col, b_col);
      dec.construct(this->x1, this->y2+5, this->x2, this->y2+25, Style::CORNER, Button::SINGLE, this->page, "▼", l_col, b_col);
      title.construct(text_x, this->y1-27-CHAR_HEIGHT_SMALL, Style::CENTRE, TEXT_SMALL, this->page, label + ":%d", &val, &zero, l_col);
      min_title.construct(this->x1-25, this->y2, Style::CENTRE, TEXT_SMALL, this->page, std::to_string(min), nullptr, &zero, l_col);
      max_title.construct(this->x1-25, this->y1, Style::CENTRE, TEXT_SMALL, this->page, std::to_string(max), nullptr, &zero, l_col);
      break;
  }

  //Buttons
  dec.set_func([&value=this->val, &mini=this->min](){if(value != mini) value--;});
  inc.set_func([&value=this->val, &maxi=this->max](){if (value != maxi) value++;});
}

Page::Page(int page_num, std::string title, Color b_col){
  //Page Constructor
  //Should call Page::go_to() to actually show the page
  this->page_num = page_num;
  if (this->page_num < PAGE_COUNT) pages[this->page_num] = this;
  this->title = title + " - " + std::to_string(this->page_num);
  this->b_col = b_col;

  buttons.push_back(&prev_page);
  buttons.push_back(&next_page);
}

//Methods
void Page::go_to(Page* page){
  std::array <Page*, PAGE_COUNT>::iterator it = std::find(pages.begin(), pages.end(), page);
  if (it == pages.end() && page != &go_sequence) return;
  clear_screen(page->b_col);
  current_page = page; //Saves new page then draws all the buttons on the page
  screen::set_pen(page->b_col);
  screen::set_eraser(page->b_col);
  screen::fill_rect(75, 0, 405, 20);
  screen::set_pen(COLOR_WHITE);
  screen::print(TEXT_SMALL, MID_X-(page->title.length()*CHAR_WIDTH_SMALL)/2, 5, "%s", page->title);
  for (std::vector <Button*>::iterator it = (page->buttons).begin(); it != (page->buttons).end(); it++) (*it)->draw();
  for (std::vector <Slider*>::iterator it = (page->sliders).begin(); it != (page->sliders).end(); it++) (*it)->draw();
  for (std::vector<Text*>::iterator it = (page->texts).begin(); it != (page->texts).end(); it++) (*it)->draw();
  if(page->setup_func) page->setup_func();
}

void Page::go_to(int page) {go_to(pages[page]);}

void Page::clear_screen(Color color){
  screen::set_pen(color);
  screen::fill_rect(PAGE_LEFT, PAGE_UP, PAGE_RIGHT, PAGE_DOWN);
}

void Button::create_options(std::vector<Button*> buttons){
  std::vector<Button*>::iterator it, it2; //For clarity

  for (it = buttons.begin(); it != buttons.end(); it++){
    Button* btn_id= *it;
    if (btn_id->form != LATCH && btn_id->form != TOGGLE){
      char error [100];
      sprintf(error, "Option Feature is only available for latch and toggle buttons! Failed on \"%s\" button.\n", btn_id->label.c_str());
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

void Button::set_background (Color colour){
  if (title) title->set_background(colour);
  if (b_col != colour){
    b_col = colour;
    b_col_dark = RGB2COLOR(int(COLOR2R(b_col)*0.8), int(COLOR2G(b_col)*0.8), int(COLOR2B(b_col)*0.8));
    if (page == Page::current_page) draw();
  }
}

void Text::set_background (int x1, int y1, Color colour){
  if (colour == 0xFFFFFFFF) colour = b_col;
  set_background(x-x1, y-y1, x+x1, y+y1, Style::CORNER, colour);
}

void Text::set_background (int x1, int y1, int x2, int y2, Style type, Color colour){
    if (colour == 0xFFFFFFFF) colour = b_col;
    std::tie(this->x1, this->y1, this->x2, this->y2) = fixPoints(x1, y1, x2, y2, type);
    set_background(colour);
}

void Text::set_background (Color colour){
  if (b_col != colour){
    b_col = colour;
    if (page == Page::current_page) draw();
  }
}

void Button::draw(){
  if (!active) return;
  if (on) { //on buttons must be drawn in a pressed state
    draw_pressed();
    return;
  }

  screen::set_pen(b_col);
  screen::set_eraser(Page::current_page->b_col); //at some point figure out why this isn't (page->b_col)
  screen::fill_rect(x1, y1, x2, y2);
  GUI::draw_oblong(x1, y1, x2, y2, 0, 0.15);

  screen::set_pen(l_col);
  screen::set_eraser(b_col);
  screen::print(TEXT_SMALL, text_x, text_y, label.c_str());
  screen::print(TEXT_SMALL, text_x1, text_y1, label1.c_str());
  if(title) title->draw();
}

void Button::draw_pressed(){
  if (!active) return;
  screen::set_eraser(page->b_col); //Erases button
  screen::erase_rect(x1, y1, x2, y2);

  screen::set_pen(b_col_dark);
  screen::set_eraser(Page::current_page->b_col);
  GUI::draw_oblong(x1, y1, x2, y2, 0.04, 0.2);

  screen::set_pen(l_col);
  screen::set_eraser(b_col_dark);
  screen::print(TEXT_SMALL, text_x, text_y, label.c_str());
  screen::print(TEXT_SMALL, text_x1, text_y1, label1.c_str());
  if(title) title->draw();
}

void Slider::draw(){
  screen::set_pen(b_col);
  screen::fill_rect(x1, y1, x2, y2);
  screen::set_pen(l_col);
  if(dir == HORIZONTAL){
    screen::fill_rect(x1+1, y1+1, map(val, min, max, x1, x2), y2-1); //Draws Bar
  }
  else{ //Vertical
    screen::fill_rect(x1+1, map(val, min, max, y2, y1), x2-1, y2-1);
  }
}

// _Text::draw();

//Function Handling
void Page::set_setup_func(std::function <void()> function) {setup_func = function;}

void Page::set_loop_func(std::function <void()> function) {loop_func = function;}

void Button::set_func(std::function <void()> function) {func = function;}

void Button::set_off_func(std::function <void()> function) {off_func = function;}

void Button::run_func() {if (func) func();}

void Button::run_off_func() {if (off_func) off_func();}

int Slider::get_val() {return val;}

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
  else if (page == Page::current_page){
    //Find a good way to "erase" the text;
    // Page::clear_screen();
    // Page::go_to(Page::current_page);
  }
}

void Button::add_text(Text& text_ref, bool overwrite) {
  if(page != text_ref.page){
    char error [120];
    sprintf(error, "Text can only be linked to a button on the same page! Failed on \"%s\" button and \"%s\" text.\n", label.c_str(), text_ref.label.c_str());
    throw std::invalid_argument(error);
    return;
  }
  title = &text_ref;
  text_ref.l_col = l_col;
  text_ref.b_col = b_col;
  text_ref.active = active;
  text_ref.type = Style::CENTRE;
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
  Page::clear_screen();
  Page::go_to(Page::current_page);
}

//Updating data and presses
void GUI::update_screen_status(){
  screen_touch_status_s_t status = screen::touch_status();
  touch_status = status.touch_status;
  x = status.x;
  y = status.y;
}

bool GUI::pressed(){
  return (GUI::touch_status == TOUCH_PRESSED || GUI::touch_status == TOUCH_HELD);
}

bool Page::pressed(){
  return (GUI::pressed() && this == Page::current_page);
}

bool Button::pressed(){
  // returns true if the button is currently being pressed
  return (Page::current_page->pressed() && active && inRange(GUI::x, x1, x2) && inRange(GUI::y, y1, y2)); //its curpage and not page because of perm btns
}

bool Button::new_press(){
   if (pressed() && !last_pressed){
     last_pressed = true;
     return true;
   }
   return false;
}

bool Button::new_release(){
  if (!pressed() && last_pressed){
    last_pressed = false;
    return true;
  }
  return false;
}

void GUI::update(){
  GUI::update_screen_status();
  Page* cur_p = Page::current_page;
  /*Page*/if (cur_p->loop_func) cur_p->loop_func();
  /*Button*/for (std::vector <Button*>::iterator it = (cur_p->buttons).begin(); it != (cur_p->buttons).end(); it++) (*it)->update();
  /*Slider*/for (std::vector <Slider*>::iterator it = (cur_p->sliders).begin(); it != (cur_p->sliders).end(); it++) (*it)->update();
  /*_Text*/for (std::vector<Text*>::iterator it = (cur_p->texts).begin(); it != (cur_p->texts).end(); it++) (*it)->update();
  /*Flash*/if (Flash.get_time() >= flash_end) GUI::end_flash();
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
          for (std::vector <Button*>::iterator option_it = options.begin(); option_it != options.end(); option_it++){
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
          for (std::vector <Button*>::iterator option_it = options.begin(); option_it != options.end(); option_it++){
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

// _Text::self_update();

void GUI::setup(){
    prev_page.set_func([&](){
      if (Page::current_page == Page::pages[1]) Page::go_to(PAGE_COUNT-1);
      else Page::go_to((Page::current_page->page_num)-1);
    });
    next_page.set_func([&](){
      if (Page::current_page == Page::pages[PAGE_COUNT-1]) Page::go_to(1);
      else Page::go_to((Page::current_page->page_num)+1);
    });

    go_sequence.buttons.erase(go_sequence.buttons.begin(), go_sequence.buttons.begin()+2);
    go_button.add_text(go_button_text);

    Page::go_to(0);
}
