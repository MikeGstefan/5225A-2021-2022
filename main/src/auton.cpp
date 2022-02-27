#include "auton.hpp"
#include "Tracking.hpp"
#include "logging.hpp"

void tilter_reset(){
    // tilter_motor.move(-40);
    // Timer vel_rise_timeout("vel_rise");
    // // waits for motor's velocity to rise or timeout to trigger
    // waitUntil(fabs(tilter_motor.get_actual_velocity()) > 45.0){
    // //   printf("%s's velocity is (rising loop): %lf\n", this->name, motor.get_actual_velocity());
    //   if (vel_rise_timeout.get_time() > 50){
    //     // printf("%s's rising loop timed out\n", this->name);
    //     break;
    //   }
    // }
    // // printf("%s's velocity done rising\n", this->name);
    // // waits until motors velocity slows down for 5 cycles
    // cycleCheck(fabs(tilter_motor.get_actual_velocity()) < 5.0, 5, 10)
    // tilter_motor.tare_position();  // resets subsystems position
    // // printf("%d, %s's reset %lf\n", millis(), this->name, motor.get_position());
    // tilter_motor.move(0);
}

void skills2(){
  // f_claw_p.set_value(0);
  // b_claw_p.set_value(0);
  // b_lift.move_absolute(10);
  // f_lift.move_absolute(10);
  // delay(2000);
  // flatten_against_wall(false);
  // delay(100);
  // tracking.reset(141.0 - reset_dist_r.get_dist(),141.0 - DIST_BACK, 180.0);
  // // delay(200);
  // master.print(1,1,"%.2f, %.2f, %.2f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));

  move_start(move_types::tank_point, tank_point_params({tracking.x_coord, 129.0, 180.0},false));
  move_start(move_types::turn_angle, turn_angle_params(90.0));
  delay(100);
  move_start(move_types::tank_point, tank_point_params({95.0, 129.0, 90.0},false,50),false);
  b_detect_goal();
  move_stop();
  drivebase.brake();
  // move_start(move_types::tank_point, tank_point_params({125.0, 129.0, 90.0},false),false);
  move_start(move_types::tank_arc, tank_arc_params({tracking.x_coord, tracking.y_coord}, {120.5, 102.0,200.0},127.0,127.0,false));//arc to face neut goal
  move_start(move_types::turn_point, turn_point_params({104.5, 72.0}));

  delay(50);
	// move_start(move_types::tank_point, tank_point_params({106.5,72.0,  235.0},false, 80.0,1.0,true,9.0,130.0),false);// drive throught neut goal
  move_start(move_types::tank_point, tank_point_params({104.5,70.0,  235.0},false, 80.0,1.0,true,9.0,130.0),false);// drive throught neut goal
  f_detect_goal();
  move_stop();
  // while(!master.get_digital(DIGITAL_L1))delay(10);
	
	move_start(move_types::turn_point, turn_point_params({71.0,43.0}, true));  // turns to face plus of rings
	delay(100);

  move_start(move_types::tank_point, tank_point_params({71.0, 44.5, 235.0},false, 80.0, 1.0, true,9.0,130.0),false);
  delay(100);
    f_lift.move_absolute(580);
    b_lift.move_absolute(700);
  move_wait_for_complete();

  delay(100);
	move_start(move_types::turn_angle, turn_angle_params(360.0,true, true, 150.0,80.0)); // turns to face platform
  delay(100);
  // return;
  // while(true)delay(10);
  // b_lift.move_absolute(700);
  while(b_lift.motor.get_position() < 680)delay(10);
  // delay(2000);
  vision_loop(15, 1500);
  drivebase.brake();
  b_lift.move_absolute(600);
  while(b_lift.motor.get_position() > 620)delay(10);
  move_start(move_types::turn_angle, turn_angle_params(360.0,true, true, 170.0,0.0, 127.0, 500)); 
  b_claw_p.set_value(0);
  delay(200);
  b_lift.move_absolute(650);
  // f_lift.move_absolute(10);
  while(b_lift.motor.get_position() < 620)delay(10);
  // drivebase.move(30.0,0.0);
  // misc.print("%d || starting backup from plat\n", millis());
  // tracking.wait_for_dist(5.0);
  // drivebase.brake();


  drivebase.move(30.0,0.0);
  misc.print("%d || starting backup from plat\n", millis());
  tracking.wait_for_dist(5.0);
  drivebase.brake();
  b_lift.move_absolute(800);
  while(b_lift.motor.get_position() < 780)delay(10);
  misc.print("%d || done backup from plat\n", millis());
  // drivebase.move(-30.0,0.0);
  // tracking.wait_for_dist(2.0);
  // drivebase.brake();
  // drivebase.brake();
  // b_lift.move_absolute(650);
  
  // return;
  move_start(move_types::turn_angle, turn_angle_params(200.0, true, false, 160.0,0.0,70.0));
  // return;
  delay(200);
   misc.print("%d || starting drive towards  plat\n", millis());
  drivebase.move(30.0,0.0);
  tracking.wait_for_dist(2.0, 750);
  misc.print("%d || done drive towards plat\n", millis());
  f_claw_p.set_value(0);
  delay(400);
  // move_start(move_types::turn_angle, turn_angle_params(360.0,true, true, 170.0,0.0, 127.0, 500)); 
  // return;
  move_start(move_types::tank_point, tank_point_params({71.0, 46.0, 235.0},false, 80.0, 1.0, true,9.0,130.0),false);




  move_start(move_types::turn_angle, turn_angle_params(270.0));
  // return;
  f_claw_p.set_value(0);
  b_lift.move_absolute(10);
	// move_start(move_types::tank_point, tank_point_params({111.0,34.0,-90.0},false,70));
	// delay(500);
	move_start(move_types::tank_point, tank_point_params({180.0,34.0,270.0},false, 70),false);
	b_detect_goal();
	move_stop();
	// drivebase.brake();
  drivebase.move(-20,0);
  delay(100);
	move_start(move_types::tank_point, tank_point_params({115.0,34.0,270.0},false));
  move_start(move_types::turn_angle, turn_angle_params(270.0, true, true,160.0,0.0,127.0,500,35.0));
	b_lift.move_absolute(820);
	while(b_lift.motor.get_position() < 800)delay(10);
	b_lift.move(10);
	flatten_against_wall(false);
  delay(100);
  tracking.reset(141.0 - DIST_BACK,reset_dist_r.get_dist(), -90.0);
  delay(200);
  master.print(1,1,"%.2f, %.2f, %.2f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));
}

void skills3(){
  // f_claw_p.set_value(0);
  // b_claw_p.set_value(0);
  // b_lift.move_absolute(10);
  // f_lift.move_absolute(10);
  // while(!master.get_digital_new_press(DIGITAL_R1))delay(10);

  // f_claw_p.set_value(1);
  // b_claw_p.set_value(1);
  // f_lift.move_absolute(200);
  // b_lift.move_absolute(820);
	// while(b_lift.motor.get_position() < 800)delay(10);
	// b_lift.move(10);
	// flatten_against_wall(false);
  // delay(100);
  // tracking.reset(141.0 - DIST_BACK,reset_dist_r.get_dist(), -90.0);
  // master.clear();
  // delay(200);
  // master.print(1,1,"%.2f, %.2f, %.2f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));
  // while(!master.get_digital_new_press(DIGITAL_R1))delay(10);

  int start_time = millis();

  move_start(move_types::tank_point, tank_point_params({111.0, 36.0, -90.0}, false));
  b_lift.move_absolute(150);
  move_start(move_types::turn_angle, turn_angle_params(0.0));
  delay(100);
  // f_lift.move_absolute(650);
  move_start(move_types::tank_arc, tank_arc_params({105.0, 36.0}, {60.0,102.0, -90.0},127.0));
  f_lift.move_absolute(10);
  f_claw_p.set_value(0);
  b_claw_p.set_value(0);
  // b_lift.move_absolute(820);
  // move_start(move_types::turn_angle, turn_angle_params(-90.0));
  move_start(move_types::tank_point, tank_point_params({30.0, 102.0, -90.0}, false, 80.0, 1.0,true,9.0,100.0));
  move_start(move_types::turn_angle, turn_angle_params(-90.0, true, true, 160.0,0.0, 127.0,500,35.0,2.0));
  // f_detect_goal();
  // move_stop();
  // drivebase.brake();
  delay(100);
  move_start(move_types::tank_point, tank_point_params({10.0, 102.0, -90.0}, false, 80.0, 1.0,true,9.0,100.0),false);
  f_detect_goal();
  move_stop();
  drivebase.brake();
  delay(200);
  move_start(move_types::tank_point, tank_point_params({30.0, 102.0, -90.0}, false, 80.0, 1.0,true,9.0,100.0),false);
  // move_start(move_types::turn_point, turn_point_params({70.0,70.0}));
  move_start(move_types::turn_angle, turn_angle_params(rad_to_deg(atan2(70.0 - tracking.x_coord, 66.0 - tracking.y_coord) )+180));
  delay(100);
  b_lift.move_absolute(10);
  move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(tracking.x_coord, tracking.y_coord, -30.0, rad_to_deg(tracking.global_angle), rad_to_deg(tracking.global_angle)), false, 90.0),true);
  move_start(move_types::turn_angle, turn_angle_params(rad_to_deg(atan2(70.0 - tracking.x_coord, 66.0 - tracking.y_coord) )+180));
  drivebase.set_state(1);
  move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(tracking.x_coord, tracking.y_coord, -150.0, rad_to_deg(tracking.global_angle), rad_to_deg(tracking.global_angle)), false, 127.0),false);
  Task([](){
    b_detect_goal();
    b_lift.move_absolute(200);
  });
  tracking.wait_for_dist(90.0);
  move_stop();
  drivebase.brake();
  // move_wait_for_complete();
  // move_stop();
  // drivebase.brake();
  // move_start(move_types::tank_point, tank_point_params({110.0,30.0,45.0}, false));
  // b_lift.move_absolute(650);
  // move_start(move_types::turn_angle, turn_angle_params(0.0));
  // delay(100);
  // move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(tracking.x_coord, 30.0, -18.0, 0.0,0.0), false, 60.0));
  // while(b_lift.motor.get_position() <630)delay(10);
  // flatten_against_wall(false);
  // // int start = millis();
  // delay(100);
  // tracking.reset(141.0- reset_dist_r.get_dist(), DIST_BACK,180.0);
  // delay(200);
  master.print(2,2,"%d", millis() - start_time);
  // delay(1000);
  // move_start(move_types::tank_point, tank_point_params({16.0, 102.0, -90.0}, false));
  // move_start(move_types::tank_point, tank_point_params({22.0, 122.0, -235.0}, false));
  // move_start(move_types::turn_angle, turn_angle_params(-180.0));
  // delay(100);
  // flatten_against_wall(false);
  // delay(100);
  // tracking.reset(reset_dist_l.get_dist(), 141.0 - DIST_BACK,180.0);
  // delay(200);
  // master.print(1,1,"%.2f, %.2f, %.2f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));
} 

