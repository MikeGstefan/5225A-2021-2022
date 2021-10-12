#include "gui.hpp"

//Static Variable Declarations
Page* Page::currentPage = 0;
std::array<Page*, 11> Page::pages = {};
last_touch_e_t Page::touch_status = E_TOUCH_RELEASED;
int16_t Page::x = 0, Page::y = 0;

//Base Pages
Page perm (0, "PERM BTNS", COLOR_PINK); //Page perm only exists because its a problem if permanent buttons think they belong to every page.
//So they think they belong to page perm, but every page knows they own these buttons.
Page liftElastic (1, "Lift - Elastic Test"); //Testing the elastics on the lift
Page liftMove (2, "Lift - Moving"); //Moving the lift to an xyz position
Page track (3, "Tracking", COLOR_BLUE); //Display tracking vals and reset btns
Page autoSel (4, "Auton"); //Select auton routes
Page driverCurve (5, "Drivers"); //Select a driver and their exp curve
Page intkTest (6, "Intake"); //Test for intake with rings
Page temps (7, "Temperature"); //Motor temps
Page mContr (8, "Motor Control");

//Default Buttons
Button prevPage(0, 0, 75, 20, Style::SIZE, Button::SINGLE, &perm, "<-");
Button nextPage(480, 0, -75, 20, Style::SIZE, Button::SINGLE, &perm, "->");

//track
Text trackX(60, 25, TEXT_SMALL, &track, "X:%.1f", &tracking.x_coord);
Text trackY(205, 25, TEXT_SMALL, &track, "Y:%.1f", &tracking.y_coord);
Text trackA(355, 25, TEXT_SMALL, &track, "A:%.1f", &angle);
Button resX(25, 45, 75, 60, Style::SIZE, Button::SINGLE, &track, "Reset X", COLOR_RED, COLOR_BLACK);
Button resY(125, 45, 75, 60, Style::SIZE, Button::SINGLE, &track, "Reset Y", COLOR_RED, COLOR_BLACK);
Button resA(225, 45, 75, 60, Style::SIZE, Button::SINGLE, &track, "Reset A", COLOR_RED, COLOR_BLACK);
Slider xVal(30, 150, 60, 30, Style::SIZE, 0, 144, &track, "X", COLOR_WHITE, COLOR_GREEN);
Slider yVal(135, 150, 60, 30, Style::SIZE, 0, 144, &track, "Y", COLOR_WHITE, COLOR_YELLOW);
Slider aVal(240, 150, 60, 30, Style::SIZE, 0, 144, &track, "A", COLOR_WHITE, COLOR_YELLOW);
Button goToXYA(Page::mid_x, 210, 40, 25, Style::CENTRE, Button::SINGLE, &track, "Move To",COLOR_RED, COLOR_BLACK);

void draw_field(){
  std::uint32_t curTime = millis();
  screen::set_pen(COLOR_BLACK);
  screen::draw_rect(318, 48, 462, 192);
  screen::draw_line(390, 48, 390, 192);
  screen::draw_line(318, 120, 462, 120);
  screen::set_pen(COLOR_RED);
  screen::draw_pixel(318+tracking.x_coord, 192-tracking.y_coord);
}

//temps
Text tempfl(25, 100, TEXT_SMALL, &temps, "Front Left: %.1f", &flTemp);
Text tempbl(25, 200, TEXT_SMALL, &temps, "Back Left: %.1f", &blTemp);
Text tempfr(250, 100, TEXT_SMALL, &temps, "Front Right: %.1f", &frTemp);
Text tempbr(250, 200, TEXT_SMALL, &temps, "Back Right: %.1f", &brTemp);

//Functions
void Page::toPrev(){
  if (currentPage == pages[1]) Page::goTo(pages[8]);
  else goTo(pages[currentPage->pageNum-1]);
}
void Page::toNext(){
  if (currentPage == pages[8]) Page::goTo(pages[1]);
  else goTo(pages[(currentPage->pageNum)+1]);
}

//Var init for text monitoring
double flTemp, blTemp, frTemp, brTemp, angle;

