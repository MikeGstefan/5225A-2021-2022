#include "gui.hpp"

//Static Variable Declarations
Page* Page::currentPage = 0;
std::array<Page*, 11> Page::pages = {};
last_touch_e_t Page::touch_status = E_TOUCH_RELEASED;
int16_t Page::x = 0, Page::y = 0;

//Permanent Buttons
void Page::toPrev(){
  if (currentPage == pages[1]) Page::goTo(pages[7]);
  else goTo(pages[currentPage->pageNum-1]);
}
void Page::toNext(){
  if (currentPage == pages[7]) Page::goTo(pages[1]);
  else goTo(pages[(currentPage->pageNum)+1]);
}

Page perm (0, "PERM BTNS", COLOR_PINK);
//Page perm only exists because its a problem if permanent buttons think they belong to every page.
//So they think they belong to page perm, but every page knows they own these buttons.
Button prevPage(0, 0, 40, 20, Button::SIZE, 0, &perm, "<-");
Button nextPage(480, 0, -40, 20, Button::SIZE, 0, &perm, "->");

void guiSetup(){

  prevPage.setTask(&(Page::toPrev));
  nextPage.setTask(&(Page::toNext));
}

//Constructors
Text::Text (int16_t point1, int16_t point2, text_format_e_t size, Page* page_ptr, std::string text, std::uint32_t label_color){
  x = point1;
  y = point2;
  txt_fmt = size;
  page = page_ptr;
  label = text;
  lcol = label_color;

  (page->texts).push_back(this);

  if(Page::currentPage == this->page || &perm == this->page) draw();
}

Button::Button(int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, style type, bool toggle, Page* page_ptr, std::string text, std::uint32_t background_color, std::uint32_t label_color){

  //Saves params to class private vars
  x1 = pt1;
  y1 = pt2;
  x2 = pt3;
  y2 = pt4;
  bcol = background_color;
  dark_bcol = RGB2COLOR(int(COLOR2R(bcol)*0.9), int(COLOR2G(bcol)*0.9), int(COLOR2B(bcol)*0.9));
  lcol = label_color;
  label = text;
  latch = toggle;

  //Saves the buttons owning page
  page = page_ptr;
  (page->buttons).push_back(this);

  // Different styles of evaluating the given coordinates
  if (type == CENTRE) {
    x1 -= pt3;
    y1 -= pt4;
    x2 += pt1;
    y2 += pt2;
  }

  if (type == SIZE) {
    x2 += x1;
    y2 += y1;
  }

  //Putting coordinates in a left-right up-down order.
  int16_t temp;
  temp = std::max(x1, x2);
  x1 = std::min(x1, x2);
  x2 = temp;

  temp = std::max(y1, y2);
  y1 = std::min(y1, y2);
  y2 = temp;

  text_x = (x1+x2-(label.length()*Page::char_width))/2;
  text_y = (y1+y2-Page::char_height)/2;


  if(Page::currentPage == this->page || &perm == this->page) draw();
}

Page::Page(int page_number, std::string name, std::uint32_t background_color){
  //Page Constructor
  //Should call Page::goTo() to actually show the page
  pageNum = page_number;
  pages[pageNum] = this;
  title = name;
  bcol = background_color;

  (this->buttons).push_back(&prevPage);
  (this->buttons).push_back(&nextPage);

}

void Page::goTo(Page* page){
  clearScreen(page->bcol);
  currentPage = page; //Saves new page then draws all the buttons on the page
  screen::set_pen(COLOR_WHITE);
  screen::set_eraser(page->bcol);
  screen::print(TEXT_SMALL, mid_x-(page->title.length()*Page::char_width)/2, 10, "%s", page->title);
  for (std::vector <Button*>::iterator it = (page->buttons).begin(); it != (page->buttons).end(); it++) (*it)->draw();
  for (std::vector<Text*>::iterator it = (page->texts).begin(); it != (page->texts).end(); it++) (*it)->draw();
}

