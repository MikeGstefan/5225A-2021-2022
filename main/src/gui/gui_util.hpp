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
std::string port_nums = "";
std::array <std::tuple<int, Button*, Button*>, 8> motor_ports;

Page ports (1, "Ports");
Text mot (10, 50, Style::CORNER, TEXT_LARGE, &ports, "Motors: %s", &port_nums);
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
Text mot_text_1 (65, 115, Style::CENTRE, TEXT_SMALL, &motor, "Port %d", &std::get<0>(motor_ports[0]));
Text mot_text_2 (180, 115, Style::CENTRE, TEXT_SMALL, &motor, "Port %d", &std::get<0>(motor_ports[1]));
Text mot_text_3 (295, 115, Style::CENTRE, TEXT_SMALL, &motor, "Port %d", &std::get<0>(motor_ports[2]));
Text mot_text_4 (410, 115, Style::CENTRE, TEXT_SMALL, &motor, "Port %d", &std::get<0>(motor_ports[3]));
Text mot_text_5 (65, 180, Style::CENTRE, TEXT_SMALL, &motor, "Port %d", &std::get<0>(motor_ports[4]));
Text mot_text_6 (180, 180, Style::CENTRE, TEXT_SMALL, &motor, "Port %d", &std::get<0>(motor_ports[5]));
Text mot_text_7 (295, 180, Style::CENTRE, TEXT_SMALL, &motor, "Port %d", &std::get<0>(motor_ports[6]));
Text mot_text_8 (410, 180, Style::CENTRE, TEXT_SMALL, &motor, "Port %d", &std::get<0>(motor_ports[7]));
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

  motor_ports = {
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_1, &mot_stop_1),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_2, &mot_stop_2),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_3, &mot_stop_3),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_4, &mot_stop_4),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_5, &mot_stop_5),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_6, &mot_stop_6),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_7, &mot_stop_7),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_8, &mot_stop_8),
  };

  for (int port=0, i=0; port<=20; port++){
    if (c::registry_get_plugged_type(port) == c::E_DEVICE_MOTOR && i < 8){
      std::get<0>(motor_ports[i]) = port+1;
      i++;
    }
  }

  for (int i = 0; i < 8; i++){
    if (std::get<0>(motor_ports[i]) != std::numeric_limits<int>::max()) port_nums.append(std::to_string(std::get<0>(motor_ports[i])) + ",");
    std::get<1>(motor_ports[i])->set_func([i](){Motor motor(std::get<0>(motor_ports[i])); motor.move(mot_speed.val);});
    std::get<2>(motor_ports[i])->set_func([i](){Motor motor(std::get<0>(motor_ports[i])); motor.move(0);});
  }
  if (port_nums.back() == ',') port_nums.pop_back();

  encAB.set_background(90, 15);
  encCD.set_background(90, 15);
  encEF.set_background(90, 15);
  resAB.set_func([&](){encoderAB.reset();});
  resCD.set_func([&](){encoderCD.reset();});
  resEF.set_func([&](){encoderEF.reset();});
  resAll.set_func([&](){encoderAB.reset(); encoderCD.reset(); encoderEF.reset();});

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
