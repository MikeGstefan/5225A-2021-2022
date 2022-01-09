#include "gui.hpp"

#ifdef GUI_UTIL
#include "gui_util.hpp"

void GUI::init(){ //Call once at start in initialize()
  GUI::setup();

  motor_ports = {
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_1, &mot_stop_1, 0, "~"),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_2, &mot_stop_2, 0, "~"),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_3, &mot_stop_3, 0, "~"),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_4, &mot_stop_4, 0, "~"),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_5, &mot_stop_5, 0, "~"),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_6, &mot_stop_6, 0, "~"),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_7, &mot_stop_7, 0, "~"),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_8, &mot_stop_8, 0, "~"),
  };

  mot_text_1.set_background(50, 7);
  mot_text_2.set_background(50, 0);
  mot_text_3.set_background(50, 0);
  mot_text_4.set_background(50, 0);
  mot_text_5.set_background(50, 0);
  mot_text_6.set_background(50, 0);
  mot_text_7.set_background(50, 0);
  mot_text_8.set_background(50, 0);

  for (int port=0, i=0; port<=20; port++){
    if (c::registry_get_plugged_type(port) == c::E_DEVICE_MOTOR && i < 8){
      std::get<0>(motor_ports[i]) = port+1;
      i++;
    }
  }

  for (int i = 0; i < 8; i++){
    if (std::get<0>(motor_ports[i]) != std::numeric_limits<int>::max()) sprintf(port_nums, "%s%d,", port_nums, std::get<0>(motor_ports[i]));
    else{
      std::get<1>(motor_ports[i])->set_active(false);
      std::get<2>(motor_ports[i])->set_active(false);
    }
    std::get<1>(motor_ports[i])->set_func([i](){c::motor_move(std::get<0>(motor_ports[i]), mot_speed.val);});
    std::get<2>(motor_ports[i])->set_func([i](){c::motor_move(std::get<0>(motor_ports[i]), 0);});
  }
  // if (port_nums.back() == ',') port_nums.pop_back();

  encAB.set_background(90, 15);
  encCD.set_background(90, 15);
  encEF.set_background(90, 15);
  resAB.set_func([&](){encoderAB.reset();});
  resCD.set_func([&](){encoderCD.reset();});
  resEF.set_func([&](){encoderEF.reset();});
  resAll.set_func([&](){encoderAB.reset(); encoderCD.reset(); encoderEF.reset();});

  pneum_btn_1.set_func([](){c::adi_digital_write(7, true);});
  pneum_btn_1.set_off_func([](){c::adi_digital_write(7, false);});

  pneum_btn_2.set_func([](){c::adi_digital_write(8, true);});
  pneum_btn_2.set_off_func([](){c::adi_digital_write(8, false);}); //Won't always work if port is not configured as out

  Page::go_to(1); //Sets it to page 1 for program start. Don't delete this. If you want to change the starting page, re-call this in initialize()
}

void GUI::background(){ //To be called continously

  //Saving vars for text display
  left_enc = encoderAB.get_value();
  right_enc = encoderCD.get_value();
  back_enc = encoderEF.get_value();

  for (int i = 0; i < 8; i++){
    if (std::get<0>(motor_ports[i]) != std::numeric_limits<int>::max()){
      Motor motor(std::get<0>(motor_ports[i]));
      std::get<4>(motor_ports[i]) = (std::to_string(std::get<0>(motor_ports[i])) + ": " + std::to_string((int)c::motor_get_actual_velocity(std::get<0>(motor_ports[i])))).c_str();
      if (fabs(motor.get_actual_velocity()) < fabs(motor.get_target_velocity())/4) std::get<3>(motor_ports[i]) += 1;
      else std::get<3>(motor_ports[i]) = 0;
      if (std::get<3>(motor_ports[i]) > 10){
        std::get<3>(motor_ports[i]) = 0;
        printf("Stopping Motor %d\n", std::get<0>(motor_ports[i]));
        motor.move(0);
      }
    }
  }

  GUI::update();
}

#endif
