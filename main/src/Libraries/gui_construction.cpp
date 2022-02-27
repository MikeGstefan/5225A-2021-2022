#include "gui.hpp"

/*Field array*/ static std::vector<std::bitset<200>> field (200, std::bitset<200>{}); //Initializes to 200 blank bitsets
/*Temperature Alert Flag*/ static bool temp_flashed;
/*Current Flashing Timer*/ extern Timer Flash;

//Var init for text monitoring
int left_enc, right_enc, back_enc, angle, elastic_up_time, elastic_down_time;
const char* motor_port_nums;
const char* no_motor_port_nums;
const char* expander_port_nums;
const char* no_pneumatic_port_nums;
const char* driver_text;
std::array<std::tuple<int, Button*, Button*, Text_*, int, char*>, 8> motor_ports; //port, run, stop, stall counter, port and rpm

//For gui to use
extern std::array<std::tuple<pros::Motor*, int, const char*, const char*, Text_*>, 8> motors_for_gui; //Declared in config.cpp
extern std::array<std::pair<pros::ADIDigitalOut*, const char*>, 8> pneumatics_for_gui; //Declared in config.cpp

Page driver_curve ("Drivers"); //Select a driver and their exp curve
Button prev_drivr(20, 70, 110, 120, GUI::Style::SIZE, Button::SINGLE, driver_curve, "Prev Driver");
Text drivr_name(MID_X, MID_Y, GUI::Style::CENTRE, TEXT_LARGE, driver_curve, "%s", driver_text);
Button next_drivr(350, 70, 110, 120, GUI::Style::SIZE, Button::SINGLE, driver_curve, "Next Driver");

Page temps ("Temperature"); //Motor temps //make actual motor names
Text mot_temp_1(75, 85, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors_for_gui[0]) + ": %dC"s, std::get<1>(motors_for_gui[0]), COLOUR(BLACK));
Text mot_temp_2(185, 85, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors_for_gui[1]) + ": %dC"s, std::get<1>(motors_for_gui[1]), COLOUR(BLACK));
Text mot_temp_3(295, 85, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors_for_gui[2]) + ": %dC"s, std::get<1>(motors_for_gui[2]), COLOUR(BLACK));
Text mot_temp_4(405, 85, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors_for_gui[3]) + ": %dC"s, std::get<1>(motors_for_gui[3]), COLOUR(BLACK));
Text mot_temp_5(75, 175, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors_for_gui[4]) + ": %dC"s, std::get<1>(motors_for_gui[4]), COLOUR(BLACK));
Text mot_temp_6(185, 175, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors_for_gui[5]) + ": %dC"s, std::get<1>(motors_for_gui[5]), COLOUR(BLACK));
Text mot_temp_7(295, 175, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors_for_gui[6]) + ": %dC"s, std::get<1>(motors_for_gui[6]), COLOUR(BLACK));
Text mot_temp_8(405, 175, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors_for_gui[7]) + ": %dC"s, std::get<1>(motors_for_gui[7]), COLOUR(BLACK));

Page auto_selection ("Auton Selector"); //Select auton routes
Button prev_auto(20, 50, 120, 100, GUI::Style::SIZE, Button::SINGLE, auto_selection, "Prev Auton");
Button next_auto(350, 50, 120, 100, GUI::Style::SIZE, Button::SINGLE, auto_selection, "Next Auton");
Text auto_name(MID_X, 100, GUI::Style::CENTRE, TEXT_LARGE, auto_selection, "%s", Autons::auton_names, Autons::cur_auton);
Button alliance(MID_X, 200, 150, 20, GUI::Style::CENTRE, Button::SINGLE, auto_selection);
Text ally_name(MID_X, 200, GUI::Style::CENTRE, TEXT_MEDIUM, auto_selection, "Alliance: %s", Autons::alliance_names, Autons::cur_alliance);

Page pos_auto_selection ("Fake Autons"); //Select auton routes
Button prev_start_position(20, 110, 100, 40, GUI::Style::SIZE, Button::SINGLE, pos_auto_selection, "Prev Start");
Button next_start_position(460, 110, -100, 40, GUI::Style::SIZE, Button::SINGLE, pos_auto_selection, "Next Start");
Button goal_1(15, 35, 140, 55, GUI::Style::SIZE, Button::TOGGLE, pos_auto_selection, "Goal 1");
Button goal_2(170, 35, 140, 55, GUI::Style::SIZE, Button::TOGGLE, pos_auto_selection, "Tall Goal");
Button goal_3(325, 35, 140, 55, GUI::Style::SIZE, Button::TOGGLE, pos_auto_selection, "Goal 3");
Text start_pos_name(MID_X, 130, GUI::Style::CENTRE, TEXT_LARGE, pos_auto_selection, "%s", Autons::start_pos_names, Autons::cur_start_pos);
Button pos_alliance(MID_X, 200, 150, 20, GUI::Style::CENTRE, Button::SINGLE, pos_auto_selection);
Text pos_ally_name(MID_X, 200, GUI::Style::CENTRE, TEXT_MEDIUM, pos_auto_selection, "Alliance: %s", Autons::alliance_names, Autons::cur_alliance);

