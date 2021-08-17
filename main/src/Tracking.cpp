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
    // printf("here1");
    moveTask->remove();
    delete moveTask;
    moveTask = nullptr;
  }
}
void Tracking::move_start_task( pros::task_fn_t function){
  moveTask = new Task(function);
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
      // printf("%f,%f\n", tracking.x_coord, tracking.y_coord);

    // printf("time: %d, TRACKING: %f, %f, %f \n", millis(), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
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
  tracking.move_start_task(move_to_target);
  delay(10);
}

void Tracking::wait_for_complete(){
  while(!tracking.move_complete) delay(10);
}
void Tracking::wait_for_error(double distance){

  while(fabs(tracking.drive_error)- distance > 1)delay(10);
}

void move_to_target(void* params){
  double error_x = 0.0, error_y = 0.0, error_a = 0.0, local_error_x = 0.0, local_error_y = 0.0;
  double error_d = 0.0, difference_a = 0.0;
  double kp_x = 17.0, kp_y = 7.0, kp_a = 100, ki_x = 0.00, ki_y = 0.0, ki_a = 0.0, kd_x = 0.0, kd_y = 0.0, kd_a = 0.0;
  double target_x = move_params.target_x, target_y = move_params.target_y, target_a = move_params.target_a;
  double end_error = move_params.end_error, end_error_a = move_params.end_error_a;
  bool Brake = move_params.Brake, debug = move_params.debug;
  int max_power = move_params.max_power;
  int lasttime = 0, starttime = millis(), integral_last_time = millis();
  double overflow, overflow_x, overflow_y, overflow_a;
  double total_power;
  double integral_x, integral_y;
  bool Overshoot = move_params.Overshoot; //
  //log_time("starting time: %d X: %f Y: %f A: %f from X: %f Y: %f A: %f \n", millis(), target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
  printf("Starting move to target X: %f Y: %f A: %f from X: %f Y: %f A: %f  Time %d\n", target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle), starttime);
  target_a = deg_to_rad(target_a);

  tracking.move_complete = false;
  tracking.movetime = millis();
  error_x = target_x - tracking.x_coord;  // I added this, senior devs check it doesn't mess with stuff
  error_y = target_y - tracking.y_coord;

  double orig_point_angle = atan2(error_x, error_y); //
  double cur_point_angle;
  int perpendicular = M_PI/2;

  while(true){
    error_x = target_x - tracking.x_coord;
    error_y = target_y - tracking.y_coord;
    error_a = target_a - tracking.global_angle;
    error_d = sqrt(pow(error_x, 2.0) + pow(error_y, 2.0));
    tracking.drive_error = error_d;
    difference_a = tracking.global_angle + atan2(error_y, error_x);
    local_error_x = cos(difference_a)*error_d;
    local_error_y = sin(difference_a)*error_d;

    if(fabs(local_error_x) > 0.3){
      tracking.power_x = cos(difference_a)*error_d * kp_x;
      if (fabs(tracking.power_x) < min_power_x&& max_power > min_power_x) tracking.power_x = min_power_x* sgn(local_error_x);
    }
    else tracking.power_x = 0.0;

    if(fabs(local_error_y) > 0.3){
      tracking.power_y = sin(difference_a)*error_d *kp_y;
      if (fabs(tracking.power_y) < min_power_y && max_power > min_power_y) tracking.power_y = min_power_y* sgn(local_error_y);
    }
    else tracking.power_y = 0.0;

    if(fabs(error_a) > deg_to_rad(end_error_a)){
      tracking.power_a = error_a * kp_a;
      if (fabs(tracking.power_a) < min_power_a) tracking.power_a = min_power_a * sgn(error_a);
    }
    else tracking.power_a = 0;


    if(millis()-integral_last_time >50){
      if (fabs(local_error_x) < 5.0){
        integral_x += fabs(local_error_x) * (millis() - integral_last_time);
      }
      if (fabs(local_error_y) < 5.0){
        integral_y += fabs(local_error_y) * (millis() - integral_last_time);
      }

      if(fabs(local_error_x)<=0.9) {
        integral_x = 0;
      }
      if(fabs(local_error_y)<=0.9) {
        integral_y = 0;
      }
    }

    tracking.power_x+= sgn(tracking.power_x) * integral_x * ki_x;
    tracking.power_y+= sgn(tracking.power_y) * integral_y* ki_y;

    total_power = fabs(tracking.power_x)+fabs(tracking.power_y)+fabs(tracking.power_a);
    if(total_power>max_power){
      tracking.power_x = tracking.power_x/total_power * max_power;
      tracking.power_y = tracking.power_y/total_power * max_power;
      tracking.power_a = tracking.power_a/total_power * max_power;
    }

    move(tracking.power_x, tracking.power_y, tracking.power_a);

    if(debug && millis() - lasttime > 50){
      printf("Error x: %f, Error y: %f, Error a: %f \n", error_x,error_y, rad_to_deg(error_a));
      printf("power x: %f, power y: %f, power a: %f \n", tracking.power_x, tracking.power_y, tracking.power_a);
      printf("GlobalX: %f, GlobalY: %f, GlobalA: %f \n",tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
      printf("integ X: %f, integ Y: %f \n",integral_x, integral_y);
      lasttime = millis();
    }

    cur_point_angle = atan2(error_x, error_y);
    if(Overshoot){
      if(fabs(orig_point_angle - cur_point_angle) > perpendicular && fabs(error_a) < deg_to_rad(end_error_a)){
        if (Brake){
          brake();  printf("\n\n ***************BRAKE");brake();
        }
        tracking.move_complete = true;
        printf("Ending move to target X: %f Y: %f A: %f at X: %f Y: %f A: %f   Time elapsed: %d\n", target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle), millis()-starttime);
        //log_time("ending starting time: %d, delta time: %d X: %f Y: %f A: %f from X: %f Y: %f A: %f \n", millis(),millis() -starttime, target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
        tracking.move_stop_task();
        tracking.movetime = 0;
        return;
      }
    }

    else if(!Overshoot){
      if(fabs(error_x) < end_error && fabs(error_y) < end_error && fabs(error_a) < deg_to_rad(end_error_a)){
        if (Brake){
          brake();  printf("\n\n ***************BRAKE");brake();
        }
        tracking.move_complete = true;
        printf("Ending move to target X: %f Y: %f A: %f at X: %f Y: %f A: %f   Time elapsed: %d\n", target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle), millis()-starttime);
        //log_time("ending starting time: %d, delta time: %d X: %f Y: %f A: %f from X: %f Y: %f A: %f \n", millis(),millis() -starttime, target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
        tracking.move_stop_task();
        tracking.movetime = 0;
        return;
      }
    }


    delay(10);
  }
}


