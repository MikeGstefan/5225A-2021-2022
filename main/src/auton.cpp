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
  f_claw(0);
  b_claw.set_state(0);
  tilt_lock.set_state(0);
  
  f_lift.set_state(f_lift_states::move_to_target,0);
  f_claw(0);
   move_start(move_types::tank_point, tank_point_params({36.0,11.75,-90.0},false),false); // grabs blue on platform 
	b_detect_goal(); 
  skills_d.print("FIRST GOAL GOt %d\n", millis() - time);
	move_stop(); 
	// drivebase.brake(); 
	move_start(move_types::tank_arc, tank_arc_params({tracking.x_coord, tracking.y_coord}, {20.5, 39.0,10.0},127.0,127.0,false));//arc to face neut goal 
  // move_start(move_types::turn_point, turn_point_params({36.5, 72.0})); 
  // delay(50); 
	move_start(move_types::tank_point, tank_point_params({47.0,109.0,  20.0},false, 127.0,1.0,true),false);// drive throught neut goal 
  f_detect_goal(); 
  skills_d.print("first neutral got %d\n", millis() - time);
  // f_lift.move_absolute(900,200); 
  Task([](){
    delay(200);
    f_lift.set_state(f_lift_states::move_to_target, f_top);
    b_lift.set_state(b_lift_states::move_to_target, b_backup);
    tilt_goal();
  });
  
  // Task([](){
  //   while(true){ 
  //     misc.print("%.2f\n", f_lift.motor.get_actual_velocity());
  //     delay(10);
  //   }
  // });
  // intk.move(127); 
  // move_stop(); 
  // move_wait_for_complete();
  move_start(move_types::tank_point, tank_point_params({58.0,118.0,  20.0},false, 60.0,1.0,true));
  // f_lift.move_absolute(500,100)
  f_lift.set_state(f_lift_states::move_to_target, f_plat);
  // move_start(move_types::tank_point, tank_point_params({54.0,117.0,  20.0},false, 60.0,1.0,true));
  while(f_lift.get_target_state() != f_lift_states::idle)delay(10);
  delay(100);
  f_claw(0);
  delay(100);
  f_lift.set_state(f_lift_states::move_to_target, f_backup);
  move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(tracking.x_coord, tracking.y_coord, -2.0, rad_to_deg(tracking.global_angle), rad_to_deg(tracking.global_angle)),false, 127.0,1.0,true, 9.0, 70.0, 0.0, 800, {1.0, 0.5}));
  // delay(100);
  
  move_start(move_types::turn_angle, turn_angle_params(210, true, false));
  delay(100);
  b_lift.set_state(b_lift_states::move_to_target, b_plat);
  while(b_lift.get_target_state() != b_lift_states::idle)delay(10);
  delay(100);
  // b_claw.set_state(0);
  tilt_lock.set_state(0);
  b_lift.set_state(b_lift_states::move_to_target, b_backup);
  move_start(move_types::tank_point, tank_point_params({54.0,110.0, 210},false, 127.0,1.0,true, 7.0, 70.0, 0.0, 800));
  // f_lift.set_state(f_lift_states::move_to_target, f_top);
  f_lift.set_state(f_lift_states::move_to_target,0);
  b_lift.set_state(b_lift_states::move_to_target,b_top);
  move_start(move_types::turn_angle, turn_angle_params(-90, true));

   move_start(move_types::tank_point, tank_point_params({128.0,118.0, 200},false, 127.0,1.0,true, 6.4, 70.0, 0.0, 0, {5.0, 0.5}));
   move_start(move_types::turn_angle, turn_angle_params(-90, true));
   flatten_against_wall(false);
   master.print(0,0,"time %d", millis() - time);
  // move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(tracking.x_coord, tracking.y_coord, 2, rad_to_deg(tracking.global_angle), rad_to_deg(tracking.global_angle)),false, 60.0,1.0,true));

}