void skills4(){
  f_claw_p.set_value(0);
  b_claw_p.set_value(0);
  b_lift.move_absolute(10);
  f_lift.move_absolute(10);
  while(!master.get_digital_new_press(DIGITAL_R1))delay(10);

  f_claw_p.set_value(1);
  b_claw_p.set_value(1);
  f_lift.move_absolute(650);
  // f_lift.move_absolute(200);
  b_lift.move_absolute(950);
	while(b_lift.motor.get_position() < 930)delay(10);
	b_lift.move(10);
	delay(100);
  flatten_against_wall(false);
  delay(100);
  tracking.reset(reset_dist_l.get_dist(), 141.0 - DIST_BACK,180.0);
  delay(200);
  master.print(1,1,"%.2f, %.2f, %.2f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));
  while(!master.get_digital_new_press(DIGITAL_R1))delay(10);


  move_start(move_types::tank_point, tank_point_params({tracking.x_coord, 130.0, 180.0}, false));
  f_lift.move_absolute(450);
  move_start(move_types::turn_angle, turn_angle_params(90.0, true, true, 160.0,0.0, 127.0,0,35.0));
  delay(100);
  move_start(move_types::tank_point, tank_point_params({39.0, 129.0, 90.0}, false, 100.0, 0.8, true,9.0,80.0,150.0));
  drivebase.brake();
  move_start(move_types::turn_angle, turn_angle_params(90.0, true, true, 160.0,0.0, 127.0,500,35.0));
  f_claw_p.set_value(0);
  delay(300);
  move_start(move_types::tank_point, tank_point_params({23.0, 130.0, 90.0}, false),false);
  delay(200);
  f_lift.move_absolute(600);
  // move_start(move_types::turn_angle, turn_angle_params(-90.0, true, false, 160.0,0.0, 127.0,0, 35.0));
}


void skillsPark(){
  f_claw_p.set_value(0);
  b_claw_p.set_value(0);
  b_lift.move_absolute(10);
  f_lift.move_absolute(10);
  while(!master.get_digital_new_press(DIGITAL_R1))delay(10);

  f_claw_p.set_value(1);
  b_claw_p.set_value(1);
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
  Led1.set_value(0);
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
	// waitUntil(f_lift_m.get_position() > 780);
  // delay(500);
  // f_lift.move_absolute(610);


  // Task([](){
  //   delay(300);
	// 	f_lift.move(70);
  //   waitUntil(f_lift_m.get_position() > 700);
  //   f_lift.motor.move_relative(0, 100);
  // });
  drivebase.set_state(1);
  Led1.set_value(1);
  gyro.climb_ramp();
  drivebase.brake();
  delay(100);
  master.print(2,2,"%d",millis() - start);



}


void lrt_auton(){
  move_start(move_types::tank_point, tank_point_params({108.0,71.0,0.0}),false);
  f_lift.reset();
  f_lift.move(-10);
  f_detect_goal();
  f_lift.move_absolute(150,100);
  // intk.move(10);
  move_stop();
  // drivebase.move(0,0);
  move_start(move_types::tank_point, tank_point_params({116.0,46.0,-45.0}),false);
  // move_start(move_types::turn_point, turn_point_params({130.0,35.0}));
  move_start(move_types::turn_angle, turn_angle_params(rad_to_deg(atan2(130.0 - tracking.x_coord, 35.0 - tracking.y_coord))+180.0));

  drivebase.brake();
  delay(100);
  move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(116.0,46.0,-20.0, rad_to_deg(tracking.global_angle), rad_to_deg(tracking.global_angle)), false,90.0,1.0,true,9.0,150.0,0.0,0,{0.5,1.0}),false);
  b_detect_goal();
  move_stop();
  drivebase.brake();
  intk.move(50);
  while(true)delay(10);
}


