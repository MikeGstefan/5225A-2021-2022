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