void guiSetup(){
  prevPage.func = &(Page::toPrev);
  nextPage.func = &(Page::toNext);

  resX.func = [&coord=tracking.x_coord](){coord = 0.0;};
  resY.func = [&coord=tracking.y_coord](){coord = 0.0;};
  resA.func = [&coord=tracking.global_angle](){coord = 0.0;};
  goToXYA.func = [x=xVal.val, y=yVal.val, a=aVal.val](){delay(1000); move_to_target_sync(x, y, a, false);};

  track.func = &draw_field;
}

void flash(std::uint32_t color, std::uint32_t time){ //has a delay
  Page::clearScreen(color);
  delay(time);
  Page::goTo(Page::currentPage);
}

std::tuple<int, int, int, int> fixPoints (int p1, int p2,int p3, int p4, Style::style type){
  int x1=p1, x2=p2, y1=p3, y2=p4, temp;

  // Different styles of evaluating the given coordinates
  if (type == Style::CENTRE) {
    x1 -= p3;
    y1 -= p4;
    x2 += p1;
    y2 += p2;
  }

  if (type == Style::SIZE) {
    x2 += x1;
    y2 += y1;
  }

  //Putting coordinates in a left-right up-down order
  temp = std::max(x1, x2);
  x1 = std::min(x1, x2);
  x2 = temp;

  temp = std::max(y1, y2);
  y1 = std::min(y1, y2);
  y2 = temp;

  return {x1,y1,x2,y2};
}

//Constructors
Text::Text (int16_t pt1, int16_t pt2, text_format_e_t size, Page* page_ptr, std::string text, std::uint32_t label_color){
  construct(pt1, pt2, size, page_ptr, text,label_color);
}

Text::Text (int16_t pt1, int16_t pt2, text_format_e_t size, Page* page_ptr, std::string text, double* val_ref, std::uint32_t label_color){
  construct(pt1, pt2, size, page_ptr, text,label_color);
  val_ptr = val_ref;
}

void Text::construct (int16_t pt1, int16_t pt2, text_format_e_t size, Page* page_ptr, std::string text, std::uint32_t label_color){
  x = pt1;
  y = pt2;
  txt_fmt = size;
  page = page_ptr;
  label = text;
  lcol = label_color;

  (page->texts).push_back(this);

  if(Page::currentPage == page || &perm == page) draw();
}

Button::Button(int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style::style type, press_type prType, Page* page_ptr, std::string text, std::uint32_t background_color, std::uint32_t label_color){
  construct(pt1, pt2, pt3, pt4, type, prType, page_ptr, text, background_color, label_color);
}

void Button::construct(int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style::style type, press_type prType, Page* page_ptr, std::string text, std::uint32_t background_color, std::uint32_t label_color){

    //Saves params to class private vars
    bcol = background_color;
    dark_bcol = RGB2COLOR(int(COLOR2R(bcol)*0.9), int(COLOR2G(bcol)*0.9), int(COLOR2B(bcol)*0.9));
    lcol = label_color;
    label = text;
    form = prType;

    //Saves the buttons owning page
    page = page_ptr;
    (page->buttons).push_back(this);

    std::tie(x1, y1, x2, y2) = fixPoints(pt1, pt2, pt3, pt4, type);
    text_x = (x1+x2-(label.length()*Page::char_width))/2;
    text_y = (y1+y2-Page::char_height)/2;


    if(Page::currentPage == page || &perm == page) draw();
}

Slider::Slider (int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style::style type, int minimum, int maximum, Page* page_ptr, std::string text, std::uint32_t background_color, std::uint32_t label_color){
  //Saves params to class private vars
  max = maximum;
  min = minimum;
  bcol = background_color;
  lcol = label_color;
  label = text;

  //Saves the buttons owning page
  page = page_ptr;
  (page->sliders).push_back(this);

  std::tie(x1, y1, x2, y2) = fixPoints(pt1, pt2, pt3, pt4, type);
  text_x = (x1+x2-(label.length()*Page::char_width))/2;
  text_y = (y1+y2-Page::char_height)/2-25;

  slideLeft.construct(x1-15, y1, x1-1, y2, Style::CORNER, Button::SINGLE, page, "<", bcol, lcol);
  slideRight.construct(x1+1, y1, x1+15, y2, Style::CORNER, Button::SINGLE, page, ">", bcol, lcol);
  slideLeft.func = [&value=val](){if(value != 0) value--;};
  slideRight.func = [&value=val, &maximum=this->max](){if (value != maximum) value++;};
}