extern Button alliance, pos_alliance; //From gui_construction.cpp (for autons)

namespace Autons{

  enum class autons{
    Skills,
    Auto2,
    Auto3,
    NUM_OF_ELEMENTS,
    Default = Skills,
  };

  enum class start_pos{
    Pos1,
    Pos2,
    Pos3,
    NUM_OF_ELEMENTS,
    Default = Pos1,
  };

  enum class alliances{
    RED,
    BLUE,
    Default = BLUE
  };

  std::string file_name = "/usd/auton.txt";
  std::string pos_file_name = "/usd/pos_auton.txt";
  std::fstream file;
  autons cur_auton = static_cast<autons>(0);
  alliances cur_alliance = static_cast<alliances>(0);
  start_pos cur_start_pos;
  int cur_goal;

  const char* auton_names[static_cast<int>(autons::NUM_OF_ELEMENTS)] = {"Skills", "Auto1", "Auto2"};
  const char* alliance_names[2] = {"Red", "Blue"};
  const char* start_pos_names[static_cast<int>(start_pos::NUM_OF_ELEMENTS)] = {"Pos1", "Pos2", "Pos3"};

  void file_update(){
    Data::log_t.data_update();
    file.open(file_name, fstream::out | fstream::trunc);
    file << auton_names[static_cast<int>(cur_auton)] << std::endl;
    file << alliance_names[static_cast<int>(cur_alliance)];
    file.close();
    Data::log_t.done_update();
    master.clear();
    master.print(0, 0, "Auton: %s          ", auton_names[static_cast<int>(cur_auton)]);
    master.print(1, 0, "Alliance: %s       ", alliance_names[static_cast<int>(cur_alliance)]);
  }

