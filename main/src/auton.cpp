#include "auton.hpp"

void tilter_reset(){
    tilter_motor.move(-40);
    Timer vel_rise_timeout("vel_rise");
    // waits for motor's velocity to rise or timeout to trigger
    while(fabs(tilter_motor.get_actual_velocity()) < 45.0){
    //   printf("%s's velocity is (rising loop): %lf\n", this->name, motor.get_actual_velocity());
      if (vel_rise_timeout.get_time() > 50){
        // printf("%s's rising loop timed out\n", this->name);
        break;
      }
      delay(10);
    }
    // printf("%s's velocity done rising\n", this->name);
    // waits until motors velocity slows down for 5 cycles
    cycleCheck(fabs(tilter_motor.get_actual_velocity()) < 5.0, 5, 10)
    tilter_motor.tare_position();  // resets subsystems position
    // printf("%d, %s's reset %lf\n", millis(), this->name, motor.get_position());
    tilter_motor.move(0);
}

void red_tall_rush(){

}

void skills(){
  lift.reset();
  tilter_reset();
	lift_piston.set_value(LOW);
	lift.motor.move_absolute(lift.bottom_position, 100);

	Timer move_timer{"move"};
    move_start(move_types::point, point_params({112.5, 14.75, -90.0}),true);
	// move_to_point();
	move_timer.print();
	// tank_move_on_arc({110.0, 14.75}, {132.0, 24.0, -180.0}, 127.0);
	// move_on_line(polar_to_vector(110.0, 14.75, 10.0, 45.0, -135.0));

  // grabs alliance goal
  move_start(move_types::point, point_params(polar_to_vector_point(110.0, 14.75, 18.0, 45.0, -135.0), 127.0, false, 1.0, false), true);
	// move_to_point(polar_to_vector_point(110.0, 14.75, 17.5, 45.0, -135.0), 127.0, false, 1.0, false);	// grabs alliance goal
	lift_piston.set_value(HIGH);

  move_start(move_types::turn_point, turn_point_params({108.0, 60.0}), true); // turns to face neutral mogo
	move_start(move_types::point, point_params({110.0, 60.0, 0.0}, 127.0, false, 0.0, true),false);  // lines up on neutral mogo
  move_start(move_types::point, point_params({110.0, 80.0, 0.0}, 127.0, false, 0.0, false),true);  // drives through netural mogo
  // move_start(move_types::tank_arc, tank_arc_params({110.0, 84.0},{80.0, 96.0, -90.0}, 127.0), true);  // arcs to first patch of rings
  // move_start(move_types::point, point_params({60.0, 96.0, -90.0}, 80.0, false, 0.0, true), true); // drives through second patch of rings
  // move_start(move_types::point, point_params({60.0, 108.0, -90.0}, 80.0, false, 0.0, true), true);  // strafes to platform

	// move_to_point({110.0, 60.0, 0.0}, 127.0, false, 0.0, false);	// in front of small neutral goal
	// move_to_point({110.0, 80.0, 0.0}, 127.0, false, 0.0, false);	// drives through small neutral goal
	// tank_move_on_arc({110.0, 84.0},{80.0, 96.0, -90.0}, 127.0);
	// move_to_point({60.0, 96.0, -90.0}, 80.0, false, 0.0, true);	// drives over rings
	// move_to_point({60.0, 108.0, -90.0}, 80.0, false, 0.0, true);	// drives to drop off small neutral

  // lift.motor.move_absolute(lift.platform_position, 100);
  // move_start(move_types::point, point_params({60.0, 108.0, -180.0}, 80.0, false, 0.0, true),true);
	// move_to_point({60.0, 108.0, -180.0}, 80.0, false, 0.0, true);	// drives to drop off small neutral
	// lift_piston.set_value(LOW);
}

std::fstream auton_file;
autons cur_auton = static_cast<autons>(0);
alliances cur_alliance = static_cast<alliances>(0);

const char* auton_names[static_cast<int>(autons::NUM_OF_ELEMENTS)] = {"Skills", "Auto1", "Auto2"};
const char* alliance_names[2] = {"Red", "Blue"};

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

  auto autonIt = std::find(auton_names, auton_names+static_cast<int>(autons::NUM_OF_ELEMENTS), auton);
  auto allianceIt = std::find(alliance_names, alliance_names+2, ally);

  if (autonIt != std::end(auton_names)) cur_auton = static_cast<autons>(std::distance(auton_names, autonIt));
  else cur_auton = autons::Skills;

  if (allianceIt != std::end(alliance_names)) cur_alliance = static_cast<alliances>(std::distance(alliance_names, allianceIt));
  else cur_alliance = alliances::BLUE;
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
      printf("\033[34mSwitched to Blue Alliance\033[0m\n"); //Convert to log
      break;

    case alliances::RED:
      cur_alliance = alliances::RED;
      alliance.set_background(COLOR_RED);
      printf("\033[31mSwitched to Red Alliance\033[0m\n"); //Convert to log
      break;
  }

  auton_file_update();
}
