#include "lift.hpp"

Lift lift;

Lift::Lift(){ // constructor
  // initializes variables
  task_removed = true;
  full = false;
  dropoff_front = true;
  ring_dropoff_level = 1; // not 0 since that isn't a valid position

  state = neutral;
  last_state = state;
  // goal heights are 13.0, 22.0, 40.0
  // initializing ring dropoff coords
  dropoff_coords[0][0] = {-21.0, 13.0}; // alliance goal in back
  dropoff_coords[0][1] = {-23.0, 22.0}; // short neutral goal in back
  dropoff_coords[0][2] = {-19.0, 40.0};  // tall neutral goal in back
  dropoff_coords[1][0] = {0.0, 0.0}; // alliance goal in front (not possible)
  dropoff_coords[1][1] = {-1.0, 22.0}; // short neutral in front
  dropoff_coords[1][2] = {-1.0, 32.0}; // tall goal in front

}

void Lift::f_bar_cal(){
  f_bar.move(-60);
  Timer vel_rise_timeout("f_bar_vel_rise");
  // waits for f_bar velocity to rise or timeout to trigger
  while(fabs(f_bar.get_actual_velocity()) < 45.0){
    printf("vel (rising loop): %lf\n", f_bar.get_actual_velocity());
    if (vel_rise_timeout.get_time() > 50){
      printf("f_bar rising timeout\n");
      break;
    }
    delay(10);
  }
  printf("done rising\n");
  // waits until f_bar velocity slows down for 5 cycles
  cycleCheck(fabs(f_bar.get_actual_velocity()) < 5.0, 5, 10)
  f_bar.tare_position();
  printf("%d, f_bar reset %lf\n", millis(), f_bar.get_position());
  f_bar.move(0);
}

void Lift::c_bar_cal(){
  c_bar.move(-60);
  Timer vel_rise_timeout("c_bar_vel_rise");
  // waits for c_bar velocity to rise or timeout to trigger
  while(fabs(c_bar.get_actual_velocity()) < 45.0){
    printf("vel (rising loop): %lf\n", c_bar.get_actual_velocity());
    if (vel_rise_timeout.get_time() > 50){
      printf("c_bar rising timeout\n");
      break;
    }
    delay(10);
  }
  printf("done rising\n");
  // waits until c_bar velocity slows down for 5 cycles
  cycleCheck(fabs(c_bar.get_actual_velocity()) < 5.0, 5, 10)
  c_bar.tare_position();
  printf("%d, c_bar reset %lf\n", millis(), c_bar.get_position());
  c_bar.move(0);
}

void Lift::f_bar_elastic_util(){

  // up time should be around 750 and down time should be around 820
  c_bar_cal();
  f_bar_cal();
  Timer f_bar_timer("f_bar_timer");
  f_bar.move_absolute(730, 200);
  waitUntil(f_bar.get_position() > 700);
  master.print(0, 0, "f_bar up time: %d", f_bar_timer.get_time());
  f_bar_timer.print();
  f_bar_timer.reset();
  f_bar.move_absolute(0, 200);
  waitUntil(f_bar.get_position() < 50);
  master.print(1, 0, "f_bar down time: %d", f_bar_timer.get_time());
  f_bar_timer.print();
  f_bar.move(0);
}


