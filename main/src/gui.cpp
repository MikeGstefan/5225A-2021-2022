#include "gui.hpp"

//Var init for text monitoring
double ringCount, angle, elasticUpTime, elasticDownTime;
double motor_temperatures[8];
std::pair <int, int> elasticTime;

//Static Variable Declarations
Page* Page::currentPage = 0;
std::array<Page*, PAGE_COUNT> Page::pages = {};
last_touch_e_t Page::touch_status = E_TOUCH_RELEASED;
int16_t Page::x = 0, Page::y = 0;

//Base Pages
Page perm (0, "PERM BTNS", COLOR_PINK); //Page perm only exists because its a problem if permanent buttons think they belong to every page.
//So they think they belong to page perm, but every page knows they own these buttons.
Button prevPage(0, 0, 75, 20, Style::SIZE, Button::SINGLE, &perm, "<-");
Button nextPage(480, 0, -75, 20, Style::SIZE, Button::SINGLE, &perm, "->");

Page elastic (1, "Elastic Test"); //Testing the elastics on the lift
Button runElastic(165, 60, 150, 55, Style::SIZE, Button::SINGLE, &elastic, "Run Elastic Test", COLOR_ORANGE, COLOR_BLACK);
Text upTime (MID_X, 160, Style::CENTRE, TEXT_SMALL, &elastic, "Up Time: %.0f", &elasticUpTime);
Text downTime(MID_X, 180, Style::CENTRE, TEXT_SMALL, &elastic, "Down Time: %.0f", &elasticDownTime);

Page liftMove (2, "Lift"); //Moving the lift to an xyz position

Page track (3, "Tracking", COLOR_WHITE); //Display tracking vals and reset btns
Text trackX(50, 45, Style::CENTRE, TEXT_SMALL, &track, "X:%.1f", &tracking.x_coord, COLOR_BLACK);
Text trackY(135, 45, Style::CENTRE, TEXT_SMALL, &track, "Y:%.1f", &tracking.y_coord, COLOR_BLACK);
Text trackA(220, 45, Style::CENTRE, TEXT_SMALL, &track, "A:%.1f", &angle, COLOR_BLACK);
Button resX(15, 60, 70, 55, Style::SIZE, Button::SINGLE, &track, "Reset X", COLOR_ORANGE, COLOR_BLACK);
Button resY(100, 60, 70, 55, Style::SIZE, Button::SINGLE, &track, "Reset Y", COLOR_ORANGE, COLOR_BLACK);
Button resA(185, 60, 70, 55, Style::SIZE, Button::SINGLE, &track, "Reset A", COLOR_ORANGE, COLOR_BLACK);

Page moving (4, "Moving");
Slider xVal(35, 45, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, &moving, "X", COLOR_WHITE, COLOR_ORANGE);
Slider yVal(35, 110, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, &moving, "Y", COLOR_WHITE, COLOR_ORANGE);
Slider aVal(35, 175, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, &moving, "A", COLOR_WHITE, COLOR_ORANGE);
Button goToXYA(320, 45, 150, 40, Style::SIZE, Button::SINGLE, &moving, "Target", COLOR_ORANGE, COLOR_BLACK);
Button goHome(320, 110, 150, 40, Style::SIZE, Button::SINGLE, &moving, "Home", COLOR_ORANGE, COLOR_BLACK);
Button goCentre(320, 175, 150, 40, Style::SIZE, Button::SINGLE, &moving, "Centre", COLOR_ORANGE, COLOR_BLACK);

Page autoSel (5, "Auton"); //Select auton routes
Button route1 (45, 90, 100, 80, Style::SIZE, Button::LATCH, &autoSel, "Route 1", COLOR_ORANGE, COLOR_BLACK);
Button route2 (190, 90, 100, 80, Style::SIZE, Button::LATCH, &autoSel, "Route 2", COLOR_ORANGE, COLOR_BLACK);
Button route3 (335, 90, 100, 80, Style::SIZE, Button::LATCH, &autoSel, "Route 3", COLOR_ORANGE, COLOR_BLACK);

