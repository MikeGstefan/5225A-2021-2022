#include "auton.hpp"
#include "Libraries/printing.hpp"
#include "Tracking.hpp"
#include "config.hpp"
#include "controller.hpp"
#include "geometry.hpp"
#include "logging.hpp"
#include "util.hpp"
#include <map>

static const std::string auton_file_name = "/usd/auton.txt";

void skills(){
  int time = millis();
  f_lift.move_absolute(10); 
  b_lift.move_absolute(10); 
  move_start(move_types::tank_point, tank_point_params({36.0,11.75,-90.0},false),false); // grabs blue on platform 
	b_detect_goal(); 
  skills_d.print("FIRST GOAL GOt %d\n", millis() - time);
	move_stop(); 
	// drivebase.brake(); 
	move_start(move_types::tank_arc, tank_arc_params({tracking.x_coord, tracking.y_coord}, {20.5, 39.0,20.0},127.0,127.0,false));//arc to face neut goal 
  // move_start(move_types::turn_point, turn_point_params({36.5, 72.0})); 
  // delay(50); 
	move_start(move_types::tank_point, tank_point_params({51.0,109.0,  20.0},false, 127.0,1.0,true,6.4,150.0),false);// drive throught neut goal 
  f_detect_goal(); 
  skills_d.print("first neutral got %d\n", millis() - time);
  f_lift.move_absolute(900,200); 
  Task([](){
    while(true){ 
      misc.print("%.2f\n", f_lift.motor.get_actual_velocity());
      delay(10);
    }
  });
  
  flatten_against_wall(false);
   delay(100);
   tracking.reset(141.0 - DIST_BACK,141.0 - reset_dist_r.get_dist(), -90.0);
  // Position reset = distance_reset_right(16);
  // master.print(0,0,"%.2f, %.2f, %.2f\n", reset.x, reset.y,rad_to_deg(reset.angle));
  // tracking.reset(141.0 - reset.y,141.0 - reset.x, -90.0 + rad_to_deg(reset.angle));
  master.print(1,1,"%.0f, %.0f, %.0f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));

  delay(100);

  // move_start(move_types::)
  // while(!master.get_digital_new_press(DIGITAL_A))delay(10);
  // f_claw.set_value(0);
  b_claw.set_value(0);
  // b_lift.move_absolute(10);
  // f_lift.move_absolute(10);
  // delay(2000);
  // flatten_against_wall(false);
  // delay(100);
  // tracking.reset(141.0 - reset_dist_r.get_dist(),141.0 - DIST_BACK, 180.0);
  // // delay(200);
  

  //21, 12.5

  move_start(move_types::tank_point, tank_point_params({tracking.x_coord - 3.0, tracking. y_coord, -90.0}));
  move_start(move_types::turn_point, turn_point_params({120.0,127.0}));
  // move_start(move_types::turn_angle, turn_angle_params(-45.0, true, true, 150.0,0.0,127.0,0, min_move_power_a, 3.0));
  delay(100);
  move_start(move_types::tank_point, tank_point_params({120.0,127.0,-45.0},false,70.0,1.0,true,6.0,150.0,0.0),false);
  Task([](){
    // delay(400);
    b_lift.reset();
  });
  move_wait_for_complete();
  move_start(move_types::turn_angle, turn_angle_params(-90.0, true,true,140.0));
  move_start(move_types::tank_point, tank_point_params({107.0,129.0,-90.0},false,70.0,1.0,true,6.0,150.0,0.0),false);
  
   f_detect_goal();
   f_lift.move_absolute(150);
  move_stop();
  drivebase.brake();
  move_start(move_types::tank_arc, tank_arc_params({tracking.x_coord, tracking.y_coord}, {120.5, 102.0,20.0},-127.0,127.0,false));//arc to face neut goal
  move_start(move_types::turn_angle, turn_angle_params(rad_to_deg(atan2(104.5 - tracking.x_coord, 72.0 - tracking.y_coord)) +180.0, true,true,140.0));
  move_start(move_types::tank_point, tank_point_params({104.5,70.0,  45.0},false, 80.0,1.0,true,9.0,130.0),false);// drive throught neut goal
  b_detect_goal();
  b_lift.move_absolute(620);
  move_stop();
  // while(!master.get_digital(DIGITAL_L1))delay(10);
  // move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(108,70.0,-40.0,30.0,30.0)));
  f_lift.move_absolute(580);
  move_start(move_types::tank_point, tank_point_params({85.0,33.0, 30.0}, false, 70.0));
  // flatten_against_wall(false);

  b_claw.set_value(0);

  int safety_check = 0;
  //bool to + -
  // int direction = (static_cast<int>(front)*2)-1; //use okapi::boolToSign
  tracking_imp.print("%d|| Start wall allign\n", millis());

	drivebase.move(-50.0,0.0);

	while((fabs(tracking.l_velo) < 2.0 ||fabs(tracking.r_velo) < 2.0) && safety_check < 12){
		safety_check++;
    misc.print(" reset things %.2f, %.2f\n",fabs(tracking.l_velo), fabs(tracking.r_velo));
		delay(10);
	}
	cycleCheck(fabs(tracking.l_velo) <1.0 && fabs(tracking.r_velo) < 1.0, 1,10);
	drivebase.move(-20.0,0.0);
	printf("%d|| Done all allign\n", millis());

  move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(tracking.x_coord, tracking.y_coord, 2, rad_to_deg(tracking.global_angle), rad_to_deg(tracking.global_angle))));

  move_start(move_types::turn_angle, turn_angle_params(200.0, true,false, 150.0,0.0,80.0, 1500));
  f_lift.move_absolute(550);
  delay(100);
  // move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(tracking.x_coord, tracking.y_coord, 5, rad_to_deg(tracking.global_angle), rad_to_deg(tracking.global_angle))));
  // move_start(move_types::turn_angle, turn_angle_params(210.0, true,false, 150.0,0.0,80.0));
  // f_claw.set_value(0);
  b_lift.move_absolute(10);
  move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(tracking.x_coord, tracking.y_coord, 2, rad_to_deg(tracking.global_angle), rad_to_deg(tracking.global_angle)), false,127.0,1.0,true,9.0,150.0,0.0,1000));
  move_start(move_types::tank_point, tank_point_params({83.0,50.0, 180.0}));
  move_start(move_types::turn_angle, turn_angle_params(135.0));
  f_lift.move_absolute(10);
  move_start(move_types::tank_point, tank_point_params({71.0,72.0,180.0}), false);
  while(f_lift.motor.get_position() > 60)delay(10);
  // intk.move(0);
  f_detect_goal();
  skills_d.print("Center grab %d\n", millis() - time);
  master.print(2,2,"grab %d", millis() - time);
  delay(100);
  f_lift.move_absolute(550, 200);
  move_wait_for_complete();
  drivebase.brake();
  delay(100);
  move_start(move_types::turn_angle, turn_angle_params(0.0));
  delay(100);
  // intk.move(127);
  move_start(move_types::tank_point, tank_point_params({71.0, 110.0,0.0},false,80.0));
  // move_stop();
  drivebase.brake();
  flatten_against_wall();
  // delay(300);
  // drivebase.move(0.0,0.0);
}


