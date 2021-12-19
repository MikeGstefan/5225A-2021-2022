#pragma once
#include "gui.hpp"

//Field array
std::vector<std::bitset<200>> field (200, std::bitset<200>{}); //Initializes to 200 blank bitsets

//Var init for text monitoring
int angle, left_enc, right_enc, back_enc, elastic_up_time, elastic_down_time;
std::string pneum_1_state, pneum_2_state, driver_text;

Page driver_curve (1, "Drivers"); //Select a driver and their exp curve
Button prev_drivr(20, 70, 110, 120, Style::SIZE, Button::SINGLE, &driver_curve, "Prev Driver");
Text drivr_name(MID_X, MID_Y, Style::CENTRE, TEXT_LARGE, &driver_curve, "%s", &driver_text);
Button next_drivr(350, 70, 110, 120, Style::SIZE, Button::SINGLE, &driver_curve, "Next Driver");

Page temps (2, "Temperature"); //Motor temps
Text mot_temp_1(75, 85, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[0]) + std::string(": %dC"), &std::get<2>(motors[0]), COLOR_BLACK);
Text mot_temp_2(185, 85, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[1]) + std::string(": %dC"), &std::get<2>(motors[1]), COLOR_BLACK);
Text mot_temp_3(295, 85, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[2]) + std::string(": %dC"), &std::get<2>(motors[2]), COLOR_BLACK);
Text mot_temp_4(405, 85, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[3]) + std::string(": %dC"), &std::get<2>(motors[3]), COLOR_BLACK);
Text mot_temp_5(75, 175, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[4]) + std::string(": %dC"), &std::get<2>(motors[4]), COLOR_BLACK);
Text mot_temp_6(185, 175, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[5]) + std::string(": %dC"), &std::get<2>(motors[5]), COLOR_BLACK);
Text mot_temp_7(295, 175, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[6]) + std::string(": %dC"), &std::get<2>(motors[6]), COLOR_BLACK);
Text mot_temp_8(405, 175, Style::CENTRE, TEXT_SMALL, &temps, std::get<4>(motors[7]) + std::string(": %dC"), &std::get<2>(motors[7]), COLOR_BLACK);

Page auto_selection (3, "Auton"); //Select auton routes
Text route (MID_X, 60, Style::CENTRE, TEXT_LARGE, &auto_selection, "Auton %d", &cur_auton);
Button route1 (45, 90, 100, 80, Style::SIZE, Button::TOGGLE, &auto_selection, "Route 1");
Button route2 (190, 90, 100, 80, Style::SIZE, Button::TOGGLE, &auto_selection, "Route 2");
Button route3 (335, 90, 100, 80, Style::SIZE, Button::TOGGLE, &auto_selection, "Route 3");

Page track (4, "Tracking"); //Display tracking vals and reset btns
Text trackX(50, 45, Style::CENTRE, TEXT_SMALL, &track, "X:%.1f", &tracking.x_coord);
Text trackY(135, 45, Style::CENTRE, TEXT_SMALL, &track, "Y:%.1f", &tracking.y_coord);
Text trackA(220, 45, Style::CENTRE, TEXT_SMALL, &track, "A:%d", &angle);
Text encL(50, 130, Style::CENTRE, TEXT_SMALL, &track, "L:%.1f", &left_enc);
Text encR(135, 130, Style::CENTRE, TEXT_SMALL, &track, "R:%.1f", &right_enc);
Text encB(220, 130, Style::CENTRE, TEXT_SMALL, &track, "B:%d", &back_enc);
Button resX(15, 60, 70, 55, Style::SIZE, Button::SINGLE, &track, "Reset X");
Button resY(100, 60, 70, 55, Style::SIZE, Button::SINGLE, &track, "Reset Y");
Button resA(185, 60, 70, 55, Style::SIZE, Button::SINGLE, &track, "Reset A");
Button resAll(15, 160, 240, 60, Style::SIZE, Button::SINGLE, &track, "Reset All");

Page moving (5, "Moving"); //Moves to target, home, or centre
Slider xVal(35, 45, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, &moving, "X");
Slider yVal(35, 110, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, &moving, "Y");
Slider aVal(35, 175, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 360, &moving, "A");
Button go_to_xya(320, 45, 150, 40, Style::SIZE, Button::SINGLE, &moving, "Target");
Button go_home(320, 110, 150, 40, Style::SIZE, Button::SINGLE, &moving, "Home");
Button go_centre(320, 175, 150, 40, Style::SIZE, Button::SINGLE, &moving, "Centre");

