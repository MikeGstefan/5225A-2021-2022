#pragma once
#include "main.h"
#include "timer.hpp"
#include "task.hpp"
#include "logging.hpp"
// #include "drive.hpp"
#include <cstdarg>
#include <array>
#include <functional>

using namespace pros;

// Buttons

// lift buttons
extern controller_digital_e_t lift_up_button;
extern controller_digital_e_t lift_down_button;

extern controller_digital_e_t reverse_drive_button;
extern controller_digital_e_t claw_toggle_button;
extern controller_digital_e_t hitch_button;

// intake buttons
extern controller_digital_e_t speed_limit_button;
extern controller_digital_e_t intake_button;
extern controller_digital_e_t intake_reverse_button;

extern bool speed_limit_active;

// misc buttons
extern controller_digital_e_t ok_button;
// extern controller_digital_e_t joy_mode_switch_button;
extern controller_digital_e_t joy_mode_switch_button;

extern controller_digital_e_t shift_button;

// partner buttons

// lift buttons

extern controller_digital_e_t partner_back_lift_up_button;
extern controller_digital_e_t partner_back_lift_down_button;

extern controller_digital_e_t partner_front_lift_up_button;
extern controller_digital_e_t partner_front_lift_down_button;

extern controller_digital_e_t partner_both_lifts_down_button;

// intake buttons
extern controller_digital_e_t partner_intake_button;
extern controller_digital_e_t partner_trans_to_intk_button;

// claw buttons
extern controller_digital_e_t partner_back_claw_toggle_button;
extern controller_digital_e_t partner_front_claw_toggle_button;
extern controller_digital_e_t partner_claw_tilt_button;
extern controller_digital_e_t partner_hitch_button;

extern controller_digital_e_t partner_joy_mode_switch_button;
extern controller_digital_e_t timer_btn;


#define num_controller 2

class _Task;

class _Controller : public pros::Controller{
  private:
    std::array<std::function<void()>,20> queue;
    int front = 0, back = 0;
    static std::array<_Controller*, num_controller> objs;
    void add_to_queue(std::function<void()>);
    void queue_handle();
    int controller_num;

    // button handling data
    bool cur_press_arr[12] = {0};
    bool last_press_arr[12] = {0};

  public:
    _Controller(pros::controller_id_e_t id);
    static void print_queue(void* params = NULL);
    static _Task controller_task;
    static void init();

    void print(std::uint8_t line, std::uint8_t col, const char* fmt, ... );
    void print(std::uint8_t line, std::uint8_t col, std::string str);
    void clear_line (std::uint8_t line);
    void clear();
    void rumble(const char* rumble_pattern);
    bool interrupt(bool analog = true, bool digital = true, bool OK_except = true);
    void wait_for_press(controller_digital_e_t button, int timeout = 0);
    /**
    * @brief Waits for any button from param buttons to be pressed
    * 
    * @param buttons list of buttons to check
    * @param timeout how long to wait before timing out on the wait
    * @return the button that was pressed. 0 if nothing pressed
    */
    controller_digital_e_t wait_for_press(std::vector<controller_digital_e_t> buttons, int timeout = 0);

    // button handling methods
    // NOTE: all the following methods are only updated every cycle as opposed to every function call, unlike the pros API

    void update_buttons();  // called once every loop, updates current and last state for every button
    bool get_button_state(pros::controller_digital_e_t button); // returns current state of desired button
    bool get_button_last_state(pros::controller_digital_e_t button); // returns last state of desired button
    bool is_rising(pros::controller_digital_e_t button); // if button wasn't pressed but now is
    bool is_falling(pros::controller_digital_e_t button); // if button was pressed but now is not

};
