#include "auton.hpp"
#include "Tracking.hpp"
#include "config.hpp"
#include "logging.hpp"
#include <string>


void skills(){
   f_lift.move_absolute(10); 
  b_lift.move_absolute(10); 
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
  drivebase.move(0.0,0.0);
}




//From gui_construction.cpp (for autons)
extern Button alliance, pos_alliance;
extern Page auto_selection, pos_auto_selection;

namespace Autons{

  

  enum class start_pos{
    POS1,
    POS2,
    POS3,
    NUM_OF_ELEMENTS,
    DEFAULT = POS1,
  };

  enum class alliances{
    RED,
    BLUE,
    NUM_OF_ELEMENTS,
    DEFAULT = RED
  };

  enum class goals{
    LEFT,
    TALL,
    RIGHT,
    NUM_OF_ELEMENTS,
    DEFAULT = TALL
  };

  autons cur_auton = autons::DEFAULT;
  alliances cur_alliance = alliances::DEFAULT;
  start_pos cur_start_pos = start_pos::DEFAULT;
  goals cur_goal = goals::DEFAULT;

  const char* auton_names[static_cast<int>(autons::NUM_OF_ELEMENTS)] = {"Skills", "CNTR", "HIGH", "low"};
  const char* start_pos_names[static_cast<int>(start_pos::NUM_OF_ELEMENTS)] = {"Pos1", "Pos2", "Pos3"};
  const char* alliance_names[2] = {"Red", "Blue"};
  const char* goal_names[3] = {"Left", "Tall", "Right"};

  std::string file_name = "/usd/auton.txt";
  std::string pos_file_name = "/usd/pos_auton.txt";
  std::fstream file;
  std::fstream pos_file;

  void file_update(){
    Data::log_t.data_update();
    file.open(file_name, fstream::out | fstream::trunc);
    pos_file.open(pos_file_name, fstream::out | fstream::trunc);
    file << static_cast<int>(cur_auton) << std::endl;
    file << static_cast<int>(cur_alliance) << std::endl;
    pos_file << static_cast<int>(cur_start_pos) << std::endl;
    pos_file << static_cast<int>(cur_goal) << std::endl;
    file.close();
    pos_file.close();
    Data::log_t.done_update();
  }

  void file_read(){
    //A lot of unnecessary safeties in here. Will remove later
    Data::log_t.data_update();
    file.open(file_name, fstream::in);
    pos_file.open(pos_file_name, fstream::in);
    master.clear();
    printf("here\n");
    if (pros::usd::is_installed()){
      if (!file){//File doesn't exist
        file.close();
        GUI::flash("Auton File not found!");
        printf("\033[92mTrying to create new Auton File.\033[0m\n");
        file_update();
        file.open(file_name, fstream::in);
      }
      if (!pos_file){//File doesn't exist
        pos_file.close();
        GUI::flash("Pos Auton File not found!");
        printf("\033[92mTrying to create new Position Auton File.\033[0m\n");
        file_update();
        pos_file.open(pos_file_name, fstream::in);
      }
    }
    else{
      GUI::flash("No SD Card!");
      printf("\033[31mNo SD card inserted.\033[0m Using default auton, start position, goal and alliance.\n");

      //Might not be needed
      cur_auton = autons::DEFAULT;
      cur_start_pos = start_pos::DEFAULT;
      cur_goal = goals::DEFAULT;
      cur_alliance = alliances::DEFAULT;
      switch_alliance(cur_alliance);
      return;
    }

    char auton[10];
    char start[10];
    char goal[10];
    char ally[5];
    file.getline(auton, 10);
    file.getline(ally, 5);
    pos_file.getline(start, 10);
    pos_file.getline(goal, 10);
    file.close();
    pos_file.close();
    Data::log_t.done_update();
    // cout<< auton;
    printf("%s\n", auton);
    master.print(0,0,"%s", auton);
    delay(500);
    const char** autonIt = std::find(auton_names, auton_names+static_cast<int>(autons::NUM_OF_ELEMENTS), auton);
    const char** startIt = std::find(start_pos_names, start_pos_names+static_cast<int>(start_pos::NUM_OF_ELEMENTS), start);
    const char** goalIt = std::find(goal_names, goal_names+static_cast<int>(goals::NUM_OF_ELEMENTS), goal);
    const char** allianceIt = std::find(alliance_names, alliance_names+2, ally);
    
    cur_auton = autonIt != std::end(auton_names) ? static_cast<autons>(std::distance(auton_names, autonIt)) : autons::DEFAULT;
    cur_start_pos = startIt != std::end(start_pos_names) ? static_cast<start_pos>(std::distance(start_pos_names, startIt)) : start_pos::DEFAULT;
    cur_goal = goalIt != std::end(goal_names) ? static_cast<goals>(std::distance(goal_names, goalIt)) : goals::DEFAULT;
    cur_alliance = allianceIt != std::end(alliance_names) ? static_cast<alliances>(std::distance(alliance_names, allianceIt)) : alliances::DEFAULT;

    switch_alliance(cur_alliance);
  }

