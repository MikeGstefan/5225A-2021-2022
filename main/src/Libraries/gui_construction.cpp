#include "gui.hpp"
#include "pros/apix.h"
#include "../config.hpp"
#include "../auton.hpp"
#include "../auton_util.hpp"
#include "../Subsystems/f_lift.hpp"
#include "../Subsystems/b_lift.hpp"
#include "../Subsystems/intake.hpp"
#include "../tracking.hpp"
#include "../drive.hpp"

//DO NOT MESS WITH INDENTATION IN THIS FILE

/*Field array*/ static std::vector<std::bitset<200>> field (200, std::bitset<200>{}); //Initializes to 200 blank bitsets
/*Temperature Alert Flag*/ static bool temp_flashed = false;

//Var init for text monitoring
  int enc_val;
  c::ext_adi_encoder_t ext_test_enc;
  c::adi_encoder_t test_enc;
  std::string motor_port_nums;
  std::string expander_port_nums;
  std::string no_pneumatic_port_nums;
  std::array<std::tuple<int, Button*, Button*, Text_*, int, std::string>, 8> motor_ports; //util: port, run, stop, stall counter, text(port and rpm)
  std::array<int, 21> expander_ports;
  std::array<Button*, 8> exp_pneum_btns;

  std::array<std::tuple<pros::Motor*, std::string, std::string, int, Text_*, Text_*, Button*, Button*>, 8> motors_for_gui {{
    {&back_r, "Back Right", "BR", 0, nullptr, nullptr, nullptr, nullptr},
    {&center_r, "Center Right", "CR", 0, nullptr, nullptr, nullptr, nullptr},
    {&front_r, "Front Right", "FR", 0, nullptr, nullptr, nullptr, nullptr},
    {&back_l, "Back Left", "BL", 0, nullptr, nullptr, nullptr, nullptr},
    {&center_l, "Center Left", "CL", 0, nullptr, nullptr, nullptr, nullptr},
    {&front_l, "Front Left", "FL", 0, nullptr, nullptr, nullptr, nullptr},
    {&f_lift_m, "Front Lift", "F", 0, nullptr, nullptr, nullptr, nullptr},
    {&b_lift_m, "Back Lift", "B", 0, nullptr, nullptr, nullptr, nullptr},
  }}; //{nullptr, "", "", 0, nullptr, nullptr, nullptr, nullptr, nullptr},
  //Motor*, Long name, Short Name, Temperature, Temperature Text, Name Text, On Button, Off button

  //For gui to use
  extern int elastic_b_up_time, elastic_b_down_time; //Declared in b_lift.cpp
  extern int elastic_f_up_time, elastic_f_down_time; //Declared in f_lift.cpp