void skills2(){
  int time = millis();
  // f_lift.set_state(f_lift_states::move_to_target,0);
  // b_lift.set_state(b_lift_states::move_to_target,b_top);
  // while(b_lift.get_target_state() != b_lift_states::idle);
  // time = millis();


  // flatten_against_wall(false);
  // f_claw(0);
  // b_claw.set_state(0);
  // tilt_lock.set_state(0);
   delay(100);
   tracking.reset(141.0 - DIST_BACK,141.0 - reset_dist_r.get_dist(), 270.0);



  // Position reset = distance_reset_right(16);
  // master.print(0,0,"%.2f, %.2f, %.2f\n", reset.x, reset.y,rad_to_deg(reset.angle));
  // tracking.reset(141.0 - reset.y,141.0 - reset.x, -90.0 + rad_to_deg(reset.angle));
  // master.print(1,1,"%.0f, %.0f, %.0f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));
  // move_start(move_types::tank_arc, tank_arc_params({tracking.x_coord, tracking.y_coord}, {108.0,96.0, 180.0}));
  move_start(move_types::tank_arc, tank_arc_params({tracking.x_coord, tracking.y_coord}, {tracking.x_coord - 24.0, tracking.y_coord - 24.0, 180.0}));
  b_lift.set_state(b_lift_states::move_to_target, b_bottom);
  move_start(move_types::tank_point, tank_point_params({104.0, 35.5, 180.0}), false);
  f_detect_goal();
  Task([](){
    delay(200);
    f_lift.set_state(f_lift_states::move_to_target, f_carry);
    // b_lift.set_state(b_lift_states::move_to_target, b_backup);
    // tilt_goal();
  });
  move_wait_for_complete();
  move_start(move_types::turn_angle, turn_angle_params(270));
  move_start(move_types::tank_point, tank_point_params({130.0, 35.5, 270.0}), false);
  b_detect_goal();
  move_stop();
  drivebase.brake();
  move_start(move_types::tank_point, tank_point_params({110.0, 35.5, 270.0},false, 127.0,1.0,true, 6.4, 70.0, 0.0, 0, {5.0, 0.5}), false);
  delay(100);
  b_lift.set_state(b_lift_states::move_to_target, b_level);
  while(b_lift.get_target_state() != b_lift_states::idle)delay(10);
  move_start(move_types::tank_point, tank_point_params({130.0, 35.5, 270.0},false, 127.0,1.0,true, 6.4, 70.0, 0.0, 0, {5.0, 0.5}));
  flatten_against_wall(false);
  // move_stop();
  // drivebase.brake();
  master.print(0,0,"time %d", millis() - time);
}

void skills3(){ 
   int time = millis();
  // f_claw(0);
  // b_claw.set_state(0);
  // tilt_lock.set_state(0);
  
  // f_lift.set_state(f_lift_states::move_to_target,0);
  // b_lift.set_state(b_lift_states::move_to_target,0);
  // f_claw(0);

  // while(!master.get_digital_new_press(DIGITAL_B))delay(10);
  // f_claw(1);
  // b_claw.set_state(1);
  // b_lift.set_state(b_lift_states::move_to_target, b_level);
  // f_lift.set_state(f_lift_states::move_to_target, f_carry);
  // while(b_lift.get_target_state() != b_lift_states::idle)delay(10);
  // flatten_against_wall(false);
  delay(100);
   tracking.reset(141.0 - DIST_BACK,reset_dist_l.get_dist(), -90.0);
  // master.print(1,1,"%.0f, %.0f, %.0f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));
  // time = millis();



  move_start(move_types::tank_point, tank_point_params({113.0, tracking.y_coord, -90}));
  move_start(move_types::turn_angle, turn_angle_params(0.0));

  move_start(move_types::tank_point, tank_point_params({110.0, 96.0, 0.0}), false);
  while(tracking.y_coord < 65.0)delay(10);
  f_lift.set_state(f_lift_states::move_to_target, f_push);
  move_start(move_types::tank_point, tank_point_params({64.0, 103.0, -45.0}), false);
  b_lift.set_state(b_lift_states::move_to_target, b_backup);
  Task([]{tilt_goal();});
  move_wait_for_complete();
  move_start(move_types::turn_angle, turn_angle_params(0.0, true, true, 5.0, 0.0, 10.0, 20.0, 127.0, 0, 45 ));
  move_start(move_types::tank_point, tank_point_params({64.0, 108, -45.0},false, 127.0,1.0,true, 20.0, 70.0, 0.0,1200, {0.75, 0.75}, 70));
  f_claw(0);
  delay(50);
  move_start(move_types::tank_point, tank_point_params({64.0, 106, 160.0},false, 127.0,1.0,true, 20.0, 70.0, 0.0,1200, {0.75, 0.75}));
  move_start(move_types::turn_angle, turn_angle_params(155.0, true, false, 5.0, 0.0, 10.0, 20.0, 127.0, 0, 45 ));
  move_start(move_types::tank_point, tank_point_params({61.0, 107, 160.0},false, 60.0,1.0,true, 20.0, 70.0, 0.0,1200, {0.75, 0.75}, 70));
  tilt_lock.set_state(0);
  delay(100);
  move_start(move_types::tank_point, tank_point_params({64.0, 96, 160.0},false, 127.0,1.0,true, 20.0, 70.0, 0.0,1200, {0.75, 0.75}));
  // return;
  // move_start(move_types::tank_point, tank_point_params({74.0, 106.5, -45.0},false, 127.0,1.0,true, 8.0, 70.0, 0.0,1200, {0.75, 0.75}));
  f_lift.set_state(f_lift_states::move_to_target, f_top); 
  // move_start(move_types::tank_point, tank_point_params({78.0, 96.0, 140.0})); 
  b_lift.set_state(b_lift_states::move_to_target, b_bottom); 
  move_start(move_types::turn_angle, turn_angle_params(50.0, true));  
  move_start(move_types::tank_point, tank_point_params({122.0, 123.0, 60.0})); 
  move_start(move_types::turn_angle, turn_angle_params(90.0)); 
  flatten_against_wall(); 
  master.print(0,0,"time %d", millis()-time);
}


