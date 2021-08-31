#include "drive.hpp"
// custom drive class methods

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
  if (curve_type == curve_types::polynomial)  curve_function = &custom_drive::polynomial;
  else curve_function = &custom_drive::exponential;
  for (short i = -127; i < 128; i++){
    lookup_table[(unsigned char)i] = (this->*curve_function)(i);
    printf("%d, %d\n", i, lookup_table[(unsigned char)i]);
  }

}
int custom_drive::lookup(int x){
  return lookup_table[(unsigned char)x];
}

custom_drive local_x, local_y, local_a;

double brake_scalar_x = 5.0,  brake_scalar_y = 5.0, brake_scalar_a = 10.0;

void drive_input(){

  tracking.power_x = master.get_analog(JOY_STRAFE);
  tracking.power_y = master.get_analog(JOY_FORWARD);
  tracking.power_a = master.get_analog(JOY_TURN);

  if(fabs(tracking.power_x) < DEADZONE){
    if (fabs(tracking.b_velo) > 0.25){
      tracking.power_x = tracking.b_velo * -1 * brake_scalar_x;
    }
    else  tracking.power_x = 0;
  }

  if(fabs(tracking.power_y) < DEADZONE){
    if (fabs(tracking.l_velo) > 0.25){
      tracking.power_y = tracking.b_velo * -1 * brake_scalar_x;
    }
    else  tracking.power_y = 0;
  }

  if(fabs(tracking.power_a) < DEADZONE){
    if (fabs(tracking.g_velocity.angle) > 0.25){
      tracking.power_a = tracking.g_velocity.angle * -1 * brake_scalar_x;
    }
    else  tracking.power_a = 0;
  }

  move(tracking.power_x, tracking.power_y, tracking.power_a);

}

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

  short cur_screen = 0;

  master.print(1, 0, "Curve type: %s", curve_types[cur_screen] == 'E' ? "Expo": "Poly");
  master.print(2, 0, "Curvature: %lf", curvatures[cur_screen]);  // updates curvature

  // master.print(0, 2, "Press b to save");

  while (!master.get_digital_new_press(E_CONTROLLER_DIGITAL_B)){  // user presses b to save and exit utility
    if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT)){ // move to next screen
      cur_screen++;
      cur_screen %= 3; // to create rollover
      master.print(0, 0, screen_text[cur_screen]);  // updates text
      master.print(1, 0, "Curve type: %s", curve_types[cur_screen] == 'E' ? "Expo": "Poly");
      master.print(2, 0, "Curvature: %lf", curvatures[cur_screen]);  // updates curvature
    }
    if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_LEFT)){  // move to previous screen
      cur_screen--;
      cur_screen %= 3; // to create rollover
      master.print(0, 0, screen_text[cur_screen]);  // updates text
      master.print(1, 0, "Curve type: %s", curve_types[cur_screen] == 'E' ? "Expo": "Poly");
      master.print(2, 0, "Curvature: %lf", curvatures[cur_screen]);  // updates curvature
    }
    if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){  // increase curvature
      if (curvatures[cur_screen] < 10.0)  curvatures[cur_screen] += 0.1;
      master.print(1, 0, "Curvature: %lf", curvatures[cur_screen]);  // updates curvature
    }
    if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)){  // decrease curvature
      if (curvatures[cur_screen] > 0.0)  curvatures[cur_screen] -= 0.1;
      master.print(1, 0, "Curvature: %lf", curvatures[cur_screen]);  // updates curvature
    }
    if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_X)){  // switch curve type curvature
      if (curve_types[cur_screen] == 'P'){
        curve_types[cur_screen] = 'E';  // curve type is now exponential
        master.print(2, 0, "Curve type: Expo", curvatures[cur_screen]);
      }
      else {
        curve_types[cur_screen] = 'P';  // curve type is now polynomial
        master.print(2, 0, "Curve type: Poly", curvatures[cur_screen]);
      }
    }

    delay(10);
  }
  master.clear();
  curve_file = fopen("/usd/curve_file.txt", "w");
  for (short i = 0; i < 3; i++){  // writes curve types and curvature to curve file
    fprintf(curve_file, "curve_type:%c curvature:%lf\n", curve_types[i], curvatures[i]);
  }
  fclose(curve_file);
  delay(150);
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
		fscanf(curve_file, "curve_type:%c curvature:%lf\n", &curve_types[i], &curvatures[i]);
	}
	local_x.update_curve_values(curve_types[0] == 'E' ? curve_types::exponential : curve_types::polynomial, curvatures[0]);
	local_y.update_curve_values(curve_types[1] == 'E' ? curve_types::exponential : curve_types::polynomial, curvatures[1]);
	local_a.update_curve_values(curve_types[2] == 'E' ? curve_types::exponential : curve_types::polynomial, curvatures[2]);

	fclose(curve_file);

}
