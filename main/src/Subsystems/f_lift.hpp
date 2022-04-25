#pragma once
#include "../Libraries/subsystem.hpp"
#include "../Libraries/pid.hpp"

constexpr int f_lift_max_velo = 100;

enum class f_lift_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  bottom, // at lowest position
  top, // at highest position
  idle,  // not doing anything
  move_to_target,  // moving to target position
  between_positions,  // not moving, but in between positions in the array
  manual,  // controlled by joystick
  NUM_OF_ELEMENTS // number of elements
};

constexpr int num_of_f_lift_states = static_cast<int>(f_lift_states::NUM_OF_ELEMENTS);

class F_Lift: public Motorized_subsystem<f_lift_states, num_of_f_lift_states, f_lift_max_velo> {
    Timer up_press{"Up_press"}, down_press{"Down_press"};
    int search_cycle_check_count = 0, bad_count = 0; // cycle check for safeties
    PID pid = PID(3.0,0.0,0.0,0.0);
    int lift_power; // for manual control
    int detection_end_error = 40; // the range the lift has to be within of a position to be considered at that position 

    // height conversion constants
    double offset_a = 365.0, offset_h = 9.75;
    double arm_len = 8.0;
    double gear_ratio = 5.0;

    // to avoid race conditions these are atomic
    std::atomic<uint8_t> index{0};
    std::atomic<int32_t> speed{127}; // max pwm applied to the lifts during a move to target

  public:
    static constexpr int bottom = 0;
    static constexpr int carry = 1;
    static constexpr int plat = 2;
    static constexpr int backup = 3;
    static constexpr int push = 4;
    static constexpr int level = 5;
    static constexpr int top = 6;
    // bottom is 1162
    std::vector<int> driver_positions = {1185, 1400, 1900, 2170, 2775};
    std::vector<int> prog_positions = {1185, 1600, 1900, 2170, 2775};

    F_Lift(Motorized_subsystem<f_lift_states, num_of_f_lift_states, f_lift_max_velo> motorized_subsystem);  // constructor
    
    void handle(bool driver_array);  // contains state machine code, (if driver_array is false, uses prog_array)
    void handle_state_change(); // cleans up and preps the machine to be in the target state
    // void handle_buttons(); // handles driver button input
    // THIS IS AN OVERLOAD for the existing set state function, accepts an index for the lift
    void set_state(const f_lift_states next_state, const uint8_t index, const int32_t speed = 127);  // requests a state change and logs it
    
    int find_index(); // returns current driver index depending on lift positions, returns -1 if not found

    // for driver control
    void increment_index();
    void decrement_index();


    // getter because index is private
    int get_index() const;
    void elastic_util(int high); // up time should be about 1100mms (ignore this time, it was on the old lift), down time should be slightly slower than that
    void move_to_top();
    bool at_bottom() const;
};

extern F_Lift f_lift;

// FRONT CLAW SUBSYSTEM

enum class f_claw_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  idle, // claw is open and NOT waiting to detect mogo
  about_to_search, // claw is open and will search in 2 seconds
  searching,  // claw is open and waiting to detect mogo
  grabbed, // holding mogo
  NUM_OF_ELEMENTS, // number of elements
};

constexpr int num_of_f_claw_states = static_cast<int>(f_claw_states::NUM_OF_ELEMENTS);

class F_Claw: public Subsystem<f_claw_states, num_of_f_claw_states> {
    Timer search_timer{"Search_timer"};
  public:
    F_Claw(Subsystem<f_claw_states, num_of_f_claw_states> subsystem);  // constructor
    void handle();  // contains state machine code
    void handle_state_change(); // cleans up and preps the machine to be in the target state
};

extern F_Claw f_claw_obj;