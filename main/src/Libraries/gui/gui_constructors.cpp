#include "gui.hpp"
#include "../include/pros/apix.h"
#include "../../auton.hpp"
#include <bitset>
// #include <cstdio>

/*Port Configs */ ADIEncoder encoderAB(1, 2, false), encoderCD(3, 4, true), encoderEF(5, 6, false);
/*Field array*/ std::vector<std::bitset<200>> field (200, std::bitset<200>{}); //Initializes to 200 blank bitsets
/*Temperature Alert Flag*/ static bool temp_flashed;
/*Current Flashing Timer*/ extern Timer Flash;

//Var init for text monitoring
int angle, left_enc, right_enc, back_enc, elastic_up_time, elastic_down_time;
const char* port_nums;
const char* driver_text;
std::array <std::tuple<int, Button*, Button*, int, char*>, 8> motor_ports;
//port, run, stop, stall counter, port and rpm

Page driver_curve ("Drivers"); //Select a driver and their exp curve
Button prev_drivr(20, 70, 110, 120, GUI::Style::SIZE, Button::SINGLE, driver_curve, "Prev Driver");
_Text drivr_name(MID_X, MID_Y, GUI::Style::CENTRE, TEXT_LARGE, driver_curve, "%s", driver_text);
Button next_drivr(350, 70, 110, 120, GUI::Style::SIZE, Button::SINGLE, driver_curve, "Next Driver");

Page temps ("Temperature"); //Motor temps //change string("") to ""s
_Text mot_temp_1(75, 85, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<2>(motors[0]) + ": %dC"s, std::get<1>(motors[0]), COLOUR(BLACK));
_Text mot_temp_2(185, 85, GUI::Style::CENTRE, TEXT_SMALL, temps, "2: %dC", std::get<1>(motors[1]), COLOUR(BLACK));
_Text mot_temp_3(295, 85, GUI::Style::CENTRE, TEXT_SMALL, temps, "3: %dC", std::get<1>(motors[2]), COLOUR(BLACK));
_Text mot_temp_4(405, 85, GUI::Style::CENTRE, TEXT_SMALL, temps, "4: %dC", std::get<1>(motors[3]), COLOUR(BLACK));
_Text mot_temp_5(75, 175, GUI::Style::CENTRE, TEXT_SMALL, temps, "5: %dC", std::get<1>(motors[4]), COLOUR(BLACK));
_Text mot_temp_6(185, 175, GUI::Style::CENTRE, TEXT_SMALL, temps, "6: %dC", std::get<1>(motors[5]), COLOUR(BLACK));
_Text mot_temp_7(295, 175, GUI::Style::CENTRE, TEXT_SMALL, temps, "7: %dC", std::get<1>(motors[6]), COLOUR(BLACK));
_Text mot_temp_8(405, 175, GUI::Style::CENTRE, TEXT_SMALL, temps, "8: %dC", std::get<1>(motors[7]), COLOUR(BLACK));

Page auto_selection ("Auton"); //Select auton routes
Button prev_auto(20, 50, 120, 100, GUI::Style::SIZE, Button::SINGLE, auto_selection, "Prev Auton");
Button next_auto(350, 50, 120, 100, GUI::Style::SIZE, Button::SINGLE, auto_selection, "Next Auton");
Button alliance(MID_X, 200, 150, 20, GUI::Style::CENTRE, Button::SINGLE, auto_selection);
_Text auto_name(MID_X, 100, GUI::Style::CENTRE, TEXT_LARGE, auto_selection, "%s", *auton_names, cur_auton);
_Text ally_name(MID_X, 200, GUI::Style::CENTRE, TEXT_MEDIUM, auto_selection, "Alliance: %s", *alliance_names, cur_alliance);