Page::Page(int page_number, std::string name, std::uint32_t background_color){
  //Page Constructor
  //Should call Page::goTo() to actually show the page
  pageNum = page_number;
  pages[pageNum] = this;
  title = name;
  bcol = background_color;

  (buttons).push_back(&prevPage);
  (buttons).push_back(&nextPage);
}

void Page::goTo(Page* page){
  clearScreen(page->bcol);
  currentPage = page; //Saves new page then draws all the buttons on the page
  screen::set_pen(COLOR_WHITE);
  screen::set_eraser(page->bcol);
  screen::print(TEXT_SMALL, mid_x-(page->title.length()*Page::char_width)/2, 10, "%s", page->title);
  for (std::vector <Button*>::iterator it = (page->buttons).begin(); it != (page->buttons).end(); it++) (*it)->draw();
  for (std::vector <Slider*>::iterator it = (page->sliders).begin(); it != (page->sliders).end(); it++) (*it)->draw();
  for (std::vector<Text*>::iterator it = (page->texts).begin(); it != (page->texts).end(); it++) (*it)->draw();
}

void Page::goTo(int page){
  goTo(pages[page]);
}

void Page::clearScreen(std::uint32_t color){
  screen::set_pen(color);
  screen::fill_rect(Page::left, Page::up, Page::right, Page::down);
}

void Text::draw(){
  screen::set_eraser(page->bcol);
  screen::set_pen(lcol);
  char buffer [70];
  if (val_ptr) {
    int length = sprintf(buffer, label.c_str(), *val_ptr);
    screen::print(txt_fmt, x, y, "%*c   ", length);
    screen::print(txt_fmt, x, y, "%s", buffer);
  }
  else screen::print(txt_fmt, x, y, label.c_str());
  prevVal = *val_ptr;
}

void Slider::draw(){
  int end_coord = x1+(x2-x1)*(val-min)/(max-min);

  screen::set_pen(bcol);
  screen::set_eraser(page->bcol);
  screen::erase_rect(x1, text_y, x2, y2);
  screen::fill_rect(x1, y1, x2, y2);
  screen::set_pen(lcol);
  screen::print(TEXT_SMALL, text_x, text_y, "%s:%.f", label.c_str(), val);
  screen::print(TEXT_SMALL, x1, text_y, "%d", min);
  screen::print(TEXT_SMALL, x2, text_y, "%d", max);
  screen::set_pen(lcol);
  screen::fill_rect(x1+1, y1+1, end_coord, y2-1); //Draws bar upto its value
  screen::set_eraser(lcol);
  screen::set_pen(bcol);
  // screen::print(TEXT_SMALL, end_coord, (y1+y2)/2, "%d", 100);
}

void Button::draw(){
  screen::set_pen(bcol);
  screen::set_eraser(bcol);
  screen::fill_rect(x1, y1, x2, y2);
  screen::set_pen(lcol);
  screen::print(TEXT_SMALL, text_x, text_y, label.c_str()); //Centers label on button
}

void Button::drawPressed(){
  if(Page::currentPage == page || &perm == page){
    screen::set_eraser(page->bcol);
    screen::erase_rect(x1, y1, x2, y2);
    screen::set_pen(dark_bcol);
    screen::set_eraser(dark_bcol);
    screen::fill_rect(x1+3, y1+3, x2-3, y2-3);
    screen::set_pen(lcol);
    screen::print(TEXT_SMALL, text_x, text_y, label.c_str()); //Centers label on button
  }
}

// void Button::del(){ //Not really used
//   //Looks for the button in the list of buttons for its page
//   std::vector <Button*>::iterator it = find(((page)->buttons).begin(), (page->buttons).end(), this);
//
//   //If the button is on the list it is removed
//   if(it != (page->buttons).end()){
//     if(Page::currentPage == page) screen::erase_rect(x1, y1, x2, y2); //Only if on current page
//     (page)->buttons.erase(it);
//   }
// }
//
// void Button::setTask(void (*pointer_to_function)()){
//   // funcPtr = pointer_to_function;
//   func = pointer_to_function;
// }

void Button::runTask(){
  // if (funcPtr != 0) (*funcPtr)(); //runs function assigned to the button
  if (func) func();
}

