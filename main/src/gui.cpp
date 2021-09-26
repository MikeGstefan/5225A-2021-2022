#include "gui.hpp"
std::map <Button*, std::string> Button::buttons = {};

Button::Button (int16_t point1, int16_t point2, int16_t point3, int16_t point4, style type, std::string text, std::uint32_t background_color, std::uint32_t label_color){
  Button::buttons[this] = text;

  x1 = point1;
  y1 = point2;
  x2 = point3;
  y2 = point4;

  if (type == CENTRE) {
    x1 -= x2;
    y1 -= y2;
    x2 = x1 + 2*x2;  //These shouldn't be 2* but now they are cuz x2 and y2 change in the previous lines
    y2 = y1 + 2*y2;
  }

  if (type == SIZE) {
    x2 += x1;
    y2 += y1;
  }

  screen::set_pen(background_color);
  screen::set_eraser(background_color);
  screen::fill_rect(x1, y1, x2, y2);
  screen::set_pen(label_color);
  screen::print(TEXT_SMALL, (x1+x2-(text.length()*disp::char_width))/2, (y1+y2-disp::char_height)/2, text.c_str()); //Centers label on button
  screen::set_eraser(COLOR_BLACK);
}

void Button::del(){
  if(buttons.count(this)){
    screen::erase_rect(x1, y1, x2, y2);
    buttons.erase(this);
  }
}

last_touch_e_t Button::touch_status = E_TOUCH_RELEASED;
int16_t Button::x = 0, Button::y = 0;

void Button::update_press_status () {
  screen_touch_status_s_t status = c::screen_touch_status();
  touch_status = status.touch_status;
  x = status.x;
  y = status.y;
}

bool Button::pressed(){
  if (touch_status == TOUCH_PRESSED || touch_status == TOUCH_HELD){
    if ((x1 <= x && x <= x2) && (y1 <= y && y <= y2)) return true;
    else if ((x2 <= x && x <= x1) && (y2 <= y && y <= y1)) return true;
  }
  return false;
}

bool Button::new_press(){
  if (touch_status == TOUCH_PRESSED){
    if ((x1 <= x && x <= x2) && (y1 <= y && y <= y2)) return true;
    else if ((x2 <= x && x <= x1) && (y2 <= y && y <= y1)) return true;
  }
  return false;
}

Button* Button::get_press(){
  std::map <Button*, std::string>::iterator it = buttons.begin();
  for (; it != buttons.end(); it++){
    if((it->first)->pressed()){
      // printf("%s", (it->second).c_str());
      return (it->first);
    }
  }
  return 0;
}
