#include "auton.hpp"

void lrt_auton(){
  Timer timer{"timer"};
	move_start(move_types::point, point_params({5.0,8.0, 45.0}),false);
  tilter.reset();
  lift.reset();
  // lift.move_absolute(250);
  move_wait_for_complete();
	intake.move(100);
	move_start(move_types::point, point_params(polar_to_vector_point(5.0,8.0,17.5,45.0,45.0),127.0,false,0.0,true,0.0,0.0,{18.0,0.0,20.0},{12.0,0.0001,1000.0},{125.0,0.0,1000.0}),false);
	// move_wait_for_error(2.0);

	move_wait_for_complete();
	move_start(move_types::point, point_params(polar_to_vector_point(5.0,8.0,13.0,45.0,45.0),127.0,false,0.0,true,0.0,0.0,{18.0,0.0,100.0},{12.0,0.0001,1000.0},{125.0,0.0,1000.0}),false);
	tilter.move_absolute(tilter.bottom_position);
	while(tilter.motor.get_position() < tilter.bottom_position - 10)delay(10);
	// move_wait_for_complete();
	// move_wait_for_error(2.0);
	delay(100);
	tilter_bottom_piston.set_value(0);
	delay(100);
	move_wait_for_complete();
	move_start(move_types::point, point_params(polar_to_vector_point(5.0,8.0,10.0,45.0,45.0),127.0,false,0.0,true,0.0,0.0,{18.0,0.0,100.0},{12.0,0.0001,1000.0},{125.0,0.0,1000.0}),true);
	// move_start(move_types::point, point_params({-3.0, 18.0,0.0}));
	// move_start(move_types::turn_point, turn_point_params({-2.5, 92.5}));
  // return;
	move_start(move_types::turn_angle, turn_angle_params(-15.0));
	move_start(move_types::line_old, line_old_params(12.75, 15.75, -5.5, 92.5,-15.0,false,true,127, false,2.5,10.0,15.0,0.6,0.2),false); // drives through 1st small neutral mogo
	// move_start(move_types::point, point_params({-2.5, 92.5, rad_to_deg(atan2(-2.5 - tracking.x_coord, 92.5 - tracking.y_coord))}),false);
	find_goal_tilter(0);
  move_stop();
  move_start(move_types::point, point_params({-9.5, 92.5,-15.0},127.0,true, 0.0,true, 0.0,0.0,{23.0,0.0,0.0},{9.5,0.0,1000.0},{125.0,0.0,1000.0},0,2.0,10.0));
	// move_wait_for_complete();
	move_start(move_types::point, point_params(polar_to_vector_point(-3.5, 92.5, -14.0, -20,-20))); // backs up from goal
	move_wait_for_error(2.0);
	tilter.move_absolute(tilter.bottom_position);
  	while(tilter.motor.get_position() < tilter.bottom_position - 30)delay(10);
  	tilter_top_piston.set_value(1);
  	tilter_bottom_piston.set_value(0);
	tilter.move_absolute(300);
	while(tilter.motor.get_position() > 310)delay(10);
	move_wait_for_complete();
	move_start(move_types::point, point_params({3.0,75.0,-105.0}));


	//going for center goal now
	tilter.move_absolute(tilter.bottom_position);
	// move_start(move_types::point, point_params(polar_to_vector_point(3.0,75.0,32.0,-110.0,-110.0)),false);
	move_start(move_types::line_old, line_old_params(3.0, 75.0, -27.0, 64.5, -105, false,true,127, false,2.5,10.0,15.0,0.6,0.2), false);
	find_goal_tilter();
	move_stop();
	// drivebase.move(0,0,0);
	move_start(move_types::tank_arc, tank_arc_params({-27.0, 64.0}, {-55.0, 41.0, -135.0}, 127.0, 127.0, false));
	move_start(move_types::point, point_params({-59.0, 36.0, -135.0},127.0,true, 0.0,true, 0.0,0.0,{23.0,0.0,0.0},{9.5,0.0,1000.0},{125.0,0.0,1000.0},0,2.0,10.0));
	master.print(0,0,"%d",timer.get_time());
}

void red_tall_rush(){

}

