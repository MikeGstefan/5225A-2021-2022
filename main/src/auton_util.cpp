#include "auton_util.hpp"

static const std::string start_pos_file_name ="/usd/start_position.txt";

//create an adaptable fll type menu selector, look at select_auton();

Reset_dist reset_dist_r(&r_dist, 7.5);
Reset_dist reset_dist_l(&l_dist, 7.5);


void f_claw(bool state){
  f_claw_o.set_state(state);
  f_claw_c.set_state(state);
}



void save_positions(){
  if(GUI::prompt("Reset position", "Press to reset the position, then move the robot.")){
    Position pos1 (141.0-8.75, 15.5, 0.0), pos2 (8.75, 15.5, 0);
    master.clear();
    master.print(0, 0, "L1:(%.1f, %.1f, %.1f)", pos1.x, pos1.y , pos1.angle);
    master.print(1, 0, "R1:(%.1f, %.1f, %.1f)", pos2.x, pos2.y , pos2.angle);

    wait_until(false){
      if(master.get_digital_new_press(DIGITAL_L1)){
        tracking.reset(pos1);
        break;
      }
      if(master.get_digital_new_press(DIGITAL_R1)){
        tracking.reset(pos2);
        break;
      }
    }

    printf("\nMove the robot to its new position.\n");

    //move the robot

    if(GUI::prompt_end("Save Positions")){
      tracking_imp.print("Saving X: %f, Y:%f, A:%f\n", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
      
      ofstream file;
      Data::log_t.data_update();
      file.open(start_pos_file_name, fstream::out | fstream::trunc);
      file << tracking.x_coord << endl;
      file << tracking.y_coord << endl;
      file << rad_to_deg(tracking.global_angle) << endl;
      file.close();
      Data::log_t.done_update();
    }
  }
}

void load_positions(){
  double x, y, a;
  ifstream file;

  Data::log_t.data_update();
  file.open(start_pos_file_name, fstream::in);
  file >> x >> y >> a;
  file.close();
  Data::log_t.done_update();

  tracking_imp.print("Loading X: %f, Y:%f, A:%f from file\n", x, y, a);
  tracking.reset(x, y, a);
}

double get_filtered_output(ADIUltrasonic sensor, int check_count, uint16_t lower_bound, uint16_t upper_bound, int timeout){
  Timer timer{"Timer"};
  int success_count = 0;

  long total_input;
  uint16_t input;
  double filtered_output;
  wait_until(timer.get_time() > timeout || success_count > check_count){
    input = sensor.get_value();
    if (in_range(input, lower_bound, upper_bound)){ //This used to be (lower_bound <= input <= upper_bound). I highly doubt that's what you wanted.

      total_input += input;
      printf("input: %d\n", input);
      success_count++;
    }
    else printf("FAILED! input: %d\n", input);
  }
  filtered_output = total_input / success_count;
  printf("filtered output: %lf\n", filtered_output);
  return filtered_output;
}

void flatten_against_wall(bool front, int cycles){ 
  int safety_check = 0;
  //bool to + -
  int direction = (static_cast<int>(front)*2)-1; //use okapi::boolToSign
  tracking_imp.print("%d|| Start wall allign\n", millis());

	drivebase.move(50.0*direction,0.0);

	wait_until((fabs(tracking.l_velo) >= 2.0 && fabs(tracking.r_velo) >= 2.0) || safety_check >= 12){
      safety_check++;
      misc.print(" reset things %.2f, %.2f\n",fabs(tracking.l_velo), fabs(tracking.r_velo));
    }
	cycleCheck(fabs(tracking.l_velo) < 1.0 && fabs(tracking.r_velo) < 1.0, 4, 10);
	drivebase.move(20.0*direction, 0.0);
	printf("%d|| Done all align\n", millis());
}

// double get_front_dist(){
//   double avg = 0.0;
//   int count = 0;
//   while(true){ 
//     // for(int i = 0; i < 3; i++){
//     if(front_dist.get() > 10){
//       avg+= front_dist.get();
//       count++;
//     }
//     if(count > 2){
//       avg/=count;
//       break;
//     }
//     delay(34);
//   }
//   return ((avg/25.4) +front_dist_dist);
  
// }

void b_detect_goal(){ 
  // cycleCheck(b_dist.get() > 80 && b_dist.get() < 90, 5, 33);
  wait_until(!tracking.move_complete);
  while(b_dist.get() > 70 && !tracking.move_complete){ 
    misc.print("looking for edge: %d\n", b_dist.get());
    delay(33);
  }
  int successCount = 0;
    while (successCount < 2 && !tracking.move_complete){
        if (b_dist.get() > 75 && b_dist.get() < 90) {
          successCount++;
          misc.print("found: %d count: %d\n", b_dist.get(), successCount);
        }
        else successCount = 0;
        misc.print("looking: %d\n", b_dist.get());
        delay(33);
    }
  misc.print("Detected %d\n", b_dist.get());
  b_claw.set_value(1);
  // b_claw_p.set_value(1);
}



void f_detect_goal(bool safety){ 
  // cycleCheck(b_dist.get() > 80 && b_dist.get() < 90, 5, 33);
  // while(f_dist.get() > 70){ 
  //   misc.print("looking for edge: %d\n", f_dist.get());
  //   delay(33);
  // }
  // int successCount = 0;
  //   while (successCount < 2){
  //       if (f_dist.get() > 70 && f_dist.get() < 90) successCount++;
  //       else successCount = 0;
  //       misc.print("looking: %d\n", f_dist.get());
  //       delay(33);
  //   }
  if(safety){ 
    wait_until(tracking.move_complete);
    // wait_until(f_touch.get_value() || tracking.move_complete);
  }
  // else wait_until(f_touch.get_value());
  
  misc.print("Detected %d\n", f_dist.get());
  // f_claw.set_state(1);
}


void detect_interference(){ 
  int time = millis();
  wait_until(move_t.get_task_ptr()->get_state() == 4){
    //numbers need funnyimh
    if(millis()-time > 1500 && fabs(tracking.g_velocity.y) < 2.0){
      drivebase.set_state(1);
      master.print(1,1,"PULLING");
      misc.print("%d || TUG DETECTED\n",millis());
      break;
    }
    else if(millis()-time > 1500 && fabs(tracking.g_velocity.y) > 3.0){
      break;
    }
  }
}

Reset_dist::Reset_dist(pros::Distance* sensor, double dist_from_center): sensor{sensor}, dist_from_center{dist_from_center}{}

double Reset_dist::get_dist(){
  double avg = 0.0;
  int count = 0;
  while(count < Reset_dist::cycles){
    if(this->sensor->get() > Reset_dist::thresh){
      count++;
      avg += this->sensor->get();
      misc.print("%d || Dist in reset %f count %d\n", millis(), (double)this->sensor->get()/25.4, count);
    }
    delay(33);
  }
  //find averaeg, convert to inches
  misc.print("%d || reset %f\n",millis(), (avg/count)/25.4);
  return ((avg/count)/25.4) + this->dist_from_center;
}
