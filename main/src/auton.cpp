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
  move_start(move_types::point, point_params(polar_to_vector_point(110.0, 14.75, 22.0, 45.0, -135.0), 127.0, false, 1.0, false), false);
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
  // move_start(move_types::arc, arc_params({135.0, 42.5},{113.0,42.5,0.0},12.0,true, 127.0, true,1.0,false, 10000.0,127.0),true); //slower
  // move_start(move_types::turn_point, turn_point_params({108.0, 60.0}), true); // turns to face neutral mogo //slowest
	// move_start(move_types::point, point_params({110.0, 76.0, 0.0}, 127.0, false, 0.0, true),false);  // lines up on neutral mogo
  move_start(move_types::line, line_params({110.0,42.5},{110.0, 76.0, 0.0}),false);
  while(tilter_dist.get() > 70)delay(10);
  tilter_top_piston.set_value(0);
  tilter_bottom_piston.set_value(1);
  delay(100);
  tilter.move_absolute(tilter.raised_position);
  master.print(0,0,"%d", skills_tmr.get_time());
  move_wait_for_complete();
  
  move_start(move_types::tank_arc, tank_arc_params({110.0, 76.0},{72.0,108.0,-90.0},127.0,127.0,false),true);
  move_start(move_types::point, point_params({72.0,108.0,-90.0}),true);

  // point_params(const Position target, const double max_power = 127.0, const bool overshoot = false, const double min_angle_percent = 0.0, const bool brake = true, const double decel_dist = 0.0, const double decel_speed = 0.0);


  // move_start(move_types::point, point_params({110.0, 80.0, 0.0}, 127.0, false, 0.0, false),true);  // drives through netural mogo
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
