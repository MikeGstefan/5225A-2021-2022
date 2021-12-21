#pragma once
#include "gui.hpp"

//Var init for text monitoring
int left_enc, right_enc, back_enc;
std::string pneum_1_state, pneum_2_state;
//get rid of field pos

Page ports(1, "Ports");

Page track (1, "Tracking"); //Display tracking vals and reset btns
Text encL(50, 130, Style::CENTRE, TEXT_SMALL, &track, "L:%.1f", &left_enc);
Text encR(135, 130, Style::CENTRE, TEXT_SMALL, &track, "R:%.1f", &right_enc);
Text encB(220, 130, Style::CENTRE, TEXT_SMALL, &track, "B:%d", &back_enc);
Button resX(15, 60, 70, 55, Style::SIZE, Button::SINGLE, &track, "Reset X");
Button resY(100, 60, 70, 55, Style::SIZE, Button::SINGLE, &track, "Reset Y");
Button resA(185, 60, 70, 55, Style::SIZE, Button::SINGLE, &track, "Reset A");
Button resAll(15, 160, 240, 60, Style::SIZE, Button::SINGLE, &track, "Reset All");

//moving motors

Page pneumatic (2, "Pneumatics"); //Pneumatic testing page
Text pneum_text_1 (125, 50, Style::CENTRE, TEXT_SMALL, &pneumatic, "PORT G: %s", &pneum_1_state);
Text pneum_text_2 (350, 50, Style::CENTRE, TEXT_SMALL, &pneumatic, "PORT H: %s", &pneum_2_state);
Button pneum_btn_1 (25, 70, 200, 80, Style::SIZE, Button::TOGGLE, &pneumatic, "PNEUMATIC 1");
Button pneum_btn_2 (250, 70, 200, 80, Style::SIZE, Button::TOGGLE, &pneumatic, "PNEUMATIC 2");

Page testing (3, "Testing"); //Blank page made so it already exists when quick tests are created
Text testing_text_1 (125, 50, Style::CENTRE, TEXT_SMALL, &testing, "BLANK TEXT 1");
Text testing_text_2 (350, 50, Style::CENTRE, TEXT_SMALL, &testing, "BLANK TEXT 2");
Button testing_button_1 (25, 70, 200, 80, Style::SIZE, Button::SINGLE, &testing, "BLANK BUTTON 1");
Button testing_button_2 (250, 70, 200, 80, Style::SIZE, Button::SINGLE, &testing, "BLANK BUTTON 2");
Slider testing_slider (MID_X, 200, 200 , 20, Style::CENTRE, Slider::HORIZONTAL, -100, 100, &testing, "BLANK SLIDER");

// Page temps (6, "Temperature"); //Motor temps
// Text mot_temp_1(75, 85, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[0]) + std::string(": %dC"), &std::get<2>(motors[0]), COLOR_BLACK);
// Text mot_temp_2(185, 85, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[1]) + std::string(": %dC"), &std::get<2>(motors[1]), COLOR_BLACK);
// Text mot_temp_3(295, 85, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[2]) + std::string(": %dC"), &std::get<2>(motors[2]), COLOR_BLACK);
// Text mot_temp_4(405, 85, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[3]) + std::string(": %dC"), &std::get<2>(motors[3]), COLOR_BLACK);
// Text mot_temp_5(75, 175, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[4]) + std::string(": %dC"), &std::get<2>(motors[4]), COLOR_BLACK);
// Text mot_temp_6(185, 175, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[5]) + std::string(": %dC"), &std::get<2>(motors[5]), COLOR_BLACK);
// Text mot_temp_7(295, 175, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[6]) + std::string(": %dC"), &std::get<2>(motors[6]), COLOR_BLACK);
// Text mot_temp_8(405, 175, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[7]) + std::string(": %dC"), &std::get<2>(motors[7]), COLOR_BLACK);
// 
// void prev_driver(){
//   if (drivebase.cur_driver == 0) drivebase.cur_driver = drivebase.num_of_drivers - 1;
//   else drivebase.cur_driver--;
//   WAIT_FOR_SCREEN_REFRESH();
//   master.print(2, 0, "Driver: %s          ", drivebase.drivers[drivebase.cur_driver].name);
// }
// void next_driver(){
//   drivebase.cur_driver++;
//   drivebase.cur_driver %= drivebase.num_of_drivers;
//   WAIT_FOR_SCREEN_REFRESH();
//   master.print(2, 0, "Driver: %s          ", drivebase.drivers[drivebase.cur_driver].name);
// }

//Functions
void Page::to_prev(){
  if (current_page == pages[1]) go_to(PAGE_COUNT-1);
  else go_to((current_page->page_num)-1);
}
void Page::to_next(){
  if (current_page == pages[PAGE_COUNT-1]) go_to(1);
  else go_to((current_page->page_num)+1);
}
void resetX(){
  tracking.x_coord = 0;
  printf("Change to mike's task way of resetting x\n");
}
void resetY(){
  tracking.y_coord = 0;
  printf("Change to mike's task way of resetting y\n");
}
void resetA(){
  tracking.global_angle = 0;
  printf("Change to mike's task way of resetting a\n");
}

void gui_setup(){ //Call once at start in initialize()
  gui_general_setup();
  Page::go_to(0);

  resX.set_func(&resetX);
  resY.set_func(&resetY);
  resA.set_func(&resetA);
  resAll.set_func([](){resetX();resetY();resetA();});

  pneum_text_1.set_background(50, 15);
  pneum_btn_1.set_func([](){claw_in.set_value(1); pneum_1_state = "ON";});
  pneum_btn_1.set_off_func([](){claw_in.set_value(0); pneum_1_state = "OFF";});

  pneum_text_2.set_background(50, 15);
  pneum_btn_2.set_func([](){claw_out.set_value(1); pneum_2_state = "ON";});
  pneum_btn_2.set_off_func([](){claw_out.set_value(0); pneum_2_state = "OFF";});

  for (int i = 0; i<4; i++){
    std::get<1>(motors[i])->set_background(110*i+40, 60, 70, 50, Style::SIZE);
    std::get<1>(motors[i+4])->set_background(110*i+40, 150, 70, 50, Style::SIZE);
  }

  Page::go_to(1); //Sets it to page 1 for program start. Don't delete this. If you want to change the starting page, re-call this in initialize()
}

void gui_background(){ //To be called continously

  //Saving vars for text display
  left_enc = LeftEncoder.get_value();
  right_enc = RightEncoder.get_value();
  back_enc = BackEncoder.get_value();

  gui_general_background();
}
