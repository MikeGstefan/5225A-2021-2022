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

  move_start(move_types::tank_point, tank_point_params({35.0,41.0,90.0}));
  // move_start(move_types::tank_point, tank_point_params({80.0,45.0,90.0}));
  // move_start(move_types::turn_angle, turn_angle_params(-70.0));
  // b_lift.set_state(b_lift_states::move_to_target,b_lift_hitch_pos);
  // move_start(move_types::tank_point, tank_point_params({120.0,36.0,-70.0}));

}

void skills2(){
  int time = millis();
  b_lift.set_state(b_lift_states::move_to_target,0);
  f_lift.set_state(f_lift_states::move_to_target,0);
  f_claw(0);
  while(!master.get_digital_new_press(DIGITAL_A))delay(10);
  f_claw(1);
  f_lift.set_state(f_lift_states::move_to_target,f_level);
  while(f_lift.get_target_state() != f_lift_states::idle)delay(10);
  move_start(move_types::tank_point, tank_point_params({0.0,21.0,0.0}));
  f_lift.set_state(f_lift_states::move_to_target, f_plat);
  while(f_lift.get_target_state() != f_lift_states::idle)delay(10);
  f_claw(0);
  Position reset = distance_reset_center();
  tracking.reset(141.0- reset.x, 141.0 - reset.y, reset.angle);
  while(!master.get_digital_new_press(DIGITAL_A))delay(10);
  move_start(move_types::tank_point, tank_point_params({71.0,109.0,0.0}));
  f_lift.set_state(f_lift_states::move_to_target, f_backup);
  move_start(move_types::tank_arc, tank_arc_params({71.0, 109.0}, {85.0, 111.0, -180.0,},-127.0, 127.0,true,7.0,0.0,40.0));
  
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
  std::string target, task;
  double angle;
  selected_positions.clear();

  ifstream file;
  Data::log_t.data_update();
  printf("\n\nLoading Autons:\n");
  file.open(auton_file_name, fstream::in);
  while(file >> target >> task >> angle){
    printf2("%s: %s at %.1f", target, task, angle);
    selected_positions.push_back(target);
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
    file << std::get<std::string>(targets[*it]) << std::endl;
    file << std::get<double>(targets[*it]) << std::endl;
    printf2("%s: %s at %.1f", *it, std::get<std::string>(targets[*it]), std::get<double>(targets[*it]));
  }
  newline();
  file.close();
  Data::log_t.done_update();
  master.clear();
  master.print(0, 0, "Saved Autons");
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

bool select_auton_task(std::string target){
  std::string choice = std::get<std::string>(targets[target]);
  double angle = std::get<double>(targets[target]);
  bool selected = false;

  wait_until(selected){
    master.clear_line(1);
    master.print(1, 0, choice);

    switch(master.wait_for_press({DIGITAL_A, DIGITAL_RIGHT, DIGITAL_LEFT, DIGITAL_B})){ //see how to use ok_button
      case DIGITAL_B: return false; break;
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

  selected_positions.push_back(target);
  std::get<std::string>(targets[target]) = choice;
  std::get<double>(targets[target]) = angle;

  return true;
}

void select_auton(){
  bool all_selected = false;
  std::map<std::string, std::tuple<Point, Point, std::string, double>>::iterator selection = targets.find("Right");
  selected_positions.clear();

  wait_until(all_selected){
    master.clear();
    master.print(0, 0, selection->first);

    std::map<std::string, std::tuple<Point, Point, std::string, double>>::iterator og = selection;

    switch(master.wait_for_press({DIGITAL_X, DIGITAL_A, DIGITAL_RIGHT, DIGITAL_LEFT})){//see how to use ok_button
      case DIGITAL_X:
        master.clear();
        save_auton();
        return;
        break;
      case DIGITAL_A:
        if(!select_auton_task(selection->first)) break;

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

      default: break;
    }

  }
}

void run_auton(){
  std::tuple<Point, Point, std::string, double> current, target;
  double angle;
  for(int i = 0; i+1 < selected_positions.size(); i++){
    current = targets[selected_positions[i]];
    target = targets[selected_positions[i+1]];
    angle = std::get<double>(target);

    if(!run_defined_auton(selected_positions[i], selected_positions[i+1])){
      move_start(move_types::tank_point, tank_point_params({std::get<1>(current), weighted_avg(angle, tracking.get_angle_in_deg(), 0.3)}, false, 127.0, 1.0, false));
      move_start(move_types::tank_point, tank_point_params({std::get<1>(target), weighted_avg(angle, tracking.get_angle_in_deg(), 0.7)}, false, 127.0, 1.0, false));
      move_start(move_types::tank_rush, tank_rush_params({std::get<0>(target), angle}, false));

      run_auton_task(std::get<std::string>(target));
    }

    master.wait_for_press(DIGITAL_R1); //Just to wait between routes to see
  }
}
