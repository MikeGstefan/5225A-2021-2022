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

//Functions
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

void GUI::setup(){ //Call once at start in initialize()
  GUI::general_setup();
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

  Page::go_to(1); //Sets it to page 1 for program start. Don't delete this. If you want to change the starting page, re-call this in initialize()
}

void GUI::background(){ //To be called continously

  //Saving vars for text display
  left_enc = LeftEncoder.get_value();
  right_enc = RightEncoder.get_value();
  back_enc = BackEncoder.get_value();

  GUI::general_background();
}
