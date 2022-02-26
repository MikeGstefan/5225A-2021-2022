#include "auton.hpp"




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
  move_stop();
  // while(!master.get_digital(DIGITAL_L1))delay(10);

	move_start(move_types::turn_point, turn_point_params({70.0,98.0}, true));  // turns to face plus of rings
	delay(100);
  // Task([](){
  //   delay(100);
  //   f_lift.move_absolute(580);
  //   b_lift.move_absolute(700);
  // });
	move_start(move_types::tank_point, tank_point_params({70.0, 95.0, 45.0},false, 80.0, 1.0, true,9.0,130.0),false); // drives in front of platform
  // return;
  delay(100);
    f_lift.move_absolute(580);
    b_lift.move_absolute(700);
  move_wait_for_complete();
	delay(100);
	move_start(move_types::turn_angle, turn_angle_params(180.0,true, true, 150.0,80.0)); // turns to face platform
  delay(100);
  // return;
  // while(true)delay(10);
  // b_lift.move_absolute(700);
  while(b_lift.motor.get_position() < 680)delay(10);
  // delay(2000);
  //vision_loop(15, 1500);
  drivebase.brake();
  b_lift.move_absolute(600);
  while(b_lift.motor.get_position() > 620)delay(10);
  move_start(move_types::turn_angle, turn_angle_params(180.0,true, true, 170.0,0.0, 127.0, 500));
  b_claw_p.set_value(0);
  delay(200);
  b_lift.move_absolute(600);
  while(b_lift.motor.get_position() < 580)delay(10);
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
  move_start(move_types::turn_angle, turn_angle_params(20.0, true, false, 160.0,0.0,70.0));
  delay(200);
   misc.print("%d || starting drive towards  plat\n", millis());
  drivebase.move(30.0,0.0);
  tracking.wait_for_dist(2.0, 750);
  misc.print("%d || done drive towards plat\n", millis());
  f_claw_p.set_value(0);
  delay(400);
  move_start(move_types::tank_point, tank_point_params({70.0, 100.0, 90.0},false, 127.0, 1.0, true,9.0,130.0));

  move_start(move_types::turn_angle, turn_angle_params(90.0));
  delay(100);
  move_start(move_types::tank_point, tank_point_params({124.0,96.0,110.0},false, 100.0, 1.0,true,8.0,150.0,0.0));
  move_start(move_types::tank_point, tank_point_params({120.0,125.0,180.0},false),false);
  delay(200);
  b_lift.move_absolute(10);
  f_lift.move_absolute(10);
  move_wait_for_complete();
  move_start(move_types::turn_angle, turn_angle_params(180.0));
  delay(100);

	// delay(100);
	// move_start(move_types::tank_arc, tank_arc_params({70.0, 100.0}, {123.0,119.0,-180.0},-127.0));
	// move_start(move_types::turn_angle, turn_angle_params(-180.0,true,true,160.0,0.0,90.0));
  // delay(100);
	// // move_start(move_types::tank_point, tank_point_params({122.0,123.0,0.0},false));
  // delay(2000);
	flatten_against_wall(false);
  delay(100);
  tracking.reset(141.0 - reset_dist_r.get_dist(),141.0 - DIST_BACK, 180.0);
  delay(200);
  master.print(1,1,"%.2f, %.2f, %.2f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));
  // while(true)delay(10);
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
  //vision_loop(15, 1500);
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

void blue_highside(){
  // tracking.x_coord = 108.0, tracking.y_coord = 16.0, tracking.global_angle = 0.0_deg;
  


  move_start(move_types::tank_rush, tank_rush_params({108.0,71.0,0.0}, false),false);
  // f_lift.reset();
  // f_lift.move(-10);
  // f_detect_goal();
  move_wait_for_complete();
  // f_lift.move_absolute(150,100);
  // intk.move(10);
  move_stop();
  move_start(move_types::tank_point, tank_point_params({108.0,35.0,0.0}, false,127.0,1.0,true,9.0,150.0,0.0,0,{2.0,0.5}),false);
  detect_interference();
  misc.print("%d||here\n",millis());
  move_wait_for_complete();
  f_lift.move_absolute(150,100);
  move_start(move_types::turn_angle, turn_angle_params(-90.0));
  drivebase.brake();
  delay(100);
  move_start(move_types::tank_point, tank_point_params({128.0,35.0,-90.0}, false,127.0,1.0,true,9.0,150.0,0.0,0,{1.0,0.5}),false);
  b_detect_goal();
  move_stop();
  delay(200);
  intk.move(127);
  delay(2000);
}



void blue_highside_tall(){
  //tracking.x_coord = 104.0, tracking.y_coord = 12.0, tracking.global_angle = -30.0_deg;


  // Task([](){
    // f_lift.reset();
    // f_lift.move(-10);
  // });
  // drivebase.set_state(1);
  move_start(move_types::tank_rush, tank_rush_params({71.0,71.0,30.0},false), false);
  // delay(200);
  // drivebase.set_state(0);
  f_lift.reset();
  f_lift.move(-10);
  move_wait_for_complete();
  // drivebase.set_state(0);
  move_start(move_types::tank_point, tank_point_params({91.0,35.0,30.0}, false,127.0,1.0,true,9.0,150.0,0.0,0,{2.0,0.5}),false);
  detect_interference();
  move_wait_for_complete();

  f_lift.move_absolute(150,100);
  move_start(move_types::turn_angle, turn_angle_params(-90.0));
  
  delay(100);
  move_start(move_types::tank_point, tank_point_params({128.0,35.0,-90.0}, false,100.0,1.0,true,9.0,150.0,0.0,0,{2.0,0.5}),false);
  b_detect_goal();
  move_stop();
  delay(200);
  intk.move(50);
  delay(2000);
}

void blue_lowside(){
  // tracking.x_coord = 24.5, tracking.y_coord = 15.0, tracking.global_angle = 9.0_deg;


  
  move_start(move_types::tank_rush, tank_rush_params({35.0,71.0,9.0}, false),false);
  f_lift.reset();
  f_lift.move(-10);
  b_lift.reset();
  b_lift.move(-5);
  // f_detect_goal();
  move_wait_for_complete();
  // f_lift.move_absolute(150,100);
  // intk.move(10);
  move_stop();
  move_start(move_types::tank_point, tank_point_params({18.0,18.0,0.0}, false,127.0,1.0,true,9.0,150.0,0.0,0,{2.0,0.5}),false);
  detect_interference();
  misc.print("%d||here\n",millis());
  move_wait_for_complete();
  f_lift.move_absolute(60,100);
  move_start(move_types::turn_angle, turn_angle_params(-60.0,true, true,150.0,0.0,80));
  drivebase.brake();
  delay(100);
  move_start(move_types::tank_point, tank_point_params({35.0,14.0,-60.0}, false,80.0),false);
  b_detect_goal();
  move_stop();
   f_lift.move_absolute(150,100);
  drivebase.brake();
  intk.move(50);
  
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