Page driverCurve (6, "Drivers"); //Select a driver and their exp curve
Button prevDrivr(30, 70, 100, 120, Style::SIZE, Button::SINGLE, &driverCurve, "Prev Driver", COLOR_ORANGE, COLOR_BLACK);
Text drivrName(MID_X, MID_Y, Style::CENTRE, TEXT_LARGE, &driverCurve, "[Driver Name]", COLOR_WHITE);
Button nextDrivr(350, 70, 100, 120, Style::SIZE, Button::SINGLE, &driverCurve, "Next Driver", COLOR_ORANGE, COLOR_BLACK);

Page intkTest (7, "Intake"); //Test for intake with rings
Text rings(MID_X, 50, Style::CENTRE, TEXT_SMALL, &intkTest, "Ring Count: %.0f", &ringCount);
Button resI (30, 90, 120, 80, Style::SIZE, Button::SINGLE, &intkTest, "Reset Motor", COLOR_ORANGE, COLOR_BLACK);
Button onOff (180, 90, 120, 80, Style::SIZE, Button::SINGLE, &intkTest, "Start/Stop", COLOR_ORANGE, COLOR_BLACK);
Button resRings (330, 90, 120, 80, Style::SIZE, Button::SINGLE, &intkTest, "Reset Ring Count", COLOR_ORANGE, COLOR_BLACK);

Page temps (8, "Temperature"); //Motor temps
Text tempfl(75, 85, Style::CENTRE, TEXT_SMALL, &temps, "FL: %.1f", &motor_temperatures[0]);
Text tempbl(185, 85, Style::CENTRE, TEXT_SMALL, &temps, "BL: %.1f", &motor_temperatures[1]);
Text tempfr(295, 85, Style::CENTRE, TEXT_SMALL, &temps, "FR: %.1f", &motor_temperatures[2]);
Text tempbr(405, 85, Style::CENTRE, TEXT_SMALL, &temps, "BR: %.1f", &motor_temperatures[3]);
Text tempc(75, 175, Style::CENTRE, TEXT_SMALL, &temps, "C: %.1f", &motor_temperatures[4]);
Text tempf(185, 175, Style::CENTRE, TEXT_SMALL, &temps, "F: %.1f", &motor_temperatures[5]);
Text tempi(295, 175, Style::CENTRE, TEXT_SMALL, &temps, "I: %.1f", &motor_temperatures[6]);
Text tempu(405, 175, Style::CENTRE, TEXT_SMALL, &temps, "U: %.1f", &motor_temperatures[7]);

//Functions
void draw_field(){
  screen::set_pen(COLOR_BLACK);
  screen::draw_rect(270, 30, 470, 230);
  screen::draw_line(370, 30, 370, 230);
  screen::draw_line(270, 130, 470, 130);
  screen::draw_pixel(270+(200*tracking.x_coord/144), 230-(200*tracking.y_coord/144));
}
void Page::toPrev(){
  if (currentPage == pages[1]) goTo(PAGE_COUNT-1);
  else goTo((currentPage->pageNum)-1);
}
void Page::toNext(){
  if (currentPage == pages[PAGE_COUNT-1]) goTo(1);
  else goTo((currentPage->pageNum)+1);
}
void prevDriver(){
  if (drivebase.cur_driver == 0) drivebase.cur_driver = drivebase.num_of_drivers - 1;
  else drivebase.cur_driver--;
  WAIT_FOR_SCREEN_REFRESH();
  // screen::fill_rect(130, 70, 350, 190);
  master.print(2, 0, "Driver: %s          ", drivebase.drivers[drivebase.cur_driver].name);
}
void nextDriver(){
  drivebase.cur_driver++;
  drivebase.cur_driver %= drivebase.num_of_drivers;
  WAIT_FOR_SCREEN_REFRESH();
  // screen::fill_rect(130, 70, 350, 190);
  master.print(2, 0, "Driver: %s          ", drivebase.drivers[drivebase.cur_driver].name);
}
std::pair <int, int> elasticUtil(){
  elasticTime = std::make_pair(800, 800);
  return elasticTime;
}

