#include "Tracking.hpp"

Tracking tracking;
move_target_params move_params;

Task *updateTask =(nullptr);
Task *moveTask = nullptr;

void updateStartTask(){
  updateTask = new Task(update);
}
void updateStopTask(){
  if(updateTask != nullptr){
    updateTask->remove();
    delete updateTask;
    updateTask = nullptr;
  }
}
void Tracking::move_stop_task(){
  if(moveTask != nullptr){
    moveTask->remove();
    delete moveTask;
    moveTask = nullptr;
  }
}
void Tracking::move_start_task(){
  moveTask = new Task(move_to_target);
}

void update(void* params){
  // LeftEncoder.reset(); RightEncoder.reset(); BackEncoder.reset();
  double DistanceLR = 14.79, DistanceB = 6.1;
  double Left, Right, Back, NewLeft, NewRight, NewBack, LastLeft = LeftEncoder.get_value()/360.0 *(2.75*M_PI), LastRight =  RightEncoder.get_value()/360.0 *(2.75*M_PI), LastBack = BackEncoder.get_value()/360.0 *(2.77*M_PI);
  double Theta = 0.0, Beta = 0.0, Alpha = 0.0;
  double RadiusR, RadiusB, h, h2;
  double Xx, Xy, Yy, Yx;
  int lasttime = 0;
  double last_x = 0, last_y = 0, last_a = 0;
  double last_vel_l = 0, last_vel_r = 0, last_vel_b = 0;

  Position last_position; // last position of robot
  int last_velocity_time = 0;    // time of last velocity update
  int velocity_update_time = 0; // time SINCE last velocity update

  double total_x = 0.0, total_y = 0.0, total_d = 0.0, total_a = 0.0;

  while(true){
    NewLeft = LeftEncoder.get_value()/360.0 *(2.75*M_PI);
    NewRight = RightEncoder.get_value()/360.0 *(2.75*M_PI);
    NewBack = BackEncoder.get_value()/360.0 *(2.75*M_PI);



    Left = NewLeft - LastLeft;
    Right = NewRight - LastRight;
    Back = NewBack - LastBack;


    velocity_update_time = millis() - last_velocity_time;
    if(velocity_update_time > 20){  // velocity is updated every 20 ms
        tracking.l_velo = (NewLeft - last_vel_l) / velocity_update_time * 1000; // velocities are in inches per second
        tracking.r_velo = (NewRight - last_vel_r) / velocity_update_time * 1000;
        tracking.b_velo = (NewBack - last_vel_b) / velocity_update_time * 1000;

        last_vel_l = NewLeft;
        last_vel_r = NewRight;
        last_vel_b = NewBack;

        // gives us linear velocity in inches per second, and angular velocity in radians per second
        tracking.g_velocity.x = (tracking.x_coord - last_position.x) / velocity_update_time * 1000;
        tracking.g_velocity.y = (tracking.y_coord - last_position.y) / velocity_update_time * 1000;
        tracking.g_velocity.angle = (tracking.global_angle - last_position.angle) / velocity_update_time * 1000;

        last_position = {tracking.x_coord, tracking.y_coord, tracking.global_angle};

        last_velocity_time = millis();
    }

    LastLeft = NewLeft;
    LastRight = NewRight;
    LastBack = NewBack;

    Theta = (Left-Right)/DistanceLR;
    if (Theta != 0){
      RadiusR = Right/Theta;
      RadiusB = Back/Theta;
      Beta = Theta/2.0;
      h = (RadiusR + DistanceLR/2) *2 *sin(Beta);
      h2 = (RadiusB + DistanceB) *2 *sin(Beta);
    }
    else{
      h = Right;
      h2 = Back;
      Beta =0.0;
    }
    Alpha = tracking.global_angle + Beta;

    Xx = h2 * cos(Alpha);
    Xy = h2 * -sin(Alpha);
    Yx = h * sin(Alpha);
    Yy = h * cos(Alpha);

  tracking.x_coord += Xx + Yx;
  tracking.y_coord += Yy + Xy;
  tracking.global_angle += Theta;

  total_x += fabs(Xx+Yx);
  total_y += fabs(Yy + Xy);
  total_a += fabs(Theta);
  // printf("time: %d, TRACKING: %f, %f, %f \n", millis(), tracking.x_coord, tracking.y_coord, tracking.global_angle/M_PI *180);
  // printf("ENCODER L: %d, R: %d, B:%d \n", LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());

  if(millis() - 50 > lasttime){
    // printf("powers| x: %lf, y: %lf, a: %lf\n", tracking.power_x, tracking.power_y, tracking.power_a);
    // printf("x: %lf, y: %lf, a: %lf\n", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));

      // printf("%f,%f\n", tracking.x_coord, tracking.y_coord);

    // printf("\ntime: %d, TRACKING: %f, %f, %f \n", millis(), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
    // printf("time: %d, TOTAL: %f, %f, %f \n", millis(), total_x, total_y, rad_to_deg(total_a));
    // printf("%d pow_a: %.1f, pow_x: %.1f, pow_y: %.1f, total_pow: %.1f\n",millis(),  tracking.power_a, tracking.power_x, tracking.power_y, fabs(tracking.power_a) + fabs(tracking.power_x) + fabs(tracking.power_y));

    // printf("ENCODER L: %d, R: %d, B:%d \n", LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());
    // printf("%d VELOCIT L: %f, R: %f\n", millis(), tracking.l_velo, tracking.r_velo);

    // printf("GLOBAL VELOCITY| x: %.2f, y: %.2f a: %.2f\n", tracking.g_velocity.x, tracking.g_velocity.y, rad_to_deg(tracking.g_velocity.angle));
    // printf("power| x: %.2f, y: % 2.f a: %.2f\n", tracking.power_x, tracking.power_y, tracking.power_a);

    // printf(" %f, %f, %f, %f\n",tracking.l_velo, tracking.r_velo, tracking.power_y, tracking.y_coord);
    // printf(" %f, %f, %f, %f\n",tracking.l_velo, tracking.r_velo, tracking.power_y, tracking.y_coord);
    lasttime = millis();
  }

  delay(10);
  }
}