void skills4(){
  int time = millis();
  
  // f_lift.set_state(f_lift_states::move_to_target,f_top);
  // b_lift.set_state(b_lift_states::move_to_target,0);
  // while(f_lift.get_target_state() != f_lift_states::idle);
  // time = millis();


  // flatten_against_wall(true);
  // f_claw(0);
  // b_claw.set_state(0);
  // tilt_lock.set_state(0);
   delay(100);
   tracking.reset(141.0 - DIST_BACK,141.0 - reset_dist_l.get_dist(), 90.0);
  // master.print(1,1,"%.0f, %.0f, %.0f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));

  // delay(100);




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
  move_stop();
  drivebase.brake();
  // move_start(move_types::tank_arc, tank_arc_params({tracking.x_coord, tracking.y_coord}, {120.5, 102.0,200.0},127.0,127.0,false));//arc to face neut goal
  // move_start(move_types::turn_angle, turn_angle_params(rad_to_deg(atan2(104.5 - tracking.x_coord, 72.0 - tracking.y_coord)) +180.0, true,true,140.0));
  move_start(move_types::tank_point, tank_point_params({106.5,73.0,  45.0},false, 127.0,1.0,true,9.0,130.0));// drive throught neut goal
  // intake.set_state(intake_states::on);
  move_start(move_types::turn_angle, turn_angle_params(-90));
  move_start(move_types::tank_point, tank_point_params({70.5,73.0,  45.0},false, 60.0,1.0,true), false);
  f_detect_goal();
  move_stop();
  drivebase.brake();
  Task([](){
    delay(150);
    f_lift.set_state(f_lift_states::move_to_target, f_carry);
  });
  move_start(move_types::tank_point, tank_point_params({50,73.0,  45.0},false, 60.0,1.0,true));
  b_lift.set_state(b_lift_states::move_to_target, b_top);
  move_start(move_types::turn_angle, turn_angle_params(angle_to_point(36.0, 107.0) + 180));
  move_start(move_types::tank_point, tank_point_params({36.0,106.0, 30.0}, false, 80.0));
  move_start(move_types::turn_angle, turn_angle_params(angle_to_point(12.0, 107.0) + 180));
  move_start(move_types::tank_point, tank_point_params({16.0,106.0, 30.0}, false, 80.0));
  move_start(move_types::tank_point, tank_point_params({22.0,106.0, 30.0}, false, 80.0));
  move_start(move_types::turn_angle, turn_angle_params(180));
  f_lift.set_state(f_lift_states::move_to_target, f_top);
  move_start(move_types::tank_point, tank_point_params({24.0,10.0, 30.0}, false, 80.0));
  flatten_against_wall();
  master.clear();
  master.print(0,0,"time %d", millis() - time);
  // master.print(1,1,"time %d", millis() - time);
  
  // master.print(2,2,"time %d", millis() - time);

  // move
}


