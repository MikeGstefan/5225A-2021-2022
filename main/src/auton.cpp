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
  // while(!master.get_digital(DIGITAL_L1))delay(10);

	move_start(move_types::turn_point, turn_point_params({70.0,98.0}, true));  // turns to face plus of rings
	delay(100);
  // Task([](){
  //   delay(100);
  //   f_lift.move_absolute(580);
  //   b_lift.move_absolute(700);
  // });
	move_start(move_types::tank_point, tank_point_params({68.0, 95.0, 45.0},false, 80.0, 1.0, true,9.0,130.0),false); // drives in front of platform
  // return;
  delay(100);
    f_lift.move_absolute(580);
    b_lift.move_absolute(700);
  move_wait_for_complete();
	delay(100);
	move_start(move_types::turn_angle, turn_angle_params(180.0,true, true, 120.0,0.0,80.0)); // turns to face platform
  delay(100);
   move_start(move_types::turn_angle, turn_angle_params(180.0,true, true, 170.0,0.0, 127.0, 500));
  // return;
  // while(true)delay(10);
  // b_lift.move_absolute(700);
  while(b_lift.motor.get_position() < 680)delay(10);
  // delay(2000);
  // distance_loop(15, 1500);
  move_start(move_types::tank_point, tank_point_params({68.0,110.0, 180.0}, false, 50.0,1.0,true,9.0,150.0,0.0,1500));
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
   move_start(move_types::tank_point, tank_point_params({71, 108.0, 20.0}, false, 70.0,1.0, true, 6.5, 150.0,0.0,1000));
  // drivebase.move(30.0,0.0);
  // tracking.wait_for_dist(2.0, 750);
  move_start(move_types::turn_angle, turn_angle_params(0.0, true, false, 190.0,0.0,90.0, 750));

  // move_start(move_types::turn_angle, turn_angle_params(0.0, true, false, 160.0,0.0,70.0));
  misc.print("%d || done drive towards plat\n", millis());
  f_claw_p.set_value(0);
  delay(400);
  drivebase.move(0,0);
  // move_start(move_types::tank_point, tank_point_params({70.0, 108.0, 0.0},false, 127.0, 1.0, true,9.0,120.0, 0.0, 0, {3.0,0.5}));
  drivebase.move(-40.0,0.0);
  while(tracking.y_coord > 108.4)delay(10);
   move_start(move_types::turn_angle, turn_angle_params(-90.0)); 
   drivebase.brake();
   delay(100);
   f_lift.move_absolute(10,100);
   intk.move(0);
   move_start(move_types::tank_point, tank_point_params({128.0, 110.0, -90.0},false, 127.0, 1.0, true,9.0,120.0, 0.0, 0, {1.0,0.5}));
   move_start(move_types::turn_angle, turn_angle_params(-90.0)); 
   drivebase.brake();
   delay(100);
   flatten_against_wall(false);
   delay(100);
}