//Constructors
  //Main GUI
    Page temps ("Temperature"); //Motor temps
      Text mot_temp_1(90, 60, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<2>(motors_for_gui[0]) + ": %dC", std::get<int>(motors_for_gui[0]), COLOUR(BLACK));
      Text mot_temp_2(90, 125, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<2>(motors_for_gui[1]) + ": %dC", std::get<int>(motors_for_gui[1]), COLOUR(BLACK));
      Text mot_temp_3(90, 190, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<2>(motors_for_gui[2]) + ": %dC", std::get<int>(motors_for_gui[2]), COLOUR(BLACK));
      Text mot_temp_4(390, 60, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<2>(motors_for_gui[3]) + ": %dC", std::get<int>(motors_for_gui[3]), COLOUR(BLACK));
      Text mot_temp_5(390, 125, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<2>(motors_for_gui[4]) + ": %dC", std::get<int>(motors_for_gui[4]), COLOUR(BLACK));
      Text mot_temp_6(390, 190, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<2>(motors_for_gui[5]) + ": %dC", std::get<int>(motors_for_gui[5]), COLOUR(BLACK));
      Text mot_temp_7(240, 90, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<2>(motors_for_gui[6]) + ": %dC", std::get<int>(motors_for_gui[6]), COLOUR(BLACK));
      Text mot_temp_8(240, 160, GUI::Style::CENTRE, TEXT_SMALL, temps, std::get<2>(motors_for_gui[7]) + ": %dC", std::get<int>(motors_for_gui[7]), COLOUR(BLACK));

    Page checks("Competition");
      Button drive_motors (15, 45, 100, 75, GUI::Style::SIZE, Button::SINGLE, checks, "Drive Motors");
      Button intakes (130, 45, 100, 75, GUI::Style::SIZE, Button::SINGLE, checks, "Intake Uptake");
      Button lifts (245, 45, 100, 75, GUI::Style::SIZE, Button::SINGLE, checks, "Lifts");
      Button pneums (360, 45, 100, 75, GUI::Style::SIZE, Button::SINGLE, checks, "Pneumatics");
      Button save_pos (15, 140, 100, 75, GUI::Style::SIZE, Button::SINGLE, checks, "Save Position");
      Button auton_selector (130, 140, 100, 75, GUI::Style::SIZE, Button::SINGLE, checks, "Select Autons");
      Button misc_checks (245, 140, 100, 75, GUI::Style::SIZE, Button::SINGLE, checks, "Misc");
      Button distance (360, 140, 100, 75, GUI::Style::SIZE, Button::SINGLE, checks, "Distance");

    Page track ("Tracking"); //Display tracking vals and reset btns
      Text track_x(50, 45, GUI::Style::CENTRE, TEXT_SMALL, track, "X:%.1f", tracking.x_coord);
      Text track_y(135, 45, GUI::Style::CENTRE, TEXT_SMALL, track, "Y:%.1f", tracking.y_coord);
      Text track_a(220, 45, GUI::Style::CENTRE, TEXT_SMALL, track, "A:%.1f", std::function([](){return tracking.get_angle_in_deg();}));
      Text enc_l(50, 130, GUI::Style::CENTRE, TEXT_SMALL, track, "L:%d", std::function([](){return LeftEncoder.get_value() != std::numeric_limits<int32_t>::max() ? LeftEncoder.get_value() : 0;}));
      Text enc_r(135, 130, GUI::Style::CENTRE, TEXT_SMALL, track, "R:%d", std::function([](){return RightEncoder.get_value() != std::numeric_limits<int32_t>::max() ? RightEncoder.get_value() : 0;}));
      Text enc_b(220, 130, GUI::Style::CENTRE, TEXT_SMALL, track, "B:%d", std::function([](){return BackEncoder.get_value() != std::numeric_limits<int32_t>::max() ? BackEncoder.get_value() : 0;}));
      Button res_x(15, 60, 70, 55, GUI::Style::SIZE, Button::SINGLE, track, "Reset X");
      Button res_y(100, 60, 70, 55, GUI::Style::SIZE, Button::SINGLE, track, "Reset Y");
      Button res_a(185, 60, 70, 55, GUI::Style::SIZE, Button::SINGLE, track, "Reset A");
      Button res_all(15, 160, 240, 60, GUI::Style::SIZE, Button::SINGLE, track, "Reset All");

    Page driver_curve ("Drivers"); //Select a driver and their exp curve
      Button prev_drivr(20, 70, 110, 120, GUI::Style::SIZE, Button::SINGLE, driver_curve, "Prev Driver");
      Text drivr_name(MID_X, MID_Y, GUI::Style::CENTRE, TEXT_LARGE, driver_curve, "%s", std::function([](){return drivebase.driver_name();}));
      Button next_drivr(350, 70, 110, 120, GUI::Style::SIZE, Button::SINGLE, driver_curve, "Next Driver");

    Page moving ("Moving"); //Moves to target, home, or centre
      Slider x_val(35, 45, 250, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 144, moving, "X");
      Slider y_val(35, 110, 250, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 144, moving, "Y");
      Slider a_val(35, 175, 250, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 360, moving, "A");
      Button go_to_xya(320, 45, 150, 40, GUI::Style::SIZE, Button::SINGLE, moving, "Target");
      Button go_home(320, 110, 150, 40, GUI::Style::SIZE, Button::SINGLE, moving, "Home");
      Button go_centre(320, 175, 150, 40, GUI::Style::SIZE, Button::SINGLE, moving, "Centre");

    Page lift_move ("Motorized Subsystems"); //Moving the lift
      Slider f_lift_val(30, 45, 300, 35, GUI::Style::SIZE, Slider::HORIZONTAL, 1000, 2750, lift_move, "Front Lift", 10);
      Slider b_lift_val(30, 110, 300, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 1000, 2750, lift_move, "Back Lift", 10);
      Button f_lift_move(470, 45, -100, 40, GUI::Style::SIZE, Button::SINGLE, lift_move, "Move Front Lift");
      Button b_lift_move(470, 95, -100, 40, GUI::Style::SIZE, Button::SINGLE, lift_move, "Move Back Lift");
      Button front_claw(470, 145, -100, 40, GUI::Style::SIZE, Button::TOGGLE, lift_move, "Front Claw");
      Button back_claw(470, 195, -100, 40, GUI::Style::SIZE, Button::TOGGLE, lift_move, "Back Claw");

    Page elastic ("Elastic Test"); //Testing the elastics on the lift
      Button check_b_elastic(145, 70, 70, 30, GUI::Style::CENTRE, Button::SINGLE, elastic, "Run Back Elastic Test");
      Button check_f_elastic(335, 70, 70, 30, GUI::Style::CENTRE, Button::SINGLE, elastic, "Run Front Elastic Test");
      Text elastic_b_up (145, 160, GUI::Style::CENTRE, TEXT_SMALL, elastic, "Up Time: %d", elastic_b_up_time);
      Text elastic_b_down(145, 180, GUI::Style::CENTRE, TEXT_SMALL, elastic, "Down Time: %d", elastic_b_down_time);
      Text elastic_f_up (335, 160, GUI::Style::CENTRE, TEXT_SMALL, elastic, "Up Time: %d", elastic_f_up_time);
      Text elastic_f_down(335, 180, GUI::Style::CENTRE, TEXT_SMALL, elastic, "Down Time: %d", elastic_f_down_time);

    Page tuning ("Tuning Tracking"); //Tests to tune tracking when on new base
      Text tuning_instructions_1(MID_X, 35, GUI::Style::CENTRE, TEXT_SMALL, tuning, "Press your desired tracking test and follow");
      Text tuning_instructions_2(MID_X, 50, GUI::Style::CENTRE, TEXT_SMALL, tuning, "the terminal for instructions and results");
      Button encoder_direction (15, 70, 100, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Encoder Direction");
      Button side_corkscrew (130, 70, 100, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Side Corkscrew");
      Button back_corkscrew (245, 70, 100, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Back Corkcrew");
      Button spin360 (360, 70, 100, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Spins");
      Button wheel_size (15, 155, 100, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Wheel Size");
      Button back_wheel_size (130, 155, 100, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Back Wheel Size");
      Button encoder_ticks (245, 155, 100, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Encoder Ticks");
      Button manual (410, 190, 35, 20, GUI::Style::CENTRE, Button::TOGGLE, tuning, "Manual");

    Page motors ("Motor Control"); //Motor Control for known ports
      Slider mot_speed_set (MID_X, 60, 180 , 15, GUI::Style::CENTRE, Slider::HORIZONTAL, -127, 127, motors, "Speed");
      Text mot_1_text (65, 115, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<1>(motors_for_gui[0]));
      Text mot_2_text (180, 115, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<1>(motors_for_gui[1]));
      Text mot_3_text (295, 115, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<1>(motors_for_gui[2]));
      Text mot_4_text (410, 115, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<1>(motors_for_gui[3]));
      Text mot_5_text (65, 180, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<1>(motors_for_gui[4]));
      Text mot_6_text (180, 180, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<1>(motors_for_gui[5]));
      Text mot_7_text (295, 180, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<1>(motors_for_gui[6]));
      Text mot_8_text (410, 180, GUI::Style::CENTRE, TEXT_SMALL, motors, std::get<1>(motors_for_gui[7]));
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

    Page pneumatics ("Pneumatics"); //Pneumatic testing page for known ports
      Button pneum_1 (15, 45, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics);
      Button pneum_2 (130, 45, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics);
      Button pneum_3 (245, 45, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics);
      Button pneum_4 (360, 45, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics);
      Button pneum_5 (15, 140, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics);
      Button pneum_6 (130, 140, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics);
      Button pneum_7 (245, 140, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics);
      Button pneum_8 (360, 140, 100, 75, GUI::Style::SIZE, Button::TOGGLE, pneumatics);
      Text pneum_1_text (0, 0, GUI::Style::CENTRE, TEXT_SMALL, pneumatics, "%s", std::function([](){return Piston::get_name(1);}));
      Text pneum_2_text (0, 0, GUI::Style::CENTRE, TEXT_SMALL, pneumatics, "%s", std::function([](){return Piston::get_name(2);}));
      Text pneum_3_text (0, 0, GUI::Style::CENTRE, TEXT_SMALL, pneumatics, "%s", std::function([](){return Piston::get_name(3);}));
      Text pneum_4_text (0, 0, GUI::Style::CENTRE, TEXT_SMALL, pneumatics, "%s", std::function([](){return Piston::get_name(4);}));
      Text pneum_5_text (0, 0, GUI::Style::CENTRE, TEXT_SMALL, pneumatics, "%s", std::function([](){return Piston::get_name(5);}));
      Text pneum_6_text (0, 0, GUI::Style::CENTRE, TEXT_SMALL, pneumatics, "%s", std::function([](){return Piston::get_name(6);}));
      Text pneum_7_text (0, 0, GUI::Style::CENTRE, TEXT_SMALL, pneumatics, "%s", std::function([](){return Piston::get_name(7);}));
      Text pneum_8_text (0, 0, GUI::Style::CENTRE, TEXT_SMALL, pneumatics, "%s", std::function([](){return Piston::get_name(8);}));


  //Utility

    Page ports ("Ports"); //Shows what ports to use on builder util
      Text mot (10, 50, GUI::Style::CORNER, TEXT_MEDIUM, ports, "Motors: %s", motor_port_nums);
      Text expanders (10, 100, GUI::Style::CORNER, TEXT_MEDIUM, ports, "Expanders: %s", expander_port_nums);
      Text no_pneumatic (10, 150, GUI::Style::CORNER, TEXT_MEDIUM, ports, "No Pneumatics (Ignore): %s", no_pneumatic_port_nums);

    Page encoders ("Encoders"); //Display tracking vals and reset btns
      Slider expander_1 (30, 90, 30, 100, GUI::Style::SIZE, Slider::VERTICAL, 0, 21, encoders, "E1");
      Slider port_1 (115, 90, 30, 100, GUI::Style::SIZE, Slider::VERTICAL, 1, 8, encoders, "P1");
      Slider port_2 (200, 90, 30, 100, GUI::Style::SIZE, Slider::VERTICAL, 1, 8, encoders, "P2");
      Button enc_set (350, 60, 50, 20, GUI::Style::CENTRE, Button::SINGLE, encoders, "Set");
      Text enc (350, 100, GUI::Style::CENTRE, TEXT_SMALL, encoders, "%s", std::function([](){return sprintf2("%d:%c%c", expander_1.get_value(), port_1.get_value()+64, port_2.get_value()+64);}));
      Text enc_degs (350, 120, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Degs: %d", enc_val);
      Text enc_rots (350, 140, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Rots: %d", std::function([](){return int(enc_val/360);}));
      Text enc_remain (350, 160, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Remaining: %d", std::function([](){return abs(enc_val-360*int(round((enc_val+sgn(enc_val)*180)/360)));}));
      Button enc_res (350, 200, 50, 20, GUI::Style::CENTRE, Button::SINGLE, encoders, "Reset");

    Page motor ("Motor Control");
      Slider mot_speed (60, 45, 300 , 30, GUI::Style::SIZE, Slider::HORIZONTAL, -127, 127, motor, "Speed");
      Button mot_jam_detect (USER_RIGHT-20, 45, -60, 30, GUI::Style::SIZE, Button::TOGGLE, motor, "Jam");
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


void main_setup(){
  //Temperature & Motor Control
    std::get<4>(motors_for_gui[0]) = &mot_temp_1;
    std::get<4>(motors_for_gui[1]) = &mot_temp_2;
    std::get<4>(motors_for_gui[2]) = &mot_temp_3;
    std::get<4>(motors_for_gui[3]) = &mot_temp_4;
    std::get<4>(motors_for_gui[4]) = &mot_temp_5;
    std::get<4>(motors_for_gui[5]) = &mot_temp_6;
    std::get<4>(motors_for_gui[6]) = &mot_temp_7;
    std::get<4>(motors_for_gui[7]) = &mot_temp_8;

    std::get<5>(motors_for_gui[0]) = &mot_1_text;
    std::get<5>(motors_for_gui[1]) = &mot_2_text;
    std::get<5>(motors_for_gui[2]) = &mot_3_text;
    std::get<5>(motors_for_gui[3]) = &mot_4_text;
    std::get<5>(motors_for_gui[4]) = &mot_5_text;
    std::get<5>(motors_for_gui[5]) = &mot_6_text;
    std::get<5>(motors_for_gui[6]) = &mot_7_text;
    std::get<5>(motors_for_gui[7]) = &mot_8_text;

    std::get<6>(motors_for_gui[0]) = &mot_1_update;
    std::get<6>(motors_for_gui[1]) = &mot_2_update;
    std::get<6>(motors_for_gui[2]) = &mot_3_update;
    std::get<6>(motors_for_gui[3]) = &mot_4_update;
    std::get<6>(motors_for_gui[4]) = &mot_5_update;
    std::get<6>(motors_for_gui[5]) = &mot_6_update;
    std::get<6>(motors_for_gui[6]) = &mot_7_update;
    std::get<6>(motors_for_gui[7]) = &mot_8_update;

    std::get<7>(motors_for_gui[0]) = &mot_1_stop;
    std::get<7>(motors_for_gui[1]) = &mot_2_stop;
    std::get<7>(motors_for_gui[2]) = &mot_3_stop;
    std::get<7>(motors_for_gui[3]) = &mot_4_stop;
    std::get<7>(motors_for_gui[4]) = &mot_5_stop;
    std::get<7>(motors_for_gui[5]) = &mot_6_stop;
    std::get<7>(motors_for_gui[6]) = &mot_7_stop;
    std::get<7>(motors_for_gui[7]) = &mot_8_stop;

    for(std::array<std::tuple<pros::Motor *, std::string, std::string, int, Text_ *, Text_ *, Button *, Button *>, 8>::iterator it = motors_for_gui.begin(); it != motors_for_gui.end(); it++){
      if(std::get<Motor*>(*it)){
        std::get<6>(*it)->set_func([=](){std::get<Motor*>(*it)->move(mot_speed_set.get_value());});
        std::get<7>(*it)->set_func([=](){std::get<Motor*>(*it)->move(0);});

        std::get<int>(*it) = std::get<Motor*>(*it)->get_temperature();
      }
      else std::get<int>(*it) = std::numeric_limits<int>::max();

      std::get<4>(*it)->set_background(40, 20);
      if(std::get<int>(*it) == std::numeric_limits<int>::max()){
        std::get<4>(*it)->set_active(false);
        std::get<5>(*it)->set_active(false);
        std::get<6>(*it)->set_active(false);
        std::get<7>(*it)->set_active(false);
      }
    }

    temps.set_loop_func([](){
      for (int i = 0; i < 8; i++){
        Text_* text = std::get<4>(motors_for_gui[i]);
        if (text){
          switch(std::get<int>(motors_for_gui[i])){
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
    });

  //Competition
    drive_motors.set_func([](){
      if(GUI::prompt("Press to check drive motors", "", 1000)){
        drivebase.move(60, 0);
        delay(1000);

        drivebase.brake();
        delay(250);

        drivebase.move(-60, 0);
        delay(1000);

        drivebase.brake();
      }
    });

    intakes.set_func([](){
      if(GUI::prompt("Press to check intake", "", 1000)){
        intake.set_state(intake_states::on);
        delay(1000);

        intake.set_state(intake_states::off);
        delay(250);

        intake.set_state(intake_states::reversed);
        delay(1000);

        intake.set_state(intake_states::off);
      }
    });

    lifts.set_func([](){
      if(GUI::prompt("Press to check lifts", "", 1000)){
        b_lift.set_state(b_lift_states::move_to_target, 0);
        f_lift.set_state(f_lift_states::move_to_target, 0);
        delay(1000);

        b_lift.set_state(b_lift_states::move_to_target, b_lift.driver_positions.size() - 1);
        f_lift.set_state(f_lift_states::move_to_target, f_lift.driver_positions.size() - 1);
        delay(1000);

        b_lift.set_state(b_lift_states::move_to_target, 0);
        f_lift.set_state(f_lift_states::move_to_target, 0);

        delay(250);

        b_lift.set_state(b_lift_states::idle, 0);
        f_lift.set_state(f_lift_states::idle, 0);
      }
    });

    pneums.set_func([](){
      for(std::array<std::pair<Piston*, Button*>, 8>::const_iterator it = Piston::list_for_gui.begin(); it != Piston::list_for_gui.end(); it++){
        Piston* piston = it->first;
        if (!piston) continue;

        if (GUI::prompt("Press to check " + piston->get_name())){
          piston->toggle_state();
          delay(500);
          piston->toggle_state();
        }
        else return;
      }
    });

    misc_checks.set_func([](){
      if (!pros::usd::is_installed()) screen_flash::start("No SD Card!");
      else(screen_flash::start("No Errors Found", term_colours::GREEN));
    });

    save_pos.set_func(save_positions);
    auton_selector.set_func(select_auton);

  //Tracking
    for (int x = 0; x < 200; x++) field[x].reset(); //Should be able to get rid of this

    track.set_setup_func([](){
      screen::set_pen(COLOUR(WHITE));
      screen::draw_rect(270, 30, 470, 230);
      screen::draw_line(370, 30, 370, 230);
      screen::draw_line(270, 130, 470, 130);
      for (int x = 0; x < 200; x++){
        for (int y = 0; y < 200; y++) if(field[x].test(y)) screen::draw_pixel(270+x, 230-y); //Draws saved tracking values
      }
    });
    track.set_loop_func([](){
      screen::set_pen(COLOUR(RED));
      screen::draw_pixel(270+(200*tracking.x_coord/144), 230-(200*tracking.y_coord/144)); //Scales to screen
    });

    res_x.set_func([](){tracking.reset(0.0, tracking.y_coord, rad_to_deg(tracking.global_angle));});
    res_y.set_func([](){tracking.reset(tracking.x_coord, 0.0, rad_to_deg(tracking.global_angle));});
    res_a.set_func([](){tracking.reset(tracking.x_coord, tracking.y_coord, 0.0);});

    res_all.set_func([](){
      RightEncoder.reset();
      LeftEncoder.reset();
      BackEncoder.reset();
      tracking.reset();
    });

  //Driving
    prev_drivr.set_func([](){drivebase.prev_driver();});
    next_drivr.set_func([](){drivebase.next_driver();});

  //Moving
    moving.set_setup_func([](){
      x_val.val = tracking.x_coord;
      y_val.val = tracking.y_coord;
      a_val.val = tracking.get_angle_in_deg();
    });

    go_to_xya.set_func([&](){
      Position target (x_val.get_value(), y_val.get_value(), a_val.get_value());
      if (GUI::prompt("Press to go to " + sprintf2("%d", target), "", 1000)) move_start(move_types::point, point_params(target));
    });
    go_home.set_func([](){
      Position target (0, 0, tracking.get_angle_in_deg());
      if (GUI::prompt("Press to go to " + sprintf2("%d", target), "", 1000)) move_start(move_types::point, point_params(target));
    });
    go_centre.set_func([](){
      Position target (72, 72, tracking.get_angle_in_deg());
      if (GUI::prompt("Press to go to " + sprintf2("%d", target), "", 1000)) move_start(move_types::point, point_params(target));
    });

  //Subsystems
    b_lift_val.min = b_lift.prog_positions.front();
    b_lift_val.max = b_lift.prog_positions.back();
    f_lift_val.min = f_lift.prog_positions.front();
    f_lift_val.max = f_lift.prog_positions.back();

    lift_move.set_setup_func([](){
      b_lift_val.val = b_lift_pot.get_value();
      f_lift_val.val = f_lift_pot.get_value();
      if(f_claw_o.get_state()) front_claw.select();
      if(b_claw.get_state()) back_claw.select();
    });

    front_claw.set_func([](){f_claw(HIGH);});
    front_claw.set_off_func([](){f_claw(LOW);});
    back_claw.set_func([](){b_claw.set_state(HIGH);});
    back_claw.set_off_func([](){b_claw.set_state(LOW);});

    f_lift_move.set_func([&](){
      if (GUI::prompt("Press to move front lift to " + std::to_string(f_lift_val.get_value()), "", 1000)){
        f_lift.move_absolute(f_lift_val.get_value());
      }
    });
    b_lift_move.set_func([&](){
      if (GUI::prompt("Press to move back lift to " + std::to_string(b_lift_val.get_value()), "", 1000)){
        b_lift.move_absolute(b_lift_val.get_value());
      }
    });

  //Elastic Test
    check_b_elastic.set_func([](){b_lift.elastic_util(1011);});
    check_f_elastic.set_func([](){f_lift.elastic_util(935);});

  //Tuning Tracking
    manual.select();
    //Figure out how to do these with the other left/right, front/back or moving in the other direction

    encoder_direction.set_func([](){ //Encoder Direction
      if(manual.on){
        if(GUI::prompt("Press, then move the robot forward and to the right", "")){
          drivebase.reset();

          if (GUI::prompt("Press when done movement")){
            printf2("The left tracking wheel is in the %s direction", LeftEncoder.get_value() > 0 ? "right" : "wrong");
            printf2("The right tracking wheel is in the %s direction", RightEncoder.get_value() > 0 ? "right" : "wrong");
            printf2("The back tracking wheel is in the %s direction", BackEncoder.get_value() > 0 ? "right" : "wrong");
          }
        }
      }
      else{
        if(GUI::prompt("Press, then clear the area", "", 1500)){
          drivebase.reset();

          drivebase.move(50.0, 20.0); // preferably strafe right instead of turning.
          tracking.wait_for_dist(30);
          drivebase.brake();

          printf2("The left tracking wheel is in the %s direction", LeftEncoder.get_value() > 0 ? "right" : "wrong");
            printf2("The right tracking wheel is in the %s direction", RightEncoder.get_value() > 0 ? "right" : "wrong");
            printf2("The back tracking wheel is in the %s direction", BackEncoder.get_value() > 0 ? "right" : "wrong");
        }
      }
    });

    encoder_ticks.set_func([](){ //Encoder missing ticks
      if(manual.on){
        if (GUI::prompt("Press, then spin the encoder", "Start the encoder in a known position. Press then button, then spin it wildly, and then stop it in that position again.")){
          drivebase.reset();

          if (GUI::prompt("Press when stopped")){
            double left = LeftEncoder.get_value() / 360.0;
            double right = RightEncoder.get_value() / 360.0;
            double back = BackEncoder.get_value() / 360.0;
            int left_rot = round(left);
            int right_rot = round(right);
            int back_rot = round(back);
            left -= left_rot;
            right -= right_rot;
            back -= back_rot;
            term_colours left_col = abs(left) <= 0.01 ? term_colours::GREEN : term_colours::RED;
            term_colours right_col = abs(right) <= 0.01 ? term_colours::GREEN : term_colours::RED;
            term_colours back_col = abs(back) <= 0.01 ? term_colours::GREEN : term_colours::RED;

            if(left == 0) printf2(term_colours::GREEN, -1, "The left encoder was perfect over %d rotations", left_rot);
            else printf2(left_col, -1, "The left encoder %s %d ticks over %d rotations", left > 0 ? "gained" : "lost", fabs(360.0*left), left_rot);

            if(right == 0) printf2(term_colours::GREEN, -1, "The right encoder was perfect over %d rotations", right_rot);
            else printf2(right_col, -1, "The right encoder %s %d ticks over %d rotations", right > 0 ? "gained" : "lost", fabs(360.0*right), right_rot);

            if(back == 0) printf2(term_colours::GREEN, -1, "The back encoder was perfect over %d rotations", back_rot);
            else printf2(back_col, -1, "The back encoder %s %d ticks over %d rotations", back > 0 ? "gained" : "lost", fabs(360.0*back), back_rot);
          }
        }
      }
      else{
        screen_flash::start("No Automatic Option");
      }
    });

    back_corkscrew.set_func([](){ //Back Corkscrew
      if(manual.on){
        if(GUI::prompt("Press, then move along y", "Please slide the robot forward along a known surface in the y-direction")){
          drivebase.reset();

          if (GUI::prompt("Press when stopped")){
            int back = BackEncoder.get_value();
            if(abs(back) <= 2) printf2(term_colours::GREEN, -1, "The back wheel is pretty accurate, it is at %d ticks.", back);
            else printf2(term_colours::RED, -1, "The back wheel is corkscrewing. It is at %d ticks. Consider turning it %sclockwise.", left, sgn(LeftEncoder.get_value()) == sgn(back) ? "" : "counter-");
          }
        }
      }
      else{
        if(GUI::prompt("Push the left side against a surface", "Please push the robot's left side along a known surface, then press.", 1500)){
          drivebase.reset();
          
          drivebase.move(50.0, -20.0);
          tracking.wait_for_dist(25);
          drivebase.brake();

          int back = BackEncoder.get_value();
          if(abs(back) <= 2) printf2(term_colours::GREEN, -1, "The back wheel is pretty accurate, it is at %d ticks.", back);
          else printf2(term_colours::RED, -1, "The back wheel is corkscrewing. It is at %d ticks. Consider turning it %sclockwise.", left, sgn(LeftEncoder.get_value()) == sgn(back) ? "" : "counter-");
        }
      }
    });

    side_corkscrew.set_func([](){ //Left-Right Corkscrew
      if(manual.on){
        if(GUI::prompt("Press, then move along x", "Please slide the robot along a known surface in the x-direction")){
          drivebase.reset();

          if(GUI::prompt("Press when stopped")){
            int left = LeftEncoder.get_value();
            int right = RightEncoder.get_value();

            if(abs(left) <= 2) printf2(term_colours::GREEN, -1, "The left wheel is pretty accurate, it is at %d ticks.", left);
            else printf2(term_colours::RED, -1, "The left wheel is corkscrewing. It is at %d ticks. Consider turning it %sclockwise.", left, sgn(left) == sgn(BackEncoder.get_value()) ? "counter-" : "");

            if(abs(right) <= 2) printf2(term_colours::GREEN, -1, "The right wheel is pretty accurate, it is at %d ticks.", right);
            else printf2(term_colours::RED, -1, "The right wheel is corkscrewing. It is at %d ticks. Consider turning it %sclockwise.", right, sgn(right) == sgn(BackEncoder.get_value()) ? "counter-" : "");
          }
        }
      }
      else{
        screen_flash::start("I cannot strafe");
      }
    });

    wheel_size.set_func([](){ //Left-Right wheel size
      if(manual.on){
        if(GUI::prompt("Press, then move the robot 30 inches", "Place the robot's side against a known surface. Press the button, then slide the robot forwards exactly 30 inches.")){
          drivebase.reset();

          if (GUI::prompt("Press when completed 30 inches")){
            printf2("The left tracking wheel's diameter is %f", 60.0/LeftEncoder.get_value());
            printf2("The right tracking wheel's diameter is %f", 60.0/RightEncoder.get_value());
          }
        }
      }
      else{
        if(GUI::prompt("Place the robot's side against a surface. Then press.", "", 1500)){
          drivebase.reset();

          int dir = random_direction();
          drivebase.move(dir*50.0, -dir*20.0);
          tracking.wait_for_dist(30); //This needs to brake at exactly 30, it can't just be past 30
          drivebase.brake();

          printf2("If the robot is far off of 30 inches, consider changing the wheel size constants.");
          printf2("Multiply the actual travelled distance by %f to get the left wheel diameter", fabs(2.0/LeftEncoder.get_value()));
          printf2("Multiply the actual travelled distance by %f to get the right wheel diameter", fabs(2.0/RightEncoder.get_value()));
        }
      }
    });

    back_wheel_size.set_func([](){ //Back wheel size
      if(manual.on){
        if(GUI::prompt("Press, then move the robot 30 inches", "Place the robot's front against a known surface. Press the button, then slide the robot sideways exactly 30 inches.")){
          drivebase.reset();

          if (GUI::prompt("Press when completed 30 inches")){
            printf2("The back tracking wheel's diameter is %f", 60.0/BackEncoder.get_value());
          }
        }
      }
      else{
        screen_flash::start("I cannot strafe");
      }
    });

    spin360.set_func([](){ //Robot turn accuracy
      if(manual.on){
        if(GUI::prompt("Press, then spin the robot", "Place the robot's against a known surface. Press the button, then spin the robot. Return it to the starting point. (Half turns are ok)")){
          flatten_against_wall();
          drivebase.reset();

          if(GUI::prompt("Press when back at start position")){
            printf2("The robot is %.2f inches %s and %.2f inches %s off the starting point.", fabs(tracking.x_coord), tracking.x_coord > 0 ? "right" : "left", fabs(tracking.y_coord), tracking.y_coord > 0 ? "forward" : "back");
            
            double turned = rad_to_deg(tracking.global_angle) / 180.0;
            double rots = round(turned);
            turned = 180*(turned-rots);
            rots /= 2;

            double dist = rots*fabs(LeftEncoder.get_value()-RightEncoder.get_value())/360.0;

            if(fabs(turned) <= 0.05) printf2(term_colours::GREEN, -1, "This seems pretty accurate. It's %.4f degrees off over %.1f rotations.", turned, rots);
            else if(turned > 0) printf2(term_colours::RED, -1, "However, the robot gained %.2f degrees over %.1f rotations. Consider decreasing the DistanceLR to %.3f.", turned, rots, dist);
            else printf2(term_colours::RED, -1, "However, the robot lost %.2f degrees over %.1f rotations. Consider increasing the DistanceLR to %.3f.", -turned, rots, dist);
          }
        }
      }
      else{
        if(GUI::prompt("Press, then spin the robot", "Place the robot's front against a known surface. Press the button, then clear the area.", 1500)){
          int dir = random_direction();
          flatten_against_wall();
          drivebase.reset();
          drivebase.move(-60.0, 0.0);
          tracking.wait_for_dist(10);
          drivebase.brake();
          drivebase.move(0.0, dir*80.0);
          wait_until(fabs(tracking.global_angle) > 9.0_rot);
          drivebase.move(0.0, dir*40.0);
          wait_until(fabs(tracking.global_angle) > 10.0_rot);
          drivebase.brake();
          drivebase.move(60.0, 0.0);
          tracking.wait_for_dist(5);
          flatten_against_wall();
          drivebase.brake();

          printf2("The robot is %.2f inches %s and %.2f inches %s off the starting point.", fabs(tracking.x_coord), tracking.x_coord > 0 ? "right" : "left", fabs(tracking.y_coord), tracking.y_coord > 0 ? "forward" : "back");
            
          double turned = rad_to_deg(tracking.global_angle) / 180.0;
          double rots = round(turned);
          turned = 180*(turned-rots);
          rots /= 2;

          double dist = rots*fabs(LeftEncoder.get_value()-RightEncoder.get_value())/360.0;

          if(fabs(turned) <= 0.05) printf2(term_colours::GREEN, -1, "This seems pretty accurate. It's %.4f degrees off over %.1f rotations.", turned, rots);
          else if(turned > 0) printf2(term_colours::RED, -1, "However, the robot gained %.2f degrees over %.1f rotations. Consider decreasing the DistanceLR to %.3f.", turned, rots, dist);
          else printf2(term_colours::RED, -1, "However, the robot lost %.2f degrees over %.1f rotations. Consider increasing the DistanceLR to %.3f.", -turned, rots, dist);
        }
      }
    });

  //Pneumatic Control
    pneum_1.add_text(pneum_1_text);
    pneum_2.add_text(pneum_2_text);
    pneum_3.add_text(pneum_3_text);
    pneum_4.add_text(pneum_4_text);
    pneum_5.add_text(pneum_5_text);
    pneum_6.add_text(pneum_6_text);
    pneum_7.add_text(pneum_7_text);
    pneum_8.add_text(pneum_8_text);

    for(std::array<std::pair<Piston*, Button*>, 8>::const_iterator it = Piston::list_for_gui.begin(); it != Piston::list_for_gui.end(); it++){
      if(it->first){
        it->second->set_func([it](){it->first->set_state(HIGH);});
        it->second->set_off_func([it](){it->first->set_state(LOW);});
      }
      else it->second->set_active(false);
    }

    pneumatics.set_setup_func([](){
      for(std::array<std::pair<Piston*, Button*>, 8>::const_iterator it = Piston::list_for_gui.begin(); it != Piston::list_for_gui.end(); it++){
        if(it->first && it->first->get_state()) it->second->select();
      }
    });
}

void main_background(){
  //Saving Field coords
  int x = 200*tracking.x_coord/144, y = 200*tracking.y_coord/144;
  if(in_range(x, 0, 199) && in_range(y, 0, 199)) field[x].set(y); //Saves position (x,y) to as tracked

  for (std::array<std::tuple<pros::Motor*, std::string, std::string, int, Text_*, Text_*, Button*, Button*>, 8>::iterator it = motors_for_gui.begin(); it != motors_for_gui.end(); it++){
    std::tuple<pros::Motor*, std::string, std::string, int, Text_*, Text_*, Button*, Button*>& mot_tup = *it;
    std::get<int>(mot_tup) = std::get<Motor*>(mot_tup) ? std::get<Motor*>(mot_tup)->get_temperature() : std::numeric_limits<int>::max();

    if (!temp_flashed && std::get<Motor*>(mot_tup) && in_range(std::get<int>(mot_tup), 55, std::numeric_limits<int>::max()-1) && screen_flash::timer.playing()){ //Overheating
      temp_flashed = true;
      temps.go_to();
      screen_flash::start(term_colours::ERROR, 10000, "%s motor is at %dC\n", std::get<1>(mot_tup).c_str(), std::get<int>(mot_tup));
      break;
    }
  }
}

void util_setup(){
  //Encoders
    enc_set.set_func([](){
      int port = expander_1.get_value();
      if(abs(port_1.get_value()-port_2.get_value()) != 1 || std::min(port_1.get_value(), port_2.get_value()) % 2 == 0){
        screen_flash::start(term_colours::ERROR, "Invalid Ports %c%c", port_1.get_value()+64, port_2.get_value()+64);
        return;
      }
      if(port){
        if(c::registry_get_plugged_type(port-1) != c::E_DEVICE_ADI){
          screen_flash::start(term_colours::ERROR, "No Expander in port %d", port);
          return;
        }
        ext_test_enc = c::ext_adi_encoder_init(port, port_1.get_value(), port_2.get_value(), false);
      }
      else test_enc = c::adi_encoder_init(port_1.get_value(), port_2.get_value(), false);
    });

    enc_res.set_func([](){
      int port = expander_1.get_value();
      if(port){
        c::ext_adi_encoder_reset(ext_test_enc);
      }
      else c::adi_encoder_reset(test_enc);
    });

    encoders.set_loop_func([](){
      if(expander_1.get_value()) enc_val = c::ext_adi_encoder_get(ext_test_enc);
      else enc_val = c::adi_encoder_get(test_enc);

      if(enc_val == std::numeric_limits<int32_t>::max()) enc_val = 0;
    });


  //Motor Control
    motor_ports = {
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_1, &mot_stop_1, &mot_text_1, 0, ""),
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_2, &mot_stop_2, &mot_text_2, 0, ""),
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_3, &mot_stop_3, &mot_text_3, 0, ""),
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_4, &mot_stop_4, &mot_text_4, 0, ""),
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_5, &mot_stop_5, &mot_text_5, 0, ""),
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_6, &mot_stop_6, &mot_text_6, 0, ""),
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_7, &mot_stop_7, &mot_text_7, 0, ""),
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_8, &mot_stop_8, &mot_text_8, 0, ""),
    };

    for (int port=0, i=0; port<21; port++){
      if (c::registry_get_plugged_type(port) == c::E_DEVICE_MOTOR && i < 8){
        std::get<0>(motor_ports[i]) = port+1;
        i++;
      }
    }

    for (std::array<std::tuple<int, Button*, Button*, Text_*, int, std::string>, 8>::const_iterator it = motor_ports.begin(); it != motor_ports.end(); it++){
      const std::tuple<int, Button*, Button*, Text_*, int, std::string>& mot_arr = *it;

      if (std::get<0>(mot_arr) != std::numeric_limits<int>::max()) motor_port_nums.append(std::to_string(std::get<0>(mot_arr)) + ",");
      else{
        std::get<1>(mot_arr)->set_active(false);
        std::get<2>(mot_arr)->set_active(false);
        std::get<3>(mot_arr)->set_active(false);
      }
      std::get<1>(mot_arr)->set_func([&](){c::motor_move(std::get<0>(mot_arr), mot_speed.get_value());});
      std::get<2>(mot_arr)->set_func([&](){c::motor_move(std::get<0>(mot_arr), 0);});
    }

    if (motor_port_nums.back() == ',') motor_port_nums.pop_back();

  //Pneumatic Control
    exp_pneum_btns = {&ADI_a, &ADI_b, &ADI_c, &ADI_d, &ADI_e, &ADI_f, &ADI_g, &ADI_h};

    for (int port=0; port<21; port++){
      if (c::registry_get_plugged_type(port) == c::E_DEVICE_ADI) expander_ports[port] = port+1;
      else expander_ports[port] = std::numeric_limits<int>::max();
    }

    for (std::array<int, 21>::const_iterator it = expander_ports.begin(); it != expander_ports.end(); it++){
      if (*it != std::numeric_limits<int>::max()) expander_port_nums.append(std::to_string(*it) + ",");
    }

    if (expander_port_nums.back() == ',') expander_port_nums.pop_back();

    for (int i = 1; i <= 8; i++){
      if(c::adi_port_get_config(i) != pros::E_ADI_DIGITAL_IN && c::adi_port_get_config(i) != E_ADI_DIGITAL_OUT){
        no_pneumatic_port_nums.push_back(char(i+64));
        no_pneumatic_port_nums.push_back(',');
      }
      exp_pneum_btns[i-1]->set_func([i](){
        int port = expander.get_value();
        if (port){
          if(c::registry_get_plugged_type(port-1) != c::E_DEVICE_ADI){
            screen_flash::start(term_colours::ERROR, "No Expander in port %d", port);
            return;
          }
          c::ext_adi_port_set_config(port, i, E_ADI_DIGITAL_OUT);
          c::ext_adi_port_set_value(port, i, HIGH);
        }
        else{
          c::adi_port_set_config(i, E_ADI_DIGITAL_OUT);
          c::adi_port_set_value(i, HIGH);
        }
      });
      exp_pneum_btns[i-1]->set_off_func([i](){
        int port = expander.get_value();
        if (port){
          if(c::registry_get_plugged_type(port-1) != c::E_DEVICE_ADI){
            screen_flash::start(term_colours::ERROR, "No Expander in port %d", port);
            return;
          }
          c::ext_adi_port_set_config(port, i, E_ADI_DIGITAL_OUT);
          c::ext_adi_port_set_value(port, i, LOW);
        }
        else{
          c::adi_port_set_config(i, E_ADI_DIGITAL_OUT);
          c::adi_port_set_value(i, LOW);
        }
      });
    }

    if (no_pneumatic_port_nums.back() == ',') no_pneumatic_port_nums.pop_back();

}

void util_background(){
  //Motor Stalled
  for (std::array<std::tuple<int, Button*, Button*, Text_*, int, std::string>, 8>::iterator it = motor_ports.begin(); it != motor_ports.end(); it++){
    int port = std::get<0>(*it);
    int& stall_count = std::get<4>(*it);
    if (port != std::numeric_limits<int>::max()){
      std::get<5>(*it) = sprintf2("%d: %d", port, c::motor_get_actual_velocity(port));
      if(mot_jam_detect.on){
        if (fabs(c::motor_get_actual_velocity(port)) < fabs(c::motor_get_target_velocity(port))/4) stall_count++;
        else stall_count = 0;
        if (stall_count > 10){
          stall_count = 0;
          screen_flash::start(term_colours::ERROR, "Stopping Motor %d\n", port);
          c::motor_move(port, 0);
        }
      }
    }
  }
}

GUI main_obj ({&temps, &checks, &track, &moving, &lift_move,  &driver_curve, &elastic, &tuning, &motors, &pneumatics}, &main_setup, &main_background);

GUI util_obj ({&ports, &encoders, &motor, &pneumatic}, &util_setup, &util_background);