Page intake_test (6, "Intake"); //Test for intake with rings
Text rings(MID_X, 50, Style::CENTRE, TEXT_LARGE, &intake_test, "Ring Count: %d", &ring_count);
Button reset_intake (30, 90, 120, 80, Style::SIZE, Button::SINGLE, &intake_test, "Reset Motor");
Button onOff (180, 90, 120, 80, Style::SIZE, Button::SINGLE, &intake_test, "Start/Stop");
Button reset_rings (330, 90, 120, 80, Style::SIZE, Button::SINGLE, &intake_test, "Reset Ring Count");

Page elastic (7, "Elastic Test"); //Testing the elastics on the lift
Button run_elastic(165, 60, 150, 55, Style::SIZE, Button::SINGLE, &elastic, "Run Elastic Test");
Text elastic_up (MID_X, 160, Style::CENTRE, TEXT_SMALL, &elastic, "Up Time: %d", &elastic_up_time);
Text elastic_down(MID_X, 180, Style::CENTRE, TEXT_SMALL, &elastic, "Down Time: %d", &elastic_down_time);

Page liftMove (8, "Lift"); //Moving the lift to an xyz position
Slider lift_x_val(35, 65, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, &liftMove, "X");
Slider lift_y_val(35, 150, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, &liftMove, "Y");
Button go_to_lift_xy(400, 128, 70, 50, Style::CENTRE, Button::SINGLE, &liftMove, "Move Lift To Target");

Page liftStates (9, "Lift States"); //Moving to various lift states
Button Neutral (15, 45, 100, 75, Style::SIZE, Button::SINGLE, &liftStates, "Neutral"); //Names are caps to avoid conflicts
Button RingPickup (130, 45, 100, 75, Style::SIZE, Button::SINGLE, &liftStates, "Ring Pickup");
Button Tip (245, 45, 100, 80, Style::SIZE, Button::SINGLE, &liftStates, "Tip");
Button Lowering (360, 45, 100, 75, Style::SIZE, Button::SINGLE, &liftStates, "Lowering");
Button Down (15, 140, 100, 75, Style::SIZE, Button::SINGLE, &liftStates, "Down");
Button Raised (130, 140, 100, 75, Style::SIZE, Button::SINGLE, &liftStates, "Raised");
Button Platform (245, 140, 100, 75, Style::SIZE, Button::SINGLE, &liftStates, "Platform");
Button ReleaseMogo(360, 140, 100, 75, Style::SIZE, Button::SINGLE, &liftStates, "Release Mogo");

Page tuning (10, "Tuning Tracking"); //Tests to tune tracking when on new base
Text tuning_instructions_1(MID_X, 35, Style::CENTRE, TEXT_SMALL, &tuning, "Press your desired tracking test");
Text tuning_instructions_2(MID_X, 50, Style::CENTRE, TEXT_SMALL, &tuning, "and follow the terminal for instructions");
Button turn_encoder (10, 75, 225, 70, Style::SIZE, Button::SINGLE, &tuning, "10 Turns");
Button perpendicular_error (245, 75, 225, 70, Style::SIZE, Button::SINGLE, &tuning, "Perpendicular Error");
Button grid (10, 155, 225, 70, Style::SIZE, Button::SINGLE, &tuning, "Grid");
Button spin360 (245, 155, 225, 70, Style::SIZE, Button::SINGLE, &tuning, "360 Spin");

Page pneumatic (11, "Pneumatics"); //Pneumatic testing page
Text pneum_text_1 (125, 50, Style::CENTRE, TEXT_SMALL, &pneumatic, "PORT G: %s", &pneum_1_state);
Text pneum_text_2 (350, 50, Style::CENTRE, TEXT_SMALL, &pneumatic, "PORT H: %s", &pneum_2_state);
Button pneum_btn_1 (25, 70, 200, 80, Style::SIZE, Button::TOGGLE, &pneumatic, "PNEUMATIC 1");
Button pneum_btn_2 (250, 70, 200, 80, Style::SIZE, Button::TOGGLE, &pneumatic, "PNEUMATIC 2");