void Page::updateScreenStatus() { //to be called continously
  screen_touch_status_s_t status = c::screen_touch_status();
  touch_status = status.touch_status;
  x = status.x;
  y = status.y;
}

void Slider::updateVal(){
  // returns true if the button is currently being pressed
  if (Page::touch_status == TOUCH_PRESSED || Page::touch_status == TOUCH_HELD){
    if ((x1 <= Page::x && Page::x <= x2) && (y1 <= Page::y && Page::y <= y2)) val = min+(max-min)*(Page::x-x1)/(x2-x1); //Gets val based on press location
  }
}

bool Button::pressed(){
  // returns true if the button is currently being pressed
  if (Page::touch_status == TOUCH_PRESSED || Page::touch_status == TOUCH_HELD){
    if ((x1 <= Page::x && Page::x <= x2) && (y1 <= Page::y && Page::y <= y2)) return true;
  }
  return false;
}

bool Button::newPress(){
  return (pressed() && !lastPressed);
}

bool Button::newRelease(){
  return (!pressed() && lastPressed);
}

void Button::createOptions(std::vector<Button*> buttons){
  for (std::vector <Button*>::iterator it = buttons.begin(); it != buttons.end(); it++){
    if ((*it)->form != LATCH) printf("Option Feature is only available for latch buttons!\n");
    (*it)->options = buttons;
  }
}

void Page::update(){
  Page::updateScreenStatus();
  if (Page::currentPage->func) Page::currentPage->func();
}

void Text::update(){
  for (std::vector<Text*>::iterator it = (Page::currentPage->texts).begin(); it != (Page::currentPage->texts).end(); it++){
    if((*it)->prevVal != *((*it)->val_ptr)) (*it)->draw();
  }
}

Slider* Slider::update(){
  for (std::vector <Slider*>::iterator it = (Page::currentPage->sliders).begin(); it != (Page::currentPage->sliders).end(); it++){
    Slider* sl_id = *it;
    sl_id->updateVal();
    if (sl_id->val != sl_id->prevVal){
      sl_id->draw();
      sl_id->prevVal = sl_id->val;
      return sl_id;
    }
  }
  return 0;
}

Button* Button::update(){ //to be called continously
  //Loops through the list of buttons on the current page to check for presses

  for (std::vector <Button*>::iterator it = (Page::currentPage->buttons).begin(); it != (Page::currentPage->buttons).end(); it++){
    Button* btn_id = *it;

    if (btn_id->form == Button::SINGLE){
      if (btn_id->newPress()){
        btn_id->lastPressed = 1;
        btn_id->drawPressed();
        btn_id->runTask();
        return btn_id;
      }

      else if (btn_id->newRelease()){
        btn_id->lastPressed = 0;
        btn_id->draw();
      }
    }

    else if (btn_id->form == Button::LATCH){
      if (btn_id->newPress()){
        btn_id->lastPressed = 1;
        btn_id->latched = !(btn_id->latched); //Toggles the latch

        //Draws button's new state
        if(btn_id->latched){
          btn_id->drawPressed();
          //If radio button
          for (std::vector <Button*>::iterator option_it = (btn_id->options).begin(); option_it != (btn_id->options).end(); option_it++){
            if ((*option_it) != btn_id) (*option_it)->latched = 0;
            (*option_it)->draw(); //Try without this
          }

        }
        else btn_id->draw();

        return btn_id;
      }

      else if (btn_id->newRelease()){
        btn_id->lastPressed = 0;
      }

      if (btn_id->latched){ //If in pressed state
        btn_id->runTask();
        delay(10);
      }
    }

    else if (btn_id->form == Button::HOLD){
      if (btn_id->newPress()){
        btn_id->lastPressed = 1;
        btn_id->drawPressed();
        //start the task in the background
        return btn_id;
      }

      else if (btn_id->newRelease()){
        btn_id->lastPressed = 0;
        btn_id->draw();
        //end the task
      }
    }

  }

  return 0;
}

void backgroundStuff(){
  Page::update();
  Button::update();
  Slider::update();
  Text::update();

  //Saving vars
  flTemp = front_l.get_temperature();
  blTemp = back_l.get_temperature();
  frTemp = front_r.get_temperature();
  brTemp = back_r.get_temperature();
  angle = rad_to_deg(tracking.global_angle);
  delay(20);
}
