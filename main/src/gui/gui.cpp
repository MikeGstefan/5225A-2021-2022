#include "gui.hpp"

//Text Vars
std::string go_string;

//Timer stuff
Timer Flash("Flash Timer", false);
std::uint32_t flash_end = std::numeric_limits<std::uint32_t>::max(); //Sets the flash's end time to max possible val

//Static Variable Declarations
Page* Page::current_page = 0;
std::array<Page*, PAGE_COUNT> Page::pages = {};
last_touch_e_t Page::touch_status = E_TOUCH_RELEASED;
int16_t Page::x = 0, Page::y = 0;

//Pages and associated stuff
Page perm (0, "PERM BTNS", COLOR_PINK); //Page perm only exists because its a problem if permanent buttons think they belong to every page.
//So they think they belong to page perm, but every page knows they own these buttons.
Button prev_page(PAGE_LEFT, PAGE_UP, 75, 20, Style::SIZE, Button::SINGLE, &perm, "<-");
Button next_page(480, 0, -75, 20, Style::SIZE, Button::SINGLE, &perm, "->");

Page go_sequence (PAGE_COUNT+1, "GO SEQUENCE");
Button go_button (300, MID_Y, 160, 90, Style::CENTRE, Button::SINGLE, &go_sequence, "PRESS TO");
Button go_back_button (20, USER_UP, 100, 50, Style::SIZE, Button::SINGLE, &go_sequence, "BACK");
Text go_button_text (300, 150, Style::CENTRE, TEXT_SMALL, &go_sequence, "%s", &go_string, (std::uint32_t)COLOR_BLACK);

bool go(std::string name, std::string message, std::uint32_t delay_time){ //Start
  printf("\n\n%s\n", message.c_str());
  printf("Press \"Continue\" when ready.\n");
  go_string = name;

  bool pressed = false, interrupted = false;
  Page* page = Page::current_page;
  Page::go_to(&go_sequence);

  while(go_button.pressed() && !interrupted){ //Wait for Release
    Page::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }
  while(!go_button.pressed() && !interrupted){ //Wait for Press
    drivebase.handle_input();
    Page::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }
  while(go_button.pressed() && !interrupted){ //Wait for Release
    Page::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }

  if (!interrupted){
    delay(delay_time);
    printf("Running\n");
  }
  else printf("Interrupted\n");
  Page::go_to(page);
  return !interrupted;
}

bool go(std::string name, std::uint32_t delay_time){ //End
  printf("Press Again when done.\n");
  go_string = "END " + name;

  bool pressed = false, interrupted = false;
  Page* page = Page::current_page;
  Page::go_to(&go_sequence);

  while(go_button.pressed() && !interrupted){ //Wait for Release
    Page::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }
  while(!go_button.pressed() && !interrupted){ //Wait for Press
    drivebase.handle_input();
    Page::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }
  while(go_button.pressed() && !interrupted){ //Wait for Release
    Page::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }

  if (!interrupted){
    delay(delay_time);
    printf("Running\n");
  }
  else printf("Interrupted\n");
  Page::go_to(page);
  return !interrupted;
}

//Utility to get coordinates for aligned objects, (buttons, sliders...) of same size
//Put in how many of buttons/sliders you want, and get properly spaced coords
void aligned_coords (int x_objects, int y_objects, int x_btn, int y_btn, int x_range, int y_range){

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
}

//Flashing
void flash(std::uint32_t color, std::uint32_t time, std::string text){
  Page::clear_screen(color);
  screen::set_pen(~color&0xFFFFFF); //Makes text inverted color of background so it is always visible
  screen::set_eraser(color);

  printf("\n\nWARNING: %s\n\n", text.c_str());

  int spaces = int(CHAR_WIDTH_LARGE*text.length()/460)+1;
  std::size_t space, last_space=0;
  std::string sub;

  master.rumble("-.-.-.-.");

  for(int i=1; i <= spaces; i++){
    space = text.find(' ', text.length()*i/spaces);
    sub = text.substr(last_space, space-last_space);
    screen::print(TEXT_LARGE, (480-sub.length()*CHAR_WIDTH_LARGE)/2, (CHAR_HEIGHT_LARGE+5)*(i-1), sub.c_str());
    last_space = space+1;
  }

  Flash.reset(); //Starts counting
  flash_end = time;
}

