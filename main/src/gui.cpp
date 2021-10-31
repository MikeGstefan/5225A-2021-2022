#include "gui.hpp"

//Miscallaneous Tings
Timer Flash("Flash Timer", false);
std::array<std::tuple<Motor*, Text*, std::string, double>, 8> motors;
std::uint32_t flash_end = std::numeric_limits<std::uint32_t>::max();

//Var init for text monitoring
double ringCount, angle, elasticUpTime, elasticDownTime;

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
Button runElastic(165, 60, 150, 55, Style::SIZE, Button::SINGLE, &elastic, "Run Elastic Test");
Text upTime (MID_X, 160, Style::CENTRE, TEXT_SMALL, &elastic, "Up Time: %.0f", &elasticUpTime);
Text downTime(MID_X, 180, Style::CENTRE, TEXT_SMALL, &elastic, "Down Time: %.0f", &elasticDownTime);

Page liftMove (2, "Lift"); //Moving the lift to an xyz position
Slider liftXVal(35, 65, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, &liftMove, "X", COLOR_WHITE, ORANGE);
Slider liftYVal(35, 150, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, &liftMove, "Y", COLOR_WHITE, ORANGE);
Button goToLiftXY(400, 128, 70, 50, Style::CENTRE, Button::SINGLE, &liftMove, "Move To Target");

Page liftStates (3, "Lift States"); //Moving to various lift states
Button Neutral (15, 45, 100, 75, Style::SIZE, Button::SINGLE, &liftStates, "Neutral"); //Names are caps to avoid conflicts
Button RingPickup (130, 45, 100, 75, Style::SIZE, Button::SINGLE, &liftStates, "Ring Pickup");
Button Tip (245, 45, 100, 80, Style::SIZE, Button::SINGLE, &liftStates, "Tip");
Button Lowering (360, 45, 100, 75, Style::SIZE, Button::SINGLE, &liftStates, "Lowering");
Button Down (15, 140, 100, 75, Style::SIZE, Button::SINGLE, &liftStates, "Down");
Button Raised (130, 140, 100, 75, Style::SIZE, Button::SINGLE, &liftStates, "Raised");
Button Platform (245, 140, 100, 75, Style::SIZE, Button::SINGLE, &liftStates, "Platform");
Button ReleaseMogo(360, 140, 100, 75, Style::SIZE, Button::SINGLE, &liftStates, "Release Mogo");

Page track (4, "Tracking"); //Display tracking vals and reset btns
Text trackX(50, 45, Style::CENTRE, TEXT_SMALL, &track, "X:%.1f", &tracking.x_coord, COLOR_WHITE);
Text trackY(135, 45, Style::CENTRE, TEXT_SMALL, &track, "Y:%.1f", &tracking.y_coord, COLOR_WHITE);
Text trackA(220, 45, Style::CENTRE, TEXT_SMALL, &track, "A:%.1f", &angle, COLOR_WHITE);
Button resX(15, 60, 70, 55, Style::SIZE, Button::SINGLE, &track, "Reset X");
Button resY(100, 60, 70, 55, Style::SIZE, Button::SINGLE, &track, "Reset Y");
Button resA(185, 60, 70, 55, Style::SIZE, Button::SINGLE, &track, "Reset A");

Page moving (5, "Moving");
Slider xVal(35, 45, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, &moving, "X", COLOR_WHITE, COLOR_ORANGE);
Slider yVal(35, 110, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, &moving, "Y", COLOR_WHITE, COLOR_ORANGE);
Slider aVal(35, 175, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, &moving, "A", COLOR_WHITE, COLOR_ORANGE);
Button goToXYA(320, 45, 150, 40, Style::SIZE, Button::SINGLE, &moving, "Target");
Button goHome(320, 110, 150, 40, Style::SIZE, Button::SINGLE, &moving, "Home");
Button goCentre(320, 175, 150, 40, Style::SIZE, Button::SINGLE, &moving, "Centre");

Page autoSel (6, "Auton"); //Select auton routes
Button route1 (45, 90, 100, 80, Style::SIZE, Button::LATCH, &autoSel, "Route 1");
Button route2 (190, 90, 100, 80, Style::SIZE, Button::LATCH, &autoSel, "Route 2");
Button route3 (335, 90, 100, 80, Style::SIZE, Button::LATCH, &autoSel, "Route 3");

Page driverCurve (7, "Drivers"); //Select a driver and their exp curve
Button prevDrivr(30, 70, 100, 120, Style::SIZE, Button::SINGLE, &driverCurve, "Prev Driver");
Text drivrName(MID_X, MID_Y, Style::CENTRE, TEXT_LARGE, &driverCurve, "[Driver Name]", COLOR_WHITE);
Button nextDrivr(350, 70, 100, 120, Style::SIZE, Button::SINGLE, &driverCurve, "Next Driver");

