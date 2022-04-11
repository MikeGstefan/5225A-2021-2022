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
  // f_lift.move_absolute(10); 
  // b_lift.move_absolute(10); 
  b_lift.set_state(b_lift_states::move_to_target,0);
  f_lift.set_state(f_lift_states::move_to_target,0);
  // move_start(move_types::tank_point, tank_point_params({36.0,11.75,-90.0},false),false); // grabs blue on platform 
	// b_detect_goal(); 
  // skills_d.print("FIRST GOAL GOt %d\n", millis() - time);
	// move_stop(); 
  // drivebase.move(0.0,0.0);
  move_start(move_types::tank_arc, tank_arc_params({tracking.x_coord, tracking.y_coord},{26.0, 41.0, 50.0}, 127.0,127.0,false));

  // move_start(move_types::tank_point, tank_point_params({35.0,41.0,90.0}));
  // move_start(move_types::tank_point, tank_point_params({80.0,45.0,90.0}));
  // move_start(move_types::turn_angle, turn_angle_params(-70.0));
  // b_lift.set_state(b_lift_states::move_to_target,b_lift_hitch_pos);
  // move_start(move_types::tank_point, tank_point_params({120.0,36.0,-70.0}));
  move_start(move_types::tank_point, tank_point_params({120.0,36.0,90.0}));


}

void skills2(){
  int time = millis();
  
  f_lift.set_state(f_lift_states::move_to_target,f_level);
  while(f_lift.get_target_state() != f_lift_states::idle);
  flatten_against_wall(true);
  f_claw(0);
   delay(100);
   tracking.reset(141.0 - DIST_BACK,141.0 - reset_dist_l.get_dist(), 90.0);
  // Position reset = distance_reset_right(16);
  // master.print(0,0,"%.2f, %.2f, %.2f\n", reset.x, reset.y,rad_to_deg(reset.angle));
  // tracking.reset(141.0 - reset.y,141.0 - reset.x, -90.0 + rad_to_deg(reset.angle));
  master.print(1,1,"%.0f, %.0f, %.0f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));

  delay(100);
  move_start(move_types::tank_point, tank_point_params({122.0, tracking.y_coord, 90.0}));
  // //43 12
  move_start(move_types::turn_angle, turn_angle_params(angle_to_point(98.0, 129.0) +180));
  move_start(move_types::tank_point, tank_point_params({98.0, 129.0, 125.0}, false,70.0), false);
  Task([](){
    delay(450);
    f_lift.set_state(f_lift_states::move_to_target,f_bottom);
  });
  b_detect_goal();
  move_stop();
  drivebase.brake();
  // move_start(move_types::tank_arc, tank_arc_params({tracking.x_coord, tracking.y_coord}, {120.5, 102.0,200.0},127.0,127.0,false));//arc to face neut goal
  // move_start(move_types::turn_angle, turn_angle_params(rad_to_deg(atan2(104.5 - tracking.x_coord, 72.0 - tracking.y_coord)) +180.0, true,true,140.0));
  move_start(move_types::tank_point, tank_point_params({104.5,70.0,  45.0},false, 127.0,1.0,true,9.0,130.0),false);// drive throught neut goal
  f_detect_goal();
  move_stop();
  Task([](){
    delay(150);
    f_lift.set_state(f_lift_states::move_to_target, f_top);
  });
  
  // drivebase.brake();
  move_start(move_types::tank_point, tank_point_params({78.0, 25.0, 200.0}, false,60.0, 1.0, true, 6.4, 70.0,0.0,4000));
  f_lift.set_state(f_lift_states::move_to_target, f_plat);
  while(f_lift.get_target_state() != f_lift_states::idle)delay(10);
  
  while(!master.get_digital_new_press(DIGITAL_A))delay(10);


  f_claw(0);
  move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(tracking.x_coord, tracking.y_coord, -1.5, rad_to_deg(tracking.global_angle), rad_to_deg(tracking.global_angle)), false,60.0));
  return;
  // move_start(move_types::turn_point, turn_point_params({120.0,127.0}));
  // move_start(move_types::turn_angle, turn_angle_params(-45.0, true, true, 150.0,0.0,127.0,0, min_move_power_a, 3.0));
  delay(100);
  move_start(move_types::tank_point, tank_point_params({120.0,127.0,-45.0},false,70.0,1.0,true,6.0,150.0,0.0),false);
  // Task([](){
  //   // delay(400);
  //   b_lift.reset();
  // });
  b_lift.set_state(b_lift_states::move_to_target, 0);
  move_wait_for_complete();
  move_start(move_types::turn_angle, turn_angle_params(-90.0, true,true,140.0));
  move_start(move_types::tank_point, tank_point_params({107.0,129.0,-90.0},false,70.0,1.0,true,6.0,150.0,0.0),false);
  
   f_detect_goal();
  //  f_lift.move_absolute(150);
  f_lift.set_state(f_lift_states::move_to_target, f_carry);
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