Page track ("Tracking"); //Display tracking vals and reset btns
Text track_x(50, 45, GUI::Style::CENTRE, TEXT_SMALL, track, "X:%.1f", tracking.x_coord);
Text track_y(135, 45, GUI::Style::CENTRE, TEXT_SMALL, track, "Y:%.1f", tracking.y_coord);
Text track_a(220, 45, GUI::Style::CENTRE, TEXT_SMALL, track, "A:%d", std::function([](){return tracking.get_angle_in_deg();}));
Text enc_l(50, 130, GUI::Style::CENTRE, TEXT_SMALL, track, "L:%d", left_enc);
Text enc_r(135, 130, GUI::Style::CENTRE, TEXT_SMALL, track, "R:%d", right_enc);
Text enc_b(220, 130, GUI::Style::CENTRE, TEXT_SMALL, track, "B:%d", back_enc);
Button res_x(15, 60, 70, 55, GUI::Style::SIZE, Button::SINGLE, track, "Reset X");
Button res_y(100, 60, 70, 55, GUI::Style::SIZE, Button::SINGLE, track, "Reset Y");
Button res_a(185, 60, 70, 55, GUI::Style::SIZE, Button::SINGLE, track, "Reset A");
Button res_all(15, 160, 240, 60, GUI::Style::SIZE, Button::SINGLE, track, "Reset All");

Page moving ("Moving"); //Moves to target, home, or centre
Slider x_val(35, 45, 250, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 144, moving, "X");
Slider y_val(35, 110, 250, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 144, moving, "Y");
Slider a_val(35, 175, 250, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 360, moving, "A");
Button go_to_xya(320, 45, 150, 40, GUI::Style::SIZE, Button::SINGLE, moving, "Target");
Button go_home(320, 110, 150, 40, GUI::Style::SIZE, Button::SINGLE, moving, "Home");
Button go_centre(320, 175, 150, 40, GUI::Style::SIZE, Button::SINGLE, moving, "Centre");

Page intake_test ("Intake"); //Test for intake with rings
Text rings(MID_X, 50, GUI::Style::CENTRE, TEXT_LARGE, intake_test, "Ring Count: N/A");
Button reset_intake (30, 90, 120, 80, GUI::Style::SIZE, Button::SINGLE, intake_test, "Reset Motor");
Button intake_switch (180, 90, 120, 80, GUI::Style::SIZE, Button::TOGGLE, intake_test, "Start/Stop");
Button reset_rings (330, 90, 120, 80, GUI::Style::SIZE, Button::SINGLE, intake_test, "Reset Ring Count");

Page elastic ("Elastic Test"); //Testing the elastics on the lift
Button run_elastic(165, 60, 150, 55, GUI::Style::SIZE, Button::SINGLE, elastic, "Run Elastic Test");
Text elastic_up (MID_X, 160, GUI::Style::CENTRE, TEXT_SMALL, elastic, "Up Time: %d", elastic_up_time);
Text elastic_down(MID_X, 180, GUI::Style::CENTRE, TEXT_SMALL, elastic, "Down Time: %d", elastic_down_time);

Page motor_subsys ("Motorized Subsystems"); //Moving the lift
Slider lift_val(30, 45, 300, 35, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 0, motor_subsys, "Lift", 10);
Slider tilter_val(30, 110, 300, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 0, motor_subsys, "Tilter", 10);
Button lift_move(470, 45, -100, 40, GUI::Style::SIZE, Button::SINGLE, motor_subsys, "Move Lift");
Button tilter_move(470, 95, -100, 40, GUI::Style::SIZE, Button::SINGLE, motor_subsys, "Move Tilter");
Button claw_switch(470, 145, -100, 40, GUI::Style::SIZE, Button::TOGGLE, motor_subsys, "Claw");
Button end_effector_switch(470, 195, -100, 40, GUI::Style::SIZE, Button::TOGGLE, motor_subsys, "End Effector");

