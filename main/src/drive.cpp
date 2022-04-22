#include "drive.hpp"
#include <numeric>

// NOTE: all timers start as paused
// for lifts
Timer up_press{"up_press", nullptr, false};
Timer down_press{"down_press", nullptr, false};

Timer f_lift_up_press{"f_lift_up_press", nullptr, false};
Timer b_lift_up_press{"b_lift_up_press", nullptr, false};
Timer f_lift_down_press{"f_lift_down_press", nullptr, false};
Timer b_lift_down_press{"b_lift_down_press", nullptr, false};

Timer toggle_press_timer{"toggle_press_timer", nullptr, false}; // for back claw

Timer buzz_timer{"buzz_timer", nullptr, true}; // buzzes every 50 ms when the speed limit is on

joy_modes joy_mode = joy_modes::lift_select;

// array<int, 7> f_lift_pos= {10, 150, 300, 475, 630, 665, 675};
// int f_lift_index = 0;
// int f_lift_time = 0;

// array<int, 7> b_lift_pos= {10, 400, 550, 660, 665, 665, 675};
// int b_lift_index = 0;
// int b_lift_time = 0;
// bool looking = false;
// bool found = false;
// int find_count = 0;
// int b_lift_time = 0;
// custom drive class methods

// singleton drivebase instance
Drivebase drivebase = {{
  driver("Nikhil", {E_CONTROLLER_ANALOG_LEFT_X, E_CONTROLLER_ANALOG_RIGHT_Y, E_CONTROLLER_ANALOG_RIGHT_X}, {0.0, 0.5, 1.0}),
  driver("Emily", {E_CONTROLLER_ANALOG_LEFT_X, E_CONTROLLER_ANALOG_RIGHT_Y, E_CONTROLLER_ANALOG_RIGHT_X}, {0.0, 0.5, 1.0}),
  driver("Sarah", {E_CONTROLLER_ANALOG_LEFT_X, E_CONTROLLER_ANALOG_RIGHT_Y, E_CONTROLLER_ANALOG_RIGHT_X}, {0.0, 0.5, 1.0}),
  driver("Alex", {E_CONTROLLER_ANALOG_LEFT_X, E_CONTROLLER_ANALOG_LEFT_Y, E_CONTROLLER_ANALOG_RIGHT_X}, {0.0, 0.5, 1.0}),
  driver("Left-stick", {E_CONTROLLER_ANALOG_RIGHT_X, E_CONTROLLER_ANALOG_LEFT_Y, E_CONTROLLER_ANALOG_LEFT_X}, {0.0, 0.5, 1.0}),
}};

// curve file_template
// curvature:0.0
// curvature:0.5
// curvature:2.0
// curvature:0.0
// curvature:0.5
// curvature:2.0
// curvature:0.0
// curvature:0.5
// curvature:2.0
// curvature:0.0
// curvature:0.5
// curvature:2.0
// curvature:0.0
// curvature:0.5
// curvature:2.0

// custom drive methods

// private methods
int custom_drive::polynomial(int x){
  double n = curvature * 0.2 + 1; // scales curvature value to match expo function
  return round(pow(127, 1 - n) * pow(abs(x), n) * sgn(x));
}
int custom_drive::exponential(int x){
  double n = curvature;
  return round(exp(0.002 * n * (abs(x) - 127)) * x);
}

// custom_drive constructor
custom_drive::custom_drive(double curvature): curvature(curvature){}

void custom_drive::fill_lookup_table(){
  for (short x = -127; x < 128; x++){ // fills lookup table with values from appropriate function
    lookup_table[(unsigned char)x] = exponential(x);
    // UNCOMMENT THESE FOR DEBUGGING, comment for performance
    // printf2("%d, %d", x, lookup_table[(unsigned char)x]);
    // delay(1);
  }
}

int custom_drive::lookup(int x){
  return lookup_table[(unsigned char)x];
}

// driver struct constructor
driver::driver(const char* name, std::array<controller_analog_e_t, 3> joy_sticks, std::array<custom_drive, 3> custom_drives):
name(name), joy_sticks{joy_sticks[0], joy_sticks[1], joy_sticks[2]}, custom_drives{custom_drives[0], custom_drives[1], custom_drives[2]}
{}

