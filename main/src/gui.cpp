#include "gui.hpp"

//Miscellaneous Tings
Timer Flash("Flash Timer", false);
std::uint32_t flash_end = std::numeric_limits<std::uint32_t>::max(); //Sets the flash's end time to max possible val
std::vector<std::bitset<200>> field (200, std::bitset<200>{}); //Initializes to 200 blank bitsets

//Var init for text monitoring
int angle, left_enc, right_enc, back_enc, elastic_up_time, elastic_down_time;
std::string pneum_1_state, pneum_2_state, driver_text, go_string;

//Static Variable Declarations
Page* Page::current_page = 0;
std::array<Page*, PAGE_COUNT> Page::pages = {};
last_touch_e_t Page::touch_status = E_TOUCH_RELEASED;
int16_t Page::x = 0, Page::y = 0;

//Pages and associated stuff
Page perm (0, "PERM BTNS", COLOR_PINK); //Page perm only exists because its a problem if permanent buttons think they belong to every page.
//So they think they belong to page perm, but every page knows they own these buttons.
Button prev_page(PAGE_LEFT, PAGE_UP, 75, 20, Style::SIZE, Button::SINGLE, &perm, "<-");
Button next_page(480, 0, -75, 20, Style::SIZE, Button::SINGLE, &perm, "->");

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
Text route (MID_X, 60, Style::CENTRE, TEXT_LARGE, &auto_selection, " Auton %d", &cur_auton);
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
Slider aVal(35, 175, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, &moving, "A");
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

Page go_sequence (PAGE_COUNT+1, "GO SEQUENCE");
Button go_button (300, MID_Y, 160, 90, Style::CENTRE, Button::SINGLE, &go_sequence, "PRESS TO");
Button go_back_button (20, USER_UP, 100, 50, Style::SIZE, Button::SINGLE, &go_sequence, "BACK");
Text go_button_text (300, 150, Style::CENTRE, TEXT_SMALL, &go_sequence, "%s", &go_string, (std::uint32_t)COLOR_BLACK);

