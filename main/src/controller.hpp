#pragma once
#include "main.h"
#include "Libraries/logging.hpp"

//Master Buttons
  constexpr controller_digital_e_t 

  //Lift Buttons
    lift_up_button = DIGITAL_R1,
    lift_down_button = DIGITAL_R2,
    both_lifts_down_button = DIGITAL_A,
    reverse_drive_button = DIGITAL_L1,
    hitch_button = DIGITAL_A,
    partner_back_lift_up_button = DIGITAL_R1,
    partner_back_lift_down_button = DIGITAL_R2,
    partner_front_lift_up_button = DIGITAL_L1,
    partner_front_lift_down_button = DIGITAL_L2,
    partner_both_lifts_down_button = DIGITAL_DOWN,
    partner_hitch_button = DIGITAL_UP,

  //Intake Buttons
    partner_intake_button = DIGITAL_B,
    partner_intake_reverse_button = DIGITAL_Y,
    speed_limit_button = DIGITAL_Y,

  //Claw Buttons
    partner_back_claw_toggle_button = DIGITAL_A,
    partner_front_claw_toggle_button = DIGITAL_LEFT,
    partner_claw_tilt_button = DIGITAL_X,
    claw_toggle_button = DIGITAL_L2,

  //Intake Buttons
    intake_button = DIGITAL_B,
    intake_reverse_button = DIGITAL_DOWN,
    partner_trans_to_intk_button = DIGITAL_Y,

  //Misc Buttons
    ok_button = DIGITAL_A,
    auton_select = DIGITAL_UP,
    timer_btn = DIGITAL_RIGHT,
    joy_mode_switch_button = DIGITAL_Y,
    partner_joy_mode_switch_button = DIGITAL_RIGHT,
    shift_button = DIGITAL_X;

class _Task;

class _Controller : public Controller{
  private:
    static int constructed;
    std::array<std::function<void() >, 20> queue;
    int front = 0, back = 0;
    static std::array<_Controller*, 2> objs;
    void add_to_queue(std::function<void() >);
    void queue_handle();
    int controller_num;

    //button handling data
    bool cur_press_arr[12] = {0};
    bool last_press_arr[12] = {0};

  public:
    _Controller(controller_id_e_t id);
    static void print_queue(void* params = NULL);
    static _Task controller_task;
    static void init();

    void clear_line (std::uint8_t line);
    void clear();
    void rumble(std::string rumble_pattern);
    bool interrupt(bool analog = true, bool digital = true, bool OK_except = true);
    void wait_for_press(controller_digital_e_t button, int timeout = 0);
    /**
    * @brief Waits for any button from param Buttons to be pressed
    * 
    * @param Buttons list of Buttons to check
    * @param timeout how long to wait before timing out on the wait
    * @return the button that was pressed. 0 if nothing pressed
    */
    controller_digital_e_t wait_for_press(std::vector<controller_digital_e_t> Buttons, int timeout = 0);

    //button handling methods
    //NOTE: all the following methods are only updated every cycle as opposed to every function call, unlike the pros API

    void update_buttons();  //called once every loop, updates current and last state for every button
    bool get_button_state(controller_digital_e_t button); //returns current state of desired button
    bool get_button_last_state(controller_digital_e_t button); //returns last state of desired button
    bool is_rising(controller_digital_e_t button); //if button wasn't pressed but now is
    bool is_falling(controller_digital_e_t button); //if button was pressed but now is not


  template <typename... Params>
  void print(std::uint8_t line, std::uint8_t col, std::string fmt, Params... args){
    std::function<void() > func = [=](){
      Controller::print(line, col, fmt.c_str(), args...);
      controller_queue.print("Printing %s to %d", sprintf2(fmt, args...), controller_num);
    };
    add_to_queue(func);
    controller_queue.print("Adding print to queue for controller %d", controller_num);
  }
};