b_claw.set_state(0);

  int safety_check = 0;
  //bool to + -
  // int direction = (static_cast<int>(front)*2)-1;
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
  f_claw(0);
  b_lift.move_absolute(10);
  move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(tracking.x_coord, tracking.y_coord, 2, rad_to_deg(tracking.global_angle), rad_to_deg(tracking.global_angle)), false,127.0,1.0,true,9.0,150.0,0.0,1000));
  move_start(move_types::tank_point, tank_point_params({83.0,50.0, 180.0}));
  move_start(move_types::turn_angle, turn_angle_params(135.0));
  f_lift.move_absolute(10);
  delay(100);
  move_start(move_types::tank_point, tank_point_params({71.0,71.0, 135.0}), false);
  b_detect_goal();
  // b_lift.move_absolute(750);
  move_stop();
  // move_start(move_types::tank_arc, tank_arc_params({71.0, 71.0}, {100.0, 36.0, 90.0}, 127.0,127.0,false,9.0,2.0,80.0));
  move_start(move_types::tank_point, tank_point_params({100.0, 36.0, 90.0}));
  move_start(move_types::turn_angle, turn_angle_params(90.0));
  b_claw.set_state(0);
  move_start(move_types::tank_point, tank_point_params({129.0, 36.0, 90.0}, false,70.0), false);

  // move_start(move_types::tank_point, tank_point_params({129.0, 36.0, 135.0}, false,80.0),false);
  f_detect_goal();
  move_stop();
  move_start(move_types::tank_point, tank_point_params({115.0, 36.0, 90.0}),false);
  delay(200);
  f_lift.move_absolute(620);
  while(f_lift.motor.get_position() < 580)delay(10);
  move_stop();
  drivebase.move(70.0,0.0);
  tracking.wait_for_dist(2.0);
  flatten_against_wall();
  delay(100);
   tracking.reset(141.0 - DIST_FRONT,reset_dist_r.get_dist(), 90.0);
  // Position reset = distance_reset_right(16);
  // master.print(0,0,"%.2f, %.2f, %.2f\n", reset.x, reset.y,rad_to_deg(reset.angle));
  // tracking.reset(141.0 - reset.y,141.0 - reset.x, -90.0 + rad_to_deg(reset.angle));
  master.print(1,1,"%.0f, %.0f, %.0f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));

  delay(100);
  b_lift.move_absolute(130);

  move_start(move_types::tank_point, tank_point_params({16.0,122.25,135.0}, false, 127.0, 1.0,true));
  move_start(move_types::tank_point, tank_point_params({18.0,120.25,135.0}, false, 127.0, 1.0,true, 6.5,150.0,0.0,1000,{1.5,1.0}));
  // move_start(move_types::tank_point, tank_point_params({108.0, 36.0, 90.0}));
  move_start(move_types::turn_angle, turn_angle_params(80.0, true, true,170.0,0.0,127.0,1000,min_move_power_a,3.0));
  delay(100);
  move_start(move_types::turn_angle, turn_angle_params(80.0, true, true,140.0,0.0,127.0,400,min_move_power_a,3.0));
  b_lift.move_absolute(200);
  drivebase.set_state(1);
  drivebase.move(70.0,0.0);
  f_lift.move_absolute(470);

  tracking.wait_for_dist(6.0);
  // b_lift.move_absolute(700);
  // move_start(move_types::tank_point, tank_point_params({108.0, 15.0, 0.0}));
  flatten_against_wall(true, 2);
  f_claw(0);
  
  
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