tuple<double, double, double, double, double, double, bool> Lift::find_arm_angles(double target_y, double target_z, lift_position_types lift_position_type){
  // variables to determine postion type for lift
  double top_arm_speed = 100, bottom_arm_speed = 200; // in pros velocity units
  double pos_time, neg_time;
  double bottom_arm_pos_angle, top_arm_pos_angle, bottom_arm_neg_angle, top_arm_neg_angle;
  bool pos_position_valid, neg_position_valid, move_valid = true; // flags to determine if move is possible

  double bottom_arm_angle, top_arm_angle;

  target_z -= offset_h;
  target_y *= -1 ;  // reverses y axis

  double point_dist = sqrt(pow(target_y, 2.0) + pow(target_z, 2.0));
  printf("point_dist: %lf\n", point_dist);
  // this is the angle formed between the line of the target and the horizontal
  double target_angle = atan2(target_z, target_y);
  printf("target_angle: %lf\n", rad_to_deg(target_angle));
  // use law of cosines to find arm angles
  // angle 'C' is the angle formed between the bottom and top arm
  // in the law of cosines, 'a' is the bottom_arm_len, 'b' is the top_arm_len, and 'c' is the point_dist
  // angle 'B' is the angle formed between the bottom arm and the line of the target, calculated using sine law
  // remember the ambiguous case of the sine law
  double C = acos((pow(bottom_arm_len, 2) + pow(top_arm_len, 2) - pow(point_dist, 2)) / (2 * bottom_arm_len * top_arm_len));
  double B = asin(top_arm_len * sin(C) / point_dist);
  printf("C: %lf\n", rad_to_deg(C));
  printf("B: %lf\n", rad_to_deg(B));

  bottom_arm_pos_angle = rad_to_deg(target_angle - B);
  top_arm_pos_angle = top_arm_offset_a + rad_to_deg(C) - bottom_arm_pos_angle;
  if (top_arm_pos_angle < 0) top_arm_pos_angle += 360; // compensates for 360 degree wraparound
  bottom_arm_pos_angle += bottom_arm_offset_a;
  printf("\nPositive position:\n");
  printf("bottom_arm_pos_angle: %lf\n", bottom_arm_pos_angle);
  printf("top_arm_pos_angle: %lf\n", top_arm_pos_angle);

  bottom_arm_neg_angle = rad_to_deg(target_angle + B);
  top_arm_neg_angle = top_arm_offset_a - rad_to_deg(C) - bottom_arm_neg_angle;
  if (top_arm_neg_angle < 0) top_arm_neg_angle += 360; // compensates for 360 degree wraparound
  bottom_arm_neg_angle += bottom_arm_offset_a;
  printf("\nNegative position:\n");
  printf("bottom_arm_neg_angle: %lf\n", bottom_arm_neg_angle);
  printf("top_arm_neg_angle: %lf\n", top_arm_neg_angle);

  // Note: All ARM angles are in degrees from this point on

  // multiplying arm angles by gear ratio
  printf("\nAFTER scaling by gear ratio:\n");
  bottom_arm_pos_angle *= bottom_arm_gear_ratio;
  top_arm_pos_angle *= top_arm_gear_ratio;
  printf("bottom_arm_pos_angle: %lf\n", bottom_arm_pos_angle);
  printf("top_arm_pos_angle: %lf\n", top_arm_pos_angle);

  bottom_arm_neg_angle *= bottom_arm_gear_ratio;
  top_arm_neg_angle *= top_arm_gear_ratio;
  printf("bottom_arm_neg_angle: %lf\n", bottom_arm_neg_angle);
  printf("top_arm_neg_angle: %lf\n\n", top_arm_neg_angle);

  // code to figure out lift position type
  pos_position_valid = bottom_arm_pos_angle < bottom_arm_upper_limit && bottom_arm_pos_angle > bottom_arm_lower_limit && top_arm_pos_angle < top_arm_upper_limit && top_arm_pos_angle > top_arm_lower_limit;
  neg_position_valid = bottom_arm_neg_angle < bottom_arm_upper_limit && bottom_arm_neg_angle > bottom_arm_lower_limit && top_arm_neg_angle < top_arm_upper_limit && top_arm_neg_angle > top_arm_lower_limit;

  switch(lift_position_type){
  case lift_position_types::positive:
    bottom_arm_angle = bottom_arm_pos_angle;
    top_arm_angle = top_arm_pos_angle;
    move_valid = pos_position_valid;
    break;
  case lift_position_types::negative:
    bottom_arm_angle = bottom_arm_neg_angle;
    top_arm_angle = top_arm_neg_angle;
    move_valid = neg_position_valid;
    break;
  case lift_position_types::fastest:
    if (pos_position_valid && neg_position_valid){
      pos_time = max((f_bar.get_position() - bottom_arm_pos_angle) / bottom_arm_speed, (c_bar.get_position() - top_arm_pos_angle) / top_arm_speed);
      neg_time = max((f_bar.get_position() - bottom_arm_neg_angle) / bottom_arm_speed, (c_bar.get_position() - top_arm_neg_angle) / top_arm_speed);
      if(neg_time < pos_time){
        bottom_arm_angle = bottom_arm_neg_angle;
        top_arm_angle = top_arm_neg_angle;
      }
      else {
        bottom_arm_angle = bottom_arm_pos_angle;
        top_arm_angle = top_arm_pos_angle;
      }
    }
    else if (pos_position_valid){
      bottom_arm_angle = bottom_arm_pos_angle;
      top_arm_angle = top_arm_pos_angle;
    }
    else if (neg_position_valid){
      bottom_arm_angle = bottom_arm_neg_angle;
      top_arm_angle = top_arm_neg_angle;
    }
    else  move_valid = false;
    break;
  }

  // defaults to positive for testing only
  // bottom_arm_angle = bottom_arm_pos_angle;
  // top_arm_angle = top_arm_pos_angle;

  printf("Final bottom_arm_angle: %lf\n", bottom_arm_angle);
  printf("Final top_arm_angle: %lf\n", top_arm_angle);
  return {top_arm_angle, bottom_arm_angle, top_arm_pos_angle, bottom_arm_pos_angle, top_arm_neg_angle, bottom_arm_neg_angle, move_valid};
}