// Drivebase class constructor
Drivebase::Drivebase(std::array<driver, num_of_drivers> drivers) : drivers(drivers){}

void Drivebase::move(double x, double y, double a){
  ERROR.print("you called the wrong function");
  // front_l.move(x + y + a);
  // front_r.move(-x + y - a);
  // back_l.move(-x + y + a);
  // back_r.move(x + y - a);
}

void Drivebase::move(double y, double a){
  front_l.move(y+a);
  front_r.move(y-a);
  back_l.move(y+a);
  back_r.move(y-a);
  center_l.move(y+a);
  center_r.move(y-a);
  // back_l.move(0);
  // back_r.move(0);
}


void Drivebase::move_side(double l, double r){
  front_l.move(l);
  front_r.move(r);
  back_l.move(l);
  back_r.move(r);
  center_l.move(l);
  center_r.move(r);
}

void Drivebase::brake(){
  front_l.move_relative(0, 200);
  front_r.move_relative(0, 200);
  back_l.move_relative(0, 200);
  back_r.move_relative(0, 200);
  center_l.move_relative(0,200);
  center_r.move_relative(0,200);
}

void Drivebase::velo_brake(){
  front_l.move_velocity(0);
  front_r.move_velocity(0);
  back_l.move_velocity(0);
  back_r.move_velocity(0);
  center_l.move_velocity(0);
  center_r.move_velocity(0);
}

void Drivebase::update_screen(){
  master.print(0, 0, "Driver: %s          ", driver_name()); // updates driver
  master.print(1, 0, screen_text[cur_screen]);  // updates text
  master.print(2, 0, "Curvature: %lf", drivers[cur_driver].custom_drives[cur_screen].curvature);  // updates curvature
}


void Drivebase::download_curve_data(){
  Data::log_t.data_update();
  curve_file = nullptr;
  curve_file = fopen("/usd/curve_file.txt", "r");
  curve_file_exists = curve_file != NULL;
  if(!curve_file_exists){
    drivers_data.print("WARNING: curve_file not found, using default data");
  }
  // reads data for each driver from file
  for (short driver = 0; driver < num_of_drivers; driver++){  // reads curve curvature from curve file
    drivers_data.print("num of drivers: %d", num_of_drivers);
    drivers_data.print("\nDRIVER: %s", drivers[driver].name);
    for (short curvature = 0; curvature < 3; curvature++){  // reads data for each axis of motion
      if (curve_file_exists)  fscanf(curve_file, "curvature:%lf\n", &drivers[driver].custom_drives[curvature].curvature);
      drivers_data.print("curvature: %lf", drivers[driver].custom_drives[curvature].curvature);
      drivers[driver].custom_drives[curvature].fill_lookup_table();
      delay(1);
    }
  }
  fclose(curve_file);
  Data::log_t.done_update();
}