void guiSetup(){ //Call once at start in initialize()
  prevPage.func = &(Page::toPrev);
  nextPage.func = &(Page::toNext);

  runElastic.func = &elasticUtil;

  resX.func = [&coord=tracking.x_coord](){printf("RESET X PLACEHOLDER\n");};
  resY.func = [&coord=tracking.y_coord](){printf("RESET Y PLACEHOLDER\n");};
  resA.func = [&coord=tracking.global_angle](){printf("RESET A PLACEHOLDER\n");};
  track.func = &draw_field;

  goToXYA.func = [&x=xVal.val, &y=yVal.val, &a=aVal.val](){delay(1000); move_to_target_sync(x, y, a, false);};
  goHome.func = [](){delay(1000); move_to_target_sync(0, 0, 0, false);};
  goCentre.func = [](){delay(1000); move_to_target_sync(72, 72, 0, false);};

  prevDrivr.func = &prevDriver;
  nextDrivr.func = &nextDriver;
  drivrName.setBackground(130, 70, 350, 190, Style::CORNER, COLOR_BLACK);

  tempfl.setBackground(40, 60, 70, 50, Style::SIZE);
  tempbl.setBackground(150, 60, 70, 50, Style::SIZE);
  tempfr.setBackground(260, 60, 70, 50, Style::SIZE);
  tempbr.setBackground(370, 60, 70, 50, Style::SIZE);

  tempc.setBackground(40, 150, 70, 50, Style::SIZE);
  tempf.setBackground(150, 150, 70, 50, Style::SIZE);
  tempi.setBackground(260, 150, 70, 50, Style::SIZE);
  tempu.setBackground(370, 150, 70, 50, Style::SIZE);

  Button::createOptions({&route1, &route2, &route3});

  Page::goTo(1);
}

//Utility to get coordinates for aligned objects, (buttons, sliders...) of same size
void alignedCoords (int x_objects, int y_objects, int x_btn, int y_btn, int x_range, int y_range){
  std::string strX = "(", subStrX, strY = "(", subStrY;

  double x_space = (x_range-x_objects*x_btn)/(x_objects+1.0);
  double y_space = (y_range-y_objects*y_btn)/(y_objects+1.0);

  for (int y = 0; y < y_objects; y++){
    for (int x = 0; x < x_objects; x++){
      printf("(%.1f, %.1f, %d, %d, Style::SIZE)\n", x_space*(x+1) + x_btn*x, y_space*(y+1) + y_btn*y + 20, x_btn, y_btn);
    }
  }
  printf("\n");

  if ((fmod(x_space, 1) != 0) || (fmod(y_space, 1) != 0)){
    printf("ALTERNATIVES:\n");
    int x_btn_new = x_btn;
    int y_btn_new = y_btn;
    double x_space_new = x_space;
    double y_space_new = y_space;

    while (fmod(x_space_new, 1) != 0){
      x_btn_new++;
      x_space_new = (x_range-x_objects*x_btn_new)/(x_objects+1.0);
    }

    while (fmod(y_space_new, 1) != 0){
      y_btn_new++;
      y_space_new = (y_range-y_objects*y_btn_new)/(y_objects+1.0);
    }

    for (int y = 0; y < y_objects; y++){
      for (int x = 0; x < x_objects; x++){
        printf("(%d, %d, %d, %d, Style::SIZE)\n", int(x_space_new*(x+1) + x_btn_new*x), int(y_space_new*(y+1) + y_btn_new*y + 20), x_btn_new, y_btn_new);
      }
    }
    printf("\n");

    x_btn_new = x_btn;
    y_btn_new = y_btn;
    x_space_new = x_space;
    y_space_new = y_space;

    while (fmod(x_space_new, 1) != 0){
      x_btn_new--;
      x_space_new = (x_range-x_objects*x_btn_new)/(x_objects+1.0);
    }

    while (fmod(y_space_new, 1) != 0){
      y_btn_new--;
      y_space_new = (y_range-y_objects*y_btn_new)/(y_objects+1.0);
    }

    for (int y = 0; y < y_objects; y++){
      for (int x = 0; x < x_objects; x++){
        printf("(%d, %d, %d, %d, Style::SIZE)\n", int(x_space_new*(x+1) + x_btn_new*x), int(y_space_new*(y+1) + y_btn_new*y + 20), x_btn_new, y_btn_new);
      }
      printf("\n");
    }
  }
}