void skills(){
  intake_piston.set_value(0);
  lift.reset();
  tilter.reset();
	lift_piston.set_value(LOW);
	lift.motor.move_absolute(lift.bottom_position, 100);
  intake.move(100);
  Timer skills_tmr {"time"};

// move_start(move_types::tank_point, tank_point_params({112.5, 14.75, -90.0},true,50.0), false);
	Timer move_timer{"move"};
  move_start(move_types::point, point_params({112.5, 14.75, -90.0}),true);
  // move_start(move_types::line, line_params({144.0 - 10.25, 14.75},{112.5, 14.75, -90.0},127.0),false);
	// move_to_point();
	// move_timer.print();
	// tank_move_on_arc({110.0, 14.75}, {132.0, 24.0, -180.0}, 127.0);
	// move_on_line(polar_to_vector(110.0, 14.75, 10.0, 45.0, -135.0));

  // grabs alliance goal
  // X: 125.56 Y: 30.31 A: -135.00
  move_start(move_types::point, point_params(polar_to_vector_point(110.0, 14.75, 24.0, 45.0, -135.0), 127.0, false, 1.0, false), false);
  // move_start(move_types::point, point_params({125.56 , 30.31, -135.00}, 127.0, false, 1.0, false), false);
  // move_start(move_types::line, line_params({112.5, 14.75}, {125.56 , 30.31, -135.00}, 127.0, false, 1.0, false), false);

  // move_wait_for_complete();
  while(!lift_trigger.get_value()){//NEED TO ADD SAFETY

    delay(10);

  }
	lift_piston.set_value(HIGH);
  move_stop();

  intake.move(0);
  tilter.move_absolute(tilter.bottom_position);
  tilter_top_piston.set_value(1);
  tilter_bottom_piston.set_value(0);
  move_start(move_types::tank_arc, tank_arc_params({135.0, 42.5},{110.0,42.5,0.0},127.0,127.0,false),true);
  move_start(move_types::point, point_params({109.5,45.5,0.0}), true);
  delay(100);//makes sure allignment motion settles, it not setting causes chance of missing yellow goal
  move_start(move_types::line_old, line_old_params(109.5,42.5,  109.5, 78.0, 0.0,false,true,127, false,0.5,5.0,15.0,0.8,0.2), false);
  while(tilter_dist.get() > 70){
    log_d.print("tilter d: %f\n", tilter_dist.get());
    delay(33);
  }
  log_d.print("%d | grabbing goal 2 at: (%.2f, %.2f, %.2f)\n",millis(),tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));
  tilter_top_piston.set_value(0);
  delay(100);
  tilter_bottom_piston.set_value(1);
  delay(200);
  tilter.move_absolute(300);

  master.print(0,0,"%d", skills_tmr.get_time());
  move_wait_for_complete();
  intake.move(100);


  // move_start(move_types::tank_arc, tank_arc_params({110.0, 75.0},{72.0,108.0,-90.0},127.0,127.0,false),true);
  // move_start(move_types::point, point_params({72.0,108.0,-90.0}),true);
  // move_start(move_types::arc, arc_params({110.0, 78.0},{90.0,92.0,0.0}, 18.0,true, 127.0, true,1.0,false),true);
  move_start(move_types::arc, arc_params({92.0, 49.0},{92.0,89,0.0}, 20.0,true, 127.0, true,1.0,false),true);
  move_start(move_types::point, point_params({50.0,96,-90.0}),true);//x pos was 66 made further to allow for  rings
  move_start(move_types::point, point_params({66.0,96,-90.0},127.0,false, 0.0, true, 0.0, 0.0,{23.0,0.0001,0.0},{9.5,0.0001,1000.0},{ 120.0,0.0,1500.0}),true);

  intake.move(0);
  intake_piston.set_value(1);
  lift.move_absolute(lift.top_position);
  tilter.move_absolute(tilter.bottom_position);
  while(tilter.motor.get_position() < tilter.bottom_position - 30)delay(10);
  tilter_top_piston.set_value(1);
  tilter_bottom_piston.set_value(0);
	tilter.move_absolute(300);
	while(tilter.motor.get_position() > 300)delay(10);

  // move_start(move_types::turn_angle, turn_angle_params(-180.0),true);
  move_start(move_types::point,point_params({68.0,96.0, -180.0}));
  move_start(move_types::point, point_params({68.0,109.5, -180.0}),true);
  lift.move_absolute(lift.platform_position);
  while(lift.motor.get_position() > lift.platform_position +10)delay(10);//first goal on platform position
  delay(100);
  lift_piston.set_value(0);
  delay(200);

  move_start(move_types::point, point_params({74.0,97,-180.0},127.0,false, 0.0, true, 0.0, 0.0,{23.0,0.0001,0.0},{9.5,0.0001,1000.0}),false);
  // move_wait_for_error
  // delay(500);
  while(tracking.y_coord > 105.0)delay(10);
  lift.move_absolute(lift.bottom_position);
  move_wait_for_complete();
  move_start(move_types::turn_angle, turn_angle_params(-270.0),true);
  move_start(move_types::point, point_params({60.0,97,-270.0},127.0,false, 0.0, true, 0.0, 0.0,{23.0,0.0001,0.0},{9.5,0.0001,1000.0}),false);
  // move_start(move_types::tank_arc, tank_arc_params({70.0,109.0},{74.5,96.0,-270},127.0,127.0,true),true);
  find_goal_lift();
  lift.move_absolute(lift.platform_position);
  move_start(move_types::point, point_params({79.0, 97.0, -270.0}));
  move_start(move_types::turn_angle, turn_angle_params(-180.0));
  move_start(move_types::point, point_params({79.0, 109.5, -180.0}));
  lift_piston.set_value(0);
  delay(200);
  //second goal dropped off

  move_start(move_types::point, point_params({79.0, 106.0, -270.0}));
  lift.move_absolute(lift.bottom_position);
  // move_start(move_types::point, point_params({130.0,132.0,-300.0}));
  // move_start(move_types::tank_arc, tank_arc_params({79.0, 106.0},{130.0,132.0,-300.0}));
  move_start(move_types::point, point_params({100.0,108.0,-270.0},127.0,true,0.0,false));
  move_start(move_types::tank_arc, tank_arc_params({79.0, 106.0},{125.0,127.0,-285.0},127.0,127.0,false));
	move_start(move_types::point, point_params({125.0,131.0,-270.0},127.0,false, 0.0, true, 0.0, 0.0,{23.0,0.000,0.0},{9.5,0.000,1000.0},{ 125.0,0.0,1000.0},3000));
  // flatten_against_wall(false);
  // delay(100);
  // tracking.reset(144.0 - get_front_dist(), 144.0- 9.0, 90.0);
  // master.print(0,0,"%.2f, %.2f, %.2f",tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
  master.print(1,1,"time: %d", skills_tmr.get_time());

}


