#pragma once
#include "../Libraries/subsystem.hpp"
#include "../pid.hpp"

#define NUM_OF_B_LIFT_POSITIONS 4 // for driver array

#define NUM_OF_B_LIFT_STATES 9
#define B_LIFT_MAX_VELOCITY 100

enum class b_lift_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  bottom, // at lowest position
  idle,  // not doing anything
  move_to_target,  // moving to target position
  top, // at highest position
  between_positions,  // not moving, but in between positions in the array
  manual,  // controlled by joystick
  shifting_to_lift_up, // lift/intake transmission switching to lift mode, moving up
  shifting_to_lift_down // lift/intake transmission switching to lift mode, moving down
};

class B_Lift: public Motorized_subsystem<b_lift_states, NUM_OF_B_LIFT_STATES, B_LIFT_MAX_VELOCITY> {
    Timer up_press{"Up_press"}, down_press{"Down_press"};
    b_lift_states after_switch_state; // the state the lift will go to after transmission switches to lift
    int bad_count = 0; // cycle check for safeties
    PID pid = PID(5.0,0.0,0.0,0.0);
    int lift_power; // for manual control

    // height conversion constants
    double offset_a = 365.0, offset_h = 9.75;
    double arm_len = 8.0;
    double gear_ratio = 5.0;

    // to avoid race conditions these are atomic
    std::atomic<uint8_t> index{0};
    std::atomic<int32_t> speed{127}; // max pwm applied to the lifts during a move to target

  public:
    vector<int> driver_positions = {1035, 1825, 1970, 2750};
    vector<int> prog_positions = {1035, 1825, 1970, 2750};

    B_Lift(Motorized_subsystem<b_lift_states, NUM_OF_B_LIFT_STATES, B_LIFT_MAX_VELOCITY> motorized_subsystem);  // constructor
    
    void handle(bool driver_array);  // contains state machine code, (if driver_array is false, uses prog_array)
    void handle_state_change(); // cleans up and preps the machine to be in the target state
    // THIS IS AN OVERLOAD for the existing set state function, accepts an index for the lift
    void set_state(const b_lift_states next_state, const uint8_t index, const int32_t speed = 127);  // requests a state change and logs it
    
    // getters because index is private
    uint8_t get_index() const{
      return index.load();
    }
    void elastic_util(int high); // up time should be about 1100mms (ignore this time, it was on the old lift), down time should be slightly slower than that
    void move_absolute(double position, double velocity = B_LIFT_MAX_VELOCITY, bool wait_for_comp = true, double end_error = 20);
};

extern B_Lift b_lift;

#define NUM_OF_B_CLAW_STATES 6

// FRONT CLAW SUBSYSTEM

enum class b_claw_states{
  managed, // being managed externally (NOT DOING ANYTHING)
  idle, // claw is open and NOT waiting to detect mogo
  about_to_search, // claw is open and will search in 2 seconds
  searching,  // claw is open and is waiting to detect mogo
  tilted, // holding mogo tilted
  flat, // holding mogo flat
};

class B_Claw: public Subsystem<b_claw_states, NUM_OF_B_CLAW_STATES> {
    Timer toggle_press_timer{"Toggle_timer"};  // when the toggle buttons was last pressed
    int search_cycle_check_count = 0; // for searching for mogo's lip
    Timer search_timer{"Search_timer"}; // timer to return to search after 2 seconds

  public:
    B_Claw(Subsystem<b_claw_states, NUM_OF_B_CLAW_STATES> subsystem);  // constructor
    void handle();  // contains state machine code
    void handle_state_change(); // cleans up and preps the machine to be in the target state

    // state_at_toggle_press is used for tilt/flat toggling
    b_claw_states state_at_toggle_press; // state of b_claw when toggle button was pressed
};

extern B_Claw b_claw_obj;