//Flashing
bool flashing=false;
std::uint32_t end_time=millis();
void flash(std::uint32_t color, std::uint32_t time, std::string text){ //has a delay
  flashing = true;
  Page::clearScreen(color);
  screen::print(TEXT_LARGE, (480-text.length()*CHAR_WIDTH_LARGE)/2, 95, text.c_str());
  master.rumble("-.-.-.-.");
  end_time = millis() + time;
}

void end_flash (){
  if (flashing && (millis() >= end_time)){
    flashing = false;
    Page::goTo(Page::currentPage);
  }
}

std::tuple<int, int, int, int> fixPoints (int p1, int p2,int p3, int p4, Style::style type){
  int x1=p1, y1=p2, x2=p3, y2=p4, temp;

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
void Text::construct (int16_t pt1, int16_t pt2, Style::style type, text_format_e_t size, Page* page_ptr, std::string text, std::uint32_t label_color){
  txt_fmt = size;
  page = page_ptr;
  lcol = label_color;

  setTitle(pt1, pt2, type, text);


  (page->texts).push_back(this);
}

void Button::construct(int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style::style type, press_type prType, Page* page_ptr, std::string text, std::uint32_t background_color, std::uint32_t label_color){

    //Saves params to class private vars
    bcol = background_color;
    dark_bcol = RGB2COLOR(int(COLOR2R(bcol)*0.9), int(COLOR2G(bcol)*0.9), int(COLOR2B(bcol)*0.9));
    lcol = label_color;
    form = prType;

    //Saves the buttons owning page
    page = page_ptr;
    (page->buttons).push_back(this);
    std::tie(x1, y1, x2, y2) = fixPoints(pt1, pt2, pt3, pt4, type);

    if (8*text.length() < x2-x1){
      text_x = (x1+x2-(text.length()*CHAR_WIDTH_SMALL))/2;
      text_y = (y1+y2-CHAR_HEIGHT_SMALL)/2;
      label = text;
    }
    else{
      std::size_t space = text.find(' ', text.length()/2);
      if (space != std::string::npos) {
        label = text.substr(0, space);
        label1 = text.substr(space+1);

        text_x = (x1+x2-(label.length()*CHAR_WIDTH_SMALL))/2;
        text_x1 = (x1+x2-(label1.length()*CHAR_WIDTH_SMALL))/2;
        text_y = (y1+y2-CHAR_HEIGHT_SMALL)/2-CHAR_HEIGHT_SMALL;
        text_y1 = (y1+y2-CHAR_HEIGHT_SMALL)/2+CHAR_HEIGHT_SMALL;
      }
    }
}

Text::Text (int16_t pt1, int16_t pt2, Style::style type, text_format_e_t size, Page* page_ptr, std::string text, std::uint32_t label_color){
  construct(pt1, pt2, type, size, page_ptr, text, label_color);
}

Text::Text (int16_t pt1, int16_t pt2, Style::style type, text_format_e_t size, Page* page_ptr, std::string text, double* val_ref, std::uint32_t label_color){
  construct(pt1, pt2, type, size, page_ptr, text, label_color);
  val_ptr = val_ref;
}

Button::Button(int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style::style type, press_type prType, Page* page_ptr, std::string text, std::uint32_t background_color, std::uint32_t label_color){
  construct(pt1, pt2, pt3, pt4, type, prType, page_ptr, text, background_color, label_color);
}

Slider::Slider (int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style::style type, direction direction, int minimum, int maximum, Page* page_ptr, std::string text, std::uint32_t background_color, std::uint32_t label_color){
  //Saves params to class private vars
  max = maximum;
  min = minimum;
  bcol = background_color;
  lcol = label_color;
  label = text;
  dir = direction;

  //Saves the buttons owning page
  page = page_ptr;
  (page->sliders).push_back(this);

  std::tie(x1, y1, x2, y2) = fixPoints(pt1, pt2, pt3, pt4, type);

  if (dir == HORIZONTAL){
    text_x = (x1+x2-(label.length()*CHAR_WIDTH_SMALL))/2;
    text_y = y1-CHAR_HEIGHT_SMALL-2;
    inc.construct(x2+5, y1, x2+25, y2, Style::CORNER, Button::SINGLE, page, ">", lcol, bcol);
    dec.construct(x1-25, y1, x1-5, y2, Style::CORNER, Button::SINGLE, page, "<", lcol, bcol);
  }
  else{
    text_x = (x1+x2-(label.length()*CHAR_WIDTH_SMALL))/2;
    text_y = (y1+y2)/2;
    inc.construct(x1, y1-25, x2, y1-5, Style::CORNER, Button::SINGLE, page, "▲", lcol, bcol);
    dec.construct(x1, y2+5, x2, y2+25, Style::CORNER, Button::SINGLE, page, "▼", lcol, bcol);
  }

  //Buttons
  dec.func = [&value=val, &minimum=this->min](){if(value != minimum) value--;};
  inc.func = [&value=val, &maximum=this->max](){if (value != maximum) value++;};
}

Page::Page(int page_number, std::string name, std::uint32_t background_color){
  //Page Constructor
  //Should call Page::goTo() to actually show the page
  pageNum = page_number;
  pages[pageNum] = this;
  title = name + " - " + std::to_string(pageNum);
  bcol = background_color;

  buttons.push_back(&prevPage);
  buttons.push_back(&nextPage);
}

//Methods
void Page::goTo(Page* page){
  std::array <Page*, PAGE_COUNT>::iterator it = std::find(pages.begin(), pages.end(), page);
  if (it == pages.end()) return;
  clearScreen(page->bcol);
  currentPage = page; //Saves new page then draws all the buttons on the page
  screen::set_pen(COLOR_BLACK);
  screen::set_eraser(COLOR_BLACK);
  screen::fill_rect(75, 0, 405, 20);
  screen::set_pen(page->bcol);
  screen::print(TEXT_SMALL, MID_X-(page->title.length()*CHAR_WIDTH_SMALL)/2, 10, "%s", page->title);
  for (std::vector <Button*>::iterator it = (page->buttons).begin(); it != (page->buttons).end(); it++) (*it)->draw();
  for (std::vector <Slider*>::iterator it = (page->sliders).begin(); it != (page->sliders).end(); it++) (*it)->draw();
  for (std::vector<Text*>::iterator it = (page->texts).begin(); it != (page->texts).end(); it++) (*it)->draw();
}

void Page::goTo(int page){
  goTo(pages[page]);
}

void Page::clearScreen(std::uint32_t color){
  screen::set_pen(color);
  screen::fill_rect(PAGE_LEFT, PAGE_UP, PAGE_RIGHT, PAGE_DOWN);
}

void Text::setTitle (int16_t pt1, int16_t pt2, Style::style type, std::string text){
  label = text;
  x = pt1;
  y = pt2;

  if (type == Style::CENTRE){
    if (txt_fmt == TEXT_SMALL){
      x -= (label.length()*CHAR_WIDTH_SMALL)/2;
      y -= CHAR_HEIGHT_SMALL/2;
    }

    else if (txt_fmt == TEXT_MEDIUM){
      x -= (label.length()*CHAR_WIDTH_MEDIUM)/2;
      y -= CHAR_HEIGHT_MEDIUM/2;
    }

    else if (txt_fmt == TEXT_LARGE){
      x -= (label.length()*CHAR_WIDTH_LARGE)/2;
      y -= CHAR_HEIGHT_LARGE/2;
    }
  }

}

void Text::setBackground (int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style::style type, std::uint32_t colour){
    std::tie(x1, y1, x2, y2) = fixPoints(pt1, pt2, pt3, pt4, type);
    bcol = colour;
}

void Text::draw(){
  if (x2 != 0 && y2 != 0){
    screen::set_pen(lcol);
    screen::fill_rect(x, y, x2, y2);
    screen::set_pen(page->bcol);
    screen::set_eraser(lcol);
  }
  else{
    screen::set_pen(lcol);
    screen::set_eraser(page->bcol);
  }

  if (val_ptr != nullptr) { //If there is a var to print
    char buffer [70];
    sprintf(buffer, label.c_str(), *val_ptr);
    screen::print(txt_fmt, x, y, "%s", buffer);
    prevVal = *val_ptr;
  }
  else screen::print(txt_fmt, x, y, "%s", label.c_str()); //Plain text (no var)
  prevLabel = label;
}

void Slider::draw(){
  screen::set_pen(bcol);
  screen::set_eraser(page->bcol);
  screen::erase_rect(x1, text_y, x2, y2);
  screen::fill_rect(x1, y1, x2, y2);
  screen::set_pen(lcol);
  if(dir == HORIZONTAL){
    screen::fill_rect(x1+1, y1+1, x1+(x2-x1)*(val-min)/(max-min), y2-1); //Draws bar upto its value
    screen::print(TEXT_SMALL, x1, text_y, "%d", min);
    screen::print(TEXT_SMALL, x2, text_y, "%d", max);
    screen::print(TEXT_SMALL, text_x, text_y, "%s:%.f", label.c_str(), val);
  }
  else{
    screen::fill_rect(x1+1, y2-(y2-y1)*(val-min)/(max-min), x2-1, y2-1); //Draws bar (vertical)
    screen::print(TEXT_SMALL, x1-25, y2, "%d", min);
    screen::print(TEXT_SMALL, x1-25, y1, "%d", max);
    screen::print(TEXT_SMALL, text_x, y1-27-CHAR_HEIGHT_SMALL, "%s:%.f", label.c_str(), val);
  }
}

void Button::draw(){
  screen::set_pen(bcol);
  screen::set_eraser(bcol);
  screen::fill_rect(x1, y1, x2, y2);
  screen::set_pen(lcol);
  screen::print(TEXT_SMALL, text_x, text_y, label.c_str());
  screen::print(TEXT_SMALL, text_x1, text_y1, label1.c_str());
}

void Button::drawPressed(){
  screen::set_eraser(page->bcol);
  screen::erase_rect(x1, y1, x2, y2);
  screen::set_pen(dark_bcol);
  screen::set_eraser(dark_bcol);
  screen::fill_rect(x1+3, y1+3, x2-3, y2-3);
  screen::set_pen(lcol);
  screen::print(TEXT_SMALL, text_x, text_y, label.c_str()); //Centers label on button
}

void Button::del(){ //Not really used
  //Looks for the button in the list of buttons for its page
  std::vector <Button*>::iterator it = std::find(((page)->buttons).begin(), (page->buttons).end(), this);

  //If the button is on the list it is removed
  if(it != (page->buttons).end()){
    if(Page::currentPage == page) screen::erase_rect(x1, y1, x2, y2); //Only if on current page
    (page)->buttons.erase(it);
  }
}

void Button::createOptions(std::vector<Button*> buttons){
  for (std::vector <Button*>::iterator it = buttons.begin(); it != buttons.end(); it++){
    if ((*it)->form != LATCH) printf("Option Feature is only available for latch buttons!\n");
    (*it)->options = buttons;
  }
}

void Button::runTask(){
  if (func) func();
}

//Updating data and presses
void Page::updateScreenStatus() {
  screen_touch_status_s_t status = c::screen_touch_status();
  touch_status = status.touch_status;
  x = status.x;
  y = status.y;
}

void Slider::updateVal(){
  // returns true if the button is currently being pressed
  if (page->pressed()){
    if ((x1 <= Page::x && Page::x <= x2) && (y1 <= Page::y && Page::y <= y2)){
      if (dir == HORIZONTAL) val = min+(max-min)*(Page::x-x1)/(x2-x1); //Gets val based on press location
      else val = min+(max-min)*(y2-Page::y)/(y2-y1);
    }
  }
}

bool Page::pressed(){
  if (Page::currentPage == this){
    if (Page::touch_status == TOUCH_PRESSED || Page::touch_status == TOUCH_HELD) return true;
  }
  return false;
}

bool Button::pressed(){
  // returns true if the button is currently being pressed
  if (Page::currentPage->pressed()){
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

void Page::update(){
  Page::updateScreenStatus();
  if (currentPage->func) currentPage->func();
}

void Text::update(){
  for (std::vector<Text*>::iterator it = (Page::currentPage->texts).begin(); it != (Page::currentPage->texts).end(); it++){
    Text* text_id = *it;
    if(text_id->val_ptr != nullptr){ //If variable given
      if(text_id->prevVal != *(text_id->val_ptr)) text_id->draw(); //If var has changed value
    }
    if(text_id->prevLabel != text_id->label) text_id->draw(); //If text has changed
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

Button* Button::update(){
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
            if ((*option_it) != btn_id){
              (*option_it)->latched = 0;
              (*option_it)->draw();
            }
          }
          btn_id->runTask();

        }
        else btn_id->draw(); //Newly unpressed
        return btn_id;
      }

      else if (btn_id->newRelease()){
        btn_id->lastPressed = 0;
      }

    }

    else if (btn_id->form == Button::HOLD){
      if (btn_id->newPress()){
        btn_id->lastPressed = 1;
        btn_id->drawPressed();
        btn_id->runTask();
        return btn_id;
      }

      if (btn_id->pressed()) btn_id->runTask();

      else if (btn_id->newRelease()){
        btn_id->lastPressed = 0;
        btn_id->draw();
        //end the task
      }
    }

  }

  return 0;
}

