#pragma once
#include "main.h"
#include "Libraries/logging.hpp"

// Master Buttons

  // Lift Buttons
    constexpr controller_digital_e_t lift_up_button = DIGITAL_R1;
    constexpr controller_digital_e_t lift_down_button = DIGITAL_R2;
    constexpr controller_digital_e_t both_lifts_down_button = DIGITAL_A;

  // Intake Buttons
    constexpr controller_digital_e_t intake_button = DIGITAL_B;
    constexpr controller_digital_e_t intake_reverse_button = DIGITAL_DOWN;

  // Misc Buttons
    constexpr controller_digital_e_t reverse_drive_button = DIGITAL_L1;
    constexpr controller_digital_e_t claw_toggle_button =  DIGITAL_L2;
    constexpr controller_digital_e_t ok_button = DIGITAL_A;
    constexpr controller_digital_e_t joy_mode_switch_button = DIGITAL_Y;
    constexpr controller_digital_e_t shift_button = DIGITAL_X;

// Partner Buttons

  // Lift Buttons
    constexpr controller_digital_e_t partner_back_lift_up_button = DIGITAL_R1;
    constexpr controller_digital_e_t partner_back_lift_down_button =  DIGITAL_R2;
    constexpr controller_digital_e_t partner_front_lift_up_button = DIGITAL_L1;
    constexpr controller_digital_e_t partner_front_lift_down_button = DIGITAL_L2;
    constexpr controller_digital_e_t partner_both_lifts_down_button = DIGITAL_DOWN;

  // Intake Buttons
    constexpr controller_digital_e_t partner_intake_button = DIGITAL_B;
    constexpr controller_digital_e_t partner_intake_reverse_button = DIGITAL_Y;

  // Claw Buttons
    constexpr controller_digital_e_t partner_back_claw_toggle_button = DIGITAL_A;
    constexpr controller_digital_e_t partner_front_claw_toggle_button = DIGITAL_LEFT;
    constexpr controller_digital_e_t partner_claw_tilt_button = DIGITAL_X;

  //Misc Buttons
    constexpr controller_digital_e_t partner_joy_mode_switch_button = DIGITAL_RIGHT;


#define num_controller 2

class _Task;

class _Controller : public Controller{
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
    _Controller(controller_id_e_t id);
    static void print_queue(void* params = NULL);
    static _Task controller_task;
    static void init();

    void clear_line (std::uint8_t line);
    void clear();
    void rumble(const char* rumble_pattern);
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

    // button handling methods
    // NOTE: all the following methods are only updated every cycle as opposed to every function call, unlike the pros API

    void update_buttons();  // called once every loop, updates current and last state for every button
    bool get_button_state(controller_digital_e_t button); // returns current state of desired button
    bool get_button_last_state(controller_digital_e_t button); // returns last state of desired button
    bool is_rising(controller_digital_e_t button); // if button wasn't pressed but now is
    bool is_falling(controller_digital_e_t button); // if button was pressed but now is not


  template <typename... Params>
  void print(std::uint8_t line, std::uint8_t col, const char* fmt, Params... args){
    std::function<void()> func = [=](){
      Controller::print(line, col, fmt, args...);
      controller_queue.print("%d| printing %s to %d\n", millis(), sprintf2(fmt, args...).c_str(), this->controller_num);
    };
    this->add_to_queue(func);
    controller_queue.print("%d| adding print to queue for controller %d\n", millis(), this->controller_num);
  }
};