void move_to_target_sync(double target_x, double target_y, double target_a, bool Brake, bool debug, int max_power, bool Overshoot, double end_error, double end_error_a, double p,double lpercent, double apercent){
  if(!tracking.move_complete) tracking.move_stop_task();
  if(moveTask != nullptr) moveTask = nullptr;
  // printf("here2");
  move_params = {target_x, target_y, target_a, Brake, debug, max_power, Overshoot, end_error, end_error_a, p, lpercent, apercent};
  tracking.move_complete = false;
  move_to_target(nullptr);

}
void move_to_target_async(double target_x, double target_y, double target_a, bool Brake, bool debug, int max_power, bool Overshoot, double end_error, double end_error_a, double p, double lpercent, double apercent){
  if(!tracking.move_complete) tracking.move_stop_task();
  move_params = {target_x, target_y, target_a, Brake, debug, max_power, Overshoot, end_error, end_error_a, p, lpercent, apercent};
  tracking.move_complete = false;
  printf("async task started %d \n", millis());
  tracking.move_start_task();
  delay(10);
}

void Tracking::wait_for_complete(){
  while(!tracking.move_complete) delay(10);
}
void Tracking::wait_for_error(double distance){

  while(fabs(tracking.drive_error)- distance > 1 &&!tracking.move_complete)delay(10);
  printf("%d done wait for error %f",millis(), tracking.drive_error);
}

void Tracking::wait_for_dist(double distance){

  while(fabs(tracking.drive_error) > distance &&!tracking.move_complete)delay(10);
  printf("%d done wait for dist %f",millis(), tracking.drive_error);
}

void move_to_target(void* params){
  double target_x = move_params.target_x, target_y = move_params.target_y, target_a = deg_to_rad(move_params.target_a);
  double end_error = move_params.end_error, end_error_a = move_params.end_error_a;
  bool Brake = move_params.Brake, debug = move_params.debug;
  int max_power = move_params.max_power;
  bool Overshoot = move_params.Overshoot; //
  double lpercent= move_params.lpercent, apercent=move_params.apercent;
  double kp_l =5.0, kp_d = 7.0, kp_a = 50.0, ki_l = 0.05, ki_d = 0.05, ki_a = 0.0, kd_l = 0.0, kd_d = 0.0, kd_a = 0.0;
  double error_angle, error_line, error_d, error_x, error_y, error_a , error_tot;
  double power_line, power_d, total_power, line_total;
  double dif_a, lx, ly, dx, dy;
  double min_lx, min_ly, min_a, min_total;
  double integ_start = 5.0, integ_l, integ_d, integ_a;
  double deriv_l, deriv_d, deriv_a, last_error_l, last_error_d, last_error_a;
  uint32_t integ_last_time = millis(), deriv_time = millis();
  int last_time = 0;
  //log_time("starting time: %d X: %f Y: %f A: %f from X: %f Y: %f A: %f \n", millis(), target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
  printf("Starting move to target X: %f Y: %f A: %f from X: %f Y: %f A: %f\n", target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));

  tracking.move_complete = false;
  tracking.movetime = millis();
  double start_error_x = target_x - tracking.x_coord, start_error_y = target_y - tracking.y_coord;
  double d_angle = atan2(start_error_x, start_error_y);


  double orig_point_angle = atan2(error_x, error_y); //
  double cur_point_angle;
  int perpendicular = M_PI/2;

  while(true){
    error_x = target_x - tracking.x_coord;
    error_y = target_y - tracking.y_coord;
    error_a = target_a - tracking.global_angle;
    error_tot = sqrt(pow(error_x,2)+ pow(error_y,2));
    tracking.drive_error = error_tot;
    error_angle = atan2(error_x,error_y)-d_angle;
    error_line = sin(error_angle)*error_tot;
    error_d = cos(error_angle)*error_tot;
    dif_a = d_angle - tracking.global_angle;


    if (millis() - integ_last_time > 50){
      if(fabs(error_line) < integ_start) integ_l += fabs(error_line) *(millis()-integ_last_time);
      if(fabs(error_d)<integ_start) integ_d += fabs(error_d)*(millis()-integ_last_time);


      if(fabs(error_line) <=0.8)integ_l = 0.0;
      if(fabs(error_d) <= 0.8)integ_d = 0.0;

      integ_last_time = millis();
    }

    if(millis()-deriv_time > 0){
      deriv_l = (error_line-last_error_l)/(millis()-deriv_time);
      deriv_d = (error_d-last_error_d)/(millis()-deriv_time);
      deriv_a = (error_a-last_error_a)/(millis()-deriv_time);

      last_error_l = error_line;
      last_error_d = error_d;
      last_error_a = error_a;
      deriv_time = millis();
    }

    power_line = error_line *kp_l;
    power_d = error_d * kp_d;
    tracking.power_a = error_a *kp_a;

    power_line += integ_l *sgn(power_line) *ki_l;
    power_d += integ_d *sgn(power_line) *ki_d;

    power_line += deriv_l*kd_l;
    power_d += deriv_d*kd_d;
    tracking.power_a += deriv_a*kd_a;


    lx = cos(dif_a) * power_line;
    ly = -sin(dif_a)* power_line;
    dx = sin(dif_a) * power_d;
    dy = cos(dif_a) * power_d;




    // if(millis() - last_time > 50){
    //   printf("pre scal lx: %f, ly: %f, dx: %f, dy: %f, a: %f\n",lx,ly,dx,dy,tracking.power_a);
    // }

    total_power = fabs(lx+dx+ly+dy+tracking.power_a);
    if(total_power > max_power){
      line_total = fabs(lx)+fabs(dx);
      if(line_total > lpercent*max_power){
        min_lx = (fabs(lx)/line_total) *lpercent*max_power;
        min_ly = (fabs(ly)/line_total) *lpercent*max_power;
      }
      else{
        min_lx = fabs(lx);
        min_ly = fabs(ly);
      }
      if(fabs(tracking.power_a) > apercent*max_power) min_a = apercent*max_power;
      else min_a = fabs(tracking.power_a);
      min_total = min_lx + min_ly + min_a;
      lx =((fabs(lx)/(total_power-min_total))*(max_power-min_total)+min_lx)*sgn(lx);
      ly =((fabs(ly)/(total_power-min_total))*(max_power-min_total)+min_ly)*sgn(ly);
      dx =(dx/(total_power-min_total))*(max_power-min_total);
      dy =(dy/(total_power-min_total))*(max_power-min_total);
      tracking.power_a =((fabs(tracking.power_a)/(total_power-min_total))*(max_power-min_total)+min_a)*sgn(tracking.power_a);
    }


    tracking.power_x = lx + dx;
    if(fabs(error_x) < end_error - 0.2) tracking.power_x = 0.0;
    else if(fabs(tracking.power_x) < min_power_x)tracking.power_x = sgn(tracking.power_x)*min_power_x;
    tracking.power_y = ly + dy;
    if(fabs(error_y) < end_error - 0.2) tracking.power_y = 0.0;
    else if(fabs(tracking.power_y) < min_power_y)tracking.power_y = sgn(tracking.power_y)*min_power_y;
    if(fabs(rad_to_deg(error_a)) < end_error_a - 1.0)tracking.power_a = 0.0;
    else if(fabs(tracking.power_a) < min_power_a)tracking.power_a = sgn(tracking.power_a)*min_power_a;




    drivebase.move(tracking.power_x,tracking.power_y,tracking.power_a);

    if(millis() - last_time > 50 && debug){
      printf("%f, %f\n", tracking.y_coord,tracking.power_y);
      // printf("post scal lx: %f, ly: %f, dx: %f, dy: %f, a: %f\n",lx,ly,dx,dy,tracking.power_a);
      // printf("mins lx: %f, ly: %f, a: %f\n",min_lx,min_ly,min_a);
      // printf("error line: %f, d: %f, x: %f, y: %f, a: %f\n",error_line, error_d,error_x, error_y, rad_to_deg(error_a));
      // printf("power line: %f, d: %f, x: %f, y: %f, a: %f\n",power_line, power_d,tracking.power_x,tracking.power_y,tracking.power_a);
      // printf("integral l: %f, d: %f\n",integ_l,integ_d);
      printf("derivative l: %f, d: %f a: %f\n",deriv_l, deriv_d, deriv_a);
      last_time = millis();
    }

    if(fabs(error_x) < end_error && fabs(error_y) < end_error && fabs(rad_to_deg(error_a)) < end_error_a){
      drivebase.brake();
      tracking.move_complete = true;
      printf("Ending move to target X: %f Y: %f A: %f at X: %f Y: %f A: %f \n", target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
      //log_time("ending starting time: %d, delta time: %d X: %f Y: %f A: %f from X: %f Y: %f A: %f \n", millis(),millis() -starttime, target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
      tracking.move_stop_task();
      return;
    }






    delay(10);
  }
}

