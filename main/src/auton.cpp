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
  intk_pnue.set_value(0);
  lift.reset();
  tilter_reset();
	lift_piston.set_value(LOW);
	lift.motor.move_absolute(lift.bottom_position, 100);
  intake.move(100);
  

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
  Timer skills_tmr {"time"};
  tilter.move_absolute(tilter.bottom_position);
  tilter_top_piston.set_value(1);
  tilter_bottom_piston.set_value(0);
  move_start(move_types::tank_arc, tank_arc_params({135.0, 42.5},{110.0,42.5,0.0},127.0,127.0,false),true);
  move_start(move_types::point, point_params({109.5,45.5,0.0}), true);
  delay(100);//makes sure allignment motion settles, it not setting causes chance of missing yellow goal
  move_start(move_types::line_old, line_old_params(109.5,42.5,  109.5, 78.0, 0.0,false,true,127, false,0.5,5.0,15.0,0.8,0.2), false);
  while(tilter_dist.get() > 70)delay(10);
  log_d.print("%d | grabbing goal 2 at: (%.2f, %.2f, %.2f)\n",millis(),tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));
  tilter_top_piston.set_value(0);
  delay(50);
  tilter_bottom_piston.set_value(1);
  delay(100);
  tilter.move_absolute(150);
  master.print(0,0,"%d", skills_tmr.get_time());
  move_wait_for_complete();
  
  // move_start(move_types::tank_arc, tank_arc_params({110.0, 75.0},{72.0,108.0,-90.0},127.0,127.0,false),true);
  // move_start(move_types::point, point_params({72.0,108.0,-90.0}),true);
  // move_start(move_types::arc, arc_params({110.0, 78.0},{90.0,92.0,0.0}, 18.0,true, 127.0, true,1.0,false),true);
  move_start(move_types::arc, arc_params({92.0, 49.0},{92.0,89,0.0}, 20.0,true, 127.0, true,1.0,false),true);
  move_start(move_types::point, point_params({52.0,96,-90.0}),true);//x pos was 66 made further to allow for  rings 
  move_start(move_types::point, point_params({68.0,96,-90.0},127.0,false, 0.0, true, 0.0, 0.0,{23.0,0.0001,0.0},{9.5,0.0001,1000.0}),true);

  intake.move(0);
  intk_pnue.set_value(1);
  lift.move_absolute(lift.top_position);
  tilter.move_absolute(tilter.bottom_position);
  while(tilter.motor.get_position() < tilter.bottom_position - 30)delay(10);
  tilter_top_piston.set_value(1);
  tilter_bottom_piston.set_value(0);
	tilter.move_absolute(150);
	while(tilter.motor.get_position() > 300)delay(10);

  // move_start(move_types::turn_angle, turn_angle_params(-180.0),true);
  move_start(move_types::point,point_params({70.0,96.0, -180.0}));
  move_start(move_types::point, point_params({70.0,110.5, -180.0}),true);
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
  move_start(move_types::point, point_params({79.0, 110.5, -180.0}));
  lift_piston.set_value(0);
  delay(200);
  //second goal dropped off

  move_start(move_types::point, point_params({79.0, 106.0, -270.0}));
  lift.move_absolute(lift.bottom_position);
  // move_start(move_types::point, point_params({130.0,132.0,-300.0}));
  // move_start(move_types::tank_arc, tank_arc_params({79.0, 106.0},{130.0,132.0,-300.0}));
  move_start(move_types::point, point_params({100.0,108.0,-270.0},127.0,true,0.0,false));
  move_start(move_types::tank_arc, tank_arc_params({79.0, 106.0},{125.0,127.0,-285.0},127.0,127.0,false));
	move_start(move_types::point, point_params({125.0,133.0,-270.0}));
}