  void save_change(std::string which){
    std::string val;
    int line;

    if(which == "auton"){
      val = auton_names[static_cast<int>(cur_auton)];
      line = 0;
    }
    else if(which == "start_pos"){
      val = start_pos_names[static_cast<int>(cur_start_pos)];
      line = 0;
    }
    else if(which == "goal"){
      val = goal_names[static_cast<int>(cur_goal)];
      line = 1;
    }
    else if(which == "alliance"){
      val = alliance_names[static_cast<int>(cur_alliance)];
      line = 2;
    }

    printf("\033[32mSwitched %s to %s\033[0m\n", which.c_str(), val.c_str());
    events.print("Switched %s to %s\n", which.c_str(), val.c_str());
    master.print(line, 0, "%s: %s          ", which.c_str(), val.c_str());
    file_update();
  }

  void prev_route(){
    cur_auton = previous_enum_value(cur_auton);
    save_change("auton");
  }

  void next_route(){
    cur_auton = next_enum_value(cur_auton);
    save_change("auton");
  }

  void prev_start_pos(){
    cur_start_pos = previous_enum_value(cur_start_pos);
    save_change("start_pos");
  }

  void next_start_pos(){
    cur_start_pos = next_enum_value(cur_start_pos);
    save_change("start_pos");
  }

  void prev_goal(){
    cur_goal = previous_enum_value(cur_goal);
    save_change("goal");
  }

  void next_goal(){
    cur_goal = next_enum_value(cur_goal);
    save_change("goal");
  }

  void set_target_goal(goals goal){
    cur_goal = goal;
    save_change("goal");
  }

  void switch_alliance(alliances new_ally){
    cur_alliance = new_ally;
    Colour new_colour = cur_alliance == alliances::BLUE ? COLOUR(BLUE) : COLOUR(RED);

    alliance.set_background(new_colour);
    pos_alliance.set_background(new_colour);
    save_change("alliance");
  }

  void give_up(){ 
    printf("Insert actual Auton Give up code here\n"); 
  } 

  void selector(){
    file_read();
    if(normal){
      auto_selection.go_to();
      while (true){
        if(master.get_digital_new_press(ok_button)) return;
        else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_L1)) prev_route();
        else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_R1)) next_route();
        else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_X)) switch_alliance();

        delay(10);
      }
    }
    else{
      pos_auto_selection.go_to();
      while (true){
        if(master.get_digital_new_press(ok_button)) return;
        else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_L1)) prev_start_pos();
        else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_R1)) next_start_pos();
        else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_X)) switch_alliance();
        else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_L2)) prev_goal();
        else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_R2)) next_goal();

        delay(10);
      }
    }
  }

}

FILE* autoFile = NULL;
autos cur_auto;

void autonFile_read(){
  Data::log_t.data_update();
  autoFile = fopen("/usd/auto.txt","r");
  if(autoFile==NULL) {printf("could not open logfile\n"); return;}
  else printf("logfile found\n");
  int file_auto;
  if(autoFile != NULL){
    fscanf(autoFile, "%d", &file_auto);
    cur_auto = static_cast<autos>(file_auto);
  }
  if(autoFile != NULL) fclose(autoFile);
  Data::log_t.done_update();
  master.print(0,0,"%s\n", auto_names[(int)cur_auto].c_str());
  delay(500);
}

void auto_select(){
  // void autonFile_read();
  master.print(0,0," HERE");
  delay(1000);
  while(true){
     if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_B)){
        // auto_increase();
        // menu_update();
        // cur_auto++;
        cur_auto = next_enum_value(cur_auto);
        master.print(2,2,"%s\n", auto_names[(int)cur_auto].c_str());
      }
      if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_X)){
        
        cur_auto = previous_enum_value(cur_auto);
        master.print(2,2,"%s\n", auto_names[(int)cur_auto].c_str());
        // auto_decrease();
        // menu_update();
      }
      // if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_Y)) side_select = true;
      if(master.get_digital_new_press(ok_button)){
        // auto_set = true;
        autoFile = fopen("/usd/auto.txt","w");
        //char name[80]  = auto_names[static_cast<int>(cur_auto)];
        if(autoFile != NULL){
          fprintf(autoFile, "%d", static_cast<int>(cur_auto));
          fclose(autoFile);
          return;
        }
        // done = true;
      }
  }
}