void Lift::move_to_target(const Vector2D& target, const lift_position_types lift_position_type, const bool wait_for_complete, const double bottom_arm_speed, const double top_arm_speed){
  auto[top_arm_angle, bottom_arm_angle, top_arm_pos_angle, bottom_arm_pos_angle, top_arm_neg_angle, bottom_arm_neg_angle, move_valid] = find_arm_angles(target.x, target.y, lift_position_type);

  // updates arm targets for lift state machine
  cur_top_arm_target = top_arm_angle;
  cur_bottom_arm_target = bottom_arm_angle;

  if (move_valid){
    printf("Lift | Starting moving to target: y: %lf, z: %lf\n", target.x, target.y);
    f_bar.move_absolute(bottom_arm_angle, bottom_arm_speed);
    if(wait_for_complete && bottom_arm_angle / bottom_arm_gear_ratio > bottom_arm_offset_a) waitUntil(f_bar.get_position() / bottom_arm_gear_ratio > bottom_arm_offset_a * 0.6);
    c_bar.move_absolute(top_arm_angle, top_arm_speed);
    if (wait_for_complete)  lift.wait_for_complete();
  }
  else printf("POSITION IS INVALID | POS: TOP: %lf, BOTTOM: %lf | NEG: TOP: %lf, BOTTOM: %lf\n", top_arm_pos_angle, bottom_arm_pos_angle, top_arm_neg_angle, bottom_arm_neg_angle);
}

void Lift::move_to_target_util(){
  double target_y = -6.0, target_z = 13.0;

  master.clear();
  delay(50);
  master.print(0, 0, "z: %lf", target_z);
  delay(50);
  master.print(1, 0, "y: %lf", target_y);
  move_to_target({target_y, target_z});

	while(true){
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT)){
			target_y++;
			delay(50);
			master.print(1, 0, "y: %lf", target_y);
      move_to_target({target_y, target_z}, lift_position_types::fastest, false);
		}
		else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_LEFT)){  // decrease height
			target_y--;
			delay(50);
			master.print(1, 0, "y: %lf", target_y);
      move_to_target({target_y, target_z}, lift_position_types::fastest, false);
		}
		if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){  // increase height
			target_z++;
			delay(50);
			master.print(0, 0, "z: %lf", target_z);
      move_to_target({target_y, target_z}, lift_position_types::fastest, false);
		}
		else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)){  // decrease height
			target_z--;
			delay(50);
			master.print(0, 0, "z: %lf", target_z);
      move_to_target({target_y, target_z}, lift_position_types::fastest, false);
		}
		delay(10);
	}
}

void Lift::move_f_bar_to_height(double target_z, const double speed){
  target_z += -base_height + 3.5;  // 3.5 inches is how much below the forks are from the top pivot point
  cur_bottom_arm_target = bottom_arm_gear_ratio * (rad_to_deg(asin(target_z / bottom_arm_len)) + bottom_arm_offset_a);
  // c_bar.move_absolute(200, 100);
  f_bar.move_absolute(cur_bottom_arm_target, speed);
}

