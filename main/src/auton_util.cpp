#include "auton_util.hpp"

double get_filtered_output(ADIUltrasonic sensor, int check_count, uint16_t lower_bound, uint16_t upper_bound, int timeout){
  Timer timer{"Timer"};
  int success_count = 0;

  long total_input;
  uint16_t input;
  double filtered_output;
  while(timer.get_time() < timeout && success_count < check_count){
    input = sensor.get_value();
    if (lower_bound <= input <= upper_bound){

      total_input += input;
      printf("input: %d\n", input);
      success_count++;
    }
    else printf("FAILED! input: %d\n", input);
    delay(10);
  }
  filtered_output = total_input / success_count;
  printf("filtered output: %lf\n", filtered_output);
  return filtered_output;
}

void flatten_against_wall(bool right){



  if(right){

  }
  else{
    drivebase.move(-70, 0, -10);
	// cycleCheck(fabs(rad_to_deg(tracking.g_velocity.angle)) >  0.01, 15,10);
	  int bad_count = 0;
    int good_count = 0;
    while(good_count < 15 && bad_count < 30){ 
      if(fabs(rad_to_deg(tracking.g_velocity.angle)) >  0.01)good_count++;
      else{
        good_count = 0;
        bad_count++;
      }
      printf("good: %d bad: %d, v: %f\n", good_count, bad_count, fabs(rad_to_deg(tracking.g_velocity.angle)));
      delay(10);
    }
    cycleCheck(fabs(rad_to_deg(tracking.g_velocity.angle)) <  0.0001, 10,10);

    drivebase.move(-40, 0, -40);
    // cycleCheck(fabs(rad_to_deg(tracking.g_velocity.angle)) >  0.01, 15,10);
    bad_count = 0;
    good_count = 0;
    while(good_count < 10 && bad_count < 30){ 
      if(fabs(rad_to_deg(tracking.g_velocity.angle)) >  0.01)good_count++;
      else{
        good_count = 0;
        bad_count++;
      }
      printf("good: %d bad: %d, v: %f\n", good_count, bad_count, fabs(rad_to_deg(tracking.g_velocity.angle)));
      delay(10);
    }
    cycleCheck(fabs(rad_to_deg(tracking.g_velocity.angle)) <  0.0001, 10,10);
    drivebase.move(-60, 0, 30);
    bad_count = 0;
    good_count = 0;
    while(good_count < 10 && bad_count < 30){ 
      if(fabs(rad_to_deg(tracking.g_velocity.angle)) >  0.01)good_count++;
      else{
        good_count = 0;
        bad_count++;
      }
      delay(10);
    }
    cycleCheck(fabs(rad_to_deg(tracking.g_velocity.angle)) <   0.0001, 10,10);
	  drivebase.move(0,0,0);

  }



}


void find_goal_lift(bool move_stop_b){
  while(!lift_trigger.get_value()){//NEED TO ADD SAFETY
  
    delay(10);

  }
	lift_piston.set_value(HIGH);
  if(move_stop_b)move_stop();
}

void find_goal_tilter(int delay_t){
    while(tilter_dist.get() > 70){
    log_d.print("tilter d: %f\n", tilter_dist.get());
    delay(33);
  }
  log_d.print("%d | grabbing goal 2 at: (%.2f, %.2f, %.2f)\n",millis(),tracking.x_coord, tracking.y_coord,rad_to_deg(tracking.global_angle));
  delay(delay_t);
  tilter_top_piston.set_value(0);
  delay(100);
  tilter_bottom_piston.set_value(1);
  delay(200);
  tilter.move_absolute(300);
  
}


double get_front_dist(){
  double avg = 0.0;
  int count = 0;
  while(true){ 
    // for(int i = 0; i < 3; i++){
    if(front_dist.get() > 10){
      avg+= front_dist.get();
      count++;
    }
    if(count > 2){
      avg/=count;
      break;
    }
    delay(34);
  }
  return ((avg/25.4) +front_dist_dist);
  
}