void Drivebase::update_lookup_table_util(){
  download_curve_data();

  master.clear();
  update_screen();

  while (!master.is_rising(E_CONTROLLER_DIGITAL_B)){  // user presses b to save and exit utility
    if(master.is_rising(E_CONTROLLER_DIGITAL_A)){  // moves to next driver
      cur_driver++;
      cur_driver %= num_of_drivers;
      update_screen();
    }
    else if(master.is_rising(E_CONTROLLER_DIGITAL_Y)){  // moves to previous driver
      if (cur_driver == 0)  cur_driver = num_of_drivers - 1;
      else cur_driver--;
      update_screen();
    }

    if(master.is_rising(E_CONTROLLER_DIGITAL_RIGHT)){ // moves to next screen
      cur_screen++;
      cur_screen %= 3; // to create rollover
      update_screen();
    }
    else if(master.is_rising(E_CONTROLLER_DIGITAL_LEFT)){  // moves to previous screen
      // rollover but without mod because c++ handles negative mods differently :(
      if (cur_screen == 0)  cur_screen = 2;
      else cur_screen--;
      update_screen();
    }

    if(master.is_rising(E_CONTROLLER_DIGITAL_UP)){  // increase curvature
      if (drivers[cur_driver].custom_drives[cur_screen].curvature < 9.9)  drivers[cur_driver].custom_drives[cur_screen].curvature += 0.1;
      master.print(2, 0, "Curvature: %lf", drivers[cur_driver].custom_drives[cur_screen].curvature);  // updates curvature
    }
    else if(master.is_rising(E_CONTROLLER_DIGITAL_DOWN)){  // decrease curvature
      printf2("pressed down");
      if (drivers[cur_driver].custom_drives[cur_screen].curvature > 0.1)  drivers[cur_driver].custom_drives[cur_screen].curvature -= 0.1;
      master.print(2, 0, "Curvature: %lf", drivers[cur_driver].custom_drives[cur_screen].curvature);  // updates curvature
    }

    delay(10);
  }
  master.clear();
  if(!curve_file_exists){
    drivers_data.print("WARNING: curve_file not found, not writing to SD card");
  }
  else {
    Data::log_t.data_update();
    curve_file = fopen("/usd/curve_file.txt", "w");

    for (short driver = 0; driver < num_of_drivers; driver++){  // uploads curve data to curve file
      for (short curvature = 0; curvature < 3; curvature++){
        fprintf(curve_file, "curvature:%lf\n", drivers[driver].custom_drives[curvature].curvature);
        drivers_data.print("curvature:%lf", drivers[driver].custom_drives[curvature].curvature);
      }
    }
    fclose(curve_file);
    Data::log_t.done_update();
  }
  master.print(0, 0, "Saved.");
  delay(200);
}

void Drivebase::handle_input(){
  // tracking.power_x = drivers[cur_driver].custom_drives[0].lookup(master.get_analog(drivers[cur_driver].joy_sticks[0]));
  tracking.power_y = drivers[cur_driver].custom_drives[1].lookup(master.get_analog(ANALOG_LEFT_Y));
  // caps drive speed at 40 if intake is on
  // if(b_lift.get_state() == b_lift_states::intake_on || b_lift.get_state() == b_lift_states::intk_jam){
  if(master.is_rising(speed_limit_button)) speed_limit_active = !speed_limit_active;

  if(speed_limit_active){
    if(buzz_timer.get_time() > 50){
      master.rumble("-");
      buzz_timer.reset();
    }
    if(fabs(tracking.power_y) > MAX_DRIVE_SPEED)  tracking.power_y = sgn(tracking.power_y) * MAX_DRIVE_SPEED;
    // caps turns speed at 40% when intake is on
    tracking.power_a = 0.4 * drivers[cur_driver].custom_drives[2].lookup(master.get_analog(ANALOG_LEFT_X));
  }
  else tracking.power_a = 0.6 * drivers[cur_driver].custom_drives[2].lookup(master.get_analog(ANALOG_LEFT_X));
  // printf2("%d, %f, %f", master.get_analog(ANALOG_LEFT_Y), tracking.power_y, tracking.power_a );
  if(fabs(tracking.power_x) < deadzone) tracking.power_x = 0.0;
  if(fabs(tracking.power_y) < deadzone) tracking.power_y = 0.0;
  if(fabs(tracking.power_a) < deadzone) tracking.power_a = 0.0;

  if(master.is_rising(reverse_drive_button)){
    master.rumble("-");
    reversed = !reversed;
    if(reversed) master.print(0, 0, "Reverse");
    else master.print(0, 0, "Forward");
  }
  if (reversed){
    tracking.power_y *= -1;
    tracking.power_x *= -1;
  }

  if(this->state && (fabs(tracking.power_y) < deadzone && fabs(tracking.power_a) < deadzone)){
    // velo_brake();
    brake();
  }
  else{ 
    move(tracking.power_y, tracking.power_a);
    
  }

  // move(tracking.power_y, tracking.power_a);
  // move(0, tracking.power_a);
}