// #define exit_condition tracking.x_coord < target_x - 5.0 || tracking.y_coord > target_y + 5.0

void rush_goal(double target_x, double target_y, double target_a){
    double end_error = 0.5, end_error_a = 5.0, kp_x = 10.0, kp_a = 120.0, local_error_x, local_error_y, difference_a, error_a, error_d, max_power = 127, total_power, scale;
    uint32_t last_time = millis();
    target_a = deg_to_rad(target_a);
    short orig_y_sgn = sgn(target_y - tracking.y_coord);
    printf("orig_y_sgn: %lf\n", target_y - tracking.y_coord);
    while(true){
        error_d = sqrt(pow(target_x - tracking.x_coord, 2) + pow(target_y - tracking.y_coord, 2));
        difference_a = atan2(target_y - tracking.y_coord, target_x - tracking.x_coord) + tracking.global_angle;
        local_error_x = error_d * cos(difference_a);
        local_error_y = error_d * sin(difference_a);
        error_a = target_a - tracking.global_angle;

        tracking.power_x = local_error_x * kp_x;
        tracking.power_a = error_a * kp_a;
        total_power = fabs(tracking.power_x) + fabs(tracking.power_a);
        if (total_power > max_power) {
            scale = max_power / total_power;
            tracking.power_x *= scale;
            tracking.power_a *= scale;
            tracking.power_y = 0;
            printf("WARNING: A ROBOT IS PUSHING US OR SOMETHING ELSE IS VERY WRONG.\n");
        }
        else  tracking.power_y = sgn(local_error_y) * (max_power - fabs(tracking.power_x) - fabs(tracking.power_a));
        // printf("")
        printf("powers: %lf, %lf, %lf\n", tracking.power_x, tracking.power_y, tracking.power_a);
        // printf("cur_y_sgn: %lf, cur_y: %lf\n", target_y - tracking.y_coord, tracking.y_coord);
        // exit condition
        if ((error_d < end_error && fabs(local_error_y) < end_error && fabs(error_a) < deg_to_rad(end_error_a)) || sgn(target_y - tracking.y_coord) != orig_y_sgn){
            // got_goal = goal_lim_switch_state;
            drivebase.move(0, -10 * sgn(orig_y_sgn), 0);
            printf("x: %lf, y: %lf, a: %lf\n", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
            printf("GOT TO GOAL: %d\n", millis());
            return;
        }
        // if(millis() - last_time > 50){
          // printf("powers| x: %lf, y: %lf, a: %lf\n", tracking.power_x, tracking.power_y, tracking.power_a);
          // printf("x: %lf, y: %lf, a: %lf\n", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));

        // }


        drivebase.move(tracking.power_x, tracking.power_y, tracking.power_a);
        delay(10);
    }

}

void rush_goal2(double target_x, double target_y, double target_a){
    double end_error = 0.5, end_error_a = 5.0, kp_x = 10.0, kp_a = 120.0, local_error_x, local_error_y, difference_a, error_a, error_d, max_power = 127, total_power, scale;
    uint32_t last_time = millis();
    target_a = deg_to_rad(target_a);
    short orig_y_sgn = sgn(target_y - tracking.y_coord);
    printf("orig_y_sgn: %lf\n", target_y - tracking.y_coord);
    while(true){
        error_d = sqrt(pow(target_x - tracking.x_coord, 2) + pow(target_y - tracking.y_coord, 2));
        difference_a = atan2(target_y - tracking.y_coord, target_x - tracking.x_coord) + tracking.global_angle;
        local_error_x = error_d * cos(difference_a);
        // local_error_y = error_d * sin(local_a);
        error_a = target_a - tracking.global_angle;

        tracking.power_x = local_error_x * kp_x;
        tracking.power_a = error_a * kp_a;
        total_power = fabs(tracking.power_x) + fabs(tracking.power_a);
        if (total_power > max_power) {
            scale = max_power / total_power;
            tracking.power_x *= scale;
            tracking.power_a *= scale;
            tracking.power_y = 0;
            printf("WARNING: A ROBOT IS PUSHING US OR SOMETHING ELSE IS VERY WRONG.\n");
        }
        else  tracking.power_y = sgn(orig_y_sgn) * (max_power - fabs(tracking.power_x) - fabs(tracking.power_a));
        printf("pow_x %f, pow_y: %f, power_a: %f",tracking.power_x, tracking.power_y, tracking.power_a);
        printf("cur_y_sgn: %lf, cur_y: %lf\n", target_y - tracking.y_coord, tracking.y_coord);
        // exit condition
        // if (claw_touch.get_value()){
        //   // got_goal = goal_lim_switch_state;
        //   claw_out.set_value(1);
        // 	claw_in.set_value(0);
        //   drivebase.brake();
        //   printf("x: %lf, y: %lf, a: %lf\n", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
        //   printf("GOT TO GOAL: %d\n", millis());
        //   return;
        // }
        if (fabs(tracking.y_coord) > fabs(target_y) + 10.0){
          printf("target_y: %lf\n", target_y);
          printf("x: %lf, y: %lf, a: %lf\n", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
          printf("FAILED GETTING TO GOAL: %d\n", millis());
          drivebase.brake();
          return;
        }
        // if(millis() - last_time > 50){
          // printf("powers| x: %lf, y: %lf, a: %lf\n", tracking.power_x, tracking.power_y, tracking.power_a);
          // printf("x: %lf, y: %lf, a: %lf\n", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));

        // }


        drivebase.move(tracking.power_x, tracking.power_y, tracking.power_a);
        delay(10);
    }

}

void move_on_arc(const Point start, Position target, const double radius, const bool positive, const double max_power, const bool angle_relative_to_arc, const double min_angle_percent, const double min_x_line_percent, const bool brake){
  Position error, kp = Position(30.0, 12.0, 180.0);

  target.angle = deg_to_rad(target.angle);
  // variable 'd' in diagram
  double chord_dist = sqrt(pow(target.x - start.x, 2) + pow(target.y - start.y, 2));
  printf("chord_dist = %f\n", chord_dist);
  double theta = atan2(target.y - start.y, target.x - start.x);
  printf("theta = %f\n", rad_to_deg(theta));
  double half_chord_dist = chord_dist / 2.0;
  printf("half_chord_dist: %lf\n", half_chord_dist);
  if(radius < half_chord_dist){
    printf("The radius needs to be at least %lf inches.\n", half_chord_dist);
    return;
  }
  double alpha = acos(half_chord_dist / radius);
  printf("alpha: %lf\n", rad_to_deg(alpha));
  theta += positive ? alpha : -alpha;

  printf("new_theta: %lf\n", rad_to_deg(theta));

  Point arc_centre = {start.x + cos(theta) * radius, start.y + sin(theta) * radius};
  printf("Arc centre: x: %lf, y: %lf\n", arc_centre.x, arc_centre.y);

  Position arc_disp;
  double hypotenuse, h;
  double angle_of_arc = atan2(target.y - arc_centre.y, target.x - arc_centre.x);
  double beta; // angle relative to horizontal of h + robot angle + arc_disp.angle

  // move on arc variables
  double power_x_scale, power_y_scale;

  double min_arc_x_power = max_power * min_x_line_percent, min_power_a = max_power * min_angle_percent;
  double pre_scaled_power_a = tracking.power_a;
  double angle_power_guarantee_xy_scale;
  double max_power_scale;
  double power_xy;

  double arc_x_local_x_power, arc_x_local_y_power, arc_y_local_x_power, arc_y_local_y_power;

  double arc_x_power;
  double pre_scaled_arc_x_power = arc_x_power;
  double arc_y_power;
  // double pre_scaled_arc_y_power = arc_y_power;

  double power_y_arc_scale;

  double arc_x_power_scale;
  double arc_y_power_scale;
  double total_power;

  while(true){
    arc_disp.angle = atan2(tracking.y_coord - arc_centre.y, tracking.x_coord - arc_centre.x);
    // printf("arc_disp.angle: %lf\n", arc_disp.angle);
    hypotenuse = sqrt(pow(tracking.x_coord - arc_centre.x, 2) + pow(tracking.y_coord - arc_centre.y, 2)); // dist from centre of arc to cur position
    arc_disp.x = radius - hypotenuse;
    arc_disp.y = radius * near_angle(angle_of_arc, arc_disp.angle);
    // printf("\n Arc displacements | x: %lf, y: %lf\n", arc_disp.x, arc_disp.y);

    // rotating arc displacements
    h = sqrt(pow(arc_disp.x, 2) + pow(arc_disp.y, 2));  // non-split calc
    // printf("h: %lf\n", h);
    // beta = tracking.global_angle + arc_disp.angle;
    beta = atan2(arc_disp.y, arc_disp.x) + tracking.global_angle + arc_disp.angle;  // non-split calc

    // printf("beta: %lf, arc_disp.angle: %lf, tracking_angle: %lf\n", rad_to_deg(atan2(arc_disp.y, arc_disp.x)), rad_to_deg(arc_disp.angle), rad_to_deg(tracking.global_angle));
    // printf("total: %lf\n", rad_to_deg(beta));

    // actually splits the powers now lol
    // also put each power through a PID later
    // arc_x_local_x_power = kp.x * fabs(arc_disp.x) * -cos(beta);
    // arc_x_local_y_power = kp.y * fabs(arc_disp.x) * -sin(beta);
    // arc_y_local_x_power = kp.x * fabs(arc_disp.y) * -sin(beta);
    // arc_y_local_y_power = kp.y * fabs(arc_disp.y) * cos(beta);

    // error.x = fabs(arc_disp.x) * -cos(beta) + fabs(arc_disp.y) * -sin(beta);
    // error.y = fabs(arc_disp.x) * -sin(beta) + fabs(arc_disp.y) * cos(beta);

    // non-split calc
    error.x = h * cos(beta);
    error.y = h * sin(beta);
    // printf("Local errors | x: %lf, y: %lf\n", error.x, error.y);

    if (angle_relative_to_arc){
        error.angle = near_angle(-arc_disp.angle, tracking.global_angle);
        printf("angle target: %lf, angle_error: %lf\n", rad_to_deg(-arc_disp.angle), rad_to_deg(error.angle));
    }
    else  error.angle = near_angle(target.angle, tracking.global_angle);
    tracking.power_x = error.x * kp.x;
    tracking.power_y = error.y * kp.y;
    tracking.power_a = error.angle * kp.angle;

    // MOVE ON ARC CODE
    total_power = fabs(tracking.power_x) + fabs(tracking.power_y) + fabs(tracking.power_a);
    /*
    if(total_power > max_power){
        // init variables

        pre_scaled_power_a = tracking.power_a;

        angle_power_guarantee_xy_scale = 1.0;
        // end of init variables

        max_power_scale = max_power / total_power;
        // printf("max_power: %lf\n", total_power * max_power_scale);
        tracking.power_x *= max_power_scale, tracking.power_y *= max_power_scale, tracking.power_a *= max_power_scale;
    // start of angle power guarantee

        // power_xy = fabs(tracking.power_x) + fabs(tracking.power_y);
        //
        // if (fabs(pre_scaled_power_a) > min_power_a){
        //     if (fabs(tracking.power_a) < min_power_a)  tracking.power_a = min_power_a * sgn(tracking.power_a);  // power_a has been overshadowed
        // }
        // // angle gets the power it demanded if pre_scaled power_a was also less than min_power_a
        // else    tracking.power_a = pre_scaled_power_a;
        // if (power_xy > 0)  angle_power_guarantee_xy_scale = (max_power - fabs(tracking.power_a)) / power_xy;
        // tracking.power_x *= angle_power_guarantee_xy_scale, tracking.power_y *= angle_power_guarantee_xy_scale;
        // printf("****power_a: %lf, x:%lf, y: %lf, pre_scaled: %lf, min: %lf\n", tracking.power_a, tracking.power_x , tracking.power_y, pre_scaled_power_a, min_power_a);

    // end of angle power guarantee

    // start of arc_x_power guarantee
        // scaling from local x and y powers to arc x and y
        //
        // arc_x_local_x_power *= angle_power_guarantee_xy_scale;
        // arc_y_local_x_power *= angle_power_guarantee_xy_scale;
        //
        // arc_x_local_y_power *= angle_power_guarantee_xy_scale;
        // arc_y_local_y_power *= angle_power_guarantee_xy_scale;
        // // move on arc stuff
        // //init
        //
        // pre_scaled_arc_x_power = arc_x_power = fabs(arc_x_local_x_power) + fabs(arc_x_local_y_power);
        // arc_y_power = fabs(arc_y_local_x_power) + fabs(arc_y_local_y_power);
        // // pre_scaled_arc_x_power = arc_x_power;
        //
        // power_xy = arc_x_power + arc_y_power;
        //
        // arc_x_power_scale = 1.0;
        // arc_y_power_scale = 1.0;
        //
        // total_power = power_xy + fabs(tracking.power_a);
        // ///////
        // // giver arc_x as much as possible without taking away power already guaranteed to angle
        // min_arc_x_power = power_xy - min_arc_x_power < 0 ? power_xy : min_x_line_percent * max_power;
        // // end of init
        // if (pre_scaled_arc_x_power > min_arc_x_power){
        //     if (arc_x_power < min_arc_x_power){   // arc_x_power has been overshadowed
        //         arc_x_power = min_arc_x_power;
        //     }
        // }
        // // x_arc_power gets the power it demanded if pre_scaled arc_power was also less than min_x_arc_power
        // else    arc_x_power = pre_scaled_arc_x_power;
        //
        // // prevents division by 0 issue
        // if (pre_scaled_arc_x_power > 0)   arc_x_power_scale = arc_x_power / pre_scaled_arc_x_power;
        // if (arc_y_power > 0) arc_y_power_scale = (power_xy - arc_x_power) / arc_y_power;
        //
        // arc_x_local_x_power *= arc_x_power_scale;
        // arc_y_local_x_power *= arc_y_power_scale;
        //
        // arc_x_local_y_power *= arc_x_power_scale;
        // arc_y_local_y_power *= arc_y_power_scale;
        //
        // tracking.power_x = arc_x_local_x_power + arc_y_local_x_power;
        // tracking.power_y = arc_x_local_y_power + arc_y_local_y_power;

    // end of move on arc stuff

        // return power_a;
    }
    */
    // end of scaling
    double sum = fabs(tracking.power_a) + fabs(tracking.power_x) + fabs(tracking.power_y);
    // printf("power_a: %lf, power_x: %lf, power_y: %lf, sum: %lf\n", tracking.power_a, tracking.power_x, tracking.power_y, sum);
    // END OF MOVE ON ARC CODE
    drivebase.move(tracking.power_x, tracking.power_y, 0);

    if (fabs(target.x - tracking.x_coord) < 0.5 && fabs(target.y - tracking.y_coord) < 0.5 && fabs(rad_to_deg(error.angle)) < 5.0){
      printf("x: %lf, y: %lf, a: %lf\n", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
      printf("MOVE FINISHED\n");
      if(brake) drivebase.brake();
      else drivebase.move(0, 0, 0);
      printf("min_a: %lf\n", min_power_a);
      break;
    }
    delay(10);
  }
}

// TANK MOVE ALGORITHMS

void tank_move_to_target(const Position target, const bool turn_dir_if_0, const double max_power, const double min_angle_percent, const bool brake){
    // Coord tracking = {0.0, 0.0, deg_to_rad(10.0)};
    Point local_error;
    Position error;

    double total_power, max_power_scale;
    int sgn_local_error_y;
    double difference_a;
    double hypotenuse;

    double kp_y = 9.0, kp_a = 150.0;
    // printf("Local errors | x: %lf, y: %lf \n", local_error.x, local_error.y);
    double min_power_a = max_power * min_angle_percent;
    double pre_scaled_power_a;
    double end_error = 0.5;

  // OVERSHOOT variables
    double cur_point_angle, orig_point_angle = atan2(target.x - tracking.x_coord, target.x - tracking.x_coord);
    bool quad_3_4;
    double  orig_pos_angle, cur_pos_angle;
    int orig_pos_sgn = sgn(orig_pos_angle), cur_pos_sgn;
    if (fabs(orig_pos_angle) > M_PI/2) quad_3_4 = true;
    else quad_3_4 = false;

    while(true){
      difference_a = tracking.global_angle + atan2(target.y - tracking.y_coord, target.x - tracking.x_coord);
      hypotenuse = sqrt(pow(target.x - tracking.x_coord, 2.0) + pow(target.y - tracking.y_coord, 2.0));

      // local_error.x = cos(difference_a) * hypotenuse; // unused

      sgn_local_error_y = sgn(local_error.y);
      printf("sgn_y: %d\n", sgn_local_error_y);
      if(sgn_local_error_y == 0){
        sgn_local_error_y = turn_dir_if_0 ? 1 : -1;
        printf("triggered\n");
      }
      local_error.y = sin(difference_a) * hypotenuse;
      error.angle = near_angle(sgn_local_error_y * M_PI / 2, difference_a);
      printf("Errors | y: %lf, a: %lf\n", local_error.y, rad_to_deg(error.angle));

      tracking.power_y = kp_y * local_error.y;
      tracking.power_a = kp_a * error.angle;

      // gives min power to local y if that is not satisfied
      if (fabs(tracking.power_y) < min_power_y && fabs(local_error.y) > 0.5) tracking.power_y = min_power_y * sgn(local_error.y);

      // scales powers
      total_power = fabs(tracking.power_y) + fabs(tracking.power_a);
      if (total_power > max_power){
        // init variables

        pre_scaled_power_a = tracking.power_a;

        // end of init variables

        max_power_scale = max_power / total_power;
        tracking.power_y *= max_power_scale;
        tracking.power_a *= max_power_scale;


    // start of angle power guarantee


        if (fabs(pre_scaled_power_a) > min_power_a){
            if (fabs(tracking.power_a) < min_power_a)  tracking.power_a = min_power_a * sgn(tracking.power_a);  // power_a has been overshadowed
        }
        // angle gets the power it demanded if pre_scaled power_a was also less than min_power_a
        else    tracking.power_a = pre_scaled_power_a;
        tracking.power_y = sgn(local_error.y) * (max_power - fabs(tracking.power_a));
        printf("****power_a: %lf, x:%lf, y: %lf, pre_scaled: %lf, min: %lf\n", tracking.power_a, tracking.power_x , tracking.power_y, pre_scaled_power_a, min_power_a);
        // end of angle power guarantee

      }
      printf("Powers | y: %lf, a: %lf\n",tracking.power_y, tracking.power_a);
      printf("time: %d, TRACKING: %f, %f, %f \n\n", millis(), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));

      error.x = target.x - tracking.x_coord;
      error.y = target.y - tracking.y_coord;

      cur_point_angle = atan2(error.x, error.y);
      cur_pos_sgn = sgn(cur_pos_angle);
      if (quad_3_4 && cur_pos_sgn != orig_pos_sgn)   cur_pos_angle += orig_pos_sgn * (M_PI * 2); printf("trg\n");

      if((fabs(target.x - tracking.x_coord) < end_error && fabs(target.y - tracking.y_coord) < end_error) || fabs(orig_point_angle - cur_point_angle) > M_PI/2){
        if (brake) drivebase.brake();
        tracking.move_complete = true;
        printf("Ending move to target X: %f Y: %f A: %f at X: %f Y: %f A: %f \n", target.x, target.y, target.angle, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
        //log_time("ending starting time: %d, delta time: %d X: %f Y: %f A: %f from X: %f Y: %f A: %f \n", millis(),millis() -starttime, target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
        // tracking.move_stop_task();
        break;
      }

      drivebase.move_tank(tracking.power_y, tracking.power_a);

      delay(10);
    }
    tank_turn_to_angle(target.angle, brake);

}

void tank_move_on_line(const Position target, const bool turn_dir_if_0, const double max_power, const double min_angle_percent, const bool brake){
    // Coord tracking = {0.0, 0.0, deg_to_rad(10.0)};
    Point local_error;
    Position error;

    double total_power, max_power_scale;
    int sgn_local_error_y;
    double difference_a;
    double hypotenuse;

    double kp_y = 9.0, kp_a = 150.0;
    // printf("Local errors | x: %lf, y: %lf \n", local_error.x, local_error.y);
    double min_power_a = max_power * min_angle_percent;
    double pre_scaled_power_a;
    double end_error = 0.5;

    // move on line variables
    Vector follow_line(target.y - tracking.y_coord, target.x - tracking.x_coord); // used to keep track of angle of follow_line relative to the vertical
    Vector line_disp(target.x - tracking.x_coord, target.y - tracking.y_coord);  // displacements relative to line
    double line_y_local_y; // local_y component of power along line

    while(true){
      // start of move_on_line stuff
      line_disp = Vector(target.x - tracking.x_coord, target.y - tracking.y_coord);  // displacements relative to line
      // line_disp.set_polar(line_disp.get_magnitude(), line_disp.get_angle() + follow_line.get_angle());  // rotates vector by line angle
      line_disp.rotate(follow_line.get_angle());  // rotates vector by line angle
      line_y_local_y = line_disp.get_y() * cos(tracking.global_angle - follow_line.get_angle());

      // end of move_on_line stuff
      difference_a = tracking.global_angle + atan2(target.y - tracking.y_coord, target.x - tracking.x_coord);
      hypotenuse = sqrt(pow(target.x - tracking.x_coord, 2.0) + pow(target.y - tracking.y_coord, 2.0));

      // local_error.x = cos(difference_a) * hypotenuse; // unused

      sgn_local_error_y = sgn(local_error.y);
      printf("sgn_y: %d\n", sgn_local_error_y);
      if(sgn_local_error_y == 0){
        sgn_local_error_y = turn_dir_if_0 ? 1 : -1;
        printf("triggered\n");
      }
      local_error.y = sin(difference_a) * hypotenuse;
      error.angle = near_angle(sgn_local_error_y * M_PI / 2, difference_a);
      printf("Errors | y: %lf, a: %lf\n", local_error.y, rad_to_deg(error.angle));

      tracking.power_y = kp_y * local_error.y;
      // tracking.power_y = kp_y * line_y_local_y;
      tracking.power_a = kp_a * error.angle;

      // gives min power to local y if that is not satisfied
      if (fabs(tracking.power_y) < min_power_y && fabs(local_error.y) > 0.5) tracking.power_y = min_power_y * sgn(local_error.y);

      // scales powers
      total_power = fabs(tracking.power_y) + fabs(tracking.power_a);
      if (total_power > max_power){
        // init variables

        pre_scaled_power_a = tracking.power_a;

        // end of init variables

        max_power_scale = max_power / total_power;
        tracking.power_y *= max_power_scale;
        tracking.power_a *= max_power_scale;


    // start of angle power guarantee


        if (fabs(pre_scaled_power_a) > min_power_a){
            if (fabs(tracking.power_a) < min_power_a)  tracking.power_a = min_power_a * sgn(tracking.power_a);  // power_a has been overshadowed
        }
        // angle gets the power it demanded if pre_scaled power_a was also less than min_power_a
        else    tracking.power_a = pre_scaled_power_a;
        tracking.power_y = sgn(local_error.y) * (max_power - fabs(tracking.power_a));
        printf("****power_a: %lf, x:%lf, y: %lf, pre_scaled: %lf, min: %lf\n", tracking.power_a, tracking.power_x , tracking.power_y, pre_scaled_power_a, min_power_a);
        // end of angle power guarantee

      }
      printf("Powers | y: %lf, a: %lf\n",tracking.power_y, tracking.power_a);
      printf("time: %d, TRACKING: %f, %f, %f \n\n", millis(), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));

      error.x = target.x - tracking.x_coord;
      error.y = target.y - tracking.y_coord;

      if(fabs(line_disp.get_y()) < 0.5){
        if (brake) drivebase.brake();
        tracking.move_complete = true;
        printf("Ending move to target X: %f Y: %f A: %f at X: %f Y: %f A: %f \n", target.x, target.y, target.angle, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
        //log_time("ending starting time: %d, delta time: %d X: %f Y: %f A: %f from X: %f Y: %f A: %f \n", millis(),millis() -starttime, target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
        // tracking.move_stop_task();
        break;
      }
      drivebase.move_tank(tracking.power_y, tracking.power_a);

      delay(10);
    }
    tank_turn_to_angle(target.angle, brake);
}

void tank_turn_to_angle(double target_a, const bool brake){
  PID angle_pid(200.0, 0.0, 0.0, 0.0, true, 0.0, 360.0);
  // drivebase.move_tank(0, angle_pid.compute(tracking.global_angle, near_angle(target_a, tracking.global_angle) + tracking.global_angle));
  target_a = deg_to_rad(target_a);
  double error_a, power_a, kp_a = 200.0;

  while(true){
    drivebase.move_tank(0, angle_pid.compute(tracking.global_angle, near_angle(target_a, tracking.global_angle) + tracking.global_angle));
    // error_a = near_angle(target_a, tracking.global_angle);
    // power_a = sgn(error_a) * max(fabs(error_a * kp_a), (double)min_power_a);
    // drivebase.move_tank(0, power_a);
    if(fabs(rad_to_deg(angle_pid.get_error())) < 1.5){
    // if(fabs(rad_to_deg(error_a)) < 1.5){
      printf("Ending turn to angle : %f at X: %f Y: %f A: %f \n", rad_to_deg(target_a), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
      drivebase.move_tank(0, 0);
      if(brake) drivebase.brake();
      return;
    }
  }
}

void tank_turn_to_target(const Point target, const bool brake){
  tank_turn_to_angle(rad_to_deg(atan2(target.x - tracking.x_coord, target.y - tracking.y_coord)), brake);
}

void tank_move_on_arc(Position target, const Point start_pos, const double power, const double max_power, const bool brake){
  Vector p2 = {start_pos.x, start_pos.y}, p1 = {target.x, target.y};  // p2 and p1 are switched because Nikhil messed up the math

  Vector disp = p2 - p1;
  target.angle = deg_to_rad(target.angle);
  // disp.set_polar(disp.get_magnitude(), disp.get_angle() + target.angle);
  disp.rotate(target.angle);  // rotates displacement by target's angle to obtain right triangles from isosceles triangle forming the arc
  double theta = 2.0 * atan2(disp.get_x(), disp.get_y());
  double radius = disp.get_magnitude() / 2.0 / sin(theta / 2.0);
  printf("radius: %lf\n", radius);
  printf("theta: %lf\n", rad_to_deg(theta));
  Point centre = {p1.get_x() + cos(target.angle) * radius, p1.get_y() - sin(target.angle) * radius};
  printf("centre | x: %lf, y: %lf\n", centre.x, centre.y);
  // finished calculating centre coord

  // init variables
  Point g_position;
  Vector arc_velocity = {0.0, 0.0};
  double linear_velocity, angular_velocity_target;
  double total_power, max_power_scale;
  // robot travels at 220 degrees/second while power is 127; kp = 175
  // from power to velocity * 1.6
  // from velocity to power / 1.6 or * 0.625
  // const double pwm_to_vel = 1.6;
  const double pwm_to_vel = 0.02792526803;
  const double kR = 25.0; // for ratio
  // const double kR = 0.75; // for difference

  const double kA = pwm_to_vel * 150.0, kB = 1 / pwm_to_vel, kP = 125.0, kD = 50.0;
  const double final_angle = atan2(target.y - centre.y, target.x - centre.x); // angle of final position to centre at end of move

  uint32_t last_d_update_time = millis();  // for derivative
  double last_angular_velocity = tracking.g_velocity.angle; // for derivative
  int orig_angle_error_sgn = sgn(target.angle - tracking.global_angle);
  double angle_diff;
  double radial_diff;
  radius = fabs(radius);
  // positive turn_dir means cw
  int turn_dir = -sgn(rad_to_deg(near_angle(atan2(target.y - centre.y, target.x - centre.x), atan2(tracking.y_coord - centre.y, tracking.x_coord - centre.x))));
  printf("turn_dir: %d\n", turn_dir);
  while(true){
    g_position = {tracking.x_coord, tracking.y_coord};
    disp = g_position - centre;
    // printf("local_a: %lf\n", rad_to_deg(disp.get_angle()));

    arc_velocity.set_cartesian(tracking.g_velocity.x, tracking.g_velocity.y);
    arc_velocity.set_polar(arc_velocity.get_magnitude(), arc_velocity.get_angle() - disp.get_angle() + (turn_dir == -1 ? 0 : M_PI));

    // difference between target angle and current angle taking into account direction of movement and sign of power
    angle_diff = near_angle(-disp.get_angle() + (turn_dir == -1 ? 0 : M_PI), tracking.global_angle + (power < 0.0 ? M_PI : 0));

    // printf("mag: %lf, a: %lf\n", arc_velocity.get_magnitude(), rad_to_deg(arc_velocity.get_angle()));
    linear_velocity = arc_velocity.get_y();
    // printf("linear_vel: %lf\n", linear_velocity);
    // printf("x: %lf, linear_v: %lf \n", arc_velocity.get_x(), linear_velocity);

    // logarithm version
    // angular_velocity_target = turn_dir * (linear_velocity / disp.get_magnitude() + kR * log(disp.get_magnitude() / radius)) + kA * angle_diff;

    // linear version
    radial_diff = disp.get_magnitude() / radius;
    // signs radial diff
    if (radial_diff < 1) radial_diff = -1/radial_diff + 1;
    else radial_diff = radial_diff -1;
    angular_velocity_target = turn_dir * (linear_velocity / disp.get_magnitude() + kR * radial_diff) + kA * angle_diff;

    // difference version
    // angular_velocity_target = turn_dir * (linear_velocity / disp.get_magnitude() + kR * (disp.get_magnitude() - radius)) + kA * angle_diff;



    // angular_velocity = linear_velocity / disp.get_magnitude();  // this doesn't work because it=sn't always the right sign
    // angular_velocity = kR * log(disp.get_magnitude() / radius); // sgn is unknown here as well
    // printf("disp_angle: %lf\n", rad_to_deg(disp.get_angle()));
    // angular_velocity = kA * (tracking.global_angle + disp.get_angle() + (power < 0.0 ? M_PI : 0));
#ifndef xy_enable
    // radial ratio
    printf("radius:%lf, angular_vel: %lf, radial_error: %lf, radial_correction: %lf, angle_diff: %lf, angle_correction: %lf\n", radius, kB * turn_dir * linear_velocity / disp.get_magnitude(), radius - disp.get_magnitude(), kB * turn_dir * kR * log(disp.get_magnitude() / radius), rad_to_deg(angle_diff), kB * kA * angle_diff);
    // radial diff
    // printf("radius:%lf, disp_mag: %lf, angular_vel: %lf, radial_error: %lf, radial_correction: %lf, angle_diff: %lf, angle_correction: %lf\n", radius, disp.get_magnitude(), kB * turn_dir * linear_velocity / disp.get_magnitude(),  turn_dir * (disp.get_magnitude() - radius), kB * turn_dir * kR * (disp.get_magnitude() - radius), rad_to_deg(angle_diff), kB * kA * angle_diff);

#endif
    // tracking.power_a = turn_dir * (kB * angular_velocity + kP * tracking.g_velocity.angle + kD * (tracking.g_velocity.angle - last_angular_velocity) / (millis() - last_d_update_time));

    // without p and d
    // tracking.power_a = kB * angular_velocity_target;

    // with p and d
    tracking.power_a = kB * angular_velocity_target + kP * (angular_velocity_target - tracking.g_velocity.angle) + kD * (tracking.g_velocity.angle - last_angular_velocity) / (millis() - last_d_update_time);



    // printf("p:%lf, d:%lf\n",  kP * (angular_velocity_target - tracking.g_velocity.angle), kD * (tracking.g_velocity.angle - last_angular_velocity));
    last_d_update_time = millis();
    last_angular_velocity = tracking.g_velocity.angle;
    // finished calculating angular velocity

    // printf("Angular displacement velocity: %lf", rad_to_deg(tracking.global_angle + disp.get_angle()));
    tracking.power_y = power;
    // power scaling
    total_power = fabs(tracking.power_y) + fabs(tracking.power_a);
    if (total_power > max_power){
      max_power_scale = max_power / total_power;
      tracking.power_y *= max_power_scale;
      tracking.power_a *= max_power_scale;
    }
    // exit condition
    // if (orig_angle_error_sgn != sgn(target.angle - tracking.global_angle)){
    if (rad_to_deg(fabs(final_angle - atan2(tracking.y_coord - centre.y, tracking.x_coord - centre.x))) < 1.5){
      printf("ang: %lf\n", atan2(tracking.y_coord - centre.y, tracking.x_coord - centre.x));
      drivebase.move_tank(0.0, 0.0);
      if (brake)  drivebase.brake();
      printf("Ending move on arc to target X: %f Y: %f A: %f at X: %f Y: %f A: %f \n", target.x, target.y, target.angle, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
      return;
    }
#ifdef xy_enable
    printf("%lf,%lf\n", tracking.x_coord, tracking.y_coord);
#else
    printf("power_y:%lf, power_a: %lf\n",tracking.power_y, tracking.power_a);
#endif
    drivebase.move_tank(tracking.power_y, tracking.power_a);
    delay(10);
  }
}

/*
// Start of velocity relation testing
	master.clear();
	delay(50);
	tracking.power_a = 127;
	while(true){
		// printf("")
		// if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP))	tracking.power_a += 5;
		// if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN))	tracking.power_a -= 5;
		master.print(0, 0, "pow:%.lf, vel:%.lf", tracking.power_a, rad_to_deg(tracking.g_velocity.angle));
		printf("pow:%.lf, vel:%.lf, raw:%lf, predicted: %lf\n", tracking.power_a, rad_to_deg(tracking.g_velocity.angle), tracking.g_velocity.angle, 50 * deg_to_rad(1.6));
		// drivebase.move_tank(0, tracking.power_a);
		drivebase.move_tank(0, 50);
		delay(50);
	}
// "approximately" x power will result in (1.6x)degrees/sec
// OR x power will result in (0.02792526803x)radians/sec

// power: 30, velocity: 35 | 1.16
// power: 60, velocity: 90 | 1.5
// power: 75, velocity: 120	| 1.6
// power: 90, velocity: 150 | 1.6
// power: 95, velocity: 165 | 1.7
// power: 127, velocity: 230 | 1.8

// End of velocity relation testing
*/