void guiBackground(){ //To be called continously
  //Saving vars for text display
  motor_temperatures[0] = front_l.get_temperature();
  motor_temperatures[1] = back_l.get_temperature();
  motor_temperatures[2] = front_r.get_temperature();
  motor_temperatures[3] = back_r.get_temperature();
  motor_temperatures[4] = c_bar.get_temperature();
  motor_temperatures[5] = f_bar.get_temperature();
  // motor_temperatures[6] = intake.get_temperature();
  // motor_temperatures[7] = uptake.get_temperature();
  ringCount = ring_count;
  elasticUpTime = elasticTime.first;
  elasticDownTime = elasticTime.second;
  angle = fmod(rad_to_deg(tracking.global_angle), 360);
  drivrName.setTitle (MID_X, MID_Y, Style::CENTRE, drivebase.drivers[drivebase.cur_driver].name);

  // if (!flashing){ //Overheating Motors
  //   for (int i = 0; i < 8; i++){
  //     if (motor_temperatures[i] >= 50 && motor_temperatures[i] != 2147483647){
  //       Page::goTo(&temps);
  //       flash(COLOR_RED, 10000, "MOTOR TEMP EXCEEDED");
  //       break;
  //     }
  //   }
  // }

  Page::update();
  Button::update();
  Slider::update();
  Text::update();
  end_flash();
}