void Drivebase::driver_practice(){
  master.clear();
  partner.clear();

  // master.print(1, 0, "Driver: %s", drivebase.drivers[drivebase.cur_driver].name);
  master.print(0, 0, "Forward");
  // master.print(B_LIFT_STATE_LINE, 0, "B_Lift: Searching     ");
  // master.print(F_LIFT_STATE_LINE, 0, "F_Lift: Searching     ");


  // initializes pneumatics in appropriate state

  // moves motors to necessary positions / speeds
  // Task([](){ 
    // b_lift.reset();
    // f_lift.reset();
  // });
  
  // f_claw_p.set_value(LOW);
  b_claw.set_state(LOW);
  // lift.move(-10); // gives holding power
  cur_driver = 0; // defaults driver to Nikhil
  // master.print(2, 0, "Driver: %s", driver_name());
  while(true){
    while(true){
      // if(master.is_rising(tracking_button)){
      //   master.print(1,1,"%.2f, %.2f, %.2f", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
      // }
      if(master.is_rising(ok_button)){
        // auton_selector(); //talk to nathan if you're uncommenting this line
        // master.print(1,1,"HERE");
        delay(2000);
      }
      drivebase.handle_input();
      // intake.handle();

      f_claw_obj.handle();
      b_claw_obj.handle();
      // intake.handle_buttons();
      // intake.handle();
     

      handle_trans();

      // prints motor temps every second
      if(screen_timer.get_time() > 1000){
        drivers_data.print("fl%.0f r%.0f bl%.0f r%.0f", front_l.get_temperature(), front_r.get_temperature(), back_l.get_temperature(), back_r.get_temperature());
        // master.print(0, 0, "fl%.0f r%.0f bl%.0f r%.0f\n", front_l.get_temperature(), front_r.get_temperature(), back_l.get_temperature(), back_r.get_temperature());
        screen_timer.reset();
      }
      // takes away control from driver when motors overheat
      if(front_l.get_temperature() >= 55 || front_r.get_temperature() >= 55 || back_r.get_temperature() >= 55 || back_l.get_temperature() >= 55 || b_lift_m.get_temperature() >= 55|| f_lift_m.get_temperature() >= 55){
        master.rumble("- - - "); // rumbles controller if motors are hot to warn driver
        // move(0, 0, 0);  // stops movement
        // return;
      }

      delay(10);
    }
    update_lookup_table_util();
    // master.clear();
    // master.print(2, 0, "Driver: %s", drivers[cur_driver].name);
  }
}

void Drivebase::non_blocking_driver_practice(){
  if(master.is_rising(E_CONTROLLER_DIGITAL_B)){
    update_lookup_table_util();
    master.clear();
    master.print(2, 0, "Driver: %s", driver_name());
  }
  else if (master.is_rising(E_CONTROLLER_DIGITAL_UP)) next_driver();
  else if (master.is_rising(E_CONTROLLER_DIGITAL_DOWN)) prev_driver();

  // actual drive code
  drivebase.handle_input();
}

void Drivebase::next_driver(){
  cur_driver++;
  cur_driver %= num_of_drivers; // rollover
  // spaces in the controller print are to overwrite names
  master.print(2, 0, "Driver: %s          ", driver_name());
}

void Drivebase::prev_driver(){
  if (cur_driver == 0)  cur_driver = num_of_drivers - 1;
  else cur_driver--;
  master.print(2, 0, "Driver: %s          ", driver_name());
}

const char* Drivebase::driver_name(){
  return drivers[cur_driver].name;
}


bool Drivebase::get_state(){
  return this->state;
}

void Drivebase::set_state(bool state){
  this->state = state;
  drive_t.set_state(state);
}


void Drivebase::handle_trans(){
  if(master.is_rising(shift_button)){
    this->set_state(!this->get_state());
  }
}

bool Drivebase::get_reverse(){
  return this->reversed;
}

int Drivebase::get_deadzone(){
  return this->deadzone;
}

void Drivebase::reset(){
  RightEncoder.reset();
  LeftEncoder.reset();
  BackEncoder.reset();
  tracking.reset();

  drivebase.move(0.0, 0.0);
  drivebase.set_state(HIGH);
}

void Drivebase::random_turn(int direction){
  if(direction == 0) direction = random_direction();
  if(direction < 0) drivebase.move_side(-40, 110);
  else if(direction > 0) drivebase.move_side(110, -40);
  tracking.wait_for_dist(6);
  move_stop(true);
}


