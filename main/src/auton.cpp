#include "auton.hpp"
#include "auton_util.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "drive.hpp"
#include "distance.hpp"
#include "Subsystems/b_lift.hpp"
#include "Subsystems/f_lift.hpp"
#include "tracking.hpp"
#include "Libraries/gui.hpp"
#include "Subsystems/hitch.hpp"

static const std::string auton_file_name = "/usd/auton.txt";

//target, task at target
std::vector<std::pair<std::string, std::string>> selected_positions;

std::map<const std::string, const Position> start_positions {
  {"Right", {110.0, 15.0}},
  {"Left", {30.0, 15.0}},
};

std::map<const std::string, const Position> goal_positions {
  {"High", {102.0, 72.0}},
  {"Tall", {72.0, 72.0}},
  {"Low", {34.5, 72.0}},
  {"AWP", {130.0, 36.0}},
  {"Ramp", {128.0, 36.0}},
};

std::map<const std::string, const Position> end_positions {
  {"Right", {110.0, 15.0}},
  {"Left", {30.0, 15.0}},
  {"Far Ramp", {110.0, 90.0}},
  {"Far AWP", {15.0, 110.0}},
};

void load_auton(){
  std::string target, task;
  selected_positions.clear();

  std::ifstream file;
  Data::log_t.data_update();
  printf2(term_colours::BLUE, "\n\nLoading Autons:");
  file.open(auton_file_name, std::fstream::in);
  while(file >> target >> task){
    printf2(term_colours::BLUE, "%s: %s", target, task);
    selected_positions.push_back({target, task});
  }
  newline();
  file.close();
  Data::log_t.done_update();
}

void save_auton(){
  std::ofstream file;
  Data::log_t.data_update();
  printf2("\n\nSaving Autons:");
  file.open(auton_file_name, std::fstream::out | std::fstream::trunc);
  for(std::vector<std::pair<std::string, std::string>>::const_iterator it = selected_positions.begin(); it != selected_positions.end(); it++){
    file << it->first << std::endl;
    file << it->second << std::endl;
    printf2("%s: %s", it->first, it->second);
    //Technically should be able to print as printf2("%s", *it);
  }
  newline();
  file.close();
  Data::log_t.done_update();
  master.clear();
  master.print(0, 0, "Saved Autons");
}

bool goal_already_selected(const std::string& goal){
  for (std::vector<std::pair<std::string, std::string>>::const_iterator it = selected_positions.begin(); it != selected_positions.end(); it++){
    if(it->first == goal) return true;
  }

  return false;
}

bool select_auton_task(std::string target){
  std::string choice = "Front";
  bool selected = false;

  while(true){
    master.clear_line(1);
    master.print(1, 0, "%s", choice);

    switch(master.wait_for_press({DIGITAL_A, DIGITAL_RIGHT, DIGITAL_LEFT, DIGITAL_B})){ //see how to use ok_button
      case DIGITAL_A:
        selected_positions.push_back({target, choice});
        return true;
        break;
      case DIGITAL_RIGHT:
        if(choice == "Front") choice = "Back";
        else if(choice == "Back") choice = "Hitch";
        else if(choice == "Hitch") choice = "Front";
        break;
      
      case DIGITAL_LEFT:
        if(choice == "Front") choice = "Hitch";
        else if(choice == "Hitch") choice = "Back";
        else if(choice == "Back") choice = "Front";
        break;
      case DIGITAL_B:
        return false;
        break;
      default: break;
    }
  }
}

void select_auton(){
  selected_positions.clear();
  master.clear();

  std::map<const std::string, const Position>::const_iterator start_selection = start_positions.find("Right");
  std::map<const std::string, const Position>::const_iterator goal_selection = goal_positions.find("High");
  std::map<const std::string, const Position>::const_iterator end_selection = end_positions.find("Right");

  
  bool done_start_selection = false;
  bool done_goal_selection = false;
  bool done_end_selection = false;

  master.clear_line(0);
  master.print(0, 0, "Start Pos");
  master.clear_line(1);
  master.print(1, 0, "%s", start_selection->first);
  while(!done_start_selection){
    switch(master.wait_for_press({DIGITAL_A, DIGITAL_RIGHT, DIGITAL_LEFT})){
      case DIGITAL_A:
        selected_positions.push_back({start_selection->first, "None"});
        done_start_selection = true;
        break;

      case DIGITAL_RIGHT:
        if(start_selection != std::prev(start_positions.end())) start_selection++;
        else start_selection = start_positions.begin();
        break;
      
      case DIGITAL_LEFT:
        if(start_selection != start_positions.begin()) start_selection--;
        else start_selection = std::prev(start_positions.end());
        break;

      default: break;
    }
  }

  int goal_count = 0;
  master.clear_line(0);
  master.print(0, 0, "Goal %d", goal_count+1);

  while(!done_goal_selection && goal_count < goal_positions.size()){
    master.clear_line(1);
    master.print(1, 0, "%s", goal_selection->first);

    switch(master.wait_for_press({DIGITAL_X, DIGITAL_A, DIGITAL_RIGHT, DIGITAL_LEFT})){
      case DIGITAL_X:
        done_goal_selection = true;
        break;

      case DIGITAL_A:
        if(!select_auton_task(goal_selection->first)) break;
        goal_count++;
        //fallthrough intentional if true

      case DIGITAL_RIGHT:
        do{
          if(goal_selection != std::prev(goal_positions.end())) goal_selection++;
          else goal_selection = goal_positions.begin();
        }
        while(!goal_already_selected(goal_selection->first));
        break;
      
      case DIGITAL_LEFT:
        do{
          if(goal_selection != goal_positions.begin()) goal_selection--;
          else goal_selection = std::prev(goal_positions.end());
        }
        while(!goal_already_selected(goal_selection->first));
        break;

      default: break;
    }
  }

  master.clear_line(0);
  master.print(0, 0, "End Pos");
  master.clear_line(1);
  master.print(1, 0, "%s", end_selection->first);

  while(!done_end_selection){
    switch(master.wait_for_press({DIGITAL_A, DIGITAL_RIGHT, DIGITAL_LEFT})){
      case DIGITAL_A:
        selected_positions.push_back({end_selection->first, "None"});
        done_end_selection = true;
        break;

      case DIGITAL_RIGHT:
        if(end_selection != std::prev(end_positions.end())) end_selection++;
        else end_selection = end_positions.begin();
        break;
      
      case DIGITAL_LEFT:
        if(end_selection != end_positions.begin()) end_selection--;
        else end_selection = std::prev(end_positions.end());
        break;

      default: break;
    }
  }

}