  void file_read(){
    Data::log_t.data_update();
    file.open(file_name, fstream::in);


    if (pros::usd::is_installed()){
      if (!file){//File doesn't exist
        file.close();
        GUI::flash(COLOUR(RED), 1000, "Auton File not found!");
        printf("\033[92mTrying to create new Auton File.\033[0m\n");
        file_update();
        file.open(file_name, fstream::in);
      }
    }
    else{
      printf("\033[31mNo SD card inserted.\033[0m Using default auton and alliance.\n");

      cur_auton = autons::Default;
      cur_alliance = alliances::Default;
      switch_alliance(cur_alliance);
      return;
    }

    char auton[10];
    char ally[5];
    file.getline(auton, 10);
    file.getline(ally, 5);
    file.close();
    Data::log_t.done_update();

    const char** autonIt = std::find(auton_names, auton_names+static_cast<int>(autons::NUM_OF_ELEMENTS), auton);
    const char** allianceIt = std::find(alliance_names, alliance_names+2, ally);
    
    cur_auton = autonIt != std::end(auton_names) ? static_cast<autons>(std::distance(auton_names, autonIt)) : autons::Default;
    cur_alliance = allianceIt != std::end(alliance_names) ? static_cast<alliances>(std::distance(alliance_names, allianceIt)) : alliances::Default;

    switch_alliance(cur_alliance);
  }