void skills_pt2(){
  intake_piston.set_value(0);
  lift.reset();
  tilter.reset();
	lift_piston.set_value(LOW);
	lift.motor.move_absolute(lift.bottom_position, 100);
  tilter.move_absolute(tilter.bottom_position);
  tilter_top_piston.set_value(1);
  tilter_bottom_piston.set_value(0);


  flatten_against_wall(false);
  delay(100);
  double dist = get_front_dist();
  tracking.reset(144.0 - dist, 144.0- 9.0, 90.0);
  master.print(0,0,"%.2f, %.2f", tracking.x_coord, tracking.y_coord);
  // move_start(move_types::point, point_params({118.0, 134.0, 80.0}),true);
  // move_start(move_types::point, point_params({111.0, 133.0, 80.0}),false);
  // find_goal_lift(false);
  // // move_start(move_types::point, point_params({114.0, 114.0, 225.0}));
  // move_start(move_types::tank_arc, tank_arc_params({111.0, 133.0}, {115.0, 115.0, 225.0},127.0, 127, false));
  // move_start(move_types::point, point_params({110.0,110.0,225.0}, 127, false, 0.0, false));
  // delay(1000);
  move_start(move_types::point, point_params({48.0,48.0,225.0}),false);
  while(tilter_dist.get() > 70){
    log_d.print("tilter d: %f\n", tilter_dist.get());
    delay(33);
  }
  log_d.print("%d | grabbing goal 4 at: (%.2f, %.2f, %.2f)\n",millis(),tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));
  tilter_top_piston.set_value(0);
  delay(100);
  tilter_bottom_piston.set_value(1);
  delay(200);
  tilter.move_absolute(300);
  move_wait_for_complete();
  // move_start(move_types::line_old, line_old_params(115.0,115,  48.0, 48.0, 225.0,false,true,127, false,0.5,5.0,15.0,0.8,0.2), false);
}
