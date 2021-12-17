#include "drive.hpp"
// custom drive class methods

// Buttons

// dropoff buttons
controller_digital_e_t ring_dropoff_button = E_CONTROLLER_DIGITAL_UP;
controller_digital_e_t switch_dropoff_side_button = E_CONTROLLER_DIGITAL_DOWN;

// spinning buttons
controller_digital_e_t turn_mogo_cw_button = E_CONTROLLER_DIGITAL_RIGHT;
controller_digital_e_t turn_mogo_ccw_button = E_CONTROLLER_DIGITAL_LEFT;

// fbar buttons
controller_digital_e_t f_bar_up_button = E_CONTROLLER_DIGITAL_L1;
controller_digital_e_t f_bar_down_button = E_CONTROLLER_DIGITAL_L2;

// misc buttons
controller_digital_e_t cancel_dropoff_button = E_CONTROLLER_DIGITAL_B;
controller_digital_e_t fill_top_goal_button = E_CONTROLLER_DIGITAL_Y;
controller_digital_e_t pickup_rings_button = E_CONTROLLER_DIGITAL_X;
controller_digital_e_t platform_down_button = E_CONTROLLER_DIGITAL_A;

controller_digital_e_t mogo_toggle_button = E_CONTROLLER_DIGITAL_R1;
controller_digital_e_t tip_mogo_button = E_CONTROLLER_DIGITAL_R2;

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
  front_l.move(x + y + a);
  front_r.move(-x + y - a);
  back_l.move(-x + y + a);
  back_r.move(x + y - a);
}

void Drivebase::move_tank(double y, double a){
  move(0.0, y, a);
}

void Drivebase::move_side(double l, double r){
  front_l.move(l);
  front_r.move(r);
  back_l.move(l);
  back_r.move(r);
}

void Drivebase::brake(){
  front_l.move_relative(0, 200);
  front_r.move_relative(0, 200);
  back_l.move_relative(0, 200);
  back_r.move_relative(0, 200);
}

void Drivebase::update_screen(){
  master.print(0, 0, "Driver: %s          ", drivers[cur_driver].name); // updates driver
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

    while (!master.get_digital_new_press(E_CONTROLLER_DIGITAL_B)){  // user presses b to save and exit utility
      if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_X)){  // moves to next driver
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
  tracking.power_x = drivers[cur_driver].custom_drives[0].lookup(master.get_analog(drivers[cur_driver].joy_sticks[0]));
  tracking.power_y = drivers[cur_driver].custom_drives[1].lookup(master.get_analog(drivers[cur_driver].joy_sticks[1]));
  tracking.power_a = drivers[cur_driver].custom_drives[2].lookup(master.get_analog(drivers[cur_driver].joy_sticks[2]));

  if(fabs(tracking.power_x) < deadzone) tracking.power_x = 0.0;
  if(fabs(tracking.power_y) < deadzone) tracking.power_y = 0.0;
  if(fabs(tracking.power_a) < deadzone) tracking.power_a = 0.0;

  move(tracking.power_x, tracking.power_y, tracking.power_a);
}

void Drivebase::driver_practice(){
  master.clear();
  cur_driver = 0; // defaults driver to Nikhil
  master.print(2, 0, "Driver: %s", drivers[cur_driver].name);
  while(true){
    while(!master.get_digital_new_press(E_CONTROLLER_DIGITAL_B)){
      if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){ // goes to next driver
        cur_driver++;
        cur_driver %= num_of_drivers; // rollover
        // spaces in the controller print are to overwrite names
        master.print(2, 0, "Driver: %s          ", drivers[cur_driver].name);
      }
      else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)){  // goes to previous driver
        if (cur_driver == 0)  cur_driver = num_of_drivers - 1;
        else cur_driver--;
        master.print(2, 0, "Driver: %s          ", drivers[cur_driver].name);
      }
      drivebase.handle_input();
      if(front_l.get_temperature() >= 55 || front_r.get_temperature() >= 55 || back_r.get_temperature() >= 55 || back_l.get_temperature() >= 55){
        move(0, 0, 0);  // stops movement
        delay(50);
        master.rumble("- - - ");
        delay(50);
        master.print(0, 0, "fl%.0f r%.0f bl%.0f r%.0f\n", front_l.get_temperature(), front_r.get_temperature(), back_l.get_temperature(), back_r.get_temperature());
        return;
      }
      // prints motor temps every 50 ms
      if(screen_timer.get_time() > 50){
        drivers_data.print("fl%.0f r%.0f bl%.0f r%.0f\n", front_l.get_temperature(), front_r.get_temperature(), back_l.get_temperature(), back_r.get_temperature());
        master.print(0, 0, "fl%.0f r%.0f bl%.0f r%.0f\n", front_l.get_temperature(), front_r.get_temperature(), back_l.get_temperature(), back_r.get_temperature());
        // rumbles controllers if motors are hot
        screen_timer.reset();
      }
      delay(10);
    }
    update_lookup_table_util();
    master.clear();
    master.print(2, 0, "Driver: %s", drivers[cur_driver].name);
  }
}
