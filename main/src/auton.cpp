#include "auton.hpp"
#include "Libraries/printing.hpp"
#include "Subsystems/f_lift.hpp"
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
  // skills_d.print("FIRST GOAL GOt %d", millis() - time);
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

}


void skills4(){
  int time = millis();
  
  f_lift.set_state(f_lift_states::move_to_target,f_top);
  b_lift.set_state(b_lift_states::move_to_target,0);
  while(f_lift.get_target_state() != f_lift_states::idle);



  flatten_against_wall(true);
  f_claw(0);
  b_claw.set_state(0);
  tilt_lock.set_state(0);
   delay(100);
   tracking.reset(141.0 - DIST_BACK,141.0 - reset_dist_l.get_dist(), 90.0);
  // Position reset = distance_reset_right(16);
  // master.print(0,0,"%.2f, %.2f, %.2f\n", reset.x, reset.y,rad_to_deg(reset.angle));
  // tracking.reset(141.0 - reset.y,141.0 - reset.x, -90.0 + rad_to_deg(reset.angle));
  master.print(1,1,"%.0f, %.0f, %.0f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));

  delay(100);
  move_start(move_types::tank_point, tank_point_params({122.0, tracking.y_coord, 90.0}));
  move_start(move_types::turn_angle, turn_angle_params(angle_to_point(101.0, 129.0) +180));
  // // //43 12
  // move_start(move_types::turn_angle, turn_angle_params(angle_to_point(98.0, 129.0) +180));
  move_start(move_types::tank_point, tank_point_params({98.0, 129.0, 125.0}, false,70.0), false);
  Task([](){
    delay(450);
    f_lift.set_state(f_lift_states::move_to_target,f_bottom);
  });
  b_detect_goal();
  intake.set_state(intake_states::on);
  move_stop();
  drivebase.brake();
  // move_start(move_types::tank_arc, tank_arc_params({tracking.x_coord, tracking.y_coord}, {120.5, 102.0,200.0},127.0,127.0,false));//arc to face neut goal
  // move_start(move_types::turn_angle, turn_angle_params(rad_to_deg(atan2(104.5 - tracking.x_coord, 72.0 - tracking.y_coord)) +180.0, true,true,140.0));
  move_start(move_types::tank_point, tank_point_params({106.5,72.0,  45.0},false, 127.0,1.0,true,9.0,130.0));// drive throught neut goal
  
  move_start(move_types::turn_angle, turn_angle_params(-90));
  move_start(move_types::tank_point, tank_point_params({70.5,72.0,  45.0},false, 127.0,1.0,true), false);
  f_detect_goal();
  move_stop();
  drivebase.brake();
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
    f_lift.set_state(f_lift_states::move_to_target, 0);
    b_lift.set_state(b_lift_states::move_to_target, 0);
  });
  
	move_start(move_types::tank_point, tank_point_params({34.5, 72.0, 45.0}, false, 80.0, 1.0, true, 9.0, 130.0), false);// drive throught neut goal
  delay(100);
  f_detect_goal();
  move_stop();
}

//name, target, common point, task at target, angle to go to target
//current is still necessary to look for collisions
std::vector<std::string> selected_positions;
std::map<std::string, std::pair<Point, std::string>> targets = {
  {"Right", {{110.0, 14.0}, "None"}},
  {"Left", {{30.0, 12.0}, "None"}},
  {"Tall", {{73.0, 71.0}, "Front"}},
  {"High", {{107.0, 71.0}, "Front"}},
  {"Low", {{34.5, 72.0}, "Front"}},
  {"AWP", {{130.0, 36.0}, "Back"}},
  {"Ramp", {{128.0, 36.0}, "Back"}},
}; //see if we ever get a hitch

void load_auton(){
  std::string target, task;
  selected_positions.clear();

  ifstream file;
  Data::log_t.data_update();
  printf2(term_colours::BLUE, -1, "\n\nLoading Autons:");
  file.open(auton_file_name, fstream::in);
  while(file >> target >> task){
    printf2(term_colours::BLUE, -1, "%s: %s", target, task);
    selected_positions.push_back(target);
  }
  newline();
  file.close();
  Data::log_t.done_update();
}

void save_auton(){
  ofstream file;
  Data::log_t.data_update();
  printf2("\n\nSaving Autons:");
  file.open(auton_file_name, fstream::out | fstream::trunc);
  for(std::vector<std::string>::const_iterator it = selected_positions.begin(); it != selected_positions.end(); it++){
    file << *it << std::endl;
    file << std::get<std::string>(targets[*it]) << std::endl;
    printf2("%s: %s", *it, std::get<std::string>(targets[*it]));
  }
  newline();
  file.close();
  Data::log_t.done_update();
  master.clear();
  master.print(0, 0, "Saved Autons");
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
  bool selected = false;

  wait_until(selected){
    master.clear_line(1);
    master.print(1, 0, choice);

    switch(master.wait_for_press({DIGITAL_A, DIGITAL_RIGHT, DIGITAL_LEFT, DIGITAL_B})){ //see how to use ok_button
      case DIGITAL_B: return false; break;
      case DIGITAL_A:
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

  return true;
}

void select_auton(){
  bool all_selected = false;
  std::map<std::string, std::pair<Point, std::string>>::const_iterator selection = targets.find("Right");
  selected_positions.clear();

  wait_until(all_selected){
    master.clear();
    master.print(0, 0, selection->first);

    const std::map<std::string, std::pair<Point, std::string>>::const_iterator og = selection;

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
  std::pair<Point, std::string> current, target;
  for(int i = 0; i+1 < selected_positions.size(); i++){
    target = targets[selected_positions[i+1]];

    screen_flash::start(term_colours::BLUE, "Starting move to %s goal", selected_positions[i+1]);

    if(!run_defined_auton(selected_positions[i], selected_positions[i+1])){
      if(target.second == "None"){
        // move_start(move_types::tank_rush, tank_rush_params({target.first, /*figure out angle based on front/back*/}, false));
      }
      else if(target.second == "Front"){
        f_lift.set_state(f_lift_states::move_to_target, 0);
        // move_start(move_types::tank_rush, tank_rush_params({target.first, /*figure out angle based on front/back*/}, false));
        // f_detect_goal();
        f_lift.set_state(f_lift_states::move_to_target, 1);
      }
      else if(target.second == "Back"){
        b_lift.set_state(b_lift_states::move_to_target, 0);
        // move_start(move_types::tank_rush, tank_rush_params({target.first, /*figure out angle based on front/back*/}, true));
        // b_detect_goal();
        b_lift.set_state(b_lift_states::move_to_target, 1);
      }

    }
  }
}