void Page::clearScreen(std::uint32_t color){
  screen::set_pen(color);
  screen::fill_rect(Page::left, Page::up, Page::right, Page::down);
}

void Text::draw(){
  screen::set_eraser((this->page)->bcol);
  screen::set_pen(lcol);
  screen::print(txt_fmt, x, y, label.c_str());
}

void Button::draw(){
  screen::set_pen(bcol);
  screen::set_eraser(bcol);
  screen::fill_rect(x1, y1, x2, y2);
  screen::set_pen(lcol);
  screen::print(TEXT_SMALL, text_x, text_y, label.c_str()); //Centers label on button
}

void Button::drawPressed(){
  if(Page::currentPage == this->page || &perm == this->page){
    screen::set_eraser((this->page)->bcol);
    screen::erase_rect(x1, y1, x2, y2);
    screen::set_pen(dark_bcol);
    screen::set_eraser(dark_bcol);
    screen::fill_rect(x1+3, y1+3, x2-3, y2-3);
    screen::set_pen(lcol);
    screen::print(TEXT_SMALL, text_x, text_y, label.c_str()); //Centers label on button
  }
}

void Button::del(){
  //Looks for the button in the list of buttons for its page
  std::vector <Button*>::iterator it = find(((this->page)->buttons).begin(), ((this->page)->buttons).end(), this);

  //If the button is on the list it is removed
  if(it != ((this->page)->buttons).end()){
    if(Page::currentPage == this->page) screen::erase_rect(x1, y1, x2, y2); //Only if on current page
    (this->page)->buttons.erase(it);
  }
}

void Button::setTask(void (*pointer_to_function)()){
  funcPtr = pointer_to_function;
}

void Button::runTask(){
  if (funcPtr != 0) (*funcPtr)(); //runs function assigned to the button
}

void Page::updateScreenStatus() { //to be called continously
  screen_touch_status_s_t status = c::screen_touch_status();
  touch_status = status.touch_status;
  x = status.x;
  y = status.y;
}

bool Button::pressed(){
  // returns true if the button is currently being pressed
  if (Page::touch_status == TOUCH_PRESSED || Page::touch_status == TOUCH_HELD){
    if ((x1 <= Page::x && Page::x <= x2) && (y1 <= Page::y && Page::y <= y2)) return true;
  }
  return false;
}

bool Button::newPress(){
  // returns true if the button was just pressed
  if (Page::touch_status == TOUCH_PRESSED){
    if ((x1 <= Page::x && Page::x <= x2) && (y1 <= Page::y && Page::y <= y2)) return true;
  }
  return false;
}

Button* Button::getPress(){ //to be called continously
  //Loops through the list of buttons on the current page to check for presses
  for (std::vector <Button*>::iterator it = (Page::currentPage->buttons).begin(); it != (Page::currentPage->buttons).end(); it++){
    Button* btn_id = *it;

    if (btn_id->latch){ //If latch-type button
      if (btn_id->pressed() && !(btn_id->lastPressed)){ //If new press
        btn_id->lastPressed = 1;
        btn_id->latched = !(btn_id->latched); //Toggles the latch

        if(btn_id->latched) btn_id->drawPressed(); //Draws button's new state
        else btn_id->draw();

        return btn_id;
      }

      else if (!(btn_id->pressed()) && btn_id->lastPressed){//If new release
        btn_id->lastPressed = 0;
      }

      if (btn_id->latched){ //If in pressed state
        btn_id->runTask();
        delay(10);
      }
    }

    else { //If single-press-style button
      if (btn_id->pressed() && !(btn_id->lastPressed)){//If new press
        btn_id->lastPressed = 1;
        btn_id->drawPressed();
        btn_id->runTask();
        return btn_id;
      }

      else if (!(btn_id->pressed()) && btn_id->lastPressed){//If new release
        btn_id->lastPressed = 0;
        btn_id->draw();
      }
    }

  }
  return 0;
}
