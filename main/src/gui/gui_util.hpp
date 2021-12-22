#pragma once
#include "gui.hpp"
#include "../include/pros/apix.h"

// Port Configs
ADIEncoder encoderAB(1, 2, false), encoderCD(3, 4, true), encoderEF(5, 6, false);
Motor motor1(1), motor2(2), motor3(3), motor4(4), motor5(5), motor6(6), motor7(7), motor8(8);
ADIDigitalOut pneum_1(7), pneum_2(8);

//Var init for text monitoring
int left_enc, right_enc, back_enc;
std::string pneum_1_state, pneum_2_state;

Page ports (1, "Ports");
Text mot (10, 50, Style::CORNER, TEXT_LARGE, &ports, "Motors: 1,2,3,4,5,6,7,8");
Text enc (10, 100, Style::CORNER, TEXT_LARGE, &ports, "Encoders: AB, CD, EF");
Text pne (10, 150, Style::CORNER, TEXT_LARGE, &ports, "Pneumatics: G, H");

Page encoders (2, "Encoders"); //Display tracking vals and reset btns
Text encAB (85, 50, Style::CENTRE, TEXT_SMALL, &encoders, "AB Encoder:%d", &left_enc);
Text encCD (240, 50, Style::CENTRE, TEXT_SMALL, &encoders, "CD Encoder:%d", &right_enc);
Text encEF (395, 50, Style::CENTRE, TEXT_SMALL, &encoders, "EF Encoder:%d", &back_enc);
Button resAB (35, 75, 100, 50, Style::SIZE, Button::SINGLE, &encoders, "Reset AB");
Button resCD (190, 75, 100, 50, Style::SIZE, Button::SINGLE, &encoders, "Reset CD");
Button resEF (345, 75, 100, 50, Style::SIZE, Button::SINGLE, &encoders, "Reset EF");
Button resAll (240, 180, 200, 30, Style::CENTRE, Button::SINGLE, &encoders, "Reset All");