void Lift::wait_for_complete(){
  waitUntil(fabs(cur_bottom_arm_target - f_bar.get_position()) < bottom_arm_end_error && fabs(cur_top_arm_target - c_bar.get_position()) < top_arm_end_error);
}

double Lift::find_y_pos(){
    return top_arm_len * cos(deg_to_rad(c_bar.get_position() / top_arm_gear_ratio - top_arm_offset_a)) - bottom_arm_len * cos(deg_to_rad(f_bar.get_position() / bottom_arm_gear_ratio - bottom_arm_offset_a));
}

double Lift::find_z_pos(){
    return offset_h + top_arm_len * sin(deg_to_rad(c_bar.get_position() / top_arm_gear_ratio - top_arm_offset_a)) + bottom_arm_len * sin(deg_to_rad(f_bar.get_position() / bottom_arm_gear_ratio - bottom_arm_offset_a));
}

double Lift::find_top_arm_angle(const double target_y){
  return rad_to_deg(deg_to_rad(top_arm_offset_a) - acos((cos(deg_to_rad(f_bar.get_position() / bottom_arm_gear_ratio) - deg_to_rad(bottom_arm_offset_a)) * bottom_arm_len + target_y) / top_arm_len)) * top_arm_gear_ratio;
}

void Lift::touch_line(const double target_y, double speed){ // should be in motor degrees
  double top_arm_angle = find_top_arm_angle(target_y);
  if (top_arm_angle > top_arm_upper_limit || top_arm_angle < top_arm_lower_limit) // move is invalid
    printf("MOVE INVALID: top arm position attempt: %lf\n", top_arm_angle);
  else
    c_bar.move_absolute(top_arm_angle, speed);
}

double Lift::get_arm_velocity_ratio(const double target_y){

  // system delay is 61.111111 ms
  // or 0.00101851851667 minutes
  double system_delay = 0.00102;

  double bottom_arm_angle = deg_to_rad(f_bar.get_position() + system_delay * f_bar.get_actual_velocity());
  // printf("acc f_bar_position_rad: %lf\t", deg_to_rad(f_bar.get_position()));
  // is returning derivative of find_top_arm_angle function
  return (bottom_arm_len * top_arm_gear_ratio * sin(deg_to_rad(bottom_arm_offset_a) - bottom_arm_angle / bottom_arm_gear_ratio)) / (top_arm_len * bottom_arm_gear_ratio * sqrt(1 - pow(bottom_arm_len * cos(deg_to_rad(bottom_arm_offset_a) - bottom_arm_angle / bottom_arm_gear_ratio) + target_y, 2) / pow(top_arm_len, 2)));

}

void Lift::move_on_line(double target_y, double target_z_start, double target_z_end, const double speed){
  move_to_target({target_y, target_z_start}, lift_position_types::fastest, true); // goes to position above the rings
  // delay(300);
  // grabs position for f_bar to reach when at bottom of ring stack
  auto[top_arm_angle, bottom_arm_angle, top_arm_pos_angle, bottom_arm_pos_angle, top_arm_neg_angle, bottom_arm_neg_angle, move_valid] = lift.find_arm_angles(target_y, target_z_end);
  f_bar.move_absolute(bottom_arm_angle, speed); // moves f_bar to bottom of ring stack
  double top_arm_target, open_loop_velocity, closed_loop_velocity;
  double ratio;
  PID top_arm(0.5, 0.0, 0.3, 0.0, true, 0.0, 100.0);

  while(fabs(target_z_end - find_z_pos()) > 0.5 || fabs(target_y - find_y_pos()) > 0.5){  // moves the chain bar to maintain the horizontal distance
      top_arm_target = find_top_arm_angle(target_y);
      // printf("target: %lf, actual: %lf, diff: %lf, vel: %lf\n", top_arm_target, c_bar.get_position(), find_top_arm_angle(target_y) - c_bar.get_position(), (find_top_arm_angle(target_y) - c_bar.get_position()) * top_arm_kp);
      ratio = get_arm_velocity_ratio(target_y);
      // printf("ratio: %lf, f_bar_pos: %lf, f_bar_pos_rad: %lf, f_bar_vel: %lf, acc_vel: %lf\n", ratio, f_bar.get_position(), deg_to_rad(f_bar.get_position()), f_bar.get_actual_velocity(), ratio * f_bar.get_actual_velocity());
      open_loop_velocity = ratio * f_bar.get_actual_velocity();
      closed_loop_velocity = top_arm.compute(c_bar.get_position(), find_top_arm_angle(target_y));

      c_bar.move_velocity(open_loop_velocity + closed_loop_velocity);
      // c_bar.move_velocity(open_loop_velocity + closed_loop_velocity);
      printf("%lf, %lf\n", find_y_pos(), find_z_pos());
      delay(10);
  }
  c_bar.move_relative(0, 100); // stops motor
}