void end_flash (){
  if (Flash.get_time() >= flash_end){
    Flash.reset(false);
    Page::go_to(Page::current_page);
    std::uint32_t flash_end = std::numeric_limits<std::uint32_t>::max(); //Sets it to max val so it'll never flash at weird times like 0
  }
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

  //Putting coordinates in a left-right up-down order in case it was accidentally reversed or given negatives. Makes checking presses easier
  int temp;

  temp = std::max(x1, x2);
  x1 = std::min(x1, x2);
  x2 = temp;

  temp = std::max(y1, y2);
  y1 = std::min(y1, y2);
  y2 = temp;
  return {x1,y1,x2,y2}; //Gives back the fixed points as a tuple
}

void draw_oblong(int16_t x1, int16_t y1, int16_t x2, int16_t y2, double kS, double kR){ //ks and kr and scale values for shrink and radius
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
void Text::construct (int16_t x, int16_t y, Style type, text_format_e_t txt_fmt, Page* page, std::string label, std::variant<std::monostate, double*, int*, std::string*> val_ptr, std::uint32_t l_col){
  this->x = x;
  this->y = y;
  this->type = type;
  this->txt_fmt = txt_fmt;
  this->page = page;
  this->label = label;
  this->val_ptr = val_ptr;
  this->l_col = l_col;
  this->b_col = page->b_col;
  this->page->texts.push_back(this);
}

void Button::construct(int16_t x1, int16_t y1, int16_t x2, int16_t y2, Style type, press_type form, Page* page, std::string text, std::uint32_t b_col, std::uint32_t l_col){

    //Saves params to class private vars
    this->b_col = b_col;
    this->b_col_dark = RGB2COLOR(int(COLOR2R(b_col)*0.8), int(COLOR2G(b_col)*0.8), int(COLOR2B(b_col)*0.8));
    this->l_col = l_col;
    this->form = form;

    //Saves the buttons owning page
    this->page = page;
    (this->page->buttons).push_back(this);
    std::tie(this->x1, this->y1, this->x2, this->y2) = fixPoints(x1, y1, x2, y2, type);

    if (8*text.length()+5 < this->x2-this->x1){
      this->text_x = (this->x1+this->x2-(text.length()*CHAR_WIDTH_SMALL))/2;
      this->text_y = (this->y1+this->y2-CHAR_HEIGHT_SMALL)/2;
      this->label = text;
    }
    else{ //Spaces it if it's too long for one line
      std::size_t space = text.find(' ', text.length()/2);
      if (space != std::string::npos){
        this->label = text.substr(0, space);
        this->label1 = text.substr(space+1);

        this->text_x = (this->x1+this->x2-(this->label.length()*CHAR_WIDTH_SMALL))/2;
        this->text_x1 = (this->x1+this->x2-(this->label1.length()*CHAR_WIDTH_SMALL))/2;
        this->text_y = (this->y1+this->y2-CHAR_HEIGHT_SMALL)/2-CHAR_HEIGHT_SMALL;
        this->text_y1 = (this->y1+this->y2-CHAR_HEIGHT_SMALL)/2+CHAR_HEIGHT_SMALL;
      }
    }
}

Text::Text (int16_t x, int16_t y, Style rect_type, text_format_e_t size, Page* page, std::string text, std::uint32_t label_color):
val_type(typeid(std::monostate)){
  construct(x, y, rect_type, size, page, text, std::monostate{}, label_color);
}

Text::Text (int16_t x, int16_t y, Style rect_type, text_format_e_t size, Page* page, std::string text, double* val_ref, std::uint32_t label_color):
val_type(typeid(double*)){
  construct(x, y, rect_type, size, page, text, val_ref, label_color);
}

Text::Text (int16_t x, int16_t y, Style rect_type, text_format_e_t size, Page* page, std::string text, int* val_ref, std::uint32_t label_color):
val_type(typeid(int*)){
  construct(x, y, rect_type, size, page, text, val_ref, label_color);
}

Text::Text (int16_t x, int16_t y, Style rect_type, text_format_e_t size, Page* page, std::string text, std::string* val_ref, std::uint32_t label_color):
val_type(typeid(std::string*)){
  construct(x, y, rect_type, size, page, text, val_ref, label_color);
}

Button::Button(int16_t x1, int16_t y1, int16_t x2, int16_t y2, Style type, press_type form, Page* page, std::string text, std::uint32_t background_color, std::uint32_t label_color){
  construct(x1, y1, x2, y2, type, form, page, text, background_color, label_color);
}

Slider::Slider (int16_t x1, int16_t y1, int16_t x2, int16_t y2, Style type, direction dir, int min, int max, Page* page, std::string label, std::uint32_t b_col, std::uint32_t l_col){
  //Saves params to class private vars
  this->max = max;
  this->min = min;
  this->b_col = b_col;
  this->l_col = l_col;
  this->label = label;
  this->dir = dir;

  //Saves the buttons owning page
  this->page = page;
  (this->page->sliders).push_back(this);

  std::tie(this->x1, this->y1, this->x2, this->y2) = fixPoints(x1, y1, x2, y2, type);

  switch(this->dir){
    case HORIZONTAL:
      this->text_x = (this->x1+this->x2-(this->label.length()*CHAR_WIDTH_SMALL))/2;
      this->text_y = this->y1-CHAR_HEIGHT_SMALL-2;
      inc.construct(this->x2+5, this->y1, this->x2+25, this->y2, Style::CORNER, Button::SINGLE, this->page, ">", this->l_col, this->b_col);
      dec.construct(this->x1-25, this->y1, this->x1-5, this->y2, Style::CORNER, Button::SINGLE, this->page, "<", this->l_col, this->b_col);
      break;

    case VERTICAL:
      this->text_x = (this->x1+this->x2-(this->label.length()*CHAR_WIDTH_SMALL))/2;
      this->text_y = (this->y1+this->y2)/2;
      inc.construct(this->x1, this->y1-25, this->x2, this->y1-5, Style::CORNER, Button::SINGLE, this->page, "▲", this->l_col, this->b_col);
      dec.construct(this->x1, this->y2+5, this->x2, this->y2+25, Style::CORNER, Button::SINGLE, this->page, "▼", this->l_col, this->b_col);
      break;
  }

  //Buttons
  dec.set_func([&value=this->val, &mini=this->min](){if(value != mini) value--;});
  inc.set_func([&value=this->val, &maxi=this->max](){if (value != maxi) value++;});
}

Page::Page(int page_num, std::string title, std::uint32_t b_col){
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

void Page::clear_screen(std::uint32_t color){
  screen::set_pen(color);
  screen::fill_rect(PAGE_LEFT, PAGE_UP, PAGE_RIGHT, PAGE_DOWN);
}

void Text::set_background (int16_t x1, int16_t y1, std::uint32_t colour){
  set_background(x-x1, y-y1, x+x1, y+y1, Style::CORNER, colour);
}

void Text::set_background (int16_t x1, int16_t y1, int16_t x2, int16_t y2, Style type, std::uint32_t colour){
    std::tie(this->x1, this->y1, this->x2, this->y2) = fixPoints(x1, y1, x2, y2, type);
    set_background(colour);
}

void Text::set_background (std::uint32_t colour){
  if (b_col != colour){
    b_col = colour;
    if (page == Page::current_page) draw();
  }
}

void Text::draw(){
  if (x2 != 0 && y2 != 0){
    screen::set_eraser(page->b_col); //Erases previous box
    screen::erase_rect(x1, y1, x2, y2);

    screen::set_pen(b_col);
    draw_oblong(x1, y1, x2, y2, 0, 0.15);
    screen::set_pen(l_col);
    screen::set_eraser(b_col);
  }
  else{
    screen::set_pen(l_col);
    screen::set_eraser(b_col);
  }

  char buffer [100];
  int length;

  if(val_type == typeid(double*)){
    length = sprintf(buffer, label.c_str(), *std::get<double*>(val_ptr));
    prev_val = *std::get<double*>(val_ptr);
  }

  else if (val_type == typeid(int*)){
    length = sprintf(buffer, label.c_str(), *std::get<int*>(val_ptr));
    prev_val = *std::get<int*>(val_ptr);
  }

  else if (val_type == typeid(std::string*)){
    length = sprintf(buffer, label.c_str(), (*std::get<std::string*>(val_ptr)).c_str());
    prev_val = *std::get<std::string*>(val_ptr);
  }

  else if (val_type == typeid(std::monostate)){
    length = sprintf(buffer, label.c_str());
  }

  int x_coord = x, y_coord = y;
  if (type == Style::CENTRE){
      switch(txt_fmt){
        case TEXT_SMALL:
          x_coord = x - (length*CHAR_WIDTH_SMALL)/2;
          y_coord = y - CHAR_HEIGHT_SMALL/2;
          break;

        case TEXT_MEDIUM:
          x_coord = x - (length*CHAR_WIDTH_MEDIUM)/2;
          y_coord = y - CHAR_HEIGHT_MEDIUM/2;
          break;

        case TEXT_LARGE:
          x_coord = x - (length*CHAR_WIDTH_LARGE)/2;
          y_coord = y - CHAR_HEIGHT_LARGE/2;
          break;
      }
    }

  screen::print(txt_fmt, x_coord, y_coord, "%s", buffer);
}

void Slider::draw(){
  screen::set_pen(b_col);
  screen::set_eraser(page->b_col);
  screen::erase_rect(x1, text_y, x2, y2);
  screen::fill_rect(x1, y1, x2, y2);
  screen::set_pen(l_col);
  if(dir == HORIZONTAL){
    screen::fill_rect(x1+1, y1+1, x1+(x2-x1)*(val-min)/(max-min), y2-1); //Draws Bar
    screen::print(TEXT_SMALL, x1, text_y, "%d", min); //Writes min
    screen::print(TEXT_SMALL, x2-CHAR_WIDTH_SMALL*std::to_string(max).length(), text_y, "%d", max); //Writes max
    screen::print(TEXT_SMALL, text_x, text_y, "%s:%.f", label.c_str(), val); //Writes current value
  }
  else{ //Vertical
    screen::fill_rect(x1+1, y2-(y2-y1)*(val-min)/(max-min), x2-1, y2-1);
    screen::print(TEXT_SMALL, x1-25, y2, "%d", min);
    screen::print(TEXT_SMALL, x1-25, y1, "%d", max);
    screen::print(TEXT_SMALL, text_x, y1-27-CHAR_HEIGHT_SMALL, "%s:%.f", label.c_str(), val);
  }
}

void Slider::draw_bar(){
  screen::set_eraser(b_col);
  screen::erase_rect(x1, y1, x2, y2);
  screen::set_pen(l_col);

  switch(dir){
    case HORIZONTAL:
      screen::fill_rect(x1+1, y1+1, x1+(x2-x1)*(val-min)/(max-min), y2-1);
      screen::print(TEXT_SMALL, text_x, text_y, "%s:%.f", label.c_str(), val);
      break;

    case VERTICAL:
      screen::fill_rect(x1+1, y2-(y2-y1)*(val-min)/(max-min), x2-1, y2-1);
      screen::print(TEXT_SMALL, text_x, y1-27-CHAR_HEIGHT_SMALL, "%s:%.f", label.c_str(), val);
      break;
  }
}

void Button::draw(){
  if (latched) { //Latched buttons must be drawn in a pressed state
    draw_pressed();
    return;
  }

  screen::set_pen(b_col);
  screen::set_eraser(Page::current_page->b_col);
  screen::fill_rect(x1, y1, x2, y2);
  draw_oblong(x1, y1, x2, y2, 0, 0.15);

  screen::set_pen(l_col);
  screen::set_eraser(b_col);
  screen::print(TEXT_SMALL, text_x, text_y, label.c_str());
  screen::print(TEXT_SMALL, text_x1, text_y1, label1.c_str());
}

void Button::draw_pressed(){
  screen::set_eraser(page->b_col); //Erases button
  screen::erase_rect(x1, y1, x2, y2);

  screen::set_pen(b_col_dark);
  screen::set_eraser(Page::current_page->b_col);
  draw_oblong(x1, y1, x2, y2, 0.04, 0.2);

  screen::set_pen(l_col);
  screen::set_eraser(b_col_dark);
  screen::print(TEXT_SMALL, text_x, text_y, label.c_str());
  screen::print(TEXT_SMALL, text_x1, text_y1, label1.c_str());
}

void Button::create_options(std::vector<Button*> buttons){
  std::vector<Button*>::iterator it, it2; //For clarity

  for (it = buttons.begin(); it != buttons.end(); it++){
    if ((*it)->form != LATCH && (*it)->form != TOGGLE) {printf("Option Feature is only available for latch and toggle buttons!\n"); return;}
  }

  for (it = buttons.begin(); it != buttons.end(); it++){//For each button in the list
    for (it2 = buttons.begin(); it2 != buttons.end(); it2++){//Go through the list and save each button
      if (*it != *it2) (*it)->options.push_back(*it2);
    }
  }
}


//Function Handling
void Page::set_setup_func(std::function <void()> function) {setup_func = function;}

void Page::set_loop_func(std::function <void()> function) {loop_func = function;}

void Button::set_func(std::function <void()> function) {func = function;}

void Button::set_off_func(std::function <void()> function) {off_func = function;}

void Button::run_func() {if (func) func();}

//Updating data and presses
void Page::update_screen_status(){
  screen_touch_status_s_t status = c::screen_touch_status();
  touch_status = status.touch_status;
  x = status.x;
  y = status.y;
}

void Slider::update_val(){
  // changes value if being touched
  if (page->pressed()){
    if ((x1 <= Page::x && Page::x <= x2) && (y1 <= Page::y && Page::y <= y2)){
      if (dir == HORIZONTAL) val = min+(max-min)*(Page::x-x1)/(x2-x1); //Gets val based on press location
      else val = min+(max-min)*(y2-Page::y)/(y2-y1);
    }
  }
}

bool Page::pressed(){
  if (this == Page::current_page){
    if (Page::touch_status == TOUCH_PRESSED || Page::touch_status == TOUCH_HELD) return true;
  }
  return false;
}

bool Button::pressed(){
  // returns true if the button is currently being pressed
  if (Page::current_page->pressed()){
    if ((x1 <= Page::x && Page::x <= x2) && (y1 <= Page::y && Page::y <= y2)) return true;
  }
  return false;
}

bool Button::new_press(){
  return (pressed() && !last_pressed);
}

bool Button::new_release(){
  return (!pressed() && last_pressed);
}

void Page::update(){
  Page::update_screen_status();
  if (current_page->loop_func) current_page->loop_func();

  Button::update();
  Slider::update();
  Text::update();
}

void Text::update(){
  for (std::vector<Text*>::iterator it = (Page::current_page->texts).begin(); it != (Page::current_page->texts).end(); it++){
    Text* text_id = *it;

    if(text_id->val_type != typeid(std::monostate)){ //If variable given
      if(text_id->val_type == typeid(double*)){
        if(std::get<double>(text_id->prev_val) != *std::get<double*>(text_id->val_ptr)) text_id->draw(); //If var has changed value
      }

      else if(text_id->val_type == typeid(int*)){
        // printf("U-%s: %d     %d\n", (text_id->label).c_str(), *std::get<int*>(text_id->val_ptr),   (std::get<int>(text_id->prev_val) != *std::get<int*>(text_id->val_ptr)));
        if(std::get<int>(text_id->prev_val) != *std::get<int*>(text_id->val_ptr)) text_id->draw(); //If var has changed value
      }

      else if(text_id->val_type == typeid(std::string*)){
        if(std::get<std::string>(text_id->prev_val) != *std::get<std::string*>(text_id->val_ptr)) text_id->draw(); //If var has changed value
      }

    }

  }
}

Slider* Slider::update(){
  for (std::vector <Slider*>::iterator it = (Page::current_page->sliders).begin(); it != (Page::current_page->sliders).end(); it++){
    Slider* sl_id = *it;
    sl_id->update_val();
    if (sl_id->val != sl_id->prev_val){
      sl_id->draw();
      sl_id->prev_val = sl_id->val;
      return sl_id;
    }
  }
  return 0;
}

Button* Button::update(){
  //Loops through the list of buttons on the current page to check for presses
  for (std::vector <Button*>::iterator it = (Page::current_page->buttons).begin(); it != (Page::current_page->buttons).end(); it++){
    Button* btn_id = *it;

    switch(btn_id->form){
      case Button::SINGLE:
        if (btn_id->new_press()){
          btn_id->last_pressed = 1;
          btn_id->draw_pressed();
          btn_id->run_func();
          return btn_id;
        }

        else if (btn_id->new_release()){
          btn_id->last_pressed = 0;
          btn_id->draw();
        }
        break;

      case Button::LATCH:
        if (btn_id->new_press()){
          btn_id->last_pressed = 1;
          btn_id->latched = !btn_id->latched; //Toggles the latch

          //Draws button's new state
          if(btn_id->latched){
            //Deselects connected buttons
            for (std::vector <Button*>::iterator option_it = (btn_id->options).begin(); option_it != (btn_id->options).end(); option_it++){
              (*option_it)->latched = 0;
              (*option_it)->draw();
            }

            btn_id->draw_pressed();
            btn_id->run_func();

          }
          else btn_id->draw(); //New press deselected it
          return btn_id;
        }

        else if (btn_id->new_release()){
          btn_id->last_pressed = 0;
        }
        if(btn_id->latched) btn_id->run_func();
        break;

      case Button::REPEAT:
        if (btn_id->new_press()){
          btn_id->last_pressed = 1;
          btn_id->draw_pressed();
          btn_id->run_func();
          return btn_id;
        }

        else if (btn_id->new_release()){
          btn_id->last_pressed = 0;
          btn_id->draw();
        }

        if (btn_id->pressed()) btn_id->run_func();
        break;

      case Button::TOGGLE:
        if (btn_id->new_press()){
          btn_id->last_pressed = 1;
          btn_id->on = !btn_id->on;

          //Draws button's new state
          if(btn_id->on){ //Just turned on
            //Turns off connected buttons
            for (std::vector <Button*>::iterator option_it = (btn_id->options).begin(); option_it != (btn_id->options).end(); option_it++){
              if ((*option_it)->on){ //If the other button is in the on state and has an off function
                (*option_it)->on = false;
                if ((*option_it)->off_func) (*option_it)->off_func();
              }
            }

            btn_id->run_func();
          }
          else if (btn_id->off_func) btn_id->off_func(); //Just turned off

          btn_id->draw_pressed();
          return btn_id;
        }

        else if (btn_id->new_release()){
          btn_id->last_pressed = 0;
          btn_id->draw();
        }
        break;
    }

  }

  return 0;
}

void gui_general_setup(){
    prev_page.set_func(&Page::to_prev); //Gives the left and right buttons their functions
    next_page.set_func(&Page::to_next); //Same thing for all following buttons

    go_sequence.buttons.erase(go_sequence.buttons.begin(), go_sequence.buttons.begin()+2);
    go_button_text.set_background(ORANGE);
}

void gui_general_background(){
  Page::update();
  end_flash();
}
