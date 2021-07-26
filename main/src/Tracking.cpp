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

  int last_velocity_time = 0;    // time of last velocity update
  int velocity_update_time = 0; // time SINCE last velocity update

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
  // printf("time: %d, TRACKING: %f, %f, %f \n", millis(), tracking.x_coord, tracking.y_coord, tracking.global_angle/M_PI *180);
  // printf("ENCODER L: %d, R: %d, B:%d \n", LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());

  if(millis() - 50 > lasttime){
      // printf("%f,%f\n", tracking.x_coord, tracking.y_coord);

    printf("time: %d, TRACKING: %f, %f, %f \n", millis(), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
    // printf("%d pow_a: %.1f, pow_x: %.1f, pow_y: %.1f, total_pow: %.1f\n",millis(),  tracking.power_a, tracking.power_x, tracking.power_y, fabs(tracking.power_a) + fabs(tracking.power_x) + fabs(tracking.power_y));

    printf("ENCODER L: %d, R: %d, B:%d \n", LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());
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

void move_to_target_sync(double target_x, double target_y, double target_a, bool Brake, bool debug, int max_power, bool Overshoot, double end_error, double end_error_a, double p){
  if(!tracking.move_complete) tracking.move_stop_task();
  if(moveTask != nullptr) moveTask = nullptr;
  // printf("here2");
  move_params = {target_x, target_y, target_a, Brake, debug, max_power, Overshoot, end_error, end_error_a, p};
  tracking.move_complete = false;
  move_to_target(nullptr);

}
void move_to_target_async(double target_x, double target_y, double target_a, bool Brake, bool debug, int max_power, bool Overshoot, double end_error, double end_error_a, double p){
  if(!tracking.move_complete) tracking.move_stop_task();
  move_params = {target_x, target_y, target_a, Brake, debug, max_power, Overshoot, end_error, end_error_a, p};
  tracking.move_complete = false;
  printf("async task started %d \n", millis());
  tracking.move_start_task();
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
  double kp = 15.0, kp_a = 140, kI_d = 0.00;
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
      tracking.power_x = cos(difference_a)*error_d * kp;
      if (fabs(tracking.power_x) < min_power&& max_power > min_power) tracking.power_x = min_power* sgn(local_error_x);
    }
    else tracking.power_x = 0.0;

    if(fabs(local_error_y) > 0.3){
      tracking.power_y = sin(difference_a)*error_d *kp;
      if (fabs(tracking.power_y) < min_power && max_power > min_power) tracking.power_y = min_power* sgn(local_error_y);
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

    tracking.power_x+= sgn(tracking.power_x) * integral_x * kI_d;
    //tracking.power_y+= sgn(tracking.power_y) * integral_y* kI_d;

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
