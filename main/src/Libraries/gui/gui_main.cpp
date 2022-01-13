#include "gui.hpp"

#ifdef GUI_MAIN
#include "gui_main.hpp"

//Functions
std::pair <int, int> elasticUtil(){ //Placeholder until func is actually written
  elastic_up_time = 800;
  elastic_down_time = 800;
  return std::make_pair(elastic_up_time, elastic_down_time); //No need to return pair
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

void GUI::init(){ //Call once at start in initialize()
  GUI::setup();

  run_elastic.set_func(&elasticUtil);

  resX.set_func(&resetX);
  resY.set_func(&resetY);
  resA.set_func(&resetA);
  resAll.set_func([](){resetX();resetY();resetA();});
  for (int x = 0; x<200; x++) field[x].reset();
  track.set_setup_func([](){
    screen::set_pen(COLOR(WHITE));
    screen::draw_rect(270, 30, 470, 230);
    screen::draw_line(370, 30, 370, 230);
    screen::draw_line(270, 130, 470, 130);
    for (int x = 0; x<200; x++){
      for (int y = 0; y<200; y++) if(field[x].test(y)) screen::draw_pixel(270+x, 230-y); //Draws saved tracking values
    }
  });
  track.set_loop_func([](){
    screen::set_pen(COLOR(RED));
    screen::draw_pixel(270+(200*tracking.x_coord/144), 230-(200*tracking.y_coord/144)); //Scales to screen
  });

  go_to_xya.set_func([&x=xVal.val, &y=yVal.val, &a=aVal.val](){
    char coord_c[20];
    sprintf(coord_c, " (%d, %d, %d)", (int)x, (int)y, (int)a);
    std::string coord = coord_c;
    if (GUI::go("GO TO" + coord, "Press to move to target selected by sliders" + coord, 1000)) move_start(move_types::point, point_params({double(x), double(y), double(a)}));
  });
  go_home.set_func([](){
    if (GUI::go("GO TO (0, 0, 0)", "Press to go to starting point (0, 0, 0)", 1000)) move_start(move_types::point, point_params({0.0, 0.0, 0.0}));
  });
  go_centre.set_func([](){
    if (GUI::go("GO TO (72, 72, 0)", "Press to go to centre field (72, 72, 0)", 1000)) move_start(move_types::point, point_params({72.0, 72.0, 72.0}));
  });

  prev_drivr.set_func([](){drivebase.prev_driver();});
  next_drivr.set_func([](){drivebase.next_driver();});

  prev_auto.set_func([&](){cur_auton = previous_enum_value(cur_auton); auton_file_update();});
  next_auto.set_func([&](){cur_auton = next_enum_value(cur_auton); auton_file_update();});
  prev_auto.set_func(&prev_auton);
  next_auto.set_func(&next_auton);
  alliance.set_func([&](){switch_alliance();});
  alliance.add_text(ally_name);

  turn_encoder.set_func([](){ //Turn the encoder
    if (GUI::go("START, THEN SPIN THE ENCODER", "Please spin the encoder any number of rotations.")){
      resetX();
      resetY();
      resetA();
      if (GUI::go("WHEN STOPPED")){
        printf("The left encoder found %d ticks\n", LeftEncoder.get_value() % 360);
        printf("The right encoder found %d ticks\n", RightEncoder.get_value() % 360);
        printf("The back encoder found %d ticks\n", BackEncoder.get_value() % 360);
      }
    }
  });
  perpendicular_error.set_func([](){ //Perpendicular Error
    if (GUI::go("START, THEN MOVE STRAIGHT ALONG Y", "Please run the robot along a known straight line in the y-axis.")){
      resetX();
      resetY();
      resetA();
      if (GUI::go("WHEN STOPPED")){
        if (tracking.x_coord < 0) printf("The robot thinks it strafed %.2f inches to the left.\nConsider turning the back tracking wheel counter-clockwise\n", tracking.x_coord);
        else if (tracking.x_coord > 0) printf("The robot thinks it strafed %.2f inches to the right.\nConsider turning the back tracking wheel clockwise\n", tracking.x_coord);
        else printf("The robot knows it strafed a perfect %.2f inches\n", tracking.x_coord); //Printing the tracking val just in case something went wrong. But it should always be 0
      }
    }
  });
  grid.set_func([](){ //Move in a random motion
    if (GUI::go("START, THEN MOVE RANDOMLY", "Please move the robot haphazardly around the field. Then return it back to the starting point.")){
      resetX();
      resetY();
      resetA();
      if (GUI::go("WHEN STOPPED")){
        printf("The robot thinks it deviated %.2f inches to the %s\n", fabs(tracking.x_coord), (tracking.x_coord < 0 ? "left" : "right"));
        printf("The robot thinks it deviated %.2f inches %s\n", fabs(tracking.y_coord), (tracking.y_coord < 0 ? "back" : "forward"));
        printf("The robot thinks it deviated %.2f degrees %s\n", fabs(rad_to_deg(tracking.global_angle)), (tracking.global_angle < 0 ? "counter-clockwise" : "clockwise"));
      }
    }
  });
  spin360.set_func([](){ //Robot turn accuracy
    if (GUI::go("START, THEN SPIN THE ROBOT", "Please spin the robot any number of rotations. Then return it back to the starting point")){
      resetX();
      resetY();
      resetA();
      if(GUI::go("WHEN STOPPED")){
        printf("The robot is %.2f inches %s and %.2f inches %s off the starting point.\n", fabs(tracking.x_coord), (tracking.x_coord < 0 ? "left" : "right"), fabs(tracking.y_coord), (tracking.y_coord < 0 ? "back" : "forward"));

        int turned = fmod(rad_to_deg(tracking.global_angle), 360);
        int lost = 360-turned;
        float rotations = round(rad_to_deg(tracking.global_angle)/180)/2.0;
        if (180 < turned && turned < 355) printf("However, the robot lost %d degrees over %.1f rotations.\n Consider increasing the DistanceLR.\n", lost, rotations);
        else if (5 < turned && turned < 180) printf("However, The robot gained %d degrees over %.1f rotations.\n Consider decreasing the DistanceLR.\n", turned, rotations);
        else printf("This seems pretty accurate. It's %d degrees off of %.1f rotations.\n", turned >= fmod(rotations*360, 360) ? turned : lost, rotations);
      }
    }
  });

  pneum_btn_1.set_func([](){lift_piston.set_value(1);});
  pneum_btn_1.set_off_func([](){lift_piston.set_value(0);});

  pneum_btn_2.set_func([](){lift_piston.set_value(1);});
  pneum_btn_2.set_off_func([](){lift_piston.set_value(0);});

  std::get<4>(motors[0]) = &mot_temp_1;
  std::get<4>(motors[1]) = &mot_temp_2;
  std::get<4>(motors[2]) = &mot_temp_3;
  std::get<4>(motors[3]) = &mot_temp_4;
  std::get<4>(motors[4]) = &mot_temp_5;
  std::get<4>(motors[5]) = &mot_temp_6;
  std::get<4>(motors[6]) = &mot_temp_7;
  std::get<4>(motors[7]) = &mot_temp_8;

  for (int i = 0; i<4; i++){
    if (std::get<4>(motors[i])) std::get<4>(motors[i])->set_background(110*i+40, 60, 70, 50, Style::SIZE);
    if (std::get<4>(motors[i+4])) std::get<4>(motors[i+4])->set_background(110*i+40, 150, 70, 50, Style::SIZE);
  }

  Page::go_to(1); //Sets it to page 1 for program start. Don't delete this. If you want to change the starting page, re-call this in initialize()
  GUI::background();
}

bool temp_flashed = false; //Is never set back to false. Once it warns the driver, it won't again until program restarted
void GUI::background(){ //To be called continously
  //Saving Field coords
  int x = 200*tracking.x_coord/144, y = 200*tracking.y_coord/144;
  if(inRange(x, 0, 199) && inRange(y, 0, 199)) field[x].set(y); //Saves position (x,y) to as tracked

  //Saving vars for text display
  angle = fmod(rad_to_deg(tracking.global_angle), 360);
  left_enc = LeftEncoder.get_value();
  right_enc = RightEncoder.get_value();
  back_enc = BackEncoder.get_value();
  driver_text = drivebase.drivers[drivebase.cur_driver].name;

  std::array<std::tuple<pros::Motor*, int, const char*, const char*, Text*>, 8>::iterator it;
  for (it = motors.begin(); it != motors.end(); it++){
    std::tuple<pros::Motor*, int, const char*, const char*, Text*>& mot_tup = *it;
    Motor* motor= std::get<0>(mot_tup);
    Text* text = std::get<4>(mot_tup);
    std::get<1>(mot_tup) = motor != nullptr ? motor->get_temperature() : std::numeric_limits<int>::max();
    int temp = std::get<1>(mot_tup);

    if (temp >= 55 && temp != std::numeric_limits<int>::max() && !Flash.playing() && !temp_flashed){ //Overheating
      temp_flashed = true;
      Page::go_to(&temps);
      char buffer[50];
      sprintf(buffer, "%s motor is at %dC\n", std::get<2>(mot_tup), temp);
      GUI::flash(COLOR(RED), 15000, buffer);
      break;
    }

    if (text && temp == std::numeric_limits<int>::max()) text->set_active(false); //If performance is bad, move this to the page setup func

    if (text != nullptr){ //Background Colors (temperature based)
      if (temp == 0) text->set_background(COLOR(WHITE)); //0
      else if (temp <= 25) text->set_background(COLOR(DODGER_BLUE)); //...20, 25
      else if (temp <= 35) text->set_background(COLOR(LAWN_GREEN)); //30, 35
      else if (temp <= 45) text->set_background(COLOR(YELLOW)); //40, 45
      else text->set_background(COLOR(RED)); //50, 55, ...
    }
  }
  GUI::update();
}

#endif