void Lift::start_task(task_fn_t function){
  stop_task();
  delete task_ptr;
  task_ptr = nullptr;
  task_removed = false;
  task_ptr = new Task(function);
  printf("created task\n");
}

void Lift::stop_task(){
  ring_dropoff_level = -1;
  if(task_removed) printf("task removed\n");
  if(task_ptr != nullptr) printf("taskptr not null\n");
  delay(10);
  if(!task_removed && task_ptr != nullptr){
    printf("stopping task, IS deallocating memory\n");
    task_removed = true;
    task_ptr->remove();
    printf("REMOVED TASK\n");
    delete task_ptr;
    printf("DELETED TASK POINTER\n");
    task_ptr = nullptr;
  }
  else    printf("stopping task, NOT deallocating memory\n");
}

void Lift::set_state(const states next_state){
  printf("Lift | Going from %s to %s\n", state_names[state], state_names[next_state]);
  last_state = state;
  state = next_state;
}

void Lift::handle(){
  printf("state: %s\n", state_names[state]);

  // Buttons accessible in any state (all functions called should stop the lift task if it is running)

  // Bring to neutral (cancel operation)
  if (master.get_digital_new_press(cancel_dropoff_button) && state != neutral){ // brings to neutral position
    printf("cancel button pressed\n");
    move_to_neutral();
    set_state(neutral);
  }
  // brings f_bar to mogo tipping height
  else if (master.get_digital_new_press(tip_mogo_button) && state != tip){
    printf("tip mogo button pressed\n");
    move_f_bar_tip();
    set_state(tip);
  }
  // lowers f_bar
  else if (master.get_digital_new_press(f_bar_down_button) && state != down && state != release_mogo){ // lowers f_bar to pickup mogos with forks if f_bar down is pressed
    printf("fbar down button pressed\n");
    lower_f_bar();
    set_state(lowering);
  }
  // switches dropoff side
  if (master.get_digital_new_press(switch_dropoff_side_button)){
    printf("switch dropoff side button pressed\n");
    dropoff_front = !dropoff_front;
    WAIT_FOR_SCREEN_REFRESH(); // remove this later
    master.print(0, 0, "Dropoff: %s", dropoff_front ? "front" : "back ");
    screen_timer.reset();
    printf("Dropoff side: %s\n",  dropoff_front ? "front" : "back ");
  }

  switch(state){
    case neutral:
      /*
      if (intake.state == intake.states::full){ // pickup rings if the intake is full
        start_task(pickup_rings);
        set_state(ring_pickup);
        intake.set_state(intake.states::searching);
      }
      */
      if (master.get_digital_new_press(pickup_rings_button)){  // picks up rings if pickup rings button is pressed
        start_task(pickup_rings);
        set_state(ring_pickup);
      }
      // ring dropoff
      else if (master.get_digital_new_press(ring_dropoff_button) && (full || true)){ // later check if mogo is oriented as well, is orring with true just for testing since we can't sense if it is full yet
          set_state(dropoff_start);
      }
      break;
    case ring_pickup:
      // ring_pickup is an empty state and therefore only buttons above switch statement are accessible
      break;
    case tip:
      // tip is an empty state and therefore only buttons above switch statement are accessible
      break;
    case lowering:
      if(fabs(get_bottom_arm_target() - f_bar.get_position()) < get_bottom_arm_end_error()){  // waits for f_bar to reach target
          printf("\n\n FINISHED LOWERING F_BAR %d\n\n", millis());
          open_forks(); // releases mogo
          set_state(lift.states::down);
      }
      break;
    case down:
      if (master.get_digital_new_press(f_bar_up_button)){
        printf("f_bar up button pressed\n");
        raise_f_bar();
        set_state(raised);
      }
      break;
    case raised:
      if (master.get_digital_new_press(f_bar_up_button)){
        printf("f_bar up button pressed\n");
        raise_f_bar_above_platform();
        set_state(above_platform);
      }
      break;
    case above_platform:
      if (master.get_digital_new_press(f_bar_up_button)){
        printf("f_bar up button pressed\n");
        lower_f_bar_in_platform();
        set_state(in_platform);
      }
      break;
    case in_platform:
      if(fabs(cur_bottom_arm_target - f_bar.get_position()) < bottom_arm_end_error){  // drops mogo once it reaches the target position
        open_forks();
        set_state(release_mogo);
      }
      break;
    case release_mogo:
      if (master.get_digital_new_press(f_bar_up_button)){
        printf("f_bar up button pressed\n");
        close_forks();  // picks up mogo
        raise_f_bar_above_platform();
        set_state(above_platform);
      }
      else if (master.get_digital_new_press(f_bar_down_button)){  // goes back up without picking up mogo
        printf("f_bar up button pressed\n");
        raise_f_bar_above_platform();
        set_state(above_platform);
      }
      break;
    case ring_dropoff:  // Actual async ring dropoff state
      // ring_dropoff is an empty state and therefore only buttons above switch statement are accessible
      break;
    case dropoff_start:
      last_dropoff_press_timer.reset();
      printf("ring dropoff button pressed\n");
      stop_task();
      if (dropoff_front){
        move_to_target(dropoff_coords[dropoff_front][1], lift_position_types::fastest, false);
        set_state(dropoff_front_mid);
      }
      else{
        move_to_target(dropoff_coords[dropoff_front][0], lift_position_types::fastest, false);
        set_state(dropoff_back_alliance);
      }
      break;
    case dropoff_back_alliance:
      // waits for press timer to timeout and to reach target
      if (last_dropoff_press_timer.get_time() > dropoff_double_press_time && fabs(cur_bottom_arm_target - f_bar.get_position()) < bottom_arm_end_error && fabs(cur_top_arm_target - c_bar.get_position()) < top_arm_end_error){
        start_task(dropoff_rings);
        set_state(ring_dropoff);
      }
      else if (master.get_digital_new_press(ring_dropoff_button)){
        last_dropoff_press_timer.reset();
        move_to_target(dropoff_coords[dropoff_front][1], lift_position_types::fastest, false);
        set_state(dropoff_back_mid);
        // moves to higher dropoff height
      }
      break;
    case dropoff_back_mid:
      // waits for press timer to timeout and to reach target
      if (last_dropoff_press_timer.get_time() > dropoff_double_press_time && fabs(cur_bottom_arm_target - f_bar.get_position()) < bottom_arm_end_error && fabs(cur_top_arm_target - c_bar.get_position()) < top_arm_end_error){
        start_task(dropoff_rings);
        set_state(ring_dropoff);
      }
      else if (master.get_digital_new_press(ring_dropoff_button)){
        last_dropoff_press_timer.reset();
        move_to_target(dropoff_coords[dropoff_front][2], lift_position_types::fastest, false);
        set_state(dropoff_back_mid);
        // moves to higher dropoff height
      }
      break;
    case dropoff_back_top:
      // waits for press timer to timeout and to reach target
      if (last_dropoff_press_timer.get_time() > dropoff_double_press_time && fabs(cur_bottom_arm_target - f_bar.get_position()) < bottom_arm_end_error && fabs(cur_top_arm_target - c_bar.get_position()) < top_arm_end_error){
        start_task(dropoff_rings);
        set_state(ring_dropoff);
      }
      else if (master.get_digital_new_press(ring_dropoff_button)){
        last_dropoff_press_timer.reset();
        set_state(dropoff_start); // moves to beginning height
      }
      break;
    case dropoff_front_mid:
      // waits for press timer to timeout and to reach target
      if (last_dropoff_press_timer.get_time() > dropoff_double_press_time && fabs(cur_bottom_arm_target - f_bar.get_position()) < bottom_arm_end_error && fabs(cur_top_arm_target - c_bar.get_position()) < top_arm_end_error){
        start_task(dropoff_rings);
        set_state(ring_dropoff);
      }
      else if (master.get_digital_new_press(ring_dropoff_button)){
        last_dropoff_press_timer.reset();
        move_to_target(dropoff_coords[dropoff_front][2], lift_position_types::fastest, false);
        set_state(dropoff_front_mid);
        // moves to higher dropoff height
      }
      break;
    case dropoff_front_top:
      // waits for press timer to timeout and to reach target
      if (last_dropoff_press_timer.get_time() > dropoff_double_press_time && fabs(cur_bottom_arm_target - f_bar.get_position()) < bottom_arm_end_error && fabs(cur_top_arm_target - c_bar.get_position()) < top_arm_end_error){
        start_task(dropoff_rings);
        set_state(ring_dropoff);
      }
      else if (master.get_digital_new_press(ring_dropoff_button)){
        last_dropoff_press_timer.reset();
        set_state(dropoff_start); // moves to higher beginning height
      }
      break;
  }
}