//The following are not normal pages and should not be accessible by scrolling through the GUI.
Page testing (12, "Testing"); //Blank page made so it already exists when quick tests are created
Text testing_text_1 (125, 50, Style::CENTRE, TEXT_SMALL, &testing, "BLANK TEXT 1");
Text testing_text_2 (350, 50, Style::CENTRE, TEXT_SMALL, &testing, "BLANK TEXT 2");
Button testing_button_1 (25, 70, 200, 80, Style::SIZE, Button::SINGLE, &testing, "BLANK BUTTON 1");
Button testing_button_2 (250, 70, 200, 80, Style::SIZE, Button::SINGLE, &testing, "BLANK BUTTON 2");
Slider testing_slider (MID_X, 200, 200 , 20, Style::CENTRE, Slider::HORIZONTAL, -100, 100, &testing, "BLANK SLIDER");

//Functions
std::pair <int, int> elasticUtil(){ //Placeholder until func is actually written
  elastic_up_time = 800;
  elastic_down_time = 800;
  return std::make_pair(elastic_up_time, elastic_down_time); //No need to return pair
}
void Page::to_prev(){
  if (current_page == pages[1]) go_to(PAGE_COUNT-1);
  else go_to((current_page->page_num)-1);
}
void Page::to_next(){
  if (current_page == pages[PAGE_COUNT-1]) go_to(1);
  else go_to((current_page->page_num)+1);
}
void prev_driver(){
  if (drivebase.cur_driver == 0) drivebase.cur_driver = drivebase.num_of_drivers - 1;
  else drivebase.cur_driver--;
  WAIT_FOR_SCREEN_REFRESH();
  master.print(2, 0, "Driver: %s          ", drivebase.drivers[drivebase.cur_driver].name);
}
void next_driver(){
  drivebase.cur_driver++;
  drivebase.cur_driver %= drivebase.num_of_drivers;
  WAIT_FOR_SCREEN_REFRESH();
  master.print(2, 0, "Driver: %s          ", drivebase.drivers[drivebase.cur_driver].name);
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

  run_elastic.set_func(&elasticUtil);

  route1.set_func([&auton=cur_auton](){auton = 1;});
  route2.set_func([&auton=cur_auton](){auton = 2;});
  route3.set_func([&auton=cur_auton](){auton = 3;});

  resX.set_func(&resetX);
  resY.set_func(&resetY);
  resA.set_func(&resetA);
  resAll.set_func([](){resetX();resetY();resetA();});
  trackX.set_background(35, 10);
  trackY.set_background(35, 10);
  trackA.set_background(35, 10);
  encL.set_background(35, 10);
  encR.set_background(35, 10);
  encB.set_background(35, 10);
  for (int x = 0; x<200; x++) field[x].reset();
  track.set_setup_func([](){
    screen::set_pen(COLOR_WHITE);
    screen::draw_rect(270, 30, 470, 230);
    screen::draw_line(370, 30, 370, 230);
    screen::draw_line(270, 130, 470, 130);
    for (int x = 0; x<200; x++){
      for (int y = 0; y<200; y++) if(field[x].test(y)) screen::draw_pixel(270+x, 230-y); //Draws saved tracking values
    }
  });
  track.set_loop_func([](){
    screen::set_pen(COLOR_RED);
    screen::draw_pixel(270+(200*tracking.x_coord/144), 230-(200*tracking.y_coord/144)); //Scales to screen
  });

  go_to_xya.set_func([&x=xVal.val, &y=yVal.val, &a=aVal.val](){
    char coord_c[20];
    sprintf(coord_c, " (%d, %d, %d)", (int)x, (int)y, (int)a);
    std::string coord = coord_c;
    if (go("GO TO" + coord, "Press to move to target selected by sliders" + coord, 1000)) move_to_target_sync(x, y, a, false);
  });
  go_home.set_func([](){if (go("GO TO (0, 0, 0)", "Press to go to starting point (0, 0, 0)", 1000)) move_to_target_sync(0, 0, 0, false);});
  go_centre.set_func([](){if (go("GO TO (72, 72, 0)", "Press to go to centre field (72, 72, 0)", 1000)) move_to_target_sync(72, 72, 0, false);});

  Button::create_options({&route1, &route2, &route3}); //Makes them exclusive, only one can be selected at a time

  prev_drivr.set_func(&prev_driver);
  next_drivr.set_func(&next_driver);
  drivr_name.set_background(130, 70, 350, 190, Style::CORNER);

  turn_encoder.set_func([](){ //Turn the encoder
    if (go("START, THEN SPIN THE ENCODER", "Please spin the encoder any number of rotations.")){
      resetX();
      resetY();
      resetA();
      if (go("WHEN STOPPED")){
        printf("The left encoder found %d ticks\n", LeftEncoder.get_value() % 360);
        printf("The right encoder found %d ticks\n", RightEncoder.get_value() % 360);
        printf("The back encoder found %d ticks\n", BackEncoder.get_value() % 360);
      }
    }
  });
  perpendicular_error.set_func([](){ //Perpendicular Error
    if (go("START, THEN MOVE STRAIGHT ALONG Y", "Please run the robot along a known straight line in the y-axis.")){
      resetX();
      resetY();
      resetA();
      if (go("WHEN STOPPED")){
        if (tracking.x_coord < 0) printf("The robot thinks it strafed %.2f inches to the left.\nConsider turning the back tracking wheel counter-clockwise\n", tracking.x_coord);
        else if (tracking.x_coord > 0) printf("The robot thinks it strafed %.2f inches to the right.\nConsider turning the back tracking wheel clockwise\n", tracking.x_coord);
        else printf("The robot knows it strafed a perfect %.2f inches\n", tracking.x_coord); //Printing the tracking val just in case something went wrong. But it should always be 0
      }
    }
  });
  grid.set_func([](){ //Move in a random motion
    if (go("START, THEN MOVE RANDOMLY", "Please move the robot haphazardly around the field. Then return it back to the starting point.")){
      resetX();
      resetY();
      resetA();
      if (go("WHEN STOPPED")){
        printf("The robot thinks it deviated %.2f inches to the %s\n", fabs(tracking.x_coord), (tracking.x_coord < 0 ? "left" : "right"));
        printf("The robot thinks it deviated %.2f inches %s\n", fabs(tracking.y_coord), (tracking.y_coord < 0 ? "back" : "forward"));
        printf("The robot thinks it deviated %.2f degrees %s\n", fabs(rad_to_deg(tracking.global_angle)), (tracking.global_angle < 0 ? "counter-clockwise" : "clockwise"));
      }
    }
  });
  spin360.set_func([](){ //Robot turn accuracy
    if (go("START, THEN SPIN THE ROBOT", "Please spin the robot any number of rotations. Then return it back to the starting point")){
      resetX();
      resetY();
      resetA();
      if(go("WHEN STOPPED")){
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

bool temp_flashed = false; //Is never set back to false. Once it warns the driver, it won't again until program restarted
void gui_background(){ //To be called continously
  //Saving Field coords
  int x = 200*tracking.x_coord/144, y = 200*tracking.y_coord/144;
  if(inRange(x, 0, 199) && inRange(y, 0, 199)) field[x].set(y); //Saves position (x,y) to as tracked

  //Saving vars for text display
  angle = fmod(rad_to_deg(tracking.global_angle), 360);
  left_enc = LeftEncoder.get_value();
  right_enc = RightEncoder.get_value();
  back_enc = BackEncoder.get_value();
  driver_text = drivebase.drivers[drivebase.cur_driver].name;

  std::array<std::tuple<pros::Motor*, Text*, int, std::string, const char*>, 8>::iterator it;
  for (it = motors.begin(); it != motors.end(); it++){
    Motor* motor= std::get<0>(*it);
    Text* text = std::get<1>(*it);
    int temp = std::get<2>(*it);
    int temperature = motor != nullptr ? motor->get_temperature() : 0;
    temperature = temperature == std::numeric_limits<int>::max() ? 0 : temperature;
    std::get<2>(*it) = temperature;

    if (temp >= 55 && !Flash.playing() && !temp_flashed){ //Overheating
      temp_flashed = true;
      Page::go_to(&temps);
      char buffer[50];
      sprintf(buffer, "%s motor is at %dC\n", std::get<3>(*it).c_str(), temp);
      flash(COLOR_RED, 10000, buffer);
      break;
    }

    if (text != nullptr){ //Background Colors (temperature based)
      if (temp == 0) text->set_background(COLOR_WHITE); //0
      else if (temp <= 25) text->set_background(COLOR_DODGER_BLUE); //...20, 25
      else if (temp <= 35) text->set_background(COLOR_LAWN_GREEN); //30, 35
      else if (temp <= 45) text->set_background(COLOR_YELLOW); //40, 45
      else text->set_background(COLOR_RED); //50, 55, ...
    }
  }
  gui_general_background();
}