void skills2(){
  Task([](){
    f_lift.reset();
    f_lift.move(-10);
  });
  
  flatten_against_wall(false);
   delay(100);
   tracking.reset(141.0 - DIST_BACK,141.0 - reset_dist_r.get_dist(), -90.0);
  // Position reset = distance_reset_right(16);
  // master.print(0,0,"%.2f, %.2f, %.2f\n", reset.x, reset.y,rad_to_deg(reset.angle));
  // tracking.reset(141.0 - reset.y,141.0 - reset.x, -90.0 + rad_to_deg(reset.angle));
  master.print(1,1,"%.0f, %.0f, %.0f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));

  delay(100);

  // move_start(move_types::)
  // while(!master.get_digital_new_press(DIGITAL_A))delay(10);
  // f_claw_p.set_value(0);
  // b_claw_p.set_value(0);
  // b_lift.move_absolute(10);
  // f_lift.move_absolute(10);
  // delay(2000);
  // flatten_against_wall(false);
  // delay(100);
  // tracking.reset(141.0 - reset_dist_r.get_dist(),141.0 - DIST_BACK, 180.0);
  // // delay(200);
  

//21, 12.5

  move_start(move_types::tank_point, tank_point_params({tracking.x_coord - 3.0, tracking. y_coord, -90.0}));
  move_start(move_types::turn_point, turn_point_params({120.0,127.0}));
  // move_start(move_types::turn_angle, turn_angle_params(-45.0, true, true, 150.0,0.0,127.0,0, min_move_power_a, 3.0));
  delay(100);
  move_start(move_types::tank_point, tank_point_params({120.0,127.0,-45.0},false,70.0,1.0,true,6.0,150.0,0.0),false);
  Task([](){
    // delay(400);
    b_lift.reset();
  });
  move_wait_for_complete();
  move_start(move_types::turn_angle, turn_angle_params(-90.0, true,true,140.0));
  move_start(move_types::tank_point, tank_point_params({107.0,129.0,-90.0},false,70.0,1.0,true,6.0,150.0,0.0),false);
  
   f_detect_goal();
   f_lift.move_absolute(150);
  move_stop();
  drivebase.brake();
  move_start(move_types::tank_arc, tank_arc_params({tracking.x_coord, tracking.y_coord}, {120.5, 102.0,20.0},-127.0,127.0,false));//arc to face neut goal
  move_start(move_types::turn_angle, turn_angle_params(rad_to_deg(atan2(104.5 - tracking.x_coord, 72.0 - tracking.y_coord)) +180.0, true,true,140.0));
  move_start(move_types::tank_point, tank_point_params({104.5,70.0,  45.0},false, 80.0,1.0,true,9.0,130.0),false);// drive throught neut goal
  b_detect_goal();
  b_lift.move_absolute(620);
  move_stop();
  // while(!master.get_digital(DIGITAL_L1))delay(10);
  // move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(108,70.0,-40.0,30.0,30.0)));
  f_lift.move_absolute(580);
  move_start(move_types::tank_point, tank_point_params({85.0,33.0, 30.0}, false, 70.0));
  // flatten_against_wall(false);

b_claw_p.set_value(0);

  int safety_check = 0;
  //bool to + -
  // int direction = (static_cast<int>(front)*2)-1;
  tracking_imp.print("%d|| Start wall allign\n", millis());

	drivebase.move(-50.0,0.0);

	while((fabs(tracking.l_velo) < 2.0 ||fabs(tracking.r_velo) < 2.0) && safety_check < 12){
		safety_check++;
    misc.print(" reset things %.2f, %.2f\n",fabs(tracking.l_velo), fabs(tracking.r_velo));
		delay(10);
	}
	cycleCheck(fabs(tracking.l_velo) <1.0 && fabs(tracking.r_velo) < 1.0, 1,10);
	drivebase.move(-20.0,0.0);
	printf("%d|| Done all allign\n", millis());

  move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(tracking.x_coord, tracking.y_coord, 2, rad_to_deg(tracking.global_angle), rad_to_deg(tracking.global_angle))));

  move_start(move_types::turn_angle, turn_angle_params(200.0, true,false, 150.0,0.0,80.0, 1500));
  f_lift.move_absolute(550);
  delay(100);
  // move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(tracking.x_coord, tracking.y_coord, 5, rad_to_deg(tracking.global_angle), rad_to_deg(tracking.global_angle))));
  // move_start(move_types::turn_angle, turn_angle_params(210.0, true,false, 150.0,0.0,80.0));
  f_claw_p.set_value(0);
  b_lift.move_absolute(10);
  move_start(move_types::tank_point, tank_point_params(polar_to_vector_point(tracking.x_coord, tracking.y_coord, 2, rad_to_deg(tracking.global_angle), rad_to_deg(tracking.global_angle)), false,127.0,1.0,true,9.0,150.0,0.0,1000));
  move_start(move_types::tank_point, tank_point_params({83.0,50.0, 180.0}));
  move_start(move_types::turn_angle, turn_angle_params(135.0));
  f_lift.move_absolute(10);
  delay(100);
  move_start(move_types::tank_point, tank_point_params({71.0,71.0, 135.0}), false);
  b_detect_goal();
  // b_lift.move_absolute(750);
  move_stop();
  // move_start(move_types::tank_arc, tank_arc_params({71.0, 71.0}, {100.0, 36.0, 90.0}, 127.0,127.0,false,9.0,2.0,80.0));
  move_start(move_types::tank_point, tank_point_params({100.0, 36.0, 90.0}));
  move_start(move_types::turn_angle, turn_angle_params(90.0));
  b_claw_p.set_value(0);
  move_start(move_types::tank_point, tank_point_params({129.0, 36.0, 90.0}, false,70.0), false);

  // move_start(move_types::tank_point, tank_point_params({129.0, 36.0, 135.0}, false,80.0),false);
  f_detect_goal();
  move_stop();
  move_start(move_types::tank_point, tank_point_params({115.0, 36.0, 90.0}),false);
  delay(200);
  f_lift.move_absolute(620);
  while(f_lift.motor.get_position() < 580)delay(10);
  move_stop();
  drivebase.move(70.0,0.0);
  tracking.wait_for_dist(2.0);
  flatten_against_wall();
  delay(100);
   tracking.reset(141.0 - DIST_FRONT,reset_dist_r.get_dist(), 90.0);
  // Position reset = distance_reset_right(16);
  // master.print(0,0,"%.2f, %.2f, %.2f\n", reset.x, reset.y,rad_to_deg(reset.angle));
  // tracking.reset(141.0 - reset.y,141.0 - reset.x, -90.0 + rad_to_deg(reset.angle));
  master.print(1,1,"%.0f, %.0f, %.0f", tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));

  delay(100);
  b_lift.move_absolute(130);

  move_start(move_types::tank_point, tank_point_params({16.0,122.25,135.0}, false, 127.0, 1.0,true));
  move_start(move_types::tank_point, tank_point_params({18.0,120.25,135.0}, false, 127.0, 1.0,true, 6.5,150.0,0.0,1000,{1.5,1.0}));
  // move_start(move_types::tank_point, tank_point_params({108.0, 36.0, 90.0}));
  move_start(move_types::turn_angle, turn_angle_params(80.0, true, true,170.0,0.0,127.0,1000,min_move_power_a,3.0));
  delay(100);
  move_start(move_types::turn_angle, turn_angle_params(80.0, true, true,140.0,0.0,127.0,400,min_move_power_a,3.0));
  b_lift.move_absolute(200);
  drivebase.set_state(1);
  drivebase.move(70.0,0.0);
  f_lift.move_absolute(470);

  tracking.wait_for_dist(6.0);
  // b_lift.move_absolute(700);
  // move_start(move_types::tank_point, tank_point_params({108.0, 15.0, 0.0}));
  flatten_against_wall(true, 2);
  f_claw_p.set_value(0);
  
}

