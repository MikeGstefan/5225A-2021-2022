#pragma once
#include "gui.hpp"
#include <bitset>

//Field array
std::vector<std::bitset<200>> field (200, std::bitset<200>{}); //Initializes to 200 blank bitsets

//Var init for text monitoring
int angle, left_enc, right_enc, back_enc, elastic_up_time, elastic_down_time;
std::string driver_text, auton_name, alliance_name; //Try to get rid of this with char**

Page driver_curve (1, "Drivers"); //Select a driver and their exp curve
Button prev_drivr(20, 70, 110, 120, Style::SIZE, Button::SINGLE, driver_curve, "Prev Driver");
Text drivr_name(MID_X, MID_Y, Style::CENTRE, TEXT_LARGE, driver_curve, "%s", driver_text);
Button next_drivr(350, 70, 110, 120, Style::SIZE, Button::SINGLE, driver_curve, "Next Driver");

Page temps (2, "Temperature"); //Motor temps
Text mot_temp_1(75, 85, Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors[0]) + std::string(": %dC"), std::get<1>(motors[0]), COLOR_BLACK);
Text mot_temp_2(185, 85, Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors[1]) + std::string(": %dC"), std::get<1>(motors[1]), COLOR_BLACK);
Text mot_temp_3(295, 85, Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors[2]) + std::string(": %dC"), std::get<1>(motors[2]), COLOR_BLACK);
Text mot_temp_4(405, 85, Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors[3]) + std::string(": %dC"), std::get<1>(motors[3]), COLOR_BLACK);
Text mot_temp_5(75, 175, Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors[4]) + std::string(": %dC"), std::get<1>(motors[4]), COLOR_BLACK);
Text mot_temp_6(185, 175, Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors[5]) + std::string(": %dC"), std::get<1>(motors[5]), COLOR_BLACK);
Text mot_temp_7(295, 175, Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors[6]) + std::string(": %dC"), std::get<1>(motors[6]), COLOR_BLACK);
Text mot_temp_8(405, 175, Style::CENTRE, TEXT_SMALL, temps, std::get<3>(motors[7]) + std::string(": %dC"), std::get<1>(motors[7]), COLOR_BLACK);

Page auto_selection (3, "Auton"); //Select auton routes
Button prev_auto(20, 70, 110, 120, Style::SIZE, Button::SINGLE, auto_selection, "Prev Auton");
Text auto_name(MID_X, MID_Y, Style::CENTRE, TEXT_LARGE, auto_selection, "%s", auton_name);
Button next_auto(350, 70, 110, 120, Style::SIZE, Button::SINGLE, auto_selection, "Next Auton");
Button alliance(MID_X, 215, 200, 20, Style::CENTRE, Button::SINGLE, auto_selection);
Text ally_name(MID_X, 215, Style::CENTRE, TEXT_MEDIUM, auto_selection, "Alliance: %s", alliance_name, COLOR_WHITE);

Page track (4, "Tracking"); //Display tracking vals and reset btns
Text trackX(50, 45, Style::CENTRE, TEXT_SMALL, track, "X:%.1f", tracking.x_coord);
Text trackY(135, 45, Style::CENTRE, TEXT_SMALL, track, "Y:%.1f", tracking.y_coord);
Text trackA(220, 45, Style::CENTRE, TEXT_SMALL, track, "A:%d", angle);
Text encL(50, 130, Style::CENTRE, TEXT_SMALL, track, "L:%.1f", left_enc);
Text encR(135, 130, Style::CENTRE, TEXT_SMALL, track, "R:%.1f", right_enc);
Text encB(220, 130, Style::CENTRE, TEXT_SMALL, track, "B:%d", back_enc);
Button resX(15, 60, 70, 55, Style::SIZE, Button::SINGLE, track, "Reset X");
Button resY(100, 60, 70, 55, Style::SIZE, Button::SINGLE, track, "Reset Y");
Button resA(185, 60, 70, 55, Style::SIZE, Button::SINGLE, track, "Reset A");
Button resAll(15, 160, 240, 60, Style::SIZE, Button::SINGLE, track, "Reset All");

Page moving (5, "Moving"); //Moves to target, home, or centre
Slider xVal(35, 45, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, moving, "X");
Slider yVal(35, 110, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, moving, "Y");
Slider aVal(35, 175, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 360, moving, "A");
Button go_to_xya(320, 45, 150, 40, Style::SIZE, Button::SINGLE, moving, "Target");
Button go_home(320, 110, 150, 40, Style::SIZE, Button::SINGLE, moving, "Home");
Button go_centre(320, 175, 150, 40, Style::SIZE, Button::SINGLE, moving, "Centre");