void move_on_line_sync(double target_x, double target_y, double target_a, bool Brake, bool debug, int max_power, bool Overshoot, double end_error, double end_error_a, double p,double lpercent, double apercent){
  if(!tracking.move_complete) tracking.move_stop_task();
  if(moveTask != nullptr) moveTask = nullptr;
  // printf("here2");
  move_params = {target_x, target_y, target_a, Brake, debug, max_power, Overshoot, end_error, end_error_a, p, lpercent, apercent};
  tracking.move_complete = false;
  move_on_line(nullptr);

}
void move_on_line_async(double target_x, double target_y, double target_a, bool Brake, bool debug, int max_power, bool Overshoot, double end_error, double end_error_a, double p, double lpercent, double apercent){
  if(!tracking.move_complete) tracking.move_stop_task();
  move_params = {target_x, target_y, target_a, Brake, debug, max_power, Overshoot, end_error, end_error_a, p, lpercent, apercent};
  tracking.move_complete = false;
  printf("async task started %d \n", millis());
  tracking.move_start_task(move_on_line);
  delay(10);
}

void move_on_line(void* params){
  double target_x = move_params.target_x, target_y = move_params.target_y, target_a = deg_to_rad(move_params.target_a);
  double end_error = move_params.end_error, end_error_a = move_params.end_error_a;
  bool Brake = move_params.Brake, debug = move_params.debug;
  int max_power = move_params.max_power;
  bool Overshoot = move_params.Overshoot; //
  double lpercent= move_params.lpercent, apercent=move_params.apercent;
  double kp_lx =17.0, kp_ly =7.0, kp_dx = 17.0, kp_dy = 7.0, kp_a = 50.0, ki_lx = 0.0, ki_ly = 0.0, ki_dx = 0.0, ki_dy = 0.0, ki_a = 0.0, kd_lx = 0.0, kd_ly = 0.0, kd_dx = 0.0, kd_dy = 0.0, kd_a = 0.0;
  double error_angle, error_line, error_d, error_x, error_y, error_a , error_tot;
  double power_line_x, power_line_y, power_d_x, power_d_y, total_power, line_total;
  double dif_a;
  double min_lx, min_ly, min_a, min_total;
  double integ_start = 5.0, integ_l, integ_d, integ_a;
  double deriv_l, deriv_d, deriv_a, last_error_l, last_error_d, last_error_a;
  uint32_t integ_last_time = millis(), deriv_time = millis(), start_time = millis();
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

    power_line_x = cos(dif_a) *error_line *kp_lx;
    power_line_y = -sin(dif_a) *error_line *kp_ly;
    power_d_x = sin(dif_a) * error_d * kp_dx;
    power_d_y = cos(dif_a) * error_d * kp_dy;
    tracking.power_a = error_a *kp_a;

    power_line_x += cos(dif_a)*integ_l  *ki_lx;
    power_line_y += -sin(dif_a)*integ_l *ki_ly;
    power_d_x += sin(dif_a)* integ_d *ki_dx;
    power_d_y += cos(dif_a)* integ_d *ki_dy;

    power_line_x += cos(dif_a)*deriv_l*kd_lx;
    power_line_y += -sin(dif_a)*deriv_l*kd_ly;
    power_d_x += sin(dif_a)*deriv_d*kd_dx;
    power_d_y += cos(dif_a)*deriv_d*kd_dy;
    tracking.power_a += deriv_a*kd_a;




    if(millis() - last_time > 50){
      printf("pre sc lx: %f, ly: %f, dx: %f, dy: %f, a: %f\n",power_line_x,power_line_y,power_d_x,power_d_y,tracking.power_a);
    }

    total_power = fabs(power_line_x+power_d_x+power_line_y+power_d_y+tracking.power_a);
    if(total_power > max_power){
      line_total = fabs(power_line_x)+fabs(power_d_x);
      if(line_total > lpercent*max_power){
        min_lx = (fabs(power_line_x)/line_total) *lpercent*max_power;
        min_ly = (fabs(power_line_y)/line_total) *lpercent*max_power;
      }
      else{
        min_lx = fabs(power_line_x);
        min_ly = fabs(power_line_y);
      }
      if(fabs(tracking.power_a) > apercent*max_power) min_a = apercent*max_power;
      else min_a = fabs(tracking.power_a);
      min_total = min_lx + min_ly + min_a;
      power_line_x =((fabs(power_line_x)/(total_power-min_total))*(max_power-min_total)+min_lx)*sgn(power_line_x);
      power_line_y =((fabs(power_line_y)/(total_power-min_total))*(max_power-min_total)+min_ly)*sgn(power_line_y);
      power_d_x =(power_d_x/(total_power-min_total))*(max_power-min_total);
      power_d_y =(power_d_y/(total_power-min_total))*(max_power-min_total);
      tracking.power_a =((fabs(tracking.power_a)/(total_power-min_total))*(max_power-min_total)+min_a)*sgn(tracking.power_a);
    }


    tracking.power_x = power_line_x + power_d_x;
    if(fabs(error_x) < end_error - 0.2) tracking.power_x = 0.0;
    else if(fabs(tracking.power_x) < min_power_x)tracking.power_x = sgn(tracking.power_x)*min_power_x;
    tracking.power_y = power_line_y + power_d_y;
    if(fabs(error_y) < end_error - 0.2) tracking.power_y = 0.0;
    else if(fabs(tracking.power_y) < min_power_y)tracking.power_y = sgn(tracking.power_y)*min_power_y;
    if(fabs(rad_to_deg(error_a)) < end_error_a - 1.0)tracking.power_a = 0.0;
    else if(fabs(tracking.power_a) < min_power_a)tracking.power_a = sgn(tracking.power_a)*min_power_a;




    move(tracking.power_x,tracking.power_y,tracking.power_a);

    if(millis() - last_time > 50 && debug){
      printf("errors x: %.2f, y: %.2f, a: %.2f\n",error_x, error_y,rad_to_deg(error_a));
      printf("errors l: %.2f, d: %.2f\n",error_line, error_d);
      printf("powers x: %.2f, y: %.2f, a: %.2f\n",tracking.power_x, tracking.power_y, tracking.power_a);
      printf("post s lx: %f, ly: %f, dx: %f, dy: %f, a: %f\n",power_line_x,power_line_y,power_d_x,power_d_y,tracking.power_a);
      if(lpercent != 0.0 || apercent != 0.0)printf("mins  lx: %.2f, ly: %.2f, a: %.2f",min_lx,min_ly,min_a);
      if(ki_lx != 0.0 || ki_ly != 0.0 || ki_dx != 0.0 || ki_dy != 0.0 || ki_a != 0.0)printf("integ  l: %.2f, d: %.2f\n",integ_l,integ_d);
      if(kd_lx != 0.0 || kd_ly != 0.0 || kd_dx != 0.0 || kd_dy != 0.0 || kd_a != 0.0)printf("deriv  l: %.2f, d: %.2f a: %.2f\n",deriv_l, deriv_d, deriv_a);
      // printf("%f, %f\n", tracking.y_coord,tracking.power_y);
      // printf("post scal lx: %f, ly: %f, dx: %f, dy: %f, a: %f\n",power_line_x,power_line_y,power_d_x,power_d_y,tracking.power_a);
      // printf("mins lx: %f, ly: %f, a: %f\n",min_lx,min_ly,min_a);
      // printf("error line: %f, d: %f, x: %f, y: %f, a: %f\n",error_line, error_d,error_x, error_y, rad_to_deg(error_a));
      // printf("power line: %f, d: %f, x: %f, y: %f, a: %f\n",power_line, power_d,tracking.power_x,tracking.power_y,tracking.power_a);
      // printf("integral l: %f, d: %f\n",integ_l,integ_d);
      // printf("derivative l: %f, d: %f a: %f\n",deriv_l, deriv_d, deriv_a);
      last_time = millis();
    }

    if(fabs(error_x) < end_error && fabs(error_y) < end_error && fabs(rad_to_deg(error_a)) < end_error_a){
      brake();
      tracking.move_complete = true;
      printf("Ending move to target X: %f Y: %f A: %f at X: %f Y: %f A: %f, time: %d \n", target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle), millis() - start_time);
      //log_time("ending starting time: %d, delta time: %d X: %f Y: %f A: %f from X: %f Y: %f A: %f \n", millis(),millis() -starttime, target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
      tracking.move_stop_task();
      return;
    }






    delay(10);
  }
}