void skillsPark(){
  // f_claw.set_value(0);
  b_claw.set_value(0);
  b_lift.move_absolute(10);
  f_lift.move_absolute(10);
  while(!master.get_digital_new_press(DIGITAL_R1))delay(10);

  // f_claw.set_value(1);
  b_claw.set_value(1);
  f_lift.move_absolute(600);
  // f_lift.move_absolute(200);
  // b_lift.move_absolute(150);
  b_lift.move_absolute(650);
	// while(b_lift.motor.get_position() < 930)delay(10);
  // while(f_lift.motor.get_position() < 650)delay(10);
  while(b_lift.motor.get_position() <630)delay(10);
	// // b_lift.move(10);
	// delay(100);

  flatten_against_wall(false);
  // Led1.set_value(0);
  delay(100);
  double dist = reset_dist_l.get_dist();
  tracking.reset(141.0- dist, DIST_BACK,0.0);
  delay(200);
  master.print(0,0, "%f", dist);
  master.print(1,1,"%.2f, %.2f, %.2f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));
  // while(!master.get_digital_new_press(DIGITAL_R1))delay(10);
  int start = millis();

  //
  // while(!master.get_digital_new_press(DIGITAL_R1))delay(10);



  move_start(move_types::tank_point, tank_point_params({tracking.x_coord, 10.0, 0.0}, false));
  move_start(move_types::turn_angle, turn_angle_params(270.0));
  delay(100);
  move_start(move_types::turn_angle, turn_angle_params(270.0, true, true, 160.0,0.0,127.0,250,35.0,2.0));
  delay(100);
  // move_start(move_types::tank_point, tank_point_params({105.0, 10.0, 270.0}, false));
  drivebase.move(40.0,0.0);
  while(tracking.x_coord  > 107.0)delay(10);
  drivebase.brake();
  b_lift.move_absolute(150);
  f_lift.move_absolute(1);
  while(f_lift.motor.get_position()  > 15)delay(10);
  f_lift.move(-15);
  // return;
  // delay(500);
  // f_lift.move_absolute(800);
	// 		b_lift.move_absolute(150);
	// wait_until(f_lift_m.get_position() > 780);
  // delay(500);
  // f_lift.move_absolute(610);


  // Task([](){
  //   delay(300);
	// 	f_lift.move(70);
  //   wait_until(f_lift_m.get_position() > 700);
  //   f_lift.motor.move_relative(0, 100);
  // });
  drivebase.set_state(1);
  // Led1.set_value(1);
  gyro.climb_ramp();
  drivebase.brake();
  delay(100);
  master.print(2,2,"%d",millis() - start);



}


void rush_high(){
  move_start(move_types::tank_point, tank_point_params({107.0, 57.0, 0.0}));
  delay(100);
  move_start(move_types::turn_point, turn_point_params({107.0, 71.0}));
  delay(100);
  move_start(move_types::tank_point, tank_point_params({107.0, 71.0, 0.0}), false);
  f_lift.reset();
  f_lift.move(-10);
  f_detect_goal();

  // move_wait_for_complete();
  // f_lift.move_absolute(150,100);
  // intk.move(10);
  move_stop();
}

void rush_tall(){
  int start_time = millis();

  move_start(move_types::tank_point, tank_point_params({83.0, 59.0, 30.0}));
  delay(100);
  move_start(move_types::turn_point, turn_point_params({73, 71}));
  delay(100);
  move_start(move_types::tank_rush, tank_rush_params({73.0, 71.0, 30.0}, false), false);
  // delay(200);
  // drivebase.set_state(0);
  f_lift.reset();
  f_lift.move(-10);
  f_detect_goal();
  move_wait_for_complete();
  master.print(2, 2, "Time: %d", millis() - start_time);
}

void rush_low(){
  Task([](){
    f_lift.reset();
    b_lift.reset();
    f_lift.move_absolute(10);
    b_lift.move_absolute(10);
  });
  
	move_start(move_types::tank_point, tank_point_params({34.5, 72.0, 45.0}, false, 80.0, 1.0, true, 9.0, 130.0), false);// drive throught neut goal
  delay(100);
  f_detect_goal();
  move_stop();
}


void blue_highside(){
  // tracking.x_coord = 108.0, tracking.y_coord = 16.0, tracking.global_angle = 0.0_deg;
  


  // move_start(move_types::tank_rush, tank_rush_params({108.0,71.0,0.0}, false),false);
  move_start(move_types::tank_point, tank_point_params({108.0,57.0,0.0}));
  delay(100);
  move_start(move_types::turn_point, turn_point_params({108.0,71.0}));
  delay(100);
  move_start(move_types::tank_point, tank_point_params({108.0,71.0,0.0}),false);
  f_lift.reset();
  f_lift.move(-10);
  f_detect_goal();

  // move_wait_for_complete();
  // f_lift.move_absolute(150,100);
  // intk.move(10);
  move_stop();
  move_start(move_types::tank_point, tank_point_params({108.0,35.0,0.0}, false,127.0,1.0,true,9.0,150.0,0.0,0,0,{2.0,0.5}),false);
  detect_interference();
  misc.print("%d||here\n",millis());
  move_wait_for_complete();
  f_lift.move_absolute(150,100);
  move_start(move_types::turn_angle, turn_angle_params(-90.0));
  drivebase.brake();
  delay(100);
  move_start(move_types::turn_angle, turn_angle_params(-90.0));
  move_start(move_types::tank_point, tank_point_params({128.0,35.0,-90.0}, false,127.0,1.0,true,9.0,150.0,0.0,0,0,{1.0,0.5}),false);
  b_detect_goal();
  move_stop();
  delay(200);
  // intk.move(127);
  delay(2000);
}

void blue_highside_tall(){
  //tracking.x_coord = 104.0, tracking.y_coord = 12.0, tracking.global_angle = -30.0_deg;


  // Task([](){
    // f_lift.reset();
    // f_lift.move(-10);
  // });
  // drivebase.set_state(1);
  int time = millis();
  // move_start(move_types::tank_rush, tank_rush_params({73.0,71.0,30.0},false), false);
  move_start(move_types::tank_point, tank_point_params({83.0,59.0,30.0}));
  delay(100);
  move_start(move_types::turn_point, turn_point_params({73,71}));
  delay(100);
  move_start(move_types::tank_rush, tank_rush_params({73.0,71.0,30.0},false), false);
  // delay(200);
  // drivebase.set_state(0);
  f_lift.reset();
  f_lift.move(-10);
  f_detect_goal();
  move_wait_for_complete();
  master.print(2,2,"time: %d", millis() - time);
  // drivebase.set_state(0);
  move_start(move_types::tank_point, tank_point_params({91.0,35.0,30.0}, false,127.0,1.0,true,9.0,150.0,0.0,0,0,{2.0,0.5}),false);
  detect_interference();
  move_wait_for_complete();

  f_lift.move_absolute(150,100);
  move_start(move_types::turn_angle, turn_angle_params(-90.0));
  
  delay(100);
  move_start(move_types::tank_point, tank_point_params({128.0,35.0,-90.0}, false,100.0,1.0,true,9.0,150.0,0.0,0,0,{2.0,0.5}),false);
  b_detect_goal();
  move_stop();
  motion_i.print("motion interrupted at %.2f %.2f %.2f\n", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
  delay(200);
  // intk.move(127);
  delay(2000);
}

void blue_lowside(){
  Task([](){
    f_lift.reset();
    b_lift.reset();
    f_lift.move_absolute(10);
    b_lift.move_absolute(10);
  });
  
  move_start(move_types::tank_point, tank_point_params({36.0,11.75,-90.0},false),false); // grabs blue on platform
	b_detect_goal();
	move_stop();
  // intk.move(127);
	// drivebase.brake();
	move_start(move_types::tank_arc, tank_arc_params({tracking.x_coord, tracking.y_coord}, {20.5, 39.0,20.0},127.0,127.0,false));//arc to face neut goal
  move_start(move_types::turn_point, turn_point_params({36.5, 72.0}));
  delay(50);
	move_start(move_types::tank_point, tank_point_params({34.5,72.0,  45.0},false, 80.0,1.0,true,9.0,130.0),false);// drive throught neut goal
  delay(100);
  f_detect_goal();
  f_lift.move_absolute(150);
  // intk.move(127);
  move_stop();
  // drivebase.move(0.0,0.0);
  // intk.move(127);
  move_start(move_types::tank_point, tank_point_params({34.0, 20.0,-90.0}), false);
  detect_interference();

  
}

void lrt_auton(){
  Task([](){
    f_lift.reset();
    b_lift.reset();
    f_lift.move_absolute(10);
    b_lift.move_absolute(10);
  });
  
  move_start(move_types::tank_point, tank_point_params({36.0,11.75,-90.0},false),false); // grabs blue on platform
	b_detect_goal();
	move_stop();
	// drivebase.brake();
	move_start(move_types::tank_arc, tank_arc_params({tracking.x_coord, tracking.y_coord}, {20.5, 39.0,20.0},127.0,127.0,false));//arc to face neut goal
  move_start(move_types::turn_point, turn_point_params({36.5, 72.0}));
  delay(50);
	move_start(move_types::tank_point, tank_point_params({34.5,72.0,  45.0},false, 80.0,1.0,true,9.0,130.0),false);// drive throught neut goal
  f_detect_goal();
  f_lift.move_absolute(150);
  // intk.move(127);
  move_stop();
}


//name, target, common point, task at target, angle to go to target
std::vector<std::string> selected_positions;
std::map<std::string, std::tuple<Point, Point, std::string, double>> targets = {
  {"Right", {{108.0, 16.0}, {104.0, 12.0}, "None", 0.0}},
  {"Left", {{30.0, 12.0}, {36.0, 24.0}, "None", 0.0}},
  {"Tall", {{73.0, 71.0}, {72.0, 60.0}, "Front", 30.0}},
  {"High", {{107.0, 71.0}, {107.0, 60.0}, "Front", 0.0}},
  {"Low", {{34.5, 72.0}, {36.0, 60.0}, "Front", 45.0}},
  {"AWP", {{130.0, 36.0}, {125.0, 30.0}, "Back", -90.0}},
  {"Ramp", {{128.0, 36.0}, {30.0, 30.0}, "Back", -90.0}},
};

void load_auton(){
  std::string str;
  selected_positions.clear();

  ifstream file;
  Data::log_t.data_update();
  printf("\n\nLoading Autons:\n");
  file.open(auton_file_name, fstream::in);
  while(file >> str){
    printf2("%s", str);
    selected_positions.push_back(str);
  }
  newline();
  file.close();
  Data::log_t.done_update();
}

void save_auton(){
  ofstream file;
  Data::log_t.data_update();
  printf("\n\nSaving Autons:\n");
  file.open(auton_file_name, fstream::out | fstream::trunc);
  for(std::vector<std::string>::iterator it = selected_positions.begin(); it != selected_positions.end(); it++){
    file << *it << std::endl;
    printf2("%s", *it);
  }
  newline();
  file.close();
  Data::log_t.done_update();
}

void run_auton_task(std::string task){
  if(task == "Front") f_detect_goal();
  if(task == "Back") b_detect_goal();
}

bool run_defined_auton(std::string start, std::string target){
//handles movement and goal pickup
  if(start == "" && target == ""){

  }
  else if(start == "" && target == ""){

  }
  else return false; //Will be false if none of the ifs matched

  return true;
}

void select_auton_task(std::string target){
  std::string choice = std::get<std::string>(targets[target]);
  double angle = std::get<double>(targets[target]);
  bool selected = false;

  selected_positions.push_back(target);

  wait_until(selected){
    master.print(1, 0, choice);

    switch(master.wait_for_press({DIGITAL_A, DIGITAL_RIGHT, DIGITAL_LEFT})){ //see how to use ok_button
      case DIGITAL_A:
        if(choice == "Back" || "Front"){
          master.print(2, 0, "%.1f", angle);
          //angle selection
          master.wait_for_press(DIGITAL_A);
        }

        selected = true;
        break;

      case DIGITAL_RIGHT:
        if(choice == "Front") choice = "Back";
        else if(choice == "Back") choice = "None";
        else if(choice == "None") choice = "Front";
        break;
      
      case DIGITAL_LEFT:
        if(choice == "Front") choice = "None";
        else if(choice == "Back") choice = "Front";
        else if(choice == "None") choice = "Back";
        break;

      default: break;
    }
  }

  std::get<std::string>(targets[target]) = choice;
  std::get<double>(targets[target]) = angle;
}

void select_auton(){
  bool all_selected = false;
  std::map<std::string, std::tuple<Point, Point, std::string, double>>::iterator selection = targets.find("Right");

  wait_until(!master.get_digital(DIGITAL_X)); //Waits to release cancel button

  wait_until(all_selected || master.get_digital(DIGITAL_X)){
    master.clear();
    master.print(0, 0, selection->first);

    std::map<std::string, std::tuple<Point, Point, std::string, double>>::iterator og = selection;

    switch(master.wait_for_press({DIGITAL_X, DIGITAL_A, DIGITAL_RIGHT, DIGITAL_LEFT})){//see how to use ok_button
      case DIGITAL_A:
        select_auton_task(selection->first);

      case DIGITAL_RIGHT:
        do{ //Goes to next available choice
          if(selection != std::prev(targets.end())) selection++;
          else selection = targets.begin();
          if(selection == og) all_selected = true;
        }
        while(!all_selected && contains(selected_positions, selection->first));
        break;
      
      case DIGITAL_LEFT:
        do{ //Goes to previous available choice
          if(selection != targets.begin()) selection--;
          else selection = std::prev(targets.end());
          if(selection == og) all_selected = true;
        }
        while(!all_selected && contains(selected_positions, selection->first));
        break;

      default: break; //breaks out of switch, and if pressed x, then immediately breaks out of loop 
    }

  }
  master.clear();
  save_auton();
}

void run_auton(){
  std::tuple<Point, Point, std::string, double> current, target;
  double angle;
  for(int i = 0; i+1 < selected_positions.size(); i++){
    current = targets[selected_positions[i]];
    target = targets[selected_positions[i+1]];
    angle = std::get<3>(target);

    if(!run_defined_auton(selected_positions[i], selected_positions[i+1])){
      move_start(move_types::tank_point, tank_point_params({std::get<1>(current), weighted_avg(angle, tracking.get_angle_in_deg(), 0.3)}, false, 127.0, 1.0, false));
      move_start(move_types::tank_point, tank_point_params({std::get<1>(target), weighted_avg(angle, tracking.get_angle_in_deg(), 0.7)}, false, 127.0, 1.0, false));
      move_start(move_types::tank_rush, tank_rush_params({std::get<0>(target), angle}, false));

      run_auton_task(std::get<std::string>(target));
    }

    master.wait_for_press(DIGITAL_R1); //Just to wait between routes to see
  }
}
