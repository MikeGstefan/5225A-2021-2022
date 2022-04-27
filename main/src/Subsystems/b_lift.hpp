#pragma once
#include "../Libraries/subsystem.hpp"
#include "../Libraries/pid.hpp"

constexpr int b_lift_max_velo = 100;
constexpr bool trans_lift_state = HIGH;
constexpr bool trans_intake_state = LOW;

enum class b_lift_states{
  managed, //being managed externally (NOT DOING ANYTHING)
  bottom, //at lowest position
  top, //at highest position
  idle,  //not doing anything
  move_to_target,  //moving to target position
  park_position,
  manual,  //controlled by joystick
  intake_off, //intake isn't running
  intake_on,  //intake is running forwards
  intk_jam,
  intake_reversed, //intake is running in reverse
  shifting_up, //lift / intake transmission shifting, moving up
  shifting_down, //lift / intake transmission shifting, moving down
  reshift, //if the intake failed to transmit, shifts to lift and enters this state, then goes back to intake-on
  NUM_OF_ELEMENTS, //number of elements
};

constexpr int num_of_b_lift_states = static_cast<int>(b_lift_states::NUM_OF_ELEMENTS);

class B_Lift: public Motorized_subsystem<b_lift_states, num_of_b_lift_states, b_lift_max_velo> {
    Timer up_press{"Up_press"}, down_press{"Down_press"};
    Timer intake_safe{"intake_safe", nullptr, false};
    int not_moving_count = 0;

    int bad_count = 0; //cycle check for safeties
    PID pid = PID(3.0, 0.0, 0.0, 0.0);
    int lift_pwr; //for manual control
    int detection_end_error = 40; //the range the lift has to be within of a position to be considered at that position 
    int intk_jam_count = 0;
    int jam_time =0;
    //height conversion constants
    double offset_a = 365.0, offset_h = 9.75;
    double arm_len = 8.0;
    double gear_ratio = 5.0;

    //to avoid race conditions these are atomic
    std::atomic<uint8_t> index{0};
    std::atomic<int32_t> speed{127}; //max pwm applied to the lifts during a move to target

  public:
    static constexpr int bottom = 0;
    static constexpr int plat = 1;
    static constexpr int backup = 2;
    static constexpr int level = 3;
    static constexpr int top = 4;

    b_lift_states after_shift_state; //the state the subsystem will go to after transmission shifts
    std::vector<int> driver_positions = {1045, 1720, 1825, 1970, 2800};
    std::vector<int> prog_positions = {1045, 1720, 1825, 1970, 2800};
    static constexpr int park_position = 1650 ;

    B_Lift(Motorized_subsystem<b_lift_states, num_of_b_lift_states, b_lift_max_velo> motorized_subsystem);  //constructor
    
    void handle(bool driver_array);  //contains state machine code, (if driver_array is false, uses prog_array)
    void handle_state_change(); //cleans up and preps the machine to be in the target state
    //THIS IS AN OVERLOAD for the existing set state function, accepts an index for the lift
    void set_state(const b_lift_states next_state, const uint8_t index, const int32_t speed = 127);  //requests a state change and logs it
    
    int find_index() const; //returns current driver index depending on lift positions, returns -1 if not found
    void handle_shift(); //checks if lift / intake transmission is in the wrong state, then shifts if necessary
    void shift(bool piston_state); //sets tranmission piston, jiggles up then down, then goes to target state

    //for driver control
    void increment_index();
    void decrement_index();

    //getters because index is private
    uint8_t get_index() const;
    void elastic_util(int high); //up time should be about 1100mms (ignore this time, it was on the old lift), down time should be slightly slower than that
    void move_to_top();
    bool at_bottom() const;
};

extern B_Lift b_lift;

enum class b_claw_states{
  managed, //being managed externally (NOT DOING ANYTHING)
  idle, //claw is open and NOT waiting to detect mogo
  about_to_search, //claw is open and will search in 2 seconds
  searching,  //claw is open and is waiting to detect mogo
  tilted, //holding mogo tilted
  //going_to_flat, //waiting 300 ms to flat
  flat, //holding mogo flat
  NUM_OF_ELEMENTS, //number of elements
};

constexpr int num_of_b_claw_states = static_cast<int>(b_claw_states::NUM_OF_ELEMENTS);

class B_Claw: public Subsystem<b_claw_states, num_of_b_claw_states> {
    Timer toggle_press_timer{"Toggle_timer"};  //when the toggle buttons was last pressed
    int search_cycle_check_count = 0; //for searching for mogo's lip
    Timer search_timer{"Search_timer"}; //timer to return to search after 2 seconds

  public:
    B_Claw(Subsystem<b_claw_states, num_of_b_claw_states> subsystem);  //constructor
    void handle();  //contains state machine code
    void handle_state_change(); //cleans up and preps the machine to be in the target state

    //state_at_toggle_press is used for tilt / flat toggling
    b_claw_states state_at_toggle_press; //state of b_claw when toggle button was pressed
};

extern B_Claw b_claw_obj;