void new_skills3(){
  // f_lift.move_absolute(470);
  // while(f_lift.motor.get_position() < 460)delay(10);
  // drivebase.move(70.0,0.0);
  

  // tracking.wait_for_dist(6.0);
  // b_lift.move_absolute(700);
  // move_start(move_types::tank_point, tank_point_params({108.0, 15.0, 0.0}));
  // flatten_against_wall(true);
  drivebase.move(5.0,0.0);
  // f_claw_p.set_value(0);
  delay(100);
  drivebase.set_state(0);



  tracking.reset(42.0, 141.0 - reset_dist_l.get_dist(), 90.0);
  delay(100);
  master.print(0,0,"%.2f %.2f %.2f", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));

  move_start(move_types::tank_point, tank_point_params({21.0,129.0, 90.0}), false);
  // tracking.wait_for_dist(3.0);
  // f_lift.move_absolute(600);
  move_wait_for_complete();
  b_claw_p.set_value(0);
  move_start(move_types::tank_point, tank_point_params({26.0,128, 90.0}));
  move_start(move_types::turn_angle, turn_angle_params(45.0));
  b_lift.move_absolute(10);
  move_start(move_types::tank_point, tank_point_params({15.0, 111.0, 45.0}, false,90.0),false);
  b_detect_goal();
  delay(100);
  move_stop();
  // drivebase.brake();
  drivebase.move(0.0,0.0);
  delay(200);
  intk.move(127);
  move_start(move_types::tank_arc, tank_arc_params({tracking.x_coord, tracking.y_coord}, {28.0, 98.0, 190.0}, 90.0,90.0,true, 4.0,0.0,40.0));
  move_start(move_types::tank_point, tank_point_params({24.0, 70.0,180.0}));
  move_start(move_types::tank_arc, tank_arc_params({24.0, 70.0}, {60.0, 38.0, 135.0}, 127.0));
  move_start(move_types::turn_angle, turn_angle_params(90.0, true,true,150.0,0.0,127.0,750));
  move_start(move_types::tank_point, tank_point_params({72.0, 39.0,90.0}, false,127.0,1.0,true,6.5,150.0,0.0,2000));
  // b_lift.move_absolute(650);
  move_start(move_types::turn_angle, turn_angle_params(0.0, true,true,150.0,0.0,127.0,1000));
  // while(b_lift.motor.get_position() < 550)delay(10);
  // move_start(move_types::tank_point, tank_point_params({72.0, 32.0,0.0}, false,127.0,1.0,true, 6.5,150.0,0.0,1000,{2.0,0.5}));
  // move_start(move_types::turn_angle, turn_angle_params(0.0));
  b_claw_p.set_value(0);
  // move_start(move_types::tank_point, tank_point_params({72.0, 39.0,0.0}, false,127.0,1.0,true, 6.5,150.0,0.0,1000,{2.0,0.5}));
  // move_start(move_types::turn_angle, turn_angle_params(0.0));
  // move_start(move_types::tank_point, tank_point_params({72.0, 30.0,0.0}, false,127.0,1.0,true, 6.5,150.0,0.0,1000,{2.0,0.5}));
  // move_start(move_types::tank_point, tank_point_params({72.0, 45.0,0.0}, false,127.0,1.0,true, 6.5,150.0,0.0,0,{2.0,0.5}));
  // move_start(move_types::turn_angle, turn_angle_params(-100.0));
  

  // move_start(move_types::tank_point, tank_point_params({28.0,125.0, 90.0}));
  // b_lift.move_absolute(480);
  // move_start(move_types::turn_angle, turn_angle_params(-90.0, true,true,150.0,0.0,90.0,0,min_move_power_a, 3.0));

  // // flatten_against_wall(false);
  // while(b_lift.motor.get_position()>500)delay(10);
  // // b_claw_p.set_value(0);
  // // delay(500);
  // int safety_check = 0;
  // tracking_imp.print("%d|| Start wall allign\n", millis());
  // // Task([](){ 
  // //   while(tracking.x_coord < 30)delay(10);
  // //   b_claw_p.set_value(0);
  // // });
  // drivebase.set_state(1);
	// // int safety_check = 0;
  // // tracking_imp.print("%d|| Start wall allign\n", millis());
  // // drivebase.set_state(1);
  // move_start(move_types::tank_point, tank_point_params({35.0,tracking.y_coord, -90.0}));
	// // drivebase.move(-80.0,0.0);

	// // while((fabs(tracking.l_velo) < 2.0 ||fabs(tracking.r_velo) < 2.0) && safety_check < 20){
	// // 	safety_check++;
  // //   misc.print(" reset things %.2f, %.2f\n",fabs(tracking.l_velo), fabs(tracking.r_velo));
	// // 	delay(10);
	// // }
	// // cycleCheck(fabs(tracking.l_velo) <1.0 || fabs(tracking.r_velo) < 1.0, 2,10);
  // // while(tracking.x_coord < 35.0)delay(10);
  // printf("end_coord : %.2f\n", tracking.x_coord);
	// // drivebase.move(0.0,0.0);
  // b_claw_p.set_value(0);
  // delay(500);
  // drivebase.move(80.0,0.0);
  // tracking.wait_for_dist(1.5);
  // // drivebase.move(0.0,0.0);
  // drivebase.brake();
  
  // // drivebase.move(0.0,0.0);
  // delay(200);
  // drivebase.move(-80.0,0.0);
  // while(tracking.x_coord < 35.5)delay(10);
  // drivebase.move(0.0,0.0);
  // delay(500);
  // drivebase.move(80.0,0.0);
  // tracking.wait_for_dist(3.0);
  // drivebase.move(0.0,0.0);
  // move_start(move_types::tank_point, tank_point_params({28.0,128.0, 90.0}));
  // while((fabs(tracking.l_velo) < 2.0 ||fabs(tracking.r_velo) < 2.0) && safety_check < 20){
	// 	safety_check++;
  //   misc.print(" reset things %.2f, %.2f\n",fabs(tracking.l_velo), fabs(tracking.r_velo));
	// 	delay(10);
	// }
	// cycleCheck(fabs(tracking.l_velo) <1.0 || fabs(tracking.r_velo) < 1.0, 2,10);
}



void skills3(){

  b_claw_p.set_value(1);


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
  move_start(move_types::turn_angle, turn_angle_params(-90.0));
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
  int time = millis();
  move_start(move_types::tank_rush, tank_rush_params({73.0,71.0,30.0},false), false);
  // move_start(move_types::tank_point, tank_point_params({71.0,71.0,30.0}), false);

  // delay(200);
  // drivebase.set_state(0);
  f_lift.reset();
  f_lift.move(-10);
  // f_detect_goal();
  move_wait_for_complete();
  master.print(2,2,"time: %d", millis() - time);
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
  motion_i.print("motion interrupted at %.2f %.2f %.2f\n", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
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