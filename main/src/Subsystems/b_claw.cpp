#include "b_claw.hpp"
// BACK CLAW SUBSYSTEM
B_Claw b_claw({"B_Claw",
{
  "idle",
  "search_lip",
  "search_bowl",
  "grabbed",
}
});

B_Claw::B_Claw(Subsystem<b_claw_states, NUM_OF_B_CLAW_STATES> subsystem): Subsystem(subsystem)  // constructor
{
  // state setup
  state = b_claw_states::search_lip;
  last_state = state;
}

void B_Claw::handle(){
  switch(state){
    case b_claw_states::idle:
    
      // enters search mode if the lift is at the bottom and it's been 2 seconds since the mogo was released
      if(master.get_digital_new_press(search_button) || release_timer.get_time() > 2000){
        master.rumble("-");
        set_state(b_claw_states::search_lip);
      }
      // grabs goal if toggle button is pressed
      if(master.get_digital_new_press(b_claw_toggle_button)){
        master.rumble("-");
        b_claw_p.set_value(HIGH);

        // raises mogo above rings automatically if lift is in bottom state
        if(f_lift.get_state() == f_lift_states::bottom){
          f_lift.index = 1; // sends f_lift to raised position
        }

        set_state(b_claw_states::grabbed);
      }
      break;

    case b_claw_states::search_lip:
      // cancels search if search button is pressed
      if(master.get_digital_new_press(search_button)){
        release_timer.reset(false); // pauses timer
        master.rumble("-");
        set_state(b_claw_states::idle);
      }
      // search for bowl of mogo once lip is detected
      if(b_dist.get() < 70){
        search_cycle_check_count = 0; // resets search cycle count

        set_state(b_claw_states::search_bowl);
      }

      // grabs goal if toggle button is pressed or mogo is detected
      if(master.get_digital_new_press(b_claw_toggle_button)){
        master.rumble("-");
        b_claw_p.set_value(HIGH);

        // raises mogo above rings automatically if lift is in bottom state
        if(f_lift.get_state() == f_lift_states::bottom){
          f_lift.index = 1; // sends f_lift to raised position
        }

        set_state(b_claw_states::grabbed);
      }
      break;

    case b_claw_states::search_bowl:
      // cancels search if search button is pressed
      if(master.get_digital_new_press(search_button)){
        release_timer.reset(false); // pauses timer
        master.rumble("-");
        set_state(b_claw_states::idle);
      }
      // grabs goal if up button is pressed or bowl is detected
      if (b_dist.get() > 70 && b_dist.get() < 95) search_cycle_check_count++;

      // grabs goal if toggle button is pressed or mogo is detected
      if(master.get_digital_new_press(b_claw_toggle_button) || search_cycle_check_count >= 2){
        master.rumble("-");
        b_claw_p.set_value(HIGH);

        // raises mogo above rings automatically if lift is in bottom state
        if(f_lift.get_state() == f_lift_states::bottom){
          f_lift.index = 1; // sends f_lift to raised position
        }

        set_state(b_claw_states::grabbed);
      }
      break;

    case b_claw_states::grabbed:
      // releases goal if down button is pressed
      if(master.get_digital_new_press(b_claw_toggle_button)){
        b_claw_p.set_value(LOW);
        release_timer.reset();

        set_state(b_claw_states::idle);
      }
      break;

  }
}