// bool Drivebase::get_lift_button(int side){
//   // bool front
//   // if(side == lift_button::front){
//   //     return !this->reversed&&;
//   // }
//   master.print(1,3,"%d",side);
//   master.print(1,1,"%d",(!this->reversed &&  master.get_analog(ANALOG_RIGHT_Y) > deadzone)&& side);
//   misc.print(" side :: %d",side);
//   misc.print(" output :: %d",(!this->reversed &&  master.get_analog(ANALOG_RIGHT_Y) > deadzone)&& side);
//   return (!this->reversed ==  master.get_analog(ANALOG_RIGHT_Y) > -1*deadzone);
// }

bool get_lift(){
  // if in lift select mode, compute the lift, otherwise return last value of cur lift
  // static bool cur_lift = (joy_mode == joy_modes::lift_select) ? (!drivebase.get_reverse() == master.get_analog(ANALOG_RIGHT_Y) > -1* drivebase.get_deadzone()) : cur_lift;
  // return cur_lift;
  return !drivebase.get_reverse() == master.get_analog(ANALOG_RIGHT_Y) > -1* drivebase.get_deadzone();
}

void handle_lift_buttons(){
  // if(master.is_rising(joy_mode_switch_button) || partner.is_rising(partner_joy_mode_switch_button)){
    if(partner.is_rising(partner_joy_mode_switch_button)){
    // toggles joy_mode
    if(joy_mode == joy_modes::lift_select)  joy_mode = joy_modes::manual;
    else joy_mode = joy_modes::lift_select;

    if(joy_mode == joy_modes::lift_select){
      master.print(0,0, "auto   ");
      partner.print(0,0, "auto   ");
      b_lift.Subsystem::set_state(b_lift_states::idle);
      f_lift.Subsystem::set_state(f_lift_states::idle);
    }
    else{
      master.print(0,0, "manual   ");
      partner.print(0,0, "manual   ");
      b_lift.Subsystem::set_state(b_lift_states::manual);
      f_lift.Subsystem::set_state(f_lift_states::manual); 
    }
  }


  // front lift increment index
  if(joy_mode != joy_modes::manual){
    if(master.is_rising(lift_up_button) && get_lift()){
      up_press.reset();
      f_lift.increment_index();
    }
    if(partner.is_rising(partner_front_lift_up_button)){
      f_lift_up_press.reset();
      f_lift.increment_index();
    }
  }

  // back lift increment index
  if(joy_mode != joy_modes::manual){
    if(master.is_rising(lift_up_button) && !get_lift()){
      up_press.reset();
      b_lift.increment_index();
    }
    if(partner.is_rising(partner_back_lift_up_button) && !partner.get_button_state(partner_back_lift_down_button)){
      b_lift_up_press.reset();
      b_lift.increment_index();
    }
  }

  // front lift decrement index
  if(joy_mode != joy_modes::manual){
    if(master.is_rising(lift_down_button) && get_lift()){
      down_press.reset();
      f_lift.decrement_index();
    }
    if(partner.is_rising(partner_front_lift_down_button)){
      f_lift_down_press.reset();
      f_lift.decrement_index();
    }
  }

  // back lift decrement index
  if(joy_mode != joy_modes::manual){
    if(master.is_rising(lift_down_button) && !get_lift()){
      down_press.reset();
      b_lift.decrement_index();
    }
    if(partner.is_rising(partner_back_lift_down_button) && !partner.get_button_state(partner_back_lift_up_button)){
      b_lift_down_press.reset();
      b_lift.decrement_index();
    }
  }

  if(b_lift.get_state() != b_lift_states::park_position && partner.get_button_state(partner_back_lift_down_button) && partner.get_button_state(partner_back_lift_up_button)){
    b_lift.Subsystem::set_state(b_lift_states::park_position);

  }
  // PARTNER STUFF


  // resets and pauses the timers if driver releases button
  // for master
  if(!master.get_digital(lift_up_button) && up_press.playing()) up_press.reset(false);
  if(!master.get_digital(lift_down_button) && down_press.playing()) down_press.reset(false);
  // for partner
  if(!partner.get_digital(partner_front_lift_up_button) && f_lift_up_press.playing()) f_lift_up_press.reset(false);
  if((!partner.get_digital(partner_back_lift_up_button) || partner.get_digital(partner_back_lift_down_button))&& b_lift_up_press.playing()) b_lift_up_press.reset(false);
  if(!partner.get_digital(partner_front_lift_down_button) && f_lift_down_press.playing()) f_lift_down_press.reset(false);
  if((!partner.get_digital(partner_back_lift_down_button) || partner.get_digital(partner_back_lift_up_button))&& b_lift_down_press.playing()) b_lift_down_press.reset(false);

  // goes to top position if up button is held
  // for master
  if(up_press.get_time() > MASTER_HOLD_TIME){
    if(get_lift())  f_lift.set_state(f_lift_states::move_to_target, f_lift.driver_positions.size() - 1);
    else  b_lift.set_state(b_lift_states::move_to_target, b_lift.driver_positions.size() - 1);
  }
  // goes to bottom position if down button is held
  if(down_press.get_time() > MASTER_HOLD_TIME){
    if(get_lift())  f_lift.set_state(f_lift_states::move_to_target, 0);
    else  b_lift.set_state(b_lift_states::move_to_target, 0);
  }
  // for partner (if lift buttons are held go all the way up/down)
  if(f_lift_up_press.get_time() > PARTNER_HOLD_TIME){
    f_lift.set_state(f_lift_states::move_to_target, f_lift.driver_positions.size() - 2);
  }
  if(b_lift_up_press.get_time() > PARTNER_HOLD_TIME){
    b_lift.set_state(b_lift_states::move_to_target, b_lift.driver_positions.size() - 2);
  }
  if(f_lift_down_press.get_time() > PARTNER_HOLD_TIME){
    f_lift.set_state(f_lift_states::move_to_target, 0);
  }
  if(b_lift_down_press.get_time() > PARTNER_HOLD_TIME){
    b_lift.set_state(b_lift_states::move_to_target, 0);
  }


  if(partner.is_rising(partner_both_lifts_down_button)){
    f_lift.set_state(f_lift_states::move_to_target, 0);
    b_lift.set_state(b_lift_states::move_to_target, 0);
  }

  // transfer to intake
  if(partner.is_rising(partner_trans_to_intk_button) && lift_t.get_state() == TRANS_LIFT_STATE){
    b_lift.Subsystem::set_state(b_lift_states::intake_off);
  }
}

