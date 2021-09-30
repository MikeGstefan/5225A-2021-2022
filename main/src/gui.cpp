#include "gui.hpp"

//Static Declarations
Page* Page::currentPage = 0;
last_touch_e_t Button::touch_status = E_TOUCH_RELEASED;
int16_t Button::x = 0, Button::y = 0;

Page::Page(int num, std::uint32_t background_color){
  //Page Constructor
  //Should call Page::goTo() to actually show the page
  pageNum = num;
  page_bcolor = background_color;
}

void Page::goTo(Page* page){
  clearScreen(page->page_bcolor);
  currentPage = page; //Saves new page then draws all the buttons on the page
  for (std::vector <Button*>::iterator it = (page->buttons).begin(); it != (page->buttons).end(); it++){
    (*it)->draw();
  }
}

void Page::clearScreen(std::uint32_t color){
  screen::set_pen(color);
  screen::fill_rect(Page::left, Page::up, Page::right, Page::down);
}

Button::Button(int16_t point1, int16_t point2, int16_t point3, int16_t point4, style type, Page* page_ptr, std::string text, std::uint32_t background_color, std::uint32_t label_color){
  // Button constructor

  //Saves the buttons owning page
  page = page_ptr;
  (page->buttons).push_back(this);

  x1 = point1;  //Saves params to class private vars
  y1 = point2;
  x2 = point3;
  y2 = point4;
  bcolor = background_color;
  lcolor = label_color;
  label = text;

  // Different styles of evaluating the given coordinates
  if (type == CENTRE) {
    x1 -= point3;
    y1 -= point4;
    x2 += point1;
    y2 += point2;
  }

  if (type == SIZE) {
    x2 += x1;
    y2 += y1;
  }

  draw();
}

void Button::draw(){
  if(Page::currentPage == this->page){
    screen::set_pen(bcolor);
    screen::set_eraser(bcolor);
    screen::fill_rect(x1, y1, x2, y2);
    screen::set_pen(lcolor);
    screen::print(TEXT_SMALL, (x1+x2-(label.length()*Page::char_width))/2, (y1+y2-Page::char_height)/2, label.c_str()); //Centers label on button
    screen::set_eraser(COLOR_BLACK);
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

void Button::runTask(){ //to be called continously
  if (funcPtr != 0 && this->pressed()) (*funcPtr); //runs function assigned to the button
}

void Button::update_press_status() { //to be called continously
  screen_touch_status_s_t status = c::screen_touch_status();
  touch_status = status.touch_status;
  x = status.x;
  y = status.y;
}

bool Button::pressed(){
  // returns true if the button is currently being pressed
  if (touch_status == TOUCH_PRESSED || touch_status == TOUCH_HELD){
    if ((x1 <= x && x <= x2) && (y1 <= y && y <= y2)) return true;
    else if ((x2 <= x && x <= x1) && (y2 <= y && y <= y1)) return true;
  }
  return false;
}

bool Button::new_press(){
  // returns true if the button was just pressed
  if (touch_status == TOUCH_PRESSED){
    if ((x1 <= x && x <= x2) && (y1 <= y && y <= y2)) return true;
    else if ((x2 <= x && x <= x1) && (y2 <= y && y <= y1)) return true;
  }
  return false;
}

Button* Button::get_press(){
  if (touch_status == TOUCH_PRESSED || touch_status == TOUCH_HELD){
    //Loops through the list of buttons on the current page to check for presses
    for (std::vector <Button*>::iterator it = (Page::currentPage->buttons).begin(); it != (Page::currentPage->buttons).end(); it++){
      if((*it)->pressed()){
        //If pressed it will run the buttons task and then return the button
        (*it)->runTask();
        return *it;
      }
    }
  }
  return 0;
}
