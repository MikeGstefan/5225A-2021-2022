#include "auton.hpp"


void red_tall_rush(){

}

void skills(){
 
}




std::fstream auton_file;
autons cur_auton = static_cast<autons>(0);
alliances cur_alliance = static_cast<alliances>(0);

const char* auton_names[static_cast<int>(autons::NUM_OF_ELEMENTS)] = {"Skills", "Auto1", "Auto2"};
const char* alliance_names[2] = {"Red", "Blue"};

extern Button alliance;

void auton_file_update(){
  Data::log_t.data_update();
  auton_file.open("/usd/auton.txt", fstream::out | fstream::trunc);
  auton_file << auton_names[static_cast<int>(cur_auton)] << std::endl;
  auton_file << alliance_names[static_cast<int>(cur_alliance)];
  auton_file.close();
  Data::log_t.done_update();
  master.clear();
  master.print(0, 0, "Auton: %s          ", auton_names[static_cast<int>(cur_auton)]);
  master.print(1, 0, "Alliance: %s       ", alliance_names[static_cast<int>(cur_alliance)]);
}

void auton_file_read(){
  Data::log_t.data_update();
  auton_file.open("/usd/auton.txt", fstream::in);

  if (!auton_file){//File doesn't exist
    auton_file.close();
    GUI::flash(COLOR_RED, 1000, "Auton File not found!");
    printf("\033[92mTrying to create new Auton File.\033[0m\n");
    auton_file_update();
    auton_file.open("/usd/auton.txt", fstream::in);

    if (!auton_file){
      auton_file.close();
      Data::log_t.done_update();
      printf("\033[31mAborting auton file read. It's not getting created.\033[0m Using default values.\n");
      
      cur_auton = autons::Skills;
      cur_alliance = alliances::BLUE;
      switch_alliance(cur_alliance);
      return;
    }
  }

  char auton[10];
  char ally[5];
  auton_file.getline(auton, 10);
  auton_file.getline(ally, 5);
  auton_file.close();
  Data::log_t.done_update();

  const char** autonIt = std::find(auton_names, auton_names+static_cast<int>(autons::NUM_OF_ELEMENTS), auton);
  const char** allianceIt = std::find(alliance_names, alliance_names+2, ally);
  
  cur_auton = (autonIt != std::end(auton_names)) ? static_cast<autons>(std::distance(auton_names, autonIt)) : autons::Skills;
  cur_alliance = (allianceIt != std::end(alliance_names)) ? static_cast<alliances>(std::distance(alliance_names, allianceIt)) : alliances::BLUE;

  switch_alliance(cur_alliance);
}

void prev_auton(){
  cur_auton = previous_enum_value(cur_auton);
  auton_file_update();
}

void next_auton(){
  cur_auton = next_enum_value(cur_auton);
  auton_file_update();
}

void switch_alliance(alliances new_ally){
  switch(new_ally){
    case alliances::BLUE:
      cur_alliance = alliances::BLUE;
      alliance.set_background(COLOR_BLUE);
      printf("\033[34mSwitched to Blue Alliance\033[0m\n");
      misc.print("Switched to Blue Alliance\n");
      break;

    case alliances::RED:
      cur_alliance = alliances::RED;
      alliance.set_background(COLOR_RED);
      printf("\033[31mSwitched to Red Alliance\033[0m\n");
      misc.print("Switched to Red Alliance\n");
      break;
  }

  auton_file_update();
}