void Lift::move_to_neutral(){ // moves lift to position right above ring stack
  stop_task();
  printf("starting mtt\n");
  close_stabber();  // in case cancel was pressed during ring pickup (to prevent a jam)
  move_to_target({-3.0, 20.0}, lift_position_types::fastest, true);
}

void Lift::move_f_bar_tip(){  // moves f_bar to mogo tipping height
  stop_task();
  move_f_bar_to_height(8.0);
}

void Lift::lower_f_bar(){
  stop_task();
  close_forks();
  move_f_bar_to_height(6.0);

  printf("\n\n STARTED LOWERING F_BAR %d \n\n", millis());
}

void Lift::raise_f_bar(){ // brings f_bar just above the ground
  stop_task();
  close_forks();
  move_f_bar_to_height(7.0);
}

void Lift::raise_f_bar_above_platform(){
  stop_task();
  move_f_bar_to_height(12.0);
}

void Lift::lower_f_bar_in_platform(){
  stop_task();
  move_f_bar_to_height(9.5);
}

void Lift::open_forks(){
  // open forks
}

void Lift::close_forks(){
  // close forks
}

void Lift::open_stabber(){
  // open stabber
}

void Lift::close_stabber(){
  // close stabber
}

double Lift::get_bottom_arm_target() const{
  return cur_bottom_arm_target;
}