//Functions
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
//Placeholder until func is actually written
std::pair <int, int> elasticUtil(){ //No need to return pair
  elastic_up_time = 800;
  elastic_down_time = 800;
  return std::make_pair(elastic_up_time, elastic_down_time);
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

bool go(std::string name, std::string message, std::uint32_t delay_time){ //Start
  printf("\n\n%s\n", message.c_str());
  printf("Press \"Continue\" when ready.\n");
  go_string = name;

  bool pressed = false, interrupted = false;
  Page* page = Page::current_page;
  Page::go_to(&go_sequence);

  while(go_button.pressed() && !interrupted){ //Wait for Release
    Page::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }
  while(!go_button.pressed() && !interrupted){ //Wait for Press
    Page::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }
  while(go_button.pressed() && !interrupted){ //Wait for Release
    Page::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }

  if (!interrupted){
    delay(delay_time);
    printf("Running\n");
  }
  else printf("Interrupted\n");
  Page::go_to(page);
  return !interrupted;
}

bool go(std::string name, std::uint32_t delay_time){ //End
  printf("Press Again when done.\n");
  go_string = "END " + name;

  bool pressed = false, interrupted = false;
  Page* page = Page::current_page;
  Page::go_to(&go_sequence);

  while(go_button.pressed() && !interrupted){ //Wait for Release
    Page::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }
  while(!go_button.pressed() && !interrupted){ //Wait for Press
    Page::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }
  while(go_button.pressed() && !interrupted){ //Wait for Release
    Page::update_screen_status();
    if (go_back_button.pressed()) interrupted = true;
    delay(2);
  }

  if (!interrupted){
    delay(delay_time);
    printf("Running\n");
  }
  else printf("Interrupted\n");
  Page::go_to(page);
  return !interrupted;
}

void gui_setup(){ //Call once at start in initialize()
  Page::go_to(0);

  prev_page.set_func(&Page::to_prev); //Gives the left and right buttons their functions
  next_page.set_func(&Page::to_next); //Same thing for all following buttons

  go_sequence.buttons.erase(go_sequence.buttons.begin(), go_sequence.buttons.begin()+2);
  go_button_text.set_background(ORANGE);

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

  mot_temp_1.set_background(40, 60, 70, 50, Style::SIZE);
  mot_temp_2.set_background(150, 60, 70, 50, Style::SIZE);
  mot_temp_3.set_background(260, 60, 70, 50, Style::SIZE);
  mot_temp_4.set_background(370, 60, 70, 50, Style::SIZE);
  mot_temp_5.set_background(40, 150, 70, 50, Style::SIZE);
  mot_temp_6.set_background(150, 150, 70, 50, Style::SIZE);
  mot_temp_7.set_background(260, 150, 70, 50, Style::SIZE);
  mot_temp_8.set_background(370, 150, 70, 50, Style::SIZE);

  Page::go_to(1); //Sets it to page 1 for program start. Don't delete this. If you want to change the starting page, call this in initialize()
}

//Utility to get coordinates for aligned objects, (buttons, sliders...) of same size
//Put in how many of buttons/sliders you want, and get properly spaced coords
void aligned_coords (int x_objects, int y_objects, int x_btn, int y_btn, int x_range, int y_range){

  double x_space = (x_range-x_objects*x_btn)/(x_objects+1.0);
  double y_space = (y_range-y_objects*y_btn)/(y_objects+1.0);

  while (fmod(x_space, 5)) //Recalculates until it gets a nice multiple of 5
    x_space = ((--x_range)-x_objects*x_btn)/(x_objects+1.0);

  while (fmod(y_space, 5))
    y_space = ((--y_range)-y_objects*y_btn)/(y_objects+1.0);

  for (int y = 0; y < y_objects; y++){
    for (int x = 0; x < x_objects; x++){
      printf("(%d, %d, %d, %d, Style::SIZE)\n", int(x_space)*(x+1) + x_btn*x, int(y_space)*(y+1) + y_btn*y + 20, x_btn, y_btn);
    }
    printf("\n");
  }
  printf("\nScreen Size: [%d, %d]\n", x_range, y_range);
  if ((x_space+x_btn)*(x_objects) > 480) printf("X out of bounds\n");
  if ((y_space+y_btn)*(y_objects) > 220) printf("Y out of bounds\n");
}

//Flashing
void flash(std::uint32_t color, std::uint32_t time, std::string text){
  Page::clear_screen(color);
  screen::set_pen(~color&0xFFFFFF); //Makes text inverted color of background so it is always visible
  screen::set_eraser(color);

  printf("%s", text.c_str());

  std::size_t space = text.find(' ', text.length()/2); //Spaces it out over 2 lines
  if (space != std::string::npos){
    screen::print(TEXT_LARGE, (480-text.length()*CHAR_WIDTH_LARGE)/2, 70, text.substr(0, space).c_str());
    screen::print(TEXT_LARGE, (480-text.length()*CHAR_WIDTH_LARGE)/2, 100, text.substr(space+1).c_str());
  }
  else screen::print(TEXT_LARGE, (480-text.length()*CHAR_WIDTH_LARGE)/2, 95, text.c_str());

  master.rumble("-.-.-.-.");
  Flash.reset(); //Starts counting
  flash_end = time;
}

void end_flash (){
  if (Flash.get_time() >= flash_end){
    Flash.reset(false);
    Page::go_to(Page::current_page);
    std::uint32_t flash_end = std::numeric_limits<std::uint32_t>::max(); //Sets it to max val so it'll never flash at weird times like 0
  }
}

//Formats coordinates based on a style (always in x1, y1, x2, y2)
std::tuple<int, int, int, int> fixPoints (int p1, int p2,int p3, int p4, Style type){
  int x1=p1, y1=p2, x2=p3, y2=p4, temp;

  // (p1, p2) is centre. p3, p4 are sizes from centre
  switch(type){
    case Style::CENTRE:
      x1 -= p3;
      y1 -= p4;
      x2 += p1;
      y2 += p2;
      break;

    case Style::SIZE:
      x2 += x1;
      y2 += y1;
      break;
  }

  //Putting coordinates in a left-right up-down order in case it was accidentally reversed or given negatives. Makes checking presses easier
  temp = std::max(x1, x2);
  x1 = std::min(x1, x2);
  x2 = temp;

  temp = std::max(y1, y2);
  y1 = std::min(y1, y2);
  y2 = temp;
  return {x1,y1,x2,y2}; //Gives back the fixed points as a tuple
}

void draw_oblong(int16_t x1, int16_t y1, int16_t x2, int16_t y2, double kS, double kR){ //ks and kr and scale values for shrink and radius
  int s = std::min(x2-x1, y2-y1)*kS; //Scale for shrinking button (pressed look)
  int r = std::min(x2-x1, y2-y1)*kR; //Scale for how rounded the button edges should be

  screen::fill_rect(x1+s, y1+s, x2-s, y2-s);
  screen::erase_rect(x1+s, y1+s, x1+s+r, y1+s+r);
  screen::erase_rect(x2-s, y1+s, x2-s-r, y1+s+r);
  screen::erase_rect(x1+s, y2-s, x1+s+r, y2-s-r);
  screen::erase_rect(x2-s, y2-s, x2-s-r, y2-s-r);
  screen::fill_circle(x1+s+r, y1+s+r, r);
  screen::fill_circle(x2-s-r, y1+s+r, r);
  screen::fill_circle(x1+s+r, y2-s-r, r);
  screen::fill_circle(x2-s-r, y2-s-r, r);
}

//Constructors
void Text::construct (int16_t pt1, int16_t pt2, Style rect_type, text_format_e_t size, Page* page_ptr, std::string text, std::variant<std::monostate, double*, int*, std::string*> val_pointer, std::uint32_t label_color){
  this->x = pt1;
  this->y = pt2;
  this->type = rect_type;
  this->txt_fmt = size;
  this->page = page_ptr;
  this->label = text;
  this->val_ptr = val_pointer;
  this->l_col = label_color;
  this->b_col = page->b_col;
  this->page->texts.push_back(this);
}

void Button::construct(int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style rect_type, press_type prType, Page* page_ptr, std::string text, std::uint32_t background_color, std::uint32_t label_color){

    //Saves params to class private vars
    this->b_col = background_color;
    this->b_col_dark = RGB2COLOR(int(COLOR2R(b_col)*0.8), int(COLOR2G(b_col)*0.8), int(COLOR2B(b_col)*0.8));
    this->l_col = label_color;
    this->form = prType;

    //Saves the buttons owning page
    this->page = page_ptr;
    (this->page->buttons).push_back(this);
    std::tie(this->x1, this->y1, this->x2, this->y2) = fixPoints(pt1, pt2, pt3, pt4, rect_type);

    if (8*text.length()+5 < x2-x1){
      this->text_x = (this->x1+this->x2-(text.length()*CHAR_WIDTH_SMALL))/2;
      this->text_y = (this->y1+this->y2-CHAR_HEIGHT_SMALL)/2;
      this->label = text;
    }
    else{ //Spaces it if it's too long for one line
      std::size_t space = text.find(' ', text.length()/2);
      if (space != std::string::npos){
        this->label = text.substr(0, space);
        this->label1 = text.substr(space+1);

        this->text_x = (this->x1+this->x2-(this->label.length()*CHAR_WIDTH_SMALL))/2;
        this->text_x1 = (this->x1+this->x2-(this->label1.length()*CHAR_WIDTH_SMALL))/2;
        this->text_y = (this->y1+this->y2-CHAR_HEIGHT_SMALL)/2-CHAR_HEIGHT_SMALL;
        this->text_y1 = (this->y1+this->y2-CHAR_HEIGHT_SMALL)/2+CHAR_HEIGHT_SMALL;
      }
    }
}

Text::Text (int16_t pt1, int16_t pt2, Style rect_type, text_format_e_t size, Page* page_ptr, std::string text, std::uint32_t label_color):
val_type(typeid(std::monostate)){
  construct(pt1, pt2, rect_type, size, page_ptr, text, std::monostate{}, label_color);
}

Text::Text (int16_t pt1, int16_t pt2, Style rect_type, text_format_e_t size, Page* page_ptr, std::string text, double* val_ref, std::uint32_t label_color):
val_type(typeid(double*)){
  construct(pt1, pt2, rect_type, size, page_ptr, text, val_ref, label_color);
}

Text::Text (int16_t pt1, int16_t pt2, Style rect_type, text_format_e_t size, Page* page_ptr, std::string text, int* val_ref, std::uint32_t label_color):
val_type(typeid(int*)){
  construct(pt1, pt2, rect_type, size, page_ptr, text, val_ref, label_color);
}

Text::Text (int16_t pt1, int16_t pt2, Style rect_type, text_format_e_t size, Page* page_ptr, std::string text, std::string* val_ref, std::uint32_t label_color):
val_type(typeid(std::string*)){
  construct(pt1, pt2, rect_type, size, page_ptr, text, val_ref, label_color);
}

Button::Button(int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style type, press_type prType, Page* page_ptr, std::string text, std::uint32_t background_color, std::uint32_t label_color){
  construct(pt1, pt2, pt3, pt4, type, prType, page_ptr, text, background_color, label_color);
}

Slider::Slider (int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style type, direction direction, int minimum, int maximum, Page* page_ptr, std::string text, std::uint32_t background_color, std::uint32_t label_color){
  //Saves params to class private vars
  this->max = maximum;
  this->min = minimum;
  this->b_col = background_color;
  this->l_col = label_color;
  this->label = text;
  this->dir = direction;

  //Saves the buttons owning page
  this->page = page_ptr;
  (this->page->sliders).push_back(this);

  std::tie(this->x1, this->y1, this->x2, this->y2) = fixPoints(pt1, pt2, pt3, pt4, type);

  switch(this->dir){
    case HORIZONTAL:
      this->text_x = (this->x1+this->x2-(this->label.length()*CHAR_WIDTH_SMALL))/2;
      this->text_y = this->y1-CHAR_HEIGHT_SMALL-2;
      inc.construct(this->x2+5, this->y1, this->x2+25, this->y2, Style::CORNER, Button::SINGLE, this->page, ">", this->l_col, this->b_col);
      dec.construct(this->x1-25, this->y1, this->x1-5, this->y2, Style::CORNER, Button::SINGLE, this->page, "<", this->l_col, this->b_col);
      break;

    case VERTICAL:
      this->text_x = (this->x1+this->x2-(this->label.length()*CHAR_WIDTH_SMALL))/2;
      this->text_y = (this->y1+this->y2)/2;
      inc.construct(this->x1, this->y1-25, this->x2, this->y1-5, Style::CORNER, Button::SINGLE, this->page, "▲", l_col, b_col);
      dec.construct(this->x1, this->y2+5, this->x2, this->y2+25, Style::CORNER, Button::SINGLE, this->page, "▼", l_col, b_col);
      break;
  }

  //Buttons
  dec.set_func([&value=this->val, &mini=this->min](){if(value != mini) value--;});
  inc.set_func([&value=this->val, &maxi=this->max](){if (value != maxi) value++;});
}

Page::Page(int page_number, std::string name, std::uint32_t background_color){
  //Page Constructor
  //Should call Page::go_to() to actually show the page
  this->page_num = page_number;
  if (this->page_num < PAGE_COUNT) pages[this->page_num] = this;
  this->title = name + " - " + std::to_string(this->page_num);
  this->b_col = background_color;

  buttons.push_back(&prev_page);
  buttons.push_back(&next_page);
}

//Methods
void Page::go_to(Page* page){
  std::array <Page*, PAGE_COUNT>::iterator it = std::find(pages.begin(), pages.end(), page);
  if (it == pages.end() && page != &go_sequence) return;
  clear_screen(page->b_col);
  current_page = page; //Saves new page then draws all the buttons on the page
  screen::set_pen(page->b_col);
  screen::set_eraser(page->b_col);
  screen::fill_rect(75, 0, 405, 20);
  screen::set_pen(COLOR_WHITE);
  screen::print(TEXT_SMALL, MID_X-(page->title.length()*CHAR_WIDTH_SMALL)/2, 5, "%s", page->title);
  for (std::vector <Button*>::iterator it = (page->buttons).begin(); it != (page->buttons).end(); it++) (*it)->draw();
  for (std::vector <Slider*>::iterator it = (page->sliders).begin(); it != (page->sliders).end(); it++) (*it)->draw();
  for (std::vector<Text*>::iterator it = (page->texts).begin(); it != (page->texts).end(); it++) (*it)->draw();
  if(page->setup_func) page->setup_func();
}

void Page::go_to(int page) {go_to(pages[page]);}

void Page::clear_screen(std::uint32_t color){
  screen::set_pen(color);
  screen::fill_rect(PAGE_LEFT, PAGE_UP, PAGE_RIGHT, PAGE_DOWN);
}

void Text::set_background (int16_t pt1, int16_t pt2, std::uint32_t colour){
  set_background(x-pt1, y-pt2, x+pt1, y+pt2, Style::CORNER, colour);
}

void Text::set_background (int16_t pt1, int16_t pt2, int16_t pt3, int16_t pt4, Style type, std::uint32_t colour){
    std::tie(x1, y1, x2, y2) = fixPoints(pt1, pt2, pt3, pt4, type);
    set_background(colour);
}

void Text::set_background (std::uint32_t colour){
  if (b_col != colour){
    b_col = colour;
    if (page == Page::current_page) draw();
  }
}

void Text::draw(){
  if (x2 != 0 && y2 != 0){
    screen::set_eraser(page->b_col); //Erases previous box
    screen::erase_rect(x1, y1, x2, y2);

    screen::set_pen(b_col);
    draw_oblong(x1, y1, x2, y2, 0, 0.15);
    screen::set_pen(l_col);
    screen::set_eraser(b_col);
  }
  else{
    screen::set_pen(l_col);
    screen::set_eraser(b_col);
  }

  char buffer [100];
  int length;

  if(val_type == typeid(double*)){
    length = sprintf(buffer, label.c_str(), *std::get<double*>(val_ptr));
    prev_val = *std::get<double*>(val_ptr);
  }

  else if (val_type == typeid(int*)){
    length = sprintf(buffer, label.c_str(), *std::get<int*>(val_ptr));
    prev_val = *std::get<int*>(val_ptr);
  }

  else if (val_type == typeid(std::string*)){
    length = sprintf(buffer, label.c_str(), (*std::get<std::string*>(val_ptr)).c_str());
    prev_val = *std::get<std::string*>(val_ptr);
  }

  else if (val_type == typeid(std::monostate)){
    length = sprintf(buffer, label.c_str());
  }

  int x_coord = x, y_coord = y;
  if (type == Style::CENTRE){
      switch(txt_fmt){
        case TEXT_SMALL:
          x_coord = x - (length*CHAR_WIDTH_SMALL)/2;
          y_coord = y - CHAR_HEIGHT_SMALL/2;
          break;

        case TEXT_MEDIUM:
          x_coord = x - (length*CHAR_WIDTH_MEDIUM)/2;
          y_coord = y - CHAR_HEIGHT_MEDIUM/2;
          break;

        case TEXT_LARGE:
          x_coord = x - (length*CHAR_WIDTH_LARGE)/2;
          y_coord = y - CHAR_HEIGHT_LARGE/2;
          break;
      }
    }

  screen::print(txt_fmt, x_coord, y_coord, "%s", buffer);
}

void Slider::draw(){
  screen::set_pen(b_col);
  screen::set_eraser(page->b_col);
  screen::erase_rect(x1, text_y, x2, y2);
  screen::fill_rect(x1, y1, x2, y2);
  screen::set_pen(l_col);
  if(dir == HORIZONTAL){
    screen::fill_rect(x1+1, y1+1, x1+(x2-x1)*(val-min)/(max-min), y2-1); //Draws Bar
    screen::print(TEXT_SMALL, x1, text_y, "%d", min); //Writes min
    screen::print(TEXT_SMALL, x2-CHAR_WIDTH_SMALL*std::to_string(max).length(), text_y, "%d", max); //Writes max
    screen::print(TEXT_SMALL, text_x, text_y, "%s:%.f", label.c_str(), val); //Writes current value
  }
  else{ //Vertical
    screen::fill_rect(x1+1, y2-(y2-y1)*(val-min)/(max-min), x2-1, y2-1);
    screen::print(TEXT_SMALL, x1-25, y2, "%d", min);
    screen::print(TEXT_SMALL, x1-25, y1, "%d", max);
    screen::print(TEXT_SMALL, text_x, y1-27-CHAR_HEIGHT_SMALL, "%s:%.f", label.c_str(), val);
  }
}

void Slider::draw_bar(){
  screen::set_eraser(b_col);
  screen::erase_rect(x1, y1, x2, y2);
  screen::set_pen(l_col);

  switch(dir){
    case HORIZONTAL:
      screen::fill_rect(x1+1, y1+1, x1+(x2-x1)*(val-min)/(max-min), y2-1);
      screen::print(TEXT_SMALL, text_x, text_y, "%s:%.f", label.c_str(), val);
      break;

    case VERTICAL:
      screen::fill_rect(x1+1, y2-(y2-y1)*(val-min)/(max-min), x2-1, y2-1);
      screen::print(TEXT_SMALL, text_x, y1-27-CHAR_HEIGHT_SMALL, "%s:%.f", label.c_str(), val);
      break;
  }
}

void Button::draw(){
  if (latched) { //Latched buttons must be drawn in a pressed state
    draw_pressed();
    return;
  }

  screen::set_pen(b_col);
  screen::set_eraser(Page::current_page->b_col);
  screen::fill_rect(x1, y1, x2, y2);
  draw_oblong(x1, y1, x2, y2, 0, 0.15);

  screen::set_pen(l_col);
  screen::set_eraser(b_col);
  screen::print(TEXT_SMALL, text_x, text_y, label.c_str());
  screen::print(TEXT_SMALL, text_x1, text_y1, label1.c_str());
}

void Button::draw_pressed(){
  screen::set_eraser(page->b_col); //Erases button
  screen::erase_rect(x1, y1, x2, y2);

  screen::set_pen(b_col_dark);
  screen::set_eraser(Page::current_page->b_col);
  draw_oblong(x1, y1, x2, y2, 0.04, 0.2);

  screen::set_pen(l_col);
  screen::set_eraser(b_col_dark);
  screen::print(TEXT_SMALL, text_x, text_y, label.c_str());
  screen::print(TEXT_SMALL, text_x1, text_y1, label1.c_str());
}

void Button::create_options(std::vector<Button*> buttons){
  std::vector<Button*>::iterator it, it2; //For clarity

  for (it = buttons.begin(); it != buttons.end(); it++){
    if ((*it)->form != LATCH && (*it)->form != TOGGLE) {printf("Option Feature is only available for latch and toggle buttons!\n"); return;}
  }

  for (it = buttons.begin(); it != buttons.end(); it++){//For each button in the list
    for (it2 = buttons.begin(); it2 != buttons.end(); it2++){//Go through the list and save each button
      if (*it != *it2) (*it)->options.push_back(*it2);
    }
  }
}


//Function Handling
void Page::set_setup_func(std::function <void()> function) {setup_func = function;}

void Page::set_loop_func(std::function <void()> function) {loop_func = function;}

void Button::set_func(std::function <void()> function) {func = function;}

void Button::set_off_func(std::function <void()> function) {off_func = function;}

void Button::run_func() {if (func) func();}

//Updating data and presses
void Page::update_screen_status(){
  screen_touch_status_s_t status = c::screen_touch_status();
  touch_status = status.touch_status;
  x = status.x;
  y = status.y;
}

void Slider::update_val(){
  // changes value if being touched
  if (page->pressed()){
    if ((x1 <= Page::x && Page::x <= x2) && (y1 <= Page::y && Page::y <= y2)){
      if (dir == HORIZONTAL) val = min+(max-min)*(Page::x-x1)/(x2-x1); //Gets val based on press location
      else val = min+(max-min)*(y2-Page::y)/(y2-y1);
    }
  }
}

bool Page::pressed(){
  if (this == Page::current_page){
    if (Page::touch_status == TOUCH_PRESSED || Page::touch_status == TOUCH_HELD) return true;
  }
  return false;
}

bool Button::pressed(){
  // returns true if the button is currently being pressed
  if (Page::current_page->pressed()){
    if ((x1 <= Page::x && Page::x <= x2) && (y1 <= Page::y && Page::y <= y2)) return true;
  }
  return false;
}

bool Button::new_press(){
  return (pressed() && !last_pressed);
}

bool Button::new_release(){
  return (!pressed() && last_pressed);
}

void Page::update(){
  Page::update_screen_status();
  if (current_page->loop_func) current_page->loop_func();

  Button::update();
  Slider::update();
  Text::update();
}

void Text::update(){
  for (std::vector<Text*>::iterator it = (Page::current_page->texts).begin(); it != (Page::current_page->texts).end(); it++){
    Text* text_id = *it;

    if(text_id->val_type != typeid(std::monostate)){ //If variable given
      if(text_id->val_type == typeid(double*)){
        if(std::get<double>(text_id->prev_val) != *std::get<double*>(text_id->val_ptr)) text_id->draw(); //If var has changed value
      }

      else if(text_id->val_type == typeid(int*)){
        // printf("U-%s: %d     %d\n", (text_id->label).c_str(), *std::get<int*>(text_id->val_ptr),   (std::get<int>(text_id->prev_val) != *std::get<int*>(text_id->val_ptr)));
        if(std::get<int>(text_id->prev_val) != *std::get<int*>(text_id->val_ptr)) text_id->draw(); //If var has changed value
      }

      else if(text_id->val_type == typeid(std::string*)){
        if(std::get<std::string>(text_id->prev_val) != *std::get<std::string*>(text_id->val_ptr)) text_id->draw(); //If var has changed value
      }

    }

  }
}

Slider* Slider::update(){
  for (std::vector <Slider*>::iterator it = (Page::current_page->sliders).begin(); it != (Page::current_page->sliders).end(); it++){
    Slider* sl_id = *it;
    sl_id->update_val();
    if (sl_id->val != sl_id->prev_val){
      sl_id->draw();
      sl_id->prev_val = sl_id->val;
      return sl_id;
    }
  }
  return 0;
}

Button* Button::update(){
  //Loops through the list of buttons on the current page to check for presses
  for (std::vector <Button*>::iterator it = (Page::current_page->buttons).begin(); it != (Page::current_page->buttons).end(); it++){
    Button* btn_id = *it;

    switch(btn_id->form){
      case Button::SINGLE:
        if (btn_id->new_press()){
          btn_id->last_pressed = 1;
          btn_id->draw_pressed();
          btn_id->run_func();
          return btn_id;
        }

        else if (btn_id->new_release()){
          btn_id->last_pressed = 0;
          btn_id->draw();
        }
        break;

      case Button::LATCH:
        if (btn_id->new_press()){
          btn_id->last_pressed = 1;
          btn_id->latched = !btn_id->latched; //Toggles the latch

          //Draws button's new state
          if(btn_id->latched){
            //Deselects connected buttons
            for (std::vector <Button*>::iterator option_it = (btn_id->options).begin(); option_it != (btn_id->options).end(); option_it++){
              (*option_it)->latched = 0;
              (*option_it)->draw();
            }

            btn_id->draw_pressed();
            btn_id->run_func();

          }
          else btn_id->draw(); //New press deselected it
          return btn_id;
        }

        else if (btn_id->new_release()){
          btn_id->last_pressed = 0;
        }
        if(btn_id->latched) btn_id->run_func();
        break;

      case Button::REPEAT:
        if (btn_id->new_press()){
          btn_id->last_pressed = 1;
          btn_id->draw_pressed();
          btn_id->run_func();
          return btn_id;
        }

        else if (btn_id->new_release()){
          btn_id->last_pressed = 0;
          btn_id->draw();
        }

        if (btn_id->pressed()) btn_id->run_func();
        break;

      case Button::TOGGLE:
        if (btn_id->new_press()){
          btn_id->last_pressed = 1;
          btn_id->on = !btn_id->on;

          //Draws button's new state
          if(btn_id->on){ //Just turned on
            //Turns off connected buttons
            for (std::vector <Button*>::iterator option_it = (btn_id->options).begin(); option_it != (btn_id->options).end(); option_it++){
              if ((*option_it)->on){ //If the other button is in the on state and has an off function
                (*option_it)->on = false;
                if ((*option_it)->off_func) (*option_it)->off_func();
              }
            }

            btn_id->run_func();
          }
          else if (btn_id->off_func) btn_id->off_func(); //Just turned off

          btn_id->draw_pressed();
          return btn_id;
        }

        else if (btn_id->new_release()){
          btn_id->last_pressed = 0;
          btn_id->draw();
        }
        break;
    }

  }

  return 0;
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

  Page::update();
  end_flash();
}