Page intake_test (6, "Intake"); //Test for intake with rings
Text rings(MID_X, 50, Style::CENTRE, TEXT_LARGE, intake_test, "Ring Count: %d", ring_count);
Button reset_intake (30, 90, 120, 80, Style::SIZE, Button::SINGLE, intake_test, "Reset Motor");
Button onOff (180, 90, 120, 80, Style::SIZE, Button::SINGLE, intake_test, "Start/Stop");
Button reset_rings (330, 90, 120, 80, Style::SIZE, Button::SINGLE, intake_test, "Reset Ring Count");

Page elastic (7, "Elastic Test"); //Testing the elastics on the lift
Button run_elastic(165, 60, 150, 55, Style::SIZE, Button::SINGLE, elastic, "Run Elastic Test");
Text elastic_up (MID_X, 160, Style::CENTRE, TEXT_SMALL, elastic, "Up Time: %d", elastic_up_time);
Text elastic_down(MID_X, 180, Style::CENTRE, TEXT_SMALL, elastic, "Down Time: %d", elastic_down_time);

Page liftMove (8, "Lift"); //Moving the lift to an xyz position
Slider lift_x_val(35, 65, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, liftMove, "X");
Slider lift_y_val(35, 150, 250, 40, Style::SIZE, Slider::HORIZONTAL, 0, 144, liftMove, "Y");
Button go_to_lift_xy(400, 128, 70, 50, Style::CENTRE, Button::SINGLE, liftMove, "Move Lift To Target");

Page liftStates (9, "Lift States"); //Moving to various lift states
Button Neutral (15, 45, 100, 75, Style::SIZE, Button::SINGLE, liftStates, "Neutral"); //Names are caps to avoid conflicts
Button RingPickup (130, 45, 100, 75, Style::SIZE, Button::SINGLE, liftStates, "Ring Pickup");
Button Tip (245, 45, 100, 80, Style::SIZE, Button::SINGLE, liftStates, "Tip");
Button Lowering (360, 45, 100, 75, Style::SIZE, Button::SINGLE, liftStates, "Lowering");
Button Down (15, 140, 100, 75, Style::SIZE, Button::SINGLE, liftStates, "Down");
Button Raised (130, 140, 100, 75, Style::SIZE, Button::SINGLE, liftStates, "Raised");
Button Platform (245, 140, 100, 75, Style::SIZE, Button::SINGLE, liftStates, "Platform");
Button ReleaseMogo(360, 140, 100, 75, Style::SIZE, Button::SINGLE, liftStates, "Release Mogo");

Page tuning (10, "Tuning Tracking"); //Tests to tune tracking when on new base
Text tuning_instructions_1(MID_X, 35, Style::CENTRE, TEXT_SMALL, tuning, "Press your desired tracking test");
Text tuning_instructions_2(MID_X, 50, Style::CENTRE, TEXT_SMALL, tuning, "and follow the terminal for instructions");
Button turn_encoder (10, 75, 225, 70, Style::SIZE, Button::SINGLE, tuning, "10 Turns");
Button perpendicular_error (245, 75, 225, 70, Style::SIZE, Button::SINGLE, tuning, "Perpendicular Error");
Button grid (10, 155, 225, 70, Style::SIZE, Button::SINGLE, tuning, "Grid");
Button spin360 (245, 155, 225, 70, Style::SIZE, Button::SINGLE, tuning, "360 Spin");

Page pneumatic (11, "Pneumatics"); //Pneumatic testing page
Text pneum_text_1 (125, 50, Style::CENTRE, TEXT_SMALL, pneumatic, "PORT G");
Text pneum_text_2 (350, 50, Style::CENTRE, TEXT_SMALL, pneumatic, "PORT G - no config for 2");
Button pneum_btn_1 (25, 70, 200, 80, Style::SIZE, Button::TOGGLE, pneumatic, "PNEUMATIC 1");
Button pneum_btn_2 (250, 70, 200, 80, Style::SIZE, Button::TOGGLE, pneumatic, "PNEUMATIC 2");

//The following are not normal pages and should not be accessible by scrolling through the GUI.
Page testing (12, "Testing"); //Blank page made so it already exists when quick tests are created
Text testing_text_1 (125, 50, Style::CENTRE, TEXT_SMALL, testing, "BLANK TEXT 1");
Text testing_text_2 (350, 50, Style::CENTRE, TEXT_SMALL, testing, "BLANK TEXT 2");
Button testing_button_1 (25, 70, 200, 80, Style::SIZE, Button::SINGLE, testing, "BLANK BUTTON 1");
Button testing_button_2 (250, 70, 200, 80, Style::SIZE, Button::SINGLE, testing, "BLANK BUTTON 2");
Slider testing_slider (MID_X, 200, 200 , 20, Style::CENTRE, Slider::HORIZONTAL, -100, 100, testing, "BLANK SLIDER");