Page track ("Tracking"); //Display tracking vals and reset btns
_Text trackX(50, 45, GUI::Style::CENTRE, TEXT_SMALL, track, "X:%.1f", tracking.x_coord);
_Text trackY(135, 45, GUI::Style::CENTRE, TEXT_SMALL, track, "Y:%.1f", tracking.y_coord);
_Text trackA(220, 45, GUI::Style::CENTRE, TEXT_SMALL, track, "A:%d", angle);
_Text encL(50, 130, GUI::Style::CENTRE, TEXT_SMALL, track, "L:%.1f", left_enc);
_Text encR(135, 130, GUI::Style::CENTRE, TEXT_SMALL, track, "R:%.1f", right_enc);
_Text encB(220, 130, GUI::Style::CENTRE, TEXT_SMALL, track, "B:%d", back_enc);
Button resX(15, 60, 70, 55, GUI::Style::SIZE, Button::SINGLE, track, "Reset X");
Button resY(100, 60, 70, 55, GUI::Style::SIZE, Button::SINGLE, track, "Reset Y");
Button resA(185, 60, 70, 55, GUI::Style::SIZE, Button::SINGLE, track, "Reset A");
Button resAll(15, 160, 240, 60, GUI::Style::SIZE, Button::SINGLE, track, "Reset All");

Page moving ("Moving"); //Moves to target, home, or centre
Slider xVal(35, 45, 250, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 144, moving, "X");
Slider yVal(35, 110, 250, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 144, moving, "Y");
Slider aVal(35, 175, 250, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 360, moving, "A");
Button go_to_xya(320, 45, 150, 40, GUI::Style::SIZE, Button::SINGLE, moving, "Target");
Button go_home(320, 110, 150, 40, GUI::Style::SIZE, Button::SINGLE, moving, "Home");
Button go_centre(320, 175, 150, 40, GUI::Style::SIZE, Button::SINGLE, moving, "Centre");

Page intake_test ("Intake"); //Test for intake with rings
_Text rings(MID_X, 50, GUI::Style::CENTRE, TEXT_LARGE, intake_test, "Ring Count: %d", ring_count);
Button reset_intake (30, 90, 120, 80, GUI::Style::SIZE, Button::SINGLE, intake_test, "Reset Motor");
Button onOff (180, 90, 120, 80, GUI::Style::SIZE, Button::SINGLE, intake_test, "Start/Stop");
Button reset_rings (330, 90, 120, 80, GUI::Style::SIZE, Button::SINGLE, intake_test, "Reset Ring Count");

Page elastic ("Elastic Test"); //Testing the elastics on the lift
Button run_elastic(165, 60, 150, 55, GUI::Style::SIZE, Button::SINGLE, elastic, "Run Elastic Test");
_Text elastic_up (MID_X, 160, GUI::Style::CENTRE, TEXT_SMALL, elastic, "Up Time: %d", elastic_up_time);
_Text elastic_down(MID_X, 180, GUI::Style::CENTRE, TEXT_SMALL, elastic, "Down Time: %d", elastic_down_time);

Page liftMove ("Lift"); //Moving the lift to an xyz position
Slider lift_x_val(35, 65, 250, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 144, liftMove, "X");
Slider lift_y_val(35, 150, 250, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 144, liftMove, "Y");
Button go_to_lift_xy(400, 128, 70, 50, GUI::Style::CENTRE, Button::SINGLE, liftMove, "Move Lift To Target");

Page liftStates ("Lift States"); //Moving to various lift states
Button Neutral (15, 45, 100, 75, GUI::Style::SIZE, Button::SINGLE, liftStates, "Neutral"); //Names are caps to avoid conflicts
Button RingPickup (130, 45, 100, 75, GUI::Style::SIZE, Button::SINGLE, liftStates, "Ring Pickup");
Button Tip (245, 45, 100, 80, GUI::Style::SIZE, Button::SINGLE, liftStates, "Tip");
Button Lowering (360, 45, 100, 75, GUI::Style::SIZE, Button::SINGLE, liftStates, "Lowering");
Button Down (15, 140, 100, 75, GUI::Style::SIZE, Button::SINGLE, liftStates, "Down");
Button Raised (130, 140, 100, 75, GUI::Style::SIZE, Button::SINGLE, liftStates, "Raised");
Button Platform (245, 140, 100, 75, GUI::Style::SIZE, Button::SINGLE, liftStates, "Platform");
Button ReleaseMogo(360, 140, 100, 75, GUI::Style::SIZE, Button::SINGLE, liftStates, "Release Mogo");