void handle_claw_buttons(){
  if(master.is_rising(claw_toggle_button)){
    if(get_lift()){ // if front claw
      // toggles f_claw open/closed when claw toggle button is pressed
      switch(f_claw_obj.get_state()){
        case f_claw_states::grabbed:
          if(F_LIFT_AT_BOTTOM) f_claw_obj.set_state(f_claw_states::about_to_search);
          else f_claw_obj.set_state(f_claw_states::idle);
          break;

        default:
          f_claw_obj.set_state(f_claw_states::grabbed);
          break;
      }
    }
    else{ // if back claw
      toggle_press_timer.reset();
      b_claw_obj.state_at_toggle_press = b_claw_obj.get_state(); // used for tilt/flat toggle code
      // logs state_at_toggle_press for debugging
      state_log.print("state_at_toggle_press: %s\n", b_claw_obj.state_names[static_cast<int>(b_claw_obj.state_at_toggle_press)]);

      // if back claw is in an open state, close it, otherwise, don't do anything
      switch(b_claw_obj.get_state()){
        case b_claw_states::idle:
          b_claw_obj.set_state(b_claw_states::tilted);
          break;

        case b_claw_states::about_to_search:
          b_claw_obj.set_state(b_claw_states::tilted);
          break;

        case b_claw_states::searching:
          b_claw_obj.set_state(b_claw_states::tilted);
          break;

        default:
          break;
      }
    }
  }
  // toggles f_claw open/closed when claw toggle button is pressed
  if(partner.is_rising(partner_front_claw_toggle_button)){
    switch(f_claw_obj.get_state()){
      case f_claw_states::grabbed:
        if(F_LIFT_AT_BOTTOM) f_claw_obj.set_state(f_claw_states::about_to_search);
        else f_claw_obj.set_state(f_claw_states::idle);
        break;

      default:
        f_claw_obj.set_state(f_claw_states::grabbed);
        break;
    }
  }

  // toggles b_claw open/closed when claw toggle button is pressed
  if(partner.is_rising(partner_back_claw_toggle_button)){
    if(b_claw_obj.get_state() == b_claw_states::tilted || b_claw_obj.get_state() == b_claw_states::flat){
      if(b_lift.get_state() == b_lift_states::bottom) b_claw_obj.set_state(b_claw_states::about_to_search);
      else b_claw_obj.set_state(b_claw_states::idle);
    }
    else  b_claw_obj.set_state(b_claw_states::tilted);
  }
  // tilt code here

  if(toggle_press_timer.get_time() > TILT_HOLD_TIME){ // if toggle button was held, toggle tilted/flat state
    toggle_press_timer.reset(false);  // resets and pauses the timer
    // only toggle the tilt state if the state was tilted or flat when the button was pressed
    if(b_claw_obj.state_at_toggle_press == b_claw_states::tilted || b_claw_obj.state_at_toggle_press == b_claw_states::flat){
      if(b_claw_obj.get_state() == b_claw_states::tilted) b_claw_obj.set_state(b_claw_states::flat);
      else if(b_claw_obj.get_state() == b_claw_states::flat)  b_claw_obj.set_state(b_claw_states::tilted);
    }
    b_claw_obj.state_at_toggle_press = b_claw_states::idle;  // resets state_at_toggle_press so this if isn't entered again
    // printf("just_set: %s\n", b_claw_obj.state_names[(int)b_claw_obj.state_at_toggle_press]);
  }
  if(master.is_falling(claw_toggle_button)){
    // if the button WASN'T held and the claw was closed when it was pressed, open the claw
    if(toggle_press_timer.get_time() < TILT_HOLD_TIME && (b_claw_obj.state_at_toggle_press == b_claw_states::tilted || b_claw_obj.state_at_toggle_press == b_claw_states::flat)){
      // printf("fail| state_at_toggle_press: %s, timer: %d\n", b_claw_obj.state_names[(int)b_claw_obj.state_at_toggle_press], toggle_press_timer.get_time());
      if(B_LIFT_AT_BOTTOM) b_claw_obj.set_state(b_claw_states::about_to_search);
      else b_claw_obj.set_state(b_claw_states::idle);     
      b_claw_obj.state_at_toggle_press = b_claw_states::idle;  // resets state_at_toggle_press so this if isn't entered again
    }
    toggle_press_timer.reset(false); // resets and pauses timer now that button isn't pressed
  }

  // toggles tilt state if partner_claw_tilt button is pressed
  if(partner.is_rising(partner_claw_tilt_button)){
    if(b_claw_obj.get_state() == b_claw_states::tilted) b_claw_obj.set_state(b_claw_states::flat);
    else if(b_claw_obj.get_state() == b_claw_states::flat) b_claw_obj.set_state(b_claw_states::tilted);
  }

  // HITCH CODE

  // toggles open/closed when hitch button is pressed
  if(master.is_rising(hitch_button) || partner.is_rising(partner_hitch_button)){
    // hitch.toggle_state();
    if(hitch_obj.get_state() == hitch_states::grabbed) hitch_obj.set_state(hitch_states::about_to_search);
    else hitch_obj.set_state(hitch_states::grabbed);
  }

}

void handle_intake_buttons(){
  // toggles intake state if intake button is pressed
  if(master.is_rising(intake_button) || partner.is_rising(partner_intake_button)){
    switch(b_lift.get_state()){
      case b_lift_states::intake_on:
        b_lift.Subsystem::set_state(b_lift_states::intake_off);
        break;
      default:
        b_lift.Subsystem::set_state(b_lift_states::intake_on);
        break;
    }
  }
  // toggles intake reverse state if intake reverse button is pressed
  if(master.is_rising(intake_reverse_button)){
    switch(b_lift.get_state()){
        case b_lift_states::intake_reversed:
          b_lift.Subsystem::set_state(b_lift_states::intake_off);
          break;
        default:
          b_lift.Subsystem::set_state(b_lift_states::intake_reversed);
          break;
    }
  }
}