bool run_defined_auton(std::string start, std::string target){
  // if(start == "Right" && target == "High") high_short();
  // else if(start == "Right" && target == "Tall") high_tall();
  // else if(start == "High" && target == "AWP") high_wp_goal();
  // else return false; //Will be false if none of the ifs matched
  // return true; // Jumps here after a movement was run


  return false;
}

void run_auton(){
  std::string cur_task, target_task;
  Position cur_pos, target_pos;
  for(int i = 0; i+1 < selected_positions.size(); i++){
    cur_task = selected_positions[i].second;
    target_task = selected_positions[i+1].second;
    
    cur_pos = goal_positions[selected_positions[i].first];
    target_pos = goal_positions[selected_positions[i+1].first];

    screen_flash::start(term_colours::BLUE, "Starting move to %s goal", selected_positions[i+1].first);

    if(run_defined_auton(selected_positions[i].first, selected_positions[i+1].first)){
      misc.print("Ran Predefined Auton Route from %s to %s", selected_positions[i].first, selected_positions[i+1].first);
    }
    else{
      if(target_task == "None"){
        b_lift.Subsystem::set_state(b_lift_states::intake_on);
        move_start(move_types::tank_point, tank_point_params(target_pos, false, 127.0, 1.0, true, 6.4, 70.0, 0.0, 0, {3, 3}));

        b_lift.Subsystem::set_state(b_lift_states::intake_off);
      }

      else if(target_task == "Front"){
        f_lift.set_state(f_lift_states::move_to_target, 0);
        wait_until(f_lift.at_bottom());
        b_lift.Subsystem::set_state(b_lift_states::intake_on);

        Task([](){detect_interference();});
        move_start(move_types::tank_rush, tank_rush_params(target_pos, false, 127.0, 1.0, false));
        drivebase.random_turn();

        b_lift.Subsystem::set_state(b_lift_states::intake_off);
        f_lift.set_state(f_lift_states::move_to_target, 1);
      }

      else if(target_task == "Back"){
        b_lift.set_state(b_lift_states::move_to_target, 0);
        wait_until(b_lift.at_bottom());

        Task([](){detect_interference();});
        move_start(move_types::tank_point, tank_point_params(target_pos, false, 127.0, 1.0, true, 6.4, 70.0, 0.0, 0, {5, 5}));
        move_start(move_types::turn_angle, turn_angle_params(point_to_angle(target_pos)+180.0, true, true, 5.0, 0.0, 10.0, 20.0, 127.0, 0, 50, 3.0));
        move_start(move_types::tank_point, tank_point_params(target_pos), false);
        
        b_claw_obj.set_state(b_claw_states::searching);
        wait_until(tracking.move_complete || b_claw_obj.get_state() == b_claw_states::tilted);
        drivebase.random_turn();

        b_lift.set_state(b_lift_states::move_to_target, 1);
      }

      else if(target_task == "Hitch"){
        b_lift.set_state(b_lift_states::move_to_target, 4);

        Task([](){detect_interference();});
        move_start(move_types::tank_point, tank_point_params(target_pos, false, 127.0, 1.0, true, 6.4, 70.0, 0.0, 0, {5, 5}));
        move_start(move_types::turn_angle, turn_angle_params(point_to_angle(target_pos)+180.0, true, true, 5.0, 0.0, 10.0, 20.0, 127.0, 0, 50, 3.0));
        move_start(move_types::tank_point, tank_point_params(target_pos), false);
        
        hitch_obj.set_state(hitch_states::searching);
        wait_until(tracking.move_complete || hitch_obj.get_state() == hitch_states::grabbed);
        drivebase.random_turn();
      }

      else ERROR.print("Wrong target selected");
    }

    delay(1000);

  }
}