  void pos_file_update(){
    Data::log_t.data_update();
    file.open(pos_file_name, fstream::out | fstream::trunc);
    file << start_pos_names[static_cast<int>(cur_start_pos)] << std::endl;
    file << cur_goal << std::endl;
    file << alliance_names[static_cast<int>(cur_alliance)];
    file.close();
    Data::log_t.done_update();
    master.clear();
    master.print(0, 0, "Start Pos: %s          ", start_pos_names[static_cast<int>(cur_start_pos)]);
    master.print(1, 0, "Goal: %d          ", cur_goal);
    master.print(2, 0, "Alliance: %s       ", alliance_names[static_cast<int>(cur_alliance)]);
  }

  void pos_file_read(){
    Data::log_t.data_update();
    file.open(pos_file_name, fstream::in);


    if (pros::usd::is_installed()){
      if (!file){//File doesn't exist
        file.close();
        GUI::flash(COLOUR(RED), 1000, "Pos Auton File not found!");
        printf("\033[92mTrying to create new Pos Auton File.\033[0m\n");
        pos_file_update();
        file.open(pos_file_name, fstream::in);
      }
    }
    else{
      printf("\033[31mNo SD card inserted.\033[0m Using default start position, goal and alliance.\n");

      cur_start_pos = start_pos::Default;
      cur_goal = 1;
      cur_alliance = alliances::Default;
      switch_alliance(cur_alliance);
      return;
    }

    char start[10];
    char goal[2];
    char ally[5];
    file.getline(start, 10);
    file.getline(goal, 2);
    file.getline(ally, 5);
    file.close();
    Data::log_t.done_update();

    const char** startIt = std::find(start_pos_names, start_pos_names+static_cast<int>(start_pos::NUM_OF_ELEMENTS), start);
    const char** allianceIt = std::find(alliance_names, alliance_names+2, ally);
    
    cur_start_pos = startIt != std::end(start_pos_names) ? static_cast<start_pos>(std::distance(start_pos_names, startIt)) : start_pos::Default;
    cur_alliance = allianceIt != std::end(alliance_names) ? static_cast<alliances>(std::distance(alliance_names, allianceIt)) : alliances::Default;
    cur_goal = stoi(goal);

    switch_alliance(cur_alliance);
  }

