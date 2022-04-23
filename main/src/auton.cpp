#include "auton.hpp"
#include "Libraries/printing.hpp"
#include "Subsystems/f_lift.hpp"
#include "Tracking.hpp"
#include "auton_util.hpp"
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
  // intake.set_state(intake_states::on);
  b_lift.Subsystem::set_state(b_lift_states::intake_on);
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

void skills_park(){
  f_claw(HIGH);
	b_claw.set_state(HIGH);
  hitch.set_state(HIGH);
  tilt_lock.set_state(LOW);
  f_lift.set_state(f_lift_states::move_to_target, 3);
  b_lift.set_state(b_lift_states::move_to_target, b_lift.prog_positions.size()-1);
  while(f_lift.get_target_state() != f_lift_states::idle)delay(10);
  flatten_against_wall();
  delay(100);
  tracking.reset(reset_dist_r.get_dist(), DIST_FRONT, 180.0);
  int s_time = millis();
  printf2("\n\n\n\n\n\nTIME:%d\n\n\n\n", millis());
  // master.wait_for_press(DIGITAL_R1);
  move_start(move_types::tank_point, tank_point_params({tracking.x_coord, 10.0, 180.0}, false, 127.0, 1.0, true, 6.4, 70.0, 0.0, 0, {0.5, 0.5}, 30.0));
  move_start(move_types::turn_angle, turn_angle_params(90.0));
  // master.wait_for_press(DIGITAL_R1);
  move_start(move_types::tank_point, tank_point_params({32.0, 12.0, 90.0}));
  f_lift.set_state(f_lift_states::move_to_target, 0);

  b_claw_obj.set_state(b_claw_states::managed);
  f_claw_obj.set_state(f_claw_states::managed);
  hitch_obj.set_state(hitch_states::managed);
  b_lift.set_state(b_lift_states::move_to_target, 5);
  f_lift.set_state(f_lift_states::move_to_target, 0);

  drivebase.move(0.0, 0.0); //so it's not locked when switching trans
  
  // f_claw(LOW);
  // b_claw.set_state(LOW);
  // hitch.set_state(LOW);
  // tilt_lock.set_state(HIGH);
	drivebase.set_state(HIGH);

  //Load goals
  // master.wait_for_press(DIGITAL_R1);
  drivebase.move(0.0, 0.0); //so it's not locked when switching trans
  while(f_lift_pot.get_value() > 1200)delay(10); //wait for bottom

	// f_claw(HIGH);
	// b_claw.set_state(HIGH);
  // hitch.set_state(HIGH);
  // tilt_lock.set_state(LOW);

  // master.wait_for_press(DIGITAL_R1);
  int start = millis();

  gyro.climb_ramp();
  gyro.level(1.6, 1000.0);

  master.clear();
  printf("\n\nStart: %d\n", start);
  printf("\n\nEnd: %d\n", millis());
  printf("\n\nTotal: %d\n", millis()-start);
  master.print(0, 0, "Time:%d", millis()-s_time);

  master.wait_for_press(DIGITAL_R1);
  hitch.set_value(LOW);
  f_claw(LOW);
  b_lift.set_state(b_lift_states::move_to_target, 0);
  f_lift.set_state(f_lift_states::move_to_target, 0);
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
  move_start(move_types::turn_angle, turn_angle_params(-90.0, true, true, 15.0, 0.0,10.0,20.0,127.0,1000));
  delay(2000);
  move_start(move_types::turn_angle, turn_angle_params(15.0, true, true, 15.0, 0.0,10.0,20.0,127.0,1000));
  high_wp_goal();
  high_line();
  // move_start(move_types::tank_point, tank_point_params({107.0, 35.0,0.0}), false);
  move_start(move_types::turn_angle, turn_angle_params(-90.0));
  // b_lift.Subsystem::set_state(b_lift_states::intake_on);
  move_start(move_types::tank_point, tank_point_params({68.0, 48.0,0.0}, false,70));

  

  //? back straight up
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
    b_lift.Subsystem::set_state(b_lift_states::intake_on);
  });
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
  move_start(move_types::tank_rush, tank_rush_params({68.5, 70.5, -45.0}, false, 127.0, 1.0,false, 180.0));
  move_start(move_types::turn_angle, turn_angle_params(20.0, true, true, 15.0, 0.0,10.0,20.0,127.0,1000));
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

  ifstream file;
  Data::log_t.data_update();
  printf2(term_colours::BLUE, "\n\nLoading Autons:");
  file.open(auton_file_name, fstream::in);
  while(file >> target >> task){
    printf2(term_colours::BLUE, "%s: %s", target, task);
    selected_positions.push_back({target, task});
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
    master.print(1, 0, choice);

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
  master.print(1, 0, start_selection->first);
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
    master.print(1, 0, goal_selection->first);

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
  master.print(1, 0, end_selection->first);

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
        wait_until(F_LIFT_AT_BOTTOM);
        b_lift.Subsystem::set_state(b_lift_states::intake_on);

        Task([](){detect_interference();});
        move_start(move_types::tank_rush, tank_rush_params(target_pos, false, 127.0, 1.0, false));
        drivebase.random_turn();

        b_lift.Subsystem::set_state(b_lift_states::intake_off);
        f_lift.set_state(f_lift_states::move_to_target, 1);
      }

      else if(target_task == "Back"){
        b_lift.set_state(b_lift_states::move_to_target, 0);
        wait_until(B_LIFT_AT_BOTTOM);

        Task([](){detect_interference();});
        move_start(move_types::tank_point, tank_point_params(target_pos, false, 127.0, 1.0, true, 6.4, 70.0, 0.0, 0, {5, 5}));
        move_start(move_types::turn_angle, turn_angle_params(angle_to_point(target_pos.x, target_pos.y)+180.0, true, true, 5.0, 0.0, 10.0, 20.0, 127.0, 0, 50, 3.0));
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
        move_start(move_types::turn_angle, turn_angle_params(angle_to_point(target_pos.x, target_pos.y)+180.0, true, true, 5.0, 0.0, 10.0, 20.0, 127.0, 0, 50, 3.0));
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