double Lift::get_bottom_arm_end_error() const{
  return bottom_arm_end_error;
}

// for ring dropoff function
array<array<Vector2D, 3>, 2>& Lift::get_dropoff_coords(){
  return dropoff_coords;
}

bool Lift::is_dropoff_front() const{
  return dropoff_front;
}

double Lift::get_ring_dropoff_level() const{
  return ring_dropoff_level;
}
// Async Functions

void pickup_rings(void* params){
  lift.open_stabber();
  lift.move_on_line(-3.00, 20.0, 9.5, 60);
  lift.close_stabber();
  delay(120); // waits for arm to stabilize
  lift.full = true;
  lift.move_on_line(-2.5, 9.5, 20.0, 55);
  lift.set_state(lift.states::neutral);
  lift.stop_task();
}

void dropoff_rings(void* params){
  printf("started ring dropoff %d\n", millis());
  lift.open_stabber();
  lift.full = false;
  delay(200); // waits for rings to slide down end_effector
  printf("finished ring dropoff %d\n", millis());
  lift.set_state(lift.states::neutral);
  lift.close_stabber();  // in case cancel was pressed during ring pickup (to prevent a jam)
  lift.move_to_target({-3.0, 20.0}, lift_position_types::fastest, true);
  lift.stop_task();
}