  void prev_route(){
    cur_auton = previous_enum_value(cur_auton);
    printf("Switched auton to %s\n", auton_names[static_cast<int>(cur_auton)]);
    events.print("Switched auton to %s\n", auton_names[static_cast<int>(cur_auton)]);
    master.print(0, 0, "Auton: %s          ", auton_names[static_cast<int>(cur_auton)]);
    file_update();
  }

  void next_route(){
    cur_auton = next_enum_value(cur_auton);
    printf("Switched auton to %s\n", auton_names[static_cast<int>(cur_auton)]);
    events.print("Switched auton to %s\n", auton_names[static_cast<int>(cur_auton)]);
    file_update();
  }

  void prev_start_pos(){
    cur_start_pos = previous_enum_value(cur_start_pos);
    printf("Switched start pos to %s\n", start_pos_names[static_cast<int>(cur_start_pos)]);
    events.print("Switched start pos to %s\n", start_pos_names[static_cast<int>(cur_start_pos)]);
    pos_file_update();
  }

  void next_start_pos(){
    cur_start_pos = next_enum_value(cur_start_pos);
    printf("Switched start pos to %s\n", start_pos_names[static_cast<int>(cur_start_pos)]);
    events.print("Switched start pos to %s\n", start_pos_names[static_cast<int>(cur_start_pos)]);
    pos_file_update();
  }

  void set_target_goal(int goal){
    cur_goal = goal;
    printf("Switched goal to %d\n", cur_goal);
    events.print("Switched goal to %d\n", cur_goal);
    pos_file_update();
  }

  void switch_alliance(){
    switch(cur_alliance){
      case alliances::RED: //Opposite, since switching alliances
        cur_alliance = alliances::BLUE;
        alliance.set_background(COLOUR(BLUE));
        pos_alliance.set_background(COLOUR(BLUE));
        printf("\033[34mSwitched to Blue Alliance\033[0m\n");
        events.print("Switched to Blue Alliance\n");
        break;

      case alliances::BLUE:
        cur_alliance = alliances::RED;
        alliance.set_background(COLOUR(RED));
        pos_alliance.set_background(COLOUR(RED));
        printf("\033[31mSwitched to Red Alliance\033[0m\n");
        events.print("Switched to Red Alliance\n");
        break;
    }

    file_update();
    pos_file_update();
  }

  void switch_alliance(alliances new_ally){
    switch(new_ally){
      case alliances::BLUE:
        cur_alliance = alliances::BLUE;
        alliance.set_background(COLOUR(BLUE));
        pos_alliance.set_background(COLOUR(BLUE));
        printf("\033[34mSwitched to Blue Alliance\033[0m\n");
        events.print("Switched to Blue Alliance\n");
        break;

      case alliances::RED:
        cur_alliance = alliances::RED;
        alliance.set_background(COLOUR(RED));
        pos_alliance.set_background(COLOUR(RED));
        printf("\033[31mSwitched to Red Alliance\033[0m\n");
        events.print("Switched to Red Alliance\n");
        break;
    }

    file_update();
    pos_file_update();
  }

}