void high_short(){

  //!turn arc
  // move_start(move_types::tank_rush, tank_rush_params({107.0, 72.0, 0.0}, false, 127.0, 1.0,false), false);
  // while(tracking.y_coord < 30.0 )delay(10);
  // move_start(move_types::tank_point, tank_point_params({120.0, 78.0,0.0}), false);
  // f_detect_goal();
  // move_wait_for_complete();


  //? goal turn and line
  // move_start(move_types::tank_rush, tank_rush_params({107.0, 72.0, 0.0}, false, 127.0, 1.0,false));
  // move_start(move_types::turn_angle, turn_angle_params(-90.0, true, true, 15.0));
  // delay(2000);
  // move_start(move_types::turn_angle, turn_angle_params(15.0, true, true, 15.0));
  // high_wp_goal();
  // high_line();


  //? goal and plus
  b_claw_obj.set_state(b_claw_states::tilted);
  f_claw_obj.set_state(f_claw_states::grabbed);
  hitch_obj.set_state(hitch_states::grabbed);
  move_start(move_types::tank_rush, tank_rush_params({107.0, 72.0, 0.0}, false, 127.0, 1.0,false));
  move_start(move_types::turn_angle, turn_angle_params(-90.0, true, true, 15.0));
  delay(2000);
  move_start(move_types::turn_angle, turn_angle_params(15.0, true, true, 15.0));
  high_wp_goal();
  high_line();
  // move_start(move_types::tank_point, tank_point_params({107.0, 35.0,0.0}), false);
  move_start(move_types::turn_angle, turn_angle_params(-90.0));
  // b_lift.Subsystem::set_state(b_lift_states::intake_on);
  move_start(move_types::tank_point, tank_point_params({68.0, 48.0,0.0}, false,70));

  

//? back straigh up
  // move_start(move_types::tank_rush, tank_rush_params({107.0, 72.0, 0.0}, false, 127.0, 1.0,false));
  // delay(100);
  // move_start(move_types::tank_point, tank_point_params({107.0, 35.0,0.0}), false);
  // delay(500);
  // f_lift.set_state(f_lift_states::move_to_target, 1);
  // move_wait_for_complete();
  // move_start(move_types::turn_angle, turn_angle_params(-90.0));
  // delay(50);
  // move_start(move_types::turn_angle, turn_angle_params(-90.0, true, true, 5.0,0.0,10.0,20.0,127.0,0,min_move_power_a, 3.0));
  // move_start(move_types::tank_point, tank_point_params({130.0, 35.0,0.0}), false);
  // b_detect_goal();
  // move_stop();
  // drivebase.brake();
  // move_start(move_types::tank_point, tank_point_params({119.0, 35.0,0.0}));
  // b_lift.Subsystem::set_state(b_lift_states::intake_on);
  // move_start(move_types::turn_angle, turn_angle_params(0.0));
  // move_start(move_types::tank_point, tank_point_params({117.0, 70.0,0.0}, false,70));

}

void high_wp_goal(){
  move_start(move_types::tank_point, tank_point_params({107.0, 35.0,0.0}), false);
  Task([](){detect_interference();});
  delay(500);
  f_lift.set_state(f_lift_states::move_to_target, 1);
  move_wait_for_complete();
  move_start(move_types::turn_angle, turn_angle_params(-90.0));
  delay(50);
  move_start(move_types::turn_angle, turn_angle_params(-90.0, true, true, 5.0,0.0,10.0,20.0,127.0,0,45, 3.0));
  move_start(move_types::tank_point, tank_point_params({130.0, 35.0,0.0}), false);
  b_detect_goal();
  Task([](){
    delay(150);
    b_lift.Subsystem::set_state(b_lift_states::intake_on);});
  move_stop();
  drivebase.brake();
  
}

void high_line(){
  move_start(move_types::tank_point, tank_point_params({119.0, 35.0,0.0}, false, 127.0, 1.0, true, 6.4, 70.0,0.0, 0,{4.0, 0.5}));
  
  move_start(move_types::turn_angle, turn_angle_params(0.0));
  move_start(move_types::tank_point, tank_point_params({117.0, 70.0,0.0}, false,70));
}

void high_tall(){ 
  b_claw_obj.set_state(b_claw_states::tilted);
  f_claw_obj.set_state(f_claw_states::grabbed);
  hitch_obj.set_state(hitch_states::grabbed);
  move_start(move_types::tank_rush, tank_rush_params({70.5, 70.5, -45.0}, false, 127.0, 1.0,false, 180.0));
  move_start(move_types::turn_angle, turn_angle_params(20.0, true, true, 15.0));
  delay(2000);
  // move_start(move_types::turn_angle, turn_angle_params(15.0, true, true, 15.0));
  high_wp_goal();
  high_line();
  move_start(move_types::tank_point, tank_point_params({107.0, 35.0,0.0}));
  // move_start(move_types::tank_point, tank_point_params({107.0, 35.0,0.0}), false);
  move_start(move_types::turn_angle, turn_angle_params(-90.0));
  // b_lift.Subsystem::set_state(b_lift_states::intake_on);
  move_start(move_types::tank_point, tank_point_params({68.0, 48.0,0.0}, false,70));
  // b_claw_obj.set_state(b_claw_states::tilted);
  // f_claw_obj.set_state(f_claw_states::grabbed);
  // hitch_obj.set_state(hitch_states::grabbed);
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
