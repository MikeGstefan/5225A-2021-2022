#include "drive.hpp"

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
    // printf("%d, %d\n", x, lookup_table[(unsigned char)x]);
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
Drivebase::Drivebase(std::array<driver, num_of_drivers> drivers) : drivers(drivers) {}

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
  if(!curve_file_exists) {
    drivers_data.print("WARNING: curve_file not found, using default data\n");
  }
  // reads data for each driver from file
  for (short driver = 0; driver < num_of_drivers; driver++){  // reads curve curvature from curve file
    drivers_data.print("num of drivers: %d\n", num_of_drivers);
    drivers_data.print("\nDRIVER: %s\n", drivers[driver].name);
    for (short curvature = 0; curvature < 3; curvature++){  // reads data for each axis of motion
      if (curve_file_exists)  fscanf(curve_file, "curvature:%lf\n", &drivers[driver].custom_drives[curvature].curvature);
      drivers_data.print("curvature: %lf\n", drivers[driver].custom_drives[curvature].curvature);
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

    while (!master.get_digital_new_press(E_CONTROLLER_DIGITAL_B)){  // user presses b to save and exit utility
      if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){  // moves to next driver
        cur_driver++;
        cur_driver %= num_of_drivers;
        update_screen();
      }
      else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_Y)){  // moves to previous driver
        if (cur_driver == 0)  cur_driver = num_of_drivers - 1;
        else cur_driver--;
        update_screen();
      }

      if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT)){ // moves to next screen
        cur_screen++;
        cur_screen %= 3; // to create rollover
        update_screen();
      }
      else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_LEFT)){  // moves to previous screen
        // rollover but without mod because c++ handles negative mods differently :(
        if (cur_screen == 0)  cur_screen = 2;
        else cur_screen--;
        update_screen();
      }

      if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){  // increase curvature
        if (drivers[cur_driver].custom_drives[cur_screen].curvature < 9.9)  drivers[cur_driver].custom_drives[cur_screen].curvature += 0.1;
        master.print(2, 0, "Curvature: %lf", drivers[cur_driver].custom_drives[cur_screen].curvature);  // updates curvature
      }
      else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)){  // decrease curvature
        printf("pressed down\n");
        if (drivers[cur_driver].custom_drives[cur_screen].curvature > 0.1)  drivers[cur_driver].custom_drives[cur_screen].curvature -= 0.1;
        master.print(2, 0, "Curvature: %lf", drivers[cur_driver].custom_drives[cur_screen].curvature);  // updates curvature
      }

      delay(10);
    }
    master.clear();
    if(!curve_file_exists) {
      drivers_data.print("WARNING: curve_file not found, not writing to SD card\n");
    }
    else {
      Data::log_t.data_update();
      curve_file = fopen("/usd/curve_file.txt", "w");

      for (short driver = 0; driver < num_of_drivers; driver++){  // uploads curve data to curve file
        for (short curvature = 0; curvature < 3; curvature++){
          fprintf(curve_file, "curvature:%lf\n", drivers[driver].custom_drives[curvature].curvature);
          drivers_data.print("curvature:%lf\n", drivers[driver].custom_drives[curvature].curvature);
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
  tracking.power_a = 0.7 * drivers[cur_driver].custom_drives[2].lookup(master.get_analog(ANALOG_LEFT_X));

  if(fabs(tracking.power_x) < deadzone) tracking.power_x = 0.0;
  if(fabs(tracking.power_y) < deadzone) tracking.power_y = 0.0;
  if(fabs(tracking.power_a) < deadzone) tracking.power_a = 0.0;

  // if(master.get_digital_new_press(reverse_drive_button)){
  //   master.rumble("-");
  //   reversed = !reversed;
  //   if(reversed) master.print(0, 0, "Reverse");
  //   else master.print(0, 0, "Forward");
  // }
  // if (reversed){
  //   tracking.power_y *= -1;
  //   tracking.power_x *= -1;
  // }

  if(this->state && (fabs(tracking.power_y) <deadzone && fabs(tracking.power_a) <deadzone)){
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
    b_lift.reset();
    b_lift.move_absolute(b_lift.bottom_position);
    f_lift.reset();
    f_lift.move_absolute(f_lift.bottom_position);
  // });
  
  // f_claw_p.set_value(LOW);
  b_claw.set_value(LOW);
  // lift.move(-10); // gives holding power
  bool intake_on = false;
  bool intake_reverse = false;
  cur_driver = 0; // defaults driver to Nikhil
  // master.print(2, 0, "Driver: %s", driver_name());
  while(true){
    while(true){
      // if(master.get_digital_new_press(tracking_button)){
      //   master.print(1,1,"%.2f, %.2f, %.2f", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
      // }
      if(master.get_digital_new_press(ok_button)){
        // auton_selector(); //talk to nathan if you're uncommenting this line
        // master.print(1,1,"HERE");
        delay(2000);
      }
      drivebase.handle_input();
      // b_lift.handle();
      // handle_lifts();
      f_lift.handle(true);
      // f_claw.handle();
      // b_claw.handle();
      // intake.handle();
     

      handle_trans();

      // prints motor temps every second
      if(screen_timer.get_time() > 1000){
        drivers_data.print("fl%.0f r%.0f bl%.0f r%.0f\n", front_l.get_temperature(), front_r.get_temperature(), back_l.get_temperature(), back_r.get_temperature());
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
  if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_B)){
    update_lookup_table_util();
    master.clear();
    master.print(2, 0, "Driver: %s", driver_name());
  }
  else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)) next_driver();
  else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)) prev_driver();

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
  drive_t.set_value(state);
}


void Drivebase::handle_trans(){
  if(master.get_digital_new_press(shift_button)){
    this->set_state(!this->get_state());
  }
}

bool Drivebase::get_reverse(){
  return this->reversed;
}

int Drivebase::get_deadzone(){
  return this->deadzone;
}


// bool Drivebase::get_lift_button(int side){
//   // bool front
//   // if(side == lift_button::front){
//   //     return !this->reversed&&;
//   // }
//   master.print(1,3,"%d",side);
//   master.print(1,1,"%d",(!this->reversed &&  master.get_analog(ANALOG_RIGHT_Y) > deadzone)&& side);
//   misc.print(" side :: %d",side);
//   misc.print(" output :: %d\n",(!this->reversed &&  master.get_analog(ANALOG_RIGHT_Y) > deadzone)&& side);
//   return (!this->reversed ==  master.get_analog(ANALOG_RIGHT_Y) > -1*deadzone);
// }