Page tuning ("Tuning Tracking"); //Tests to tune tracking when on new base
_Text tuning_instructions_1(MID_X, 35, GUI::Style::CENTRE, TEXT_SMALL, tuning, "Press your desired tracking test");
_Text tuning_instructions_2(MID_X, 50, GUI::Style::CENTRE, TEXT_SMALL, tuning, "and follow the terminal for instructions");
Button turn_encoder (10, 75, 225, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "10 Turns");
Button perpendicular_error (245, 75, 225, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Perpendicular Error");
Button grid (10, 155, 225, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Grid");
Button spin360 (245, 155, 225, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "360 Spin");

Page pneumatic ("Pneumatics"); //Pneumatic testing page
_Text pneum_text_1 (125, 50, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "PORT G");
_Text pneum_text_2 (350, 50, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "PORT G - no config for 2");
Button pneum_btn_1 (25, 70, 200, 80, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "PNEUMATIC 1");
Button pneum_btn_2 (250, 70, 200, 80, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "PNEUMATIC 2");

Page ports ("Ports");
_Text mot (10, 50, GUI::Style::CORNER, TEXT_LARGE, ports, "Motors: %s", port_nums);
_Text enc (10, 100, GUI::Style::CORNER, TEXT_LARGE, ports, "Encoders: AB, CD, EF");
_Text pne (10, 150, GUI::Style::CORNER, TEXT_LARGE, ports, "Pneumatics: G, H");

Page encoders ("Encoders"); //Display tracking vals and reset btns
_Text encAB (85, 50, GUI::Style::CENTRE, TEXT_SMALL, encoders, "AB Encoder:%d", left_enc);
_Text encCD (240, 50, GUI::Style::CENTRE, TEXT_SMALL, encoders, "CD Encoder:%d", right_enc);
_Text encEF (395, 50, GUI::Style::CENTRE, TEXT_SMALL, encoders, "EF Encoder:%d", back_enc);
Button resAB (35, 75, 100, 50, GUI::Style::SIZE, Button::SINGLE, encoders, "Reset AB");
Button resCD (190, 75, 100, 50, GUI::Style::SIZE, Button::SINGLE, encoders, "Reset CD");
Button resEF (345, 75, 100, 50, GUI::Style::SIZE, Button::SINGLE, encoders, "Reset EF");
Button resAllEnc (240, 180, 200, 30, GUI::Style::CENTRE, Button::SINGLE, encoders, "Reset All");

Page motor ("Motor Control");
Slider mot_speed (MID_X, 60, 180 , 15, GUI::Style::CENTRE, Slider::HORIZONTAL, -127, 127, motor, "Speed");
_Text mot_text_1 (65, 115, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<4>(motor_ports[0]));
_Text mot_text_2 (180, 115, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<4>(motor_ports[1]));
_Text mot_text_3 (295, 115, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<4>(motor_ports[2]));
_Text mot_text_4 (410, 115, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<4>(motor_ports[3]));
_Text mot_text_5 (65, 180, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<4>(motor_ports[4]));
_Text mot_text_6 (180, 180, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<4>(motor_ports[5]));
_Text mot_text_7 (295, 180, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<4>(motor_ports[6]));
_Text mot_text_8 (410, 180, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<4>(motor_ports[7]));
Button mot_update_1 (15, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
Button mot_update_2 (130, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
Button mot_update_3 (245, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
Button mot_update_4 (360, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
Button mot_update_5 (15, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
Button mot_update_6 (130, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
Button mot_update_7 (245, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
Button mot_update_8 (360, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
Button mot_stop_1 (70, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");
Button mot_stop_2 (185, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");
Button mot_stop_3 (300, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");
Button mot_stop_4 (415, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");
Button mot_stop_5 (70, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");
Button mot_stop_6 (185, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");
Button mot_stop_7 (300, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");
Button mot_stop_8 (415, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");

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

void main_init(){
  run_elastic.set_func(&elasticUtil);

  resX.set_func(&resetX);
  resY.set_func(&resetY);
  resA.set_func(&resetA);
  resAll.set_func([](){resetX();resetY();resetA();});
  for (int x = 0; x < 200; x++) field[x].reset();
  track.set_setup_func([](){
    screen::set_pen(COLOUR(WHITE));
    screen::draw_rect(270, 30, 470, 230);
    screen::draw_line(370, 30, 370, 230);
    screen::draw_line(270, 130, 470, 130);
    for (int x = 0; x<200; x++){
      for (int y = 0; y<200; y++) if(field[x].test(y)) screen::draw_pixel(270+x, 230-y); //Draws saved tracking values
    }
  });
  track.set_loop_func([](){
    screen::set_pen(COLOUR(RED));
    screen::draw_pixel(270+(200*tracking.x_coord/144), 230-(200*tracking.y_coord/144)); //Scales to screen
  });

  go_to_xya.set_func([&](){
    char coord_c[20];
    sprintf(coord_c, " (%d, %d, %d)", (int)xVal.get_value(), (int)yVal.get_value(), (int)aVal.get_value());
    std::string coord = coord_c;
    if (GUI::go("GO TO" + coord, "Press to move to target selected by sliders" + coord, 1000)) move_start(move_types::point, point_params({double(xVal.get_value()), double(yVal.get_value()), double(aVal.get_value())}));
  });
  go_home.set_func([](){
    if (GUI::go("GO TO (0, 0, 0)", "Press to go to starting point (0, 0, 0)", 1000)) move_start(move_types::point, point_params({0.0, 0.0, 0.0}));
  });
  go_centre.set_func([](){
    if (GUI::go("GO TO (72, 72, 0)", "Press to go to centre field (72, 72, 0)", 1000)) move_start(move_types::point, point_params({72.0, 72.0, 72.0}));
  });

  prev_drivr.set_func([](){drivebase.prev_driver();});
  next_drivr.set_func([](){drivebase.next_driver();});

  prev_auto.set_func(&prev_auton);
  next_auto.set_func(&next_auton);
  alliance.set_func([&](){switch_alliance();}); //Has to be in a lambda since param type is alliances not void
  alliance.add_text(ally_name);

  turn_encoder.set_func([](){ //Turn the encoder
    if (GUI::go("START, THEN SPIN THE ENCODER", "Please spin the encoder any number of rotations.")){
      resetX();
      resetY();
      resetA();
      if (GUI::go_end("WHEN STOPPED")){
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
      if (GUI::go_end("WHEN STOPPED")){
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
      if (GUI::go_end("WHEN STOPPED")){
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
      if(GUI::go_end("WHEN STOPPED")){
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
    if (std::get<4>(motors[i])) std::get<4>(motors[i])->set_background(110*i+40, 60, 70, 50, GUI::Style::SIZE);
    if (std::get<4>(motors[i+4])) std::get<4>(motors[i+4])->set_background(110*i+40, 150, 70, 50, GUI::Style::SIZE);
  }
}

void main_background(){
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
      GUI::go_to(&temps);
      char buffer[50];
      sprintf(buffer, "%s motor is at %dC\n", std::get<2>(mot_tup), temp);
      GUI::flash(COLOUR(RED), 15000, buffer);
      break;
    }

    if (text && temp == std::numeric_limits<int>::max()) text->set_active(false); //If performance is bad, move this to the page setup func

    if (text != nullptr){ //Background Colours (temperature based)
      if (temp == 0) text->set_background(COLOUR(WHITE)); //0
      else if (temp <= 25) text->set_background(COLOUR(DODGER_BLUE)); //...20, 25
      else if (temp <= 35) text->set_background(COLOUR(LAWN_GREEN)); //30, 35
      else if (temp <= 45) text->set_background(COLOUR(YELLOW)); //40, 45
      else text->set_background(COLOUR(RED)); //50, 55, ...
    }
  }
}

void util_init(){
  ; //Don't know why, but the {} don't match up without this

  motor_ports = {
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_1, &mot_stop_1, 0, "378"),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_2, &mot_stop_2, 0, ""),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_3, &mot_stop_3, 0, ""),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_4, &mot_stop_4, 0, ""),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_5, &mot_stop_5, 0, ""),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_6, &mot_stop_6, 0, ""),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_7, &mot_stop_7, 0, ""),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_8, &mot_stop_8, 0, ""),
  };

  for (int port=0, i=0; port<=20; port++){
    if (c::registry_get_plugged_type(port) == c::E_DEVICE_MOTOR && i < 8){
      std::get<0>(motor_ports[i]) = port+1;
      i++;
    }
  }

  std::string port_num_string;
  for (int i = 0; i < 8; i++){
    std::get<4>(motor_ports[i]) = (char*)malloc(8*sizeof(char));
    if (std::get<0>(motor_ports[i]) != std::numeric_limits<int>::max()) port_num_string.append(std::to_string(std::get<0>(motor_ports[i])) + ",");
    else{
      std::get<1>(motor_ports[i])->set_active(false);
      std::get<2>(motor_ports[i])->set_active(false);
    }
    std::get<1>(motor_ports[i])->set_func([i](){c::motor_move(std::get<0>(motor_ports[i]), mot_speed.get_value());});
    std::get<2>(motor_ports[i])->set_func([i](){c::motor_move(std::get<0>(motor_ports[i]), 0);});
  }
  if (port_num_string.back() == ',') port_num_string.pop_back();
  port_nums = strdup(port_num_string.c_str());

  resAB.set_func([&](){encoderAB.reset();});
  resCD.set_func([&](){encoderCD.reset();});
  resEF.set_func([&](){encoderEF.reset();});
  resAll.set_func([&](){encoderAB.reset(); encoderCD.reset(); encoderEF.reset();});

  pneum_btn_1.set_func([](){c::adi_digital_write(7, true);});
  pneum_btn_1.set_off_func([](){c::adi_digital_write(7, false);});

  pneum_btn_2.set_func([](){c::adi_digital_write(7, true);});
  pneum_btn_2.set_off_func([](){c::adi_digital_write(7, false);}); //Won't always work if port is not configured as out
}

void util_background(){
  //Saving vars for text display
  left_enc = encoderAB.get_value();
  right_enc = encoderCD.get_value();
  back_enc = encoderEF.get_value();

//Motor Stalled
  for (int i = 0; i < 8; i++){
    if (std::get<0>(motor_ports[i]) != std::numeric_limits<int>::max()){
      Motor motor(std::get<0>(motor_ports[i]));
      strcpy(std::get<4>(motor_ports[i]), (std::to_string(std::get<0>(motor_ports[i])) + ": " + std::to_string((int)c::motor_get_actual_velocity(std::get<0>(motor_ports[i])))).c_str());
      if (fabs(motor.get_actual_velocity()) < fabs(motor.get_target_velocity())/4) std::get<3>(motor_ports[i]) += 1;
      else std::get<3>(motor_ports[i]) = 0;
      if (std::get<3>(motor_ports[i]) > 10){
        std::get<3>(motor_ports[i]) = 0;
        printf("Stopping Motor %d\n", std::get<0>(motor_ports[i]));
        motor.move(0);
      }
    }
  }
}

GUI g_main ({&driver_curve, &temps, &auto_selection, &track, &moving, &intake_test, &elastic, &liftMove, &liftStates, &tuning, &pneumatic}, &main_init, &main_background);

GUI g_util ({&ports, &encoders, &motor, &pneumatic}, &util_init, &util_background);
