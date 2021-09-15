#include "drive.hpp"
// custom drive class methods


// Emily  & Sarah: forward and strafing on left, and right is turning

// #define JOY_FORWARD E_CONTROLLER_ANALOG_LEFT_Y
// #define JOY_STRAFE E_CONTROLLER_ANALOG_LEFT_X
// #define JOY_TURN E_CONTROLLER_ANALOG_RIGHT_X
// #define JOY_LIFT E_CONTROLLER_ANALOG_RIGHT_Y

//Nikhil's controls

#define JOY_FORWARD E_CONTROLLER_ANALOG_RIGHT_Y
#define JOY_STRAFE E_CONTROLLER_ANALOG_LEFT_X
#define JOY_TURN E_CONTROLLER_ANALOG_RIGHT_X
#define JOY_LIFT E_CONTROLLER_ANALOG_LEFT_Y
#define DEADZONE 7


// private methods
int custom_drive::polynomial(int x){
  double n = curvature * 0.2 + 1; // scales curvature value to match expo function
  return round(pow(127, 1 - n) * pow(abs(x), n) * sgn(x));
}
int custom_drive::exponential(int x){
  double n = curvature;
  return round(exp(0.002 * n * (abs(x) - 127)) * x);
}

// public methods
void custom_drive::update_curve_values(curve_types curve_type, double curvature){
    this-> curve_type = curve_type;
    this-> curvature = curvature;
}
void custom_drive::fill_lookup_table(){
  int(custom_drive::*curve_function)(int x);  // function pointer to store curve function
  curve_function = curve_type == curve_types::polynomial ? &custom_drive::polynomial : &custom_drive::exponential;
  for (short i = -127; i < 128; i++){ // fills lookup table with values from appropriate function
    lookup_table[(unsigned char)i] = (this->*curve_function)(i);
    printf("%d, %d\n", i, lookup_table[(unsigned char)i]);
  }
}
int custom_drive::lookup(int x){
  return lookup_table[(unsigned char)x];
}

custom_drive custom_drives[3];  // 0 is local_x, 1 is local_y, 2 is local_a

double brake_scalar_x = 5.0,  brake_scalar_y = 5.0, brake_scalar_a = 10.0;

void drive_input(){

  tracking.power_x = custom_drives[0].lookup(master.get_analog(JOY_STRAFE));
  tracking.power_y = custom_drives[1].lookup(master.get_analog(JOY_FORWARD));
  tracking.power_a = custom_drives[2].lookup(master.get_analog(JOY_TURN));

  if(fabs(tracking.power_x) < DEADZONE){
    // no brakes cuz Mikey doesn't like braking since they burn motors faster :(
    // if (fabs(tracking.b_velo) > 0.25){
    //   tracking.power_x = tracking.b_velo * -1 * brake_scalar_x;
    // }
    // else  tracking.power_x = 0.0;
    tracking.power_x = 0.0;
  }

  if(fabs(tracking.power_y) < DEADZONE){
    // no brakes cuz Mikey doesn't like braking since they burn motors faster :(
    // if (fabs(tracking.l_velo) > 0.25){
    //   tracking.power_y = tracking.b_velo * -1 * brake_scalar_x;
    // }
    // else  tracking.power_y = 0.0;
    tracking.power_y = 0.0;
  }

  if(fabs(tracking.power_a) < DEADZONE){
    // no brakes cuz Mikey doesn't like braking since they burn motors faster :(
    // if (fabs(tracking.g_velocity.angle) > 0.25){
    //   tracking.power_a = tracking.g_velocity.angle * -1 * brake_scalar_x;
    // }
    // else  tracking.power_a = 0.0;
    tracking.power_a = 0.0;
  }
  move(tracking.power_x, tracking.power_y, tracking.power_a);
  // printf("input: %d, output:%f, \n",master.get_analog(JOY_TURN), tracking.power_a);
}

// curve file template
/*
curve_type:P curvature:3.0
curve_type:E curvature:2.7
curve_type:P curvature:2.8
*/

void update_lookup_table_util(){
  const char* screen_text[3] = {"LOCAL_X CURVE:", "LOCAL_Y CURVE:", "LOCAL_A CURVE:"};
  double curvatures[3];
  char curve_types[3];

  FILE* curve_file = nullptr;
  curve_file = fopen("/usd/curve_file.txt", "r");
  if(curve_file == NULL) {
    printf("curve_file not found\n");
    return;
  }

  for (short i = 0; i < 3; i++){  // reads curve types and curvature from curve file
    fscanf(curve_file, "curve_type:%c curvature:%lf\n", &curve_types[i], &curvatures[i]);
  }
  fclose(curve_file);
  for (short i = 0; i < 3; i++){  // writes curve types and curvature to curve file
    printf("curve_type:%c curvature:%lf\n", curve_types[i], curvatures[i]);
  }

  short cur_screen = 0;
  master.clear();
  delay(50);
  master.print(0, 0, screen_text[cur_screen]);  // updates text
  delay(50);
  master.print(1, 0, "Curve type: %s", curve_types[cur_screen] == 'E' ? "Expo": "Poly");
  delay(50);
  master.print(2, 0, "Curvature: %lf", curvatures[cur_screen]);  // updates curvature
  delay(50);

  // master.print(0, 2, "Press b to save");
  Timer screen_timeout{"screen_timeout"};

  while (!master.get_digital_new_press(E_CONTROLLER_DIGITAL_B)){  // user presses b to save and exit utility
    if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT)){ // move to next screen
      printf("pressed left\n");
      cur_screen++;
      cur_screen %= 3; // to create rollover
      while(screen_timeout.get_time() < 50) delay(10);
      screen_timeout.reset();
      master.print(0, 0, screen_text[cur_screen]);  // updates text
      delay(50);
      master.print(1, 0, "Curve type: %s", curve_types[cur_screen] == 'E' ? "Expo": "Poly");
      delay(50);
      master.print(2, 0, "Curvature: %lf", curvatures[cur_screen]);  // updates curvature
      delay(50);
    }
    if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_LEFT)){  // move to previous screen
      // rollover but without mod because c++ handles negative mods differently :(
      printf("pressed left\n");
      if (cur_screen == 0)  cur_screen = 2;
      else cur_screen--;
      while(screen_timeout.get_time() < 50) delay(10);
      screen_timeout.reset();
      master.print(0, 0, screen_text[cur_screen]);  // updates text
      delay(50);
      master.print(1, 0, "Curve type: %s", curve_types[cur_screen] == 'E' ? "Expo": "Poly");
      delay(50);
      master.print(2, 0, "Curvature: %lf", curvatures[cur_screen]);  // updates curvature
      delay(50);
    }
    if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_X)){  // switch curve type curvature
      printf("pressed x\n");
      if (curve_types[cur_screen] == 'P'){
        curve_types[cur_screen] = 'E';  // curve type is now exponential
        while(screen_timeout.get_time() < 50) delay(10);
        screen_timeout.reset();
        master.print(1, 0, "Curve type: Expo", curvatures[cur_screen]);
      }
      else {
        curve_types[cur_screen] = 'P';  // curve type is now polynomial
        while(screen_timeout.get_time() < 50) delay(10);
        screen_timeout.reset();
        master.print(1, 0, "Curve type: Poly", curvatures[cur_screen]);
      }
    }
    if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){  // increase curvature
      printf("pressed up\n");
      if (curvatures[cur_screen] < 10.0)  curvatures[cur_screen] += 0.1;
      while(screen_timeout.get_time() < 50) delay(10);
      screen_timeout.reset();
      master.print(2, 0, "Curvature: %lf", curvatures[cur_screen]);  // updates curvature
    }
    if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)){  // decrease curvature
      printf("pressed down\n");
      if (curvatures[cur_screen] > 0.0)  curvatures[cur_screen] -= 0.1;
      while(screen_timeout.get_time() < 50) delay(10);
      screen_timeout.reset();
      master.print(2, 0, "Curvature: %lf", curvatures[cur_screen]);  // updates curvature
    }
    delay(10);
  }
  master.clear();
  curve_file = fopen("/usd/curve_file.txt", "w");
  for (short i = 0; i < 3; i++){  // writes curve types and curvature to curve file
    fprintf(curve_file, "curve_type:%c curvature:%lf\n", curve_types[i], curvatures[i]);
  }
  fclose(curve_file);
  while(screen_timeout.get_time() < 50) delay(10);
  screen_timeout.reset();
  master.print(0, 0, "Saved.");
}

void custom_drive::custom_curve_init(){
  double curvatures[3];
	char curve_types[3];

	FILE* curve_file = nullptr;
	curve_file = fopen("/usd/curve_file.txt", "r");
	if(curve_file == NULL) {
		printf("WARNING: curve_file not found\n");
    return;
	}

	for (short i = 0; i < 3; i++){
		fscanf(curve_file, "curve_type:%c curvature:%lf\n", &curve_types[i], &curvatures[i]); // saves curve data to appropriate array
  	custom_drives[i].update_curve_values(curve_types[i] == 'E' ? curve_types::exponential : curve_types::polynomial, curvatures[i]);
    custom_drives[i].fill_lookup_table();
  }

	fclose(curve_file);
}