Page intkTest (8, "Intake"); //Test for intake with rings
Text rings(MID_X, 50, Style::CENTRE, TEXT_SMALL, &intkTest, "Ring Count: %.0f", &ringCount);
Button resI (30, 90, 120, 80, Style::SIZE, Button::SINGLE, &intkTest, "Reset Motor");
Button onOff (180, 90, 120, 80, Style::SIZE, Button::SINGLE, &intkTest, "Start/Stop");
Button resRings (330, 90, 120, 80, Style::SIZE, Button::SINGLE, &intkTest, "Reset Ring Count");

Page temps (9, "Temperature"); //Motor temps
Text tempfl(75, 85, Style::CENTRE, TEXT_SMALL, &temps, "FL: %.1f", &std::get<3>(motors[0]), COLOR_BLACK);
Text tempbl(185, 85, Style::CENTRE, TEXT_SMALL, &temps, "BL: %.1f", &std::get<3>(motors[1]), COLOR_BLACK);
Text tempfr(295, 85, Style::CENTRE, TEXT_SMALL, &temps, "FR: %.1f", &std::get<3>(motors[2]), COLOR_BLACK);
Text tempbr(405, 85, Style::CENTRE, TEXT_SMALL, &temps, "BR: %.1f", &std::get<3>(motors[3]), COLOR_BLACK);
Text tempc(75, 175, Style::CENTRE, TEXT_SMALL, &temps, "C: %.1f", &std::get<3>(motors[4]), COLOR_BLACK);
Text tempf(185, 175, Style::CENTRE, TEXT_SMALL, &temps, "F: %.1f", &std::get<3>(motors[5]), COLOR_BLACK);
Text tempi(295, 175, Style::CENTRE, TEXT_SMALL, &temps, "I: %.1f", &std::get<3>(motors[6]), COLOR_BLACK);
Text tempu(405, 175, Style::CENTRE, TEXT_SMALL, &temps, "U: %.1f", &std::get<3>(motors[7]), COLOR_BLACK);

//Functions
void setup_field(){
  screen::set_pen(COLOR_WHITE);
  screen::draw_rect(270, 30, 470, 230);
  screen::draw_line(370, 30, 370, 230);
  screen::draw_line(270, 130, 470, 130);
}
void draw_field(){
  screen::set_pen(COLOR_RED);
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
  elasticUpTime = 800;
  elasticDownTime = 800;
  return std::make_pair(elasticUpTime, elasticDownTime);
}

void guiSetup(){ //Call once at start in initialize()
  prevPage.func = &(Page::toPrev);
  nextPage.func = &(Page::toNext);

  runElastic.func = &elasticUtil;

  resX.func = [&coord=tracking.x_coord](){printf("RESET X PLACEHOLDER\n");};
  resY.func = [&coord=tracking.y_coord](){printf("RESET Y PLACEHOLDER\n");};
  resA.func = [&coord=tracking.global_angle](){printf("RESET Angle PLACEHOLDER\n");};
  track.setupFunc = &setup_field;
  track.loopFunc = &draw_field;

  goToXYA.func = [&x=xVal.val, &y=yVal.val, &a=aVal.val](){delay(1000); move_to_target_sync(x, y, a, false);};
  goHome.func = [](){delay(1000); move_to_target_sync(0, 0, 0, false);};
  goCentre.func = [](){delay(1000); move_to_target_sync(72, 72, 0, false);};

  Button::createOptions({&route1, &route2, &route3});

  prevDrivr.func = &prevDriver;
  nextDrivr.func = &nextDriver;
  drivrName.setBackground(130, 70, 350, 190, Style::CORNER);

  tempfl.setBackground(40, 60, 70, 50, Style::SIZE);
  tempbl.setBackground(150, 60, 70, 50, Style::SIZE);
  tempfr.setBackground(260, 60, 70, 50, Style::SIZE);
  tempbr.setBackground(370, 60, 70, 50, Style::SIZE);
  tempc.setBackground(40, 150, 70, 50, Style::SIZE);
  tempf.setBackground(150, 150, 70, 50, Style::SIZE);
  tempi.setBackground(260, 150, 70, 50, Style::SIZE);
  tempu.setBackground(370, 150, 70, 50, Style::SIZE);
  //hate that i have to do it like this
  motors[0] = std::make_tuple(&front_l, &tempfl, "FRONT LEFT",  0);
  motors[1] = std::make_tuple(&front_r, &tempbl, "FRONT RIGHT", 0);
  motors[2] = std::make_tuple(&back_l,  &tempfr, "BACK LEFT",   0);
  motors[3] = std::make_tuple(&back_r,  &tempbr, "BACK RIGHT",  0);
  motors[4] = std::make_tuple(&f_bar,   &tempc,  "FOUR BAR",    0);
  motors[5] = std::make_tuple(&c_bar,   &tempf,  "CHAIN BAR",   0);
  motors[6] = std::make_tuple(&intk,    &tempi,  "INTAKE",      0);
  motors[7] = std::make_tuple(&uptk,    &tempu,  "UPTAKE",      0);

  Page::goTo(1);
}