Page motor (3, "Motor Control");
Slider mot_speed (MID_X, 60, 200 , 15, Style::CENTRE, Slider::HORIZONTAL, -127, 127, &motor, "Speed");
Text mot_text_1 (65, 115, Style::CENTRE, TEXT_SMALL, &motor, "Port 1");
Text mot_text_2 (180, 115, Style::CENTRE, TEXT_SMALL, &motor, "Port 2");
Text mot_text_3 (295, 115, Style::CENTRE, TEXT_SMALL, &motor, "Port 3");
Text mot_text_4 (410, 115, Style::CENTRE, TEXT_SMALL, &motor, "Port 4");
Text mot_text_5 (65, 180, Style::CENTRE, TEXT_SMALL, &motor, "Port 5");
Text mot_text_6 (180, 180, Style::CENTRE, TEXT_SMALL, &motor, "Port 6");
Text mot_text_7 (295, 180, Style::CENTRE, TEXT_SMALL, &motor, "Port 7");
Text mot_text_8 (410, 180, Style::CENTRE, TEXT_SMALL, &motor, "Port 8");
Button mot_update_1 (15, 125, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Run");
Button mot_update_2 (130, 125, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Run");
Button mot_update_3 (245, 125, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Run");
Button mot_update_4 (360, 125, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Run");
Button mot_update_5 (15, 190, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Run");
Button mot_update_6 (130, 190, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Run");
Button mot_update_7 (245, 190, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Run");
Button mot_update_8 (360, 190, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Run");
Button mot_stop_1 (70, 125, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Stop");
Button mot_stop_2 (185, 125, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Stop");
Button mot_stop_3 (300, 125, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Stop");
Button mot_stop_4 (415, 125, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Stop");
Button mot_stop_5 (70, 190, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Stop");
Button mot_stop_6 (185, 190, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Stop");
Button mot_stop_7 (300, 190, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Stop");
Button mot_stop_8 (415, 190, 45, 30, Style::SIZE, Button::SINGLE, &motor, "Stop");

Page pneumatic (4, "Pneumatics"); //Pneumatic testing page
Text pneum_text_1 (125, 50, Style::CENTRE, TEXT_SMALL, &pneumatic, "PORT G: %s", &pneum_1_state);
Text pneum_text_2 (350, 50, Style::CENTRE, TEXT_SMALL, &pneumatic, "PORT H: %s", &pneum_2_state);
Button pneum_btn_1 (25, 70, 200, 80, Style::SIZE, Button::TOGGLE, &pneumatic, "PNEUMATIC 1");
Button pneum_btn_2 (250, 70, 200, 80, Style::SIZE, Button::TOGGLE, &pneumatic, "PNEUMATIC 2");

Page testing (5, "Testing"); //Blank page made so it already exists when quick tests are created
Text testing_text_1 (125, 50, Style::CENTRE, TEXT_SMALL, &testing, "BLANK TEXT 1");
Text testing_text_2 (350, 50, Style::CENTRE, TEXT_SMALL, &testing, "BLANK TEXT 2");
Button testing_button_1 (25, 70, 200, 80, Style::SIZE, Button::SINGLE, &testing, "BLANK BUTTON 1");
Button testing_button_2 (250, 70, 200, 80, Style::SIZE, Button::SINGLE, &testing, "BLANK BUTTON 2");
Slider testing_slider (MID_X, 200, 200 , 20, Style::CENTRE, Slider::HORIZONTAL, -100, 100, &testing, "BLANK SLIDER");

void GUI::setup(){ //Call once at start in initialize()
  GUI::general_setup();

  std::array <int, 8> motors = {};

  for (int port=0, i=0; port<=20; port++){
    if (c::registry_get_plugged_type(port) == c::E_DEVICE_MOTOR && i < 8){
      motors[i] = port+1;
      i++;
    }
  }


  encAB.set_background(90, 15);
  encCD.set_background(90, 15);
  encEF.set_background(90, 15);
  resAB.set_func([&](){encoderAB.reset();});
  resCD.set_func([&](){encoderCD.reset();});
  resEF.set_func([&](){encoderEF.reset();});
  resAll.set_func([&](){encoderAB.reset(); encoderCD.reset(); encoderEF.reset();});

  mot_update_1.set_func([&](){motor1.move(mot_speed.val);});
  mot_stop_1.  set_func([&](){motor1.move(0);});
  mot_update_2.set_func([&](){motor2.move(mot_speed.val);});
  mot_stop_2.  set_func([&](){motor2.move(0);});
  mot_update_3.set_func([&](){motor3.move(mot_speed.val);});
  mot_stop_3.  set_func([&](){motor3.move(0);});
  mot_update_4.set_func([&](){motor4.move(mot_speed.val);});
  mot_stop_4.  set_func([&](){motor4.move(0);});
  mot_update_5.set_func([&](){motor5.move(mot_speed.val);});
  mot_stop_5.  set_func([&](){motor5.move(0);});
  mot_update_6.set_func([&](){motor6.move(mot_speed.val);});
  mot_stop_6.  set_func([&](){motor6.move(0);});
  mot_update_7.set_func([&](){motor7.move(mot_speed.val);});
  mot_stop_7.  set_func([&](){motor7.move(0);});
  mot_update_8.set_func([&](){motor8.move(mot_speed.val);});
  mot_stop_8.  set_func([&](){motor8.move(0);});

  pneum_text_1.set_background(50, 15);
  pneum_btn_1.set_func([](){pneum_1.set_value(1); pneum_1_state = "ON";});
  pneum_btn_1.set_off_func([](){pneum_1.set_value(0); pneum_1_state = "OFF";});

  pneum_text_2.set_background(50, 15);
  pneum_btn_2.set_func([](){pneum_2.set_value(1); pneum_2_state = "ON";});
  pneum_btn_2.set_off_func([](){pneum_2.set_value(0); pneum_2_state = "OFF";});

  Page::go_to(1); //Sets it to page 1 for program start. Don't delete this. If you want to change the starting page, re-call this in initialize()
}

void GUI::background(){ //To be called continously

  //Saving vars for text display
  left_enc = encoderAB.get_value();
  right_enc = encoderCD.get_value();
  back_enc = encoderEF.get_value();

  GUI::general_background();
}