Page tuning ("Tuning Tracking"); //Tests to tune tracking when on new base
Text tuning_instructions_1(MID_X, 35, GUI::Style::CENTRE, TEXT_SMALL, tuning, "Press your desired tracking test");
Text tuning_instructions_2(MID_X, 50, GUI::Style::CENTRE, TEXT_SMALL, tuning, "and follow the terminal for instructions");
Button turn_encoder (10, 75, 225, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "10 Turns");
Button perpendicular_error (245, 75, 225, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Perpendicular Error");
Button grid (10, 155, 225, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Grid");
Button spin360 (245, 155, 225, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "360 Spin");

Page ports ("Ports"); //Shows what ports to use on builder util
Text mot (10, 50, GUI::Style::CORNER, TEXT_MEDIUM, ports, "Motors: %s", motor_port_nums);
Text expanders (10, 100, GUI::Style::CORNER, TEXT_MEDIUM, ports, "Expanders: %s", expander_port_nums);
Text no_mot (10, 150, GUI::Style::CORNER, TEXT_MEDIUM, ports, "No Motors: %s", no_motor_port_nums);
Text no_pneumatic (10, 200, GUI::Style::CORNER, TEXT_MEDIUM, ports, "No Pneumatics: %s", no_pneumatic_port_nums);

Page encoders ("Encoders"); //Display tracking vals and reset btns
Text AB (85, 35, GUI::Style::CENTRE, TEXT_SMALL, encoders, "AB");
Text CD (240, 35, GUI::Style::CENTRE, TEXT_SMALL, encoders, "CD");
Text EF (395, 35, GUI::Style::CENTRE, TEXT_SMALL, encoders, "EF");
Text AB_degs (85, 50, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Degs: %d", right_enc);
Text CD_degs (240, 50, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Degs: %d", left_enc);
Text EF_degs (395, 50, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Degs: %d", back_enc);
Text AB_rots (85, 65, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Rots: %d", std::function([](){return int(right_enc/360);}));
Text CD_rots (240, 65, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Rots: %d", std::function([](){return int(left_enc/360);}));
Text EF_rots (395, 65, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Rots: %d", std::function([](){return int(back_enc/360);}));
Text AB_remain (85, 80, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Remaining: %d", std::function([](){return abs(right_enc-360*int(round((right_enc+sgn(right_enc)*180)/360)));}));
Text CD_remain (240, 80, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Remaining: %d", std::function([](){return abs(left_enc-360*int(round((left_enc+sgn(right_enc)*180)/360)));}));
Text EF_remain (395, 80, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Remaining: %d", std::function([](){return abs(back_enc-360*int(round((back_enc+sgn(right_enc)*180)/360)));}));
Button AB_res (35, 100, 100, 50, GUI::Style::SIZE, Button::SINGLE, encoders, "Reset AB");
Button CD_res (190, 100, 100, 50, GUI::Style::SIZE, Button::SINGLE, encoders, "Reset CD");
Button EF_res (345, 100, 100, 50, GUI::Style::SIZE, Button::SINGLE, encoders, "Reset EF");
Button res_all_enc (240, 200, 200, 30, GUI::Style::CENTRE, Button::SINGLE, encoders, "Reset All");

Page motor ("Motor Control");
Slider mot_speed (MID_X, 60, 180 , 15, GUI::Style::CENTRE, Slider::HORIZONTAL, -127, 127, motor, "Speed");
Text mot_text_1 (65, 115, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[0]));
Text mot_text_2 (180, 115, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[1]));
Text mot_text_3 (295, 115, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[2]));
Text mot_text_4 (410, 115, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[3]));
Text mot_text_5 (65, 180, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[4]));
Text mot_text_6 (180, 180, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[5]));
Text mot_text_7 (295, 180, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[6]));
Text mot_text_8 (410, 180, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[7]));
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

Page pneumatic ("Pneumatics"); //Pneumatic testing page for random ports
Slider expander (MID_X, 60, 180 , 15, GUI::Style::CENTRE, Slider::HORIZONTAL, 0, 21, pneumatic, "Expander");
Text ADI_a_text (65, 115, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "A");
Text ADI_b_text (180, 115, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "B");
Text ADI_c_text (295, 115, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "C");
Text ADI_d_text (410, 115, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "D");
Text ADI_e_text (65, 180, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "E");
Text ADI_f_text (180, 180, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "F");
Text ADI_g_text (295, 180, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "G");
Text ADI_h_text (410, 180, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "H");
Button ADI_a (15, 125, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");
Button ADI_b (130, 125, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");
Button ADI_c (245, 125, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");
Button ADI_d (360, 125, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");
Button ADI_e (15, 190, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");
Button ADI_f (130, 190, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");
Button ADI_g (245, 190, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");
Button ADI_h (360, 190, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");

Page pneumatics ("Pneumatics"); //Pneumatic testing page for known ports
Button pneum_1 (15, 45, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics, pneumatics_for_gui[0].second); //Names are caps to avoid conflicts
Button pneum_2 (130, 45, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics, pneumatics_for_gui[1].second);
Button pneum_3 (245, 45, 100, 80, GUI::Style::SIZE, Button::TOGGLE, pneumatics, pneumatics_for_gui[2].second);
Button pneum_4 (360, 45, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics, pneumatics_for_gui[3].second);
Button pneum_5 (15, 140, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics, pneumatics_for_gui[4].second);
Button pneum_6 (130, 140, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics, pneumatics_for_gui[5].second);
Button pneum_7 (245, 140, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics, pneumatics_for_gui[6].second);
Button pneum_8 (360, 140, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics, pneumatics_for_gui[7].second);

Page motors ("Motor Control");
Slider mot_speed_set (MID_X, 60, 180 , 15, GUI::Style::CENTRE, Slider::HORIZONTAL, -127, 127, motors, "Speed");
Text mot_1_text (65, 115, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<2>(motors_for_gui[0]));
Text mot_2_text (180, 115, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<2>(motors_for_gui[1]));
Text mot_3_text (295, 115, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<2>(motors_for_gui[2]));
Text mot_4_text (410, 115, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<2>(motors_for_gui[3]));
Text mot_5_text (65, 180, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<2>(motors_for_gui[4]));
Text mot_6_text (180, 180, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<2>(motors_for_gui[5]));
Text mot_7_text (295, 180, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<2>(motors_for_gui[6]));
Text mot_8_text (410, 180, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<2>(motors_for_gui[7]));
Button mot_1_update (15, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Run");
Button mot_2_update (130, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Run");
Button mot_3_update (245, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Run");
Button mot_4_update (360, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Run");
Button mot_5_update (15, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Run");
Button mot_6_update (130, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Run");
Button mot_7_update (245, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Run");
Button mot_8_update (360, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Run");
Button mot_1_stop (70, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Stop");
Button mot_2_stop (185, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Stop");
Button mot_3_stop (300, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Stop");
Button mot_4_stop (415, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Stop");
Button mot_5_stop (70, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Stop");
Button mot_6_stop (185, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Stop");
Button mot_7_stop (300, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Stop");
Button mot_8_stop (415, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motors, "Stop");

//Expander vars
std::array<int, 21> expander_ports;
std::array<Button*, 8> expander_btns = {&ADI_a, &ADI_b, &ADI_c, &ADI_d, &ADI_e, &ADI_f, &ADI_g, &ADI_h};

void main_setup(){
  for (int x = 0; x < 200; x++) field[x].reset(); //Should be able to get rid of this

  driver_curve.set_loop_func([](){driver_text = drivebase.driver_name();});
  prev_drivr.set_func([](){drivebase.prev_driver();});
  next_drivr.set_func([](){drivebase.next_driver();});

  prev_auto.set_func(Autons::prev_route);
  next_auto.set_func(Autons::next_route);
  alliance.set_func([](){Autons::switch_alliance();}); //Called in lambda to specify void version
  alliance.add_text(ally_name);

  Button::create_options({&goal_1, &goal_2, &goal_3});
  prev_start_position.set_func(Autons::prev_start_pos);
  next_start_position.set_func(Autons::next_start_pos);
  goal_1.set_func([](){Autons::set_target_goal(1);});
  goal_2.set_func([](){Autons::set_target_goal(2);});
  goal_3.set_func([](){Autons::set_target_goal(3);});
  pos_alliance.set_func([](){Autons::switch_alliance();});
  pos_alliance.add_text(pos_ally_name);

  intake_switch.set_func([](){printf("sorry nathan");});
  
  res_x.set_func([](){tracking.reset(0.0, tracking.y_coord, rad_to_deg(tracking.global_angle));});
  res_y.set_func([](){tracking.reset(tracking.x_coord, 0.0, rad_to_deg(tracking.global_angle));});
  res_a.set_func([](){tracking.reset(tracking.x_coord, tracking.y_coord);});
  res_all.set_func([](){tracking.reset();});
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
    angle = tracking.get_angle_in_deg();
    left_enc = LeftEncoder.get_value();
    right_enc = RightEncoder.get_value();
    back_enc = BackEncoder.get_value();
  });

  go_to_xya.set_func([&](){
    int x = x_val.get_value(), y = y_val.get_value(), a = a_val.get_value();
    char coord_c[20];
    snprintf(coord_c, 20, " (%d, %d, %d)", x, y, a);
    std::string coord = coord_c;
    if (GUI::go("GO TO" + coord, "Press to move to target selected by sliders" + coord, 1000)) move_start(move_types::point, point_params({double(x), double(y), double(a)}));
  });
  go_home.set_func([](){
    if (GUI::go("GO TO (0, 0, 0)", "Press to go to starting point (0, 0, 0)", 1000)) move_start(move_types::point, point_params({0.0, 0.0, 0.0}));
  });
  go_centre.set_func([](){
    if (GUI::go("GO TO (72, 72, 0)", "Press to go to centre field (72, 72, 0)", 1000)) move_start(move_types::point, point_params({72.0, 72.0, 72.0}));
  });

  lift_move.set_func([&](){
    char coord_c[20];
    snprintf(coord_c, 20, " %d", lift_val.get_value());
    std::string coord = coord_c;
    if (GUI::go("Move lift to" + coord, "Press to move lift to" + coord, 1000)) {printf("sorry nathan");}//lift.move_absolute(lift_val.get_value());
  });
  tilter_move.set_func([&](){
    char coord_c[20];
    snprintf(coord_c, 20, " %d", tilter_val.get_value());
    std::string coord = coord_c;
    if (GUI::go("Move tilter to" + coord, "Press to move tilter to" + coord, 1000))printf("sorry nathan");// tilter.move_absolute(tilter_val.get_value());
  });
  claw_switch.set_func([](){printf("sorry nathan");});
  claw_switch.set_off_func([](){printf("sorry nathan");});
  end_effector_switch.set_func([](){printf("sorry nathan");});
  end_effector_switch.set_off_func([](){printf("sorry nathan");});

  //Known Pneumatics Port Setup
  if(pneumatics_for_gui[0].first){
    pneum_1.set_func([](){pneumatics_for_gui[0].first->set_value(HIGH);});
    pneum_1.set_off_func([](){pneumatics_for_gui[0].first->set_value(LOW);});
  }
  else pneum_1.set_active(false);
  if(pneumatics_for_gui[1].first){
    pneum_2.set_func([](){pneumatics_for_gui[1].first->set_value(HIGH);});
    pneum_2.set_off_func([](){pneumatics_for_gui[1].first->set_value(LOW);});
  }
  else pneum_2.set_active(false);
  if(pneumatics_for_gui[2].first){
    pneum_3.set_func([](){pneumatics_for_gui[2].first->set_value(HIGH);});
    pneum_3.set_off_func([](){pneumatics_for_gui[2].first->set_value(LOW);});
  }
  else pneum_3.set_active(false);
  if(pneumatics_for_gui[3].first){
    pneum_4.set_func([](){pneumatics_for_gui[3].first->set_value(HIGH);});
    pneum_4.set_off_func([](){pneumatics_for_gui[3].first->set_value(LOW);});
  }
  else pneum_4.set_active(false);
  if(pneumatics_for_gui[4].first){
    pneum_5.set_func([](){pneumatics_for_gui[4].first->set_value(HIGH);});
    pneum_5.set_off_func([](){pneumatics_for_gui[4].first->set_value(LOW);});
  }
  else pneum_5.set_active(false);
  if(pneumatics_for_gui[5].first){
    pneum_6.set_func([](){pneumatics_for_gui[5].first->set_value(HIGH);});
    pneum_6.set_off_func([](){pneumatics_for_gui[5].first->set_value(LOW);});
  }
  else pneum_6.set_active(false);
  if(pneumatics_for_gui[6].first){
    pneum_7.set_func([](){pneumatics_for_gui[6].first->set_value(HIGH);});
    pneum_7.set_off_func([](){pneumatics_for_gui[6].first->set_value(LOW);});
  }
  else pneum_7.set_active(false);
  if(pneumatics_for_gui[7].first){
    pneum_8.set_func([](){pneumatics_for_gui[7].first->set_value(HIGH);});
    pneum_8.set_off_func([](){pneumatics_for_gui[7].first->set_value(LOW);});
  }
  else pneum_8.set_active(false);

  //Known motors function setup
  if(std::get<0>(motors_for_gui[0])){
    mot_1_update.set_func([](){std::get<0>(motors_for_gui[0])->move(mot_speed_set.get_value());});
    mot_1_stop.set_func([](){std::get<0>(motors_for_gui[0])->move(0);});
  }
  else{
    mot_1_text.set_active(false);
    mot_1_update.set_active(false);
    mot_1_stop.set_active(false);
  }
  if(std::get<0>(motors_for_gui[1])){
    mot_2_update.set_func([](){std::get<0>(motors_for_gui[1])->move(mot_speed_set.get_value());});
    mot_2_stop.set_func([](){std::get<0>(motors_for_gui[1])->move(0);});
  }
  else{
    mot_2_text.set_active(false);
    mot_2_update.set_active(false);
    mot_2_stop.set_active(false);
  }
  if(std::get<0>(motors_for_gui[2])){
    mot_3_update.set_func([](){std::get<0>(motors_for_gui[2])->move(mot_speed_set.get_value());});
    mot_3_stop.set_func([](){std::get<0>(motors_for_gui[2])->move(0);});
  }
  else{
    mot_3_text.set_active(false);
    mot_3_update.set_active(false);
    mot_3_stop.set_active(false);
  }
  if(std::get<0>(motors_for_gui[3])){
    mot_4_update.set_func([](){std::get<0>(motors_for_gui[3])->move(mot_speed_set.get_value());});
    mot_4_stop.set_func([](){std::get<0>(motors_for_gui[3])->move(0);});
  }
  else{
    mot_4_text.set_active(false);
    mot_4_update.set_active(false);
    mot_4_stop.set_active(false);
  }
  if(std::get<0>(motors_for_gui[4])){
    mot_5_update.set_func([](){std::get<0>(motors_for_gui[4])->move(mot_speed_set.get_value());});
    mot_5_stop.set_func([](){std::get<0>(motors_for_gui[4])->move(0);});
  }
  else{
    mot_5_text.set_active(false);
    mot_5_update.set_active(false);
    mot_5_stop.set_active(false);
  }
  if(std::get<0>(motors_for_gui[5])){
    mot_6_update.set_func([](){std::get<0>(motors_for_gui[5])->move(mot_speed_set.get_value());});
    mot_6_stop.set_func([](){std::get<0>(motors_for_gui[5])->move(0);});
  }
  else{
    mot_6_text.set_active(false);
    mot_6_update.set_active(false);
    mot_6_stop.set_active(false);
  }
  if(std::get<0>(motors_for_gui[6])){
    mot_7_update.set_func([](){std::get<0>(motors_for_gui[6])->move(mot_speed_set.get_value());});
    mot_7_stop.set_func([](){std::get<0>(motors_for_gui[6])->move(0);});
  }
  else{
    mot_7_text.set_active(false);
    mot_7_update.set_active(false);
    mot_7_stop.set_active(false);
  }
  if(std::get<0>(motors_for_gui[7])){
    mot_8_update.set_func([](){std::get<0>(motors_for_gui[7])->move(mot_speed_set.get_value());});
    mot_8_stop.set_func([](){std::get<0>(motors_for_gui[7])->move(0);});
  }
  else{
    mot_8_text.set_active(false);
    mot_8_update.set_active(false);
    mot_8_stop.set_active(false);
  }


  turn_encoder.set_func([](){ //Turn the encoder
    if (GUI::go("START, THEN SPIN THE ENCODER", "Please spin the encoder any number of rotations.")){
      RightEncoder.reset();
      LeftEncoder.reset();
      BackEncoder.reset();
      tracking.reset();
      if (GUI::go_end("WHEN STOPPED")){
        printf("The left encoder found %d ticks\n", LeftEncoder.get_value() % 360);
        printf("The right encoder found %d ticks\n", RightEncoder.get_value() % 360);
        printf("The back encoder found %d ticks\n", BackEncoder.get_value() % 360);
      }
    }
  });
  perpendicular_error.set_func([](){ //Perpendicular Error
    if (GUI::go("START, THEN MOVE FORWARD ALONG Y", "Please run the robot along a known straight line in the y-axis.")){
      RightEncoder.reset();
      LeftEncoder.reset();
      BackEncoder.reset();
      tracking.reset();
      if (GUI::go_end("WHEN STOPPED")){
        if (tracking.x_coord < 0) printf("The robot thinks it strafed %.2f inches to the left.\nConsider turning the back tracking wheel counter-clockwise\n", tracking.x_coord);
        else if (tracking.x_coord > 0) printf("The robot thinks it strafed %.2f inches to the right.\nConsider turning the back tracking wheel clockwise\n", tracking.x_coord);
        else printf("The robot knows it strafed a perfect %.2f inches\n", tracking.x_coord); //Printing the tracking val just in case something went wrong. But it should always be 0
      }
    }
  });
  grid.set_func([](){ //Move in a random motion
    if (GUI::go("START, THEN MOVE RANDOMLY", "Please move the robot haphazardly around the field. Then return it back to the starting point.")){
      RightEncoder.reset();
      LeftEncoder.reset();
      BackEncoder.reset();
      tracking.reset();
      if (GUI::go_end("WHEN STOPPED")){
        printf("The robot thinks it deviated %.2f inches to the %s\n", fabs(tracking.x_coord), (tracking.x_coord < 0 ? "left" : "right"));
        printf("The robot thinks it deviated %.2f inches %s\n", fabs(tracking.y_coord), (tracking.y_coord < 0 ? "back" : "forward"));
        printf("The robot thinks it deviated %.2f degrees %s\n", fabs(rad_to_deg(tracking.global_angle)), (tracking.global_angle < 0 ? "counter-clockwise" : "clockwise"));
      }
    }
  });
  spin360.set_func([](){ //Robot turn accuracy
    if (GUI::go("START, THEN SPIN THE ROBOT", "Please spin the robot any number of rotations. Then return it back to the starting point")){
      RightEncoder.reset();
      LeftEncoder.reset();
      BackEncoder.reset();
      tracking.reset();
      if(GUI::go_end("WHEN STOPPED")){
        printf("The robot is %.2f inches %s and %.2f inches %s off the starting point.\n", fabs(tracking.x_coord), (tracking.x_coord < 0 ? "left" : "right"), fabs(tracking.y_coord), (tracking.y_coord < 0 ? "back" : "forward"));

        int turned = tracking.get_angle_in_deg();
        int lost = 360-turned;
        float rotations = round(rad_to_deg(tracking.global_angle)/180)/2.0;
        if (180 < turned && turned < 355) printf("However, the robot lost %d degrees over %.1f rotations.\n Consider increasing the DistanceLR.\n", lost, rotations);
        else if (5 < turned && turned < 180) printf("However, The robot gained %d degrees over %.1f rotations.\n Consider decreasing the DistanceLR.\n", turned, rotations);
        else printf("This seems pretty accurate. It's %d degrees off of %.1f rotations.\n", turned >= fmod(rotations*360, 360) ? turned : lost, rotations);
      }
    }
  });

  std::get<4>(motors_for_gui[0]) = &mot_temp_1;
  std::get<4>(motors_for_gui[1]) = &mot_temp_2;
  std::get<4>(motors_for_gui[2]) = &mot_temp_3;
  std::get<4>(motors_for_gui[3]) = &mot_temp_4;
  std::get<4>(motors_for_gui[4]) = &mot_temp_5;
  std::get<4>(motors_for_gui[5]) = &mot_temp_6;
  std::get<4>(motors_for_gui[6]) = &mot_temp_7;
  std::get<4>(motors_for_gui[7]) = &mot_temp_8;

  for (int i = 0; i < 7; i++) std::get<4>(motors_for_gui[i])->set_background(40, 30);
}

void main_background(){
  //Saving Field coords
  int x = 200*tracking.x_coord/144, y = 200*tracking.y_coord/144;
  if(inRange(x, 0, 199) && inRange(y, 0, 199)) field[x].set(y); //Saves position (x,y) to as tracked

  for (std::array<std::tuple<Motor*, int, const char*, const char*, Text_*>, 8>::iterator it = motors_for_gui.begin(); it != motors_for_gui.end(); it++){
    std::tuple<Motor*, int, const char*, const char*, Text_*>& mot_tup = *it;
    Motor* motor = std::get<0>(mot_tup);
    Text_* text = std::get<4>(mot_tup);
    std::get<1>(mot_tup) = motor == nullptr ? std::numeric_limits<int>::max() : std::get<0>(*it)->get_temperature();
    int temp = std::get<1>(mot_tup);

    if (temp >= 55 && temp != std::numeric_limits<int>::max() && !Flash.playing() && !temp_flashed){ //Overheating
      temp_flashed = true;
      printf("Moving to temps\n");
      temps.go_to();
      char buffer[50];
      snprintf(buffer, 50, "%s motor is at %dC\n", std::get<2>(mot_tup), temp);
      GUI::flash(COLOUR(RED), 15000, buffer);
      break;
    }

    if (text && temp == std::numeric_limits<int>::max()) text->set_active(false); //If performance is bad, move this to the page setup func. Should be able to take out the if(text)
    
    //Move this to page loop func
    if (text != nullptr){ //Background Colours (temperature based)
      switch(temp){
        case 0:
        case 5:
          text->set_background(COLOUR(WHITE)); break;
        case 10:
        case 15:
          text->set_background(COLOUR(BLUE)); break;
        case 20:
        case 25:
          text->set_background(COLOUR(DODGER_BLUE)); break;
        case 30:
        case 35:
          text->set_background(COLOUR(LAWN_GREEN)); break;
        case 40:
        case 45:
          text->set_background(COLOUR(YELLOW)); break;
        case 50:
          text->set_background(COLOUR(ORANGE_RED)); break;
        case 55:
          text->set_background(COLOUR(RED)); break;
        default:
          text->set_background(COLOUR(BLACK)); break;
      }
    }
  }
}

void util_setup(){
  motor_ports = {
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_1, &mot_stop_1, &mot_text_1, 0, (char*)malloc(10*sizeof(char))),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_2, &mot_stop_2, &mot_text_2, 0, (char*)malloc(10*sizeof(char))),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_3, &mot_stop_3, &mot_text_3, 0, (char*)malloc(10*sizeof(char))),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_4, &mot_stop_4, &mot_text_4, 0, (char*)malloc(10*sizeof(char))),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_5, &mot_stop_5, &mot_text_5, 0, (char*)malloc(10*sizeof(char))),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_6, &mot_stop_6, &mot_text_6, 0, (char*)malloc(10*sizeof(char))),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_7, &mot_stop_7, &mot_text_7, 0, (char*)malloc(10*sizeof(char))),
    std::make_tuple(std::numeric_limits<int>::max(), &mot_update_8, &mot_stop_8, &mot_text_8, 0, (char*)malloc(10*sizeof(char))),
  };

  //Motor Port Detection
  for (int port=0, i=0; port<21; port++){
    if (c::registry_get_plugged_type(port) == c::E_DEVICE_MOTOR && i < 8){
      std::get<0>(motor_ports[i]) = port+1;
      i++;
    }
  }

  //Motor port numbers and motor buttons
  std::string motor_port_string;
  for (std::array<std::tuple<int, Button*, Button*, Text_*, int, char*>, 8>::iterator it = motor_ports.begin(); it != motor_ports.end(); it++){
    std::tuple<int, Button*, Button*, Text_*, int, char*>& mot_arr = *it;

    if (std::get<0>(mot_arr) != std::numeric_limits<int>::max()) motor_port_string.append(std::to_string(std::get<0>(mot_arr)) + ",");
    else{
      std::get<1>(mot_arr)->set_active(false);
      std::get<2>(mot_arr)->set_active(false);
      std::get<3>(mot_arr)->set_active(false);
    }
    std::get<1>(mot_arr)->set_func([&](){c::motor_move(std::get<0>(mot_arr), mot_speed.get_value());});
    std::get<2>(mot_arr)->set_func([&](){c::motor_move(std::get<0>(mot_arr), 0);});
  }
  if (motor_port_string.back() == ',') motor_port_string.pop_back();
  motor_port_nums = strdup(motor_port_string.c_str());

  //Motor not allowed port numbers
  // std::string no_motor_port_string;
  // for (int port=0; port<21; port++){
  //   if (c::registry_get_bound_type(port) != c::E_DEVICE_MOTOR && c::registry_get_bound_type(port) != c::E_DEVICE_UNDEFINED){
  //     no_motor_port_string.append(std::to_string(port+1) + ",");
  //   }
  // }

  // if (no_motor_port_string.back() == ',') no_motor_port_string.pop_back();
  // no_motor_port_nums = strdup(no_motor_port_string.c_str());


  //Expander Port Detection
  for (int port=0; port<21; port++){
    if (c::registry_get_plugged_type(port) == c::E_DEVICE_ADI) expander_ports[port] = port+1;
    else expander_ports[port] = std::numeric_limits<int>::max();
  }

  std::string expander_port_string;
  for (std::array<int, 21>::iterator it = expander_ports.begin(); it != expander_ports.end(); it++){
    if (*it != std::numeric_limits<int>::max()) expander_port_string.append(std::to_string(*it) + ",");
  }
  if (expander_port_string.back() == ',') expander_port_string.pop_back();
  expander_port_nums = strdup(expander_port_string.c_str());


  //Expander buttons
  std::string no_pneumatic_port_string;
  for (int i = 1; i <= 8; i++){
    if(c::adi_port_get_config(i) != pros::E_ADI_DIGITAL_IN && c::adi_port_get_config(i) != E_ADI_DIGITAL_OUT){
      no_pneumatic_port_string.push_back(char(i+64));
      no_pneumatic_port_string.push_back(',');
    }
    expander_btns[i-1]->set_func([i](){
      int expander_port = expander.get_value();
      if (expander_port){
        if(c::registry_get_plugged_type(expander_port-1) != c::E_DEVICE_ADI){
          GUI::flash(COLOUR(RED), 1000, "No Expander in port %d", expander_port);
        }
        c::ext_adi_port_set_config(expander_port, i, E_ADI_DIGITAL_OUT);
        c::ext_adi_port_set_value(expander_port, i, HIGH);
      }
      else{
        c::adi_port_set_config(i, E_ADI_DIGITAL_OUT);
        c::adi_port_set_value(i, HIGH);
      }
    });
    expander_btns[i-1]->set_off_func([i](){
      int expander_port = expander.get_value();
      if (expander_port){
        if(c::registry_get_plugged_type(expander_port-1) != c::E_DEVICE_ADI){
          GUI::flash(COLOUR(RED), 1000, "No Expander in port %d", expander_port);
        }
        c::ext_adi_port_set_config(expander_port, i, E_ADI_DIGITAL_OUT);
        c::ext_adi_port_set_value(expander_port, i, LOW);
      }
      else{
        c::adi_port_set_config(i, E_ADI_DIGITAL_OUT);
        c::adi_port_set_value(i, LOW);
      }
    });
  }

  if (no_pneumatic_port_string.back() == ',') no_pneumatic_port_string.pop_back();
  no_pneumatic_port_nums = strdup(no_pneumatic_port_string.c_str());


  encoders.set_loop_func([](){
    left_enc = LeftEncoder.get_value();
    right_enc = RightEncoder.get_value();
    back_enc = BackEncoder.get_value();
  });
  AB_res.set_func([&](){RightEncoder.reset();});
  CD_res.set_func([&](){LeftEncoder.reset();});
  EF_res.set_func([&](){BackEncoder.reset();});
  res_all_enc.set_func([&](){LeftEncoder.reset(); RightEncoder.reset(); BackEncoder.reset();});
}

void util_background(){
  //Motor Stalled
  for (std::array<std::tuple<int, Button*, Button*, Text_*, int, char*>, 8>::iterator it = motor_ports.begin(); it != motor_ports.end(); it++){
    std::tuple<int, Button*, Button*, Text_*, int, char*>& mot_arr = *it;
    int port = std::get<0>(mot_arr);
    if (port != std::numeric_limits<int>::max()){
      snprintf(std::get<5>(mot_arr), 9, "%d: %d", port, (int)c::motor_get_actual_velocity(port));
      if (fabs(c::motor_get_actual_velocity(port)) < fabs(c::motor_get_target_velocity(port))/4) std::get<4>(mot_arr) += 1;
      else std::get<4>(mot_arr) = 0;
      if (std::get<4>(mot_arr) > 10){
        std::get<4>(mot_arr) = 0;
        printf("Stopping Motor %d\n", port);
        c::motor_move(port, 0);
      }
    }
  }
}

GUI g_main ({&temps, &driver_curve, &auto_selection, &pos_auto_selection, &track, &moving, &intake_test, &elastic, &motor_subsys, &tuning, &motors, &pneumatics}, &main_setup, &main_background);

GUI g_util ({&ports, &encoders, &motor, &pneumatic}, &util_setup, &util_background);