//Utility to get coordinates for aligned objects, (buttons, sliders...) of same size
void alignedCoords (int x_objects, int y_objects, int x_btn, int y_btn, int x_range, int y_range){

  double x_space = (x_range-x_objects*x_btn)/(x_objects+1.0);
  double y_space = (y_range-y_objects*y_btn)/(y_objects+1.0);

  while ((fmod(x_space, 5) != 0))
    x_space = ((--x_range)-x_objects*x_btn)/(x_objects+1.0);

  while((fmod(y_space, 5) != 0))
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
void flash(std::uint32_t color, std::uint32_t time, std::string text){ //has a delay
  Page::clearScreen(color);
  screen::set_pen(~color&0xFFFFFF);
  screen::set_eraser(color);

  std::size_t space = text.find(' ', text.length()/2);
  if (space != std::string::npos){
    screen::print(TEXT_LARGE, (480-text.length()*CHAR_WIDTH_LARGE)/2, 70, text.substr(0, space).c_str());
    screen::print(TEXT_LARGE, (480-text.length()*CHAR_WIDTH_LARGE)/2, 100, text.substr(space+1).c_str());
  }
  else screen::print(TEXT_LARGE, (480-text.length()*CHAR_WIDTH_LARGE)/2, 95, text.c_str());

  master.rumble("-.-.-.-.");
  Flash.reset();
  flash_end = time;
}

void end_flash (){
  if (Flash.get_time() >= flash_end){
    Flash.reset(false);
    Page::goTo(Page::currentPage);
    std::uint32_t flash_end=std::numeric_limits<std::uint32_t>::max();
  }
}

std::tuple<int, int, int, int> fixPoints (int p1, int p2,int p3, int p4, Style type){
  int x1=p1, y1=p2, x2=p3, y2=p4, temp;

  // Different styles of evaluating the given coordinates
  if (type == Style::CENTRE){
    x1 -= p3;
    y1 -= p4;
    x2 += p1;
    y2 += p2;
  }

  if (type == Style::SIZE){
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
void Text::construct (int16_t pt1, int16_t pt2, Style type, text_format_e_t size, Page* page_ptr, std::string text, std::uint32_t label_color){
  txt_fmt = size;
  page = page_ptr;
  lcol = label_color;

  setTitle(pt1, pt2, type, text);


  (page->texts).push_back(this);
}

void Button::construct(int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style type, press_type prType, Page* page_ptr, std::string text, std::uint32_t background_color, std::uint32_t label_color){

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
      if (space != std::string::npos){
        label = text.substr(0, space);
        label1 = text.substr(space+1);

        text_x = (x1+x2-(label.length()*CHAR_WIDTH_SMALL))/2;
        text_x1 = (x1+x2-(label1.length()*CHAR_WIDTH_SMALL))/2;
        text_y = (y1+y2-CHAR_HEIGHT_SMALL)/2-CHAR_HEIGHT_SMALL;
        text_y1 = (y1+y2-CHAR_HEIGHT_SMALL)/2+CHAR_HEIGHT_SMALL;
      }
    }
}

Text::Text (int16_t pt1, int16_t pt2, Style type, text_format_e_t size, Page* page_ptr, std::string text, std::uint32_t label_color){
  construct(pt1, pt2, type, size, page_ptr, text, label_color);
}

Text::Text (int16_t pt1, int16_t pt2, Style type, text_format_e_t size, Page* page_ptr, std::string text, double* val_ref, std::uint32_t label_color){
  construct(pt1, pt2, type, size, page_ptr, text, label_color);
  val_ptr = val_ref;
}

Button::Button(int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style type, press_type prType, Page* page_ptr, std::string text, std::uint32_t background_color, std::uint32_t label_color){
  construct(pt1, pt2, pt3, pt4, type, prType, page_ptr, text, background_color, label_color);
}

Slider::Slider (int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style type, direction direction, int minimum, int maximum, Page* page_ptr, std::string text, std::uint32_t background_color, std::uint32_t label_color){
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

  // if (1) printf("This is not perm, it is %s\n", name.c_str());

  // buttons.push_back
  buttons.push_back(&prevPage);
  buttons.push_back(&nextPage);
}

//Methods
void Page::goTo(Page* page){
  std::array <Page*, PAGE_COUNT>::iterator it = std::find(pages.begin(), pages.end(), page);
  if (it == pages.end()) return;
  clearScreen(page->bcol);
  currentPage = page; //Saves new page then draws all the buttons on the page
  screen::set_pen(page->bcol);
  screen::set_eraser(page->bcol);
  screen::fill_rect(75, 0, 405, 20);
  screen::set_pen(COLOR_WHITE);
  screen::print(TEXT_SMALL, MID_X-(page->title.length()*CHAR_WIDTH_SMALL)/2, 5, "%s", page->title);
  for (std::vector <Button*>::iterator it = (page->buttons).begin(); it != (page->buttons).end(); it++) (*it)->draw();
  for (std::vector <Slider*>::iterator it = (page->sliders).begin(); it != (page->sliders).end(); it++) (*it)->draw();
  for (std::vector<Text*>::iterator it = (page->texts).begin(); it != (page->texts).end(); it++) (*it)->draw();
  if(page->setupFunc) page->setupFunc();
}

void Page::goTo(int page){
  goTo(pages[page]);
}

void Page::clearScreen(std::uint32_t color){
  screen::set_pen(color);
  screen::fill_rect(PAGE_LEFT, PAGE_UP, PAGE_RIGHT, PAGE_DOWN);
}

void Text::setTitle (int16_t pt1, int16_t pt2, Style type, std::string text){
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

void Text::setBackground (int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style type, std::uint32_t colour){
    std::tie(x1, y1, x2, y2) = fixPoints(pt1, pt2, pt3, pt4, type);
    setBackground(colour);

}

void Text::setBackground (std::uint32_t colour){
    std::uint32_t prevCol = bcol;
    bcol = colour;
    if (prevCol != bcol && page == Page::currentPage) draw();
}

void Text::draw(){
  if (x2 != 0 && y2 != 0){
    screen::set_pen(bcol);
    screen::fill_rect(x1, y1, x2, y2);
    screen::set_pen(lcol);
    screen::set_eraser(bcol);
  }
  else{
    screen::set_pen(lcol);
    screen::set_eraser(page->bcol);
  }

  if (val_ptr != nullptr){ //If there is a var to print
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
  if (latched){
    drawPressed();
    return;
  }
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
void Page::updateScreenStatus(){
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
  if (this == Page::currentPage){
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
  if (currentPage->loopFunc) currentPage->loopFunc();
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

bool temp_flashed = false; //Is never set back to false. Once it warns the driver, it won't again until program restarted
void guiBackground(){ //To be called continously
  //Saving vars for text display
  std::array<std::tuple<Motor*, Text*, std::string, double>, 8>::iterator it;
  for (it = motors.begin(); it != motors.end(); it++){ //Setting Motor temps
    double temperature = std::get<0>(*it)->get_temperature();
    std::get<3>(*it) = temperature == HUGE_VAL ? 0 : temperature;
  }

  ringCount = ring_count;
  angle = fmod(rad_to_deg(tracking.global_angle), 360);
  drivrName.setTitle (MID_X, MID_Y, Style::CENTRE, drivebase.drivers[drivebase.cur_driver].name);

  if (!Flash.playing() && !temp_flashed){ //Overheating Motors
    temp_flashed = true;
    for (it = motors.begin(); it != motors.end(); it++){
      if (std::get<3>(*it) >= 55){
        Page::goTo(&temps);
        char buffer[50];
        sprintf(buffer, "%s motor is at %.0fC\n", std::get<2>(*it).c_str(), std::get<3>(*it));
        flash(COLOR_RED, 10000, buffer);
        break;
      }
    }
  }

  for (it = motors.begin(); it != motors.end(); it++){ //Setting motor background colors
    if (std::get<3>(*it) <= 25) std::get<1>(*it)->setBackground(COLOR_DODGER_BLUE);
    else if (std::get<3>(*it) <= 35) std::get<1>(*it)->setBackground(COLOR_LAWN_GREEN);
    else if (std::get<3>(*it) <= 45) std::get<1>(*it)->setBackground(COLOR_YELLOW);
    else std::get<1>(*it)->setBackground(COLOR_RED);
  }

  Page::update();
  Button::update();
  Slider::update();
  Text::update();
  end_flash();
}
