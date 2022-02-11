#include "Tracking.hpp"

Tracking tracking;

_Task update_t(update, "Tracking");
_Task move_t(move_on_arc, "Movement");
Gyro gyro(ramp_imu);
// std::variant<arc_params, line_params, tank_arc_params, point_params, tank_point_params, turn_angle_params, turn_point_params, line_old_params> params_g;
arc_params arc_params_g;
line_params line_params_g;
tank_arc_params tank_arc_params_g;
point_params point_params_g;
tank_point_params tank_point_params_g;
turn_angle_params turn_angle_params_g;
turn_point_params turn_point_params_g;
line_old_params line_old_params_g;



void Tracking::reset(double x, double y, double a){
  update_t.data_update();
  tracking_imp.print("resetting tracking from %.2f, %.2f, %.2f to %.2f, %.2f, %.2f\n", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle),x,y,a);
  tracking.x_coord = x;
  tracking.y_coord = y;
  tracking.global_angle = deg_to_rad(a);
  update_t.done_update();
}

double Tracking::get_angle_in_deg(){
  return fmod(rad_to_deg(global_angle), 360);
}

void Tracking::wait_for_dist(double distance, int timeout){
  const Point start_pos = {tracking.x_coord, tracking.y_coord};
  double delta_dist = 0.0;
  int start_time = millis();
  while(distance >= delta_dist){
    delta_dist = sqrt(pow(tracking.x_coord -start_pos.x,2) + pow(tracking.y_coord - start_pos.y,2));
    delay(10);
    if(timeout != 0 && millis() - start_time > timeout)break;
  }
}

void update(void* params){
  _Task* ptr = _Task::get_obj(params);
  Timer data_timer{"tracking logs"};
  // LeftEncoder.reset(); RightEncoder.reset(); BackEncoder.reset();
  double DistanceLR = 9.787, DistanceB = 0.5;
  double Left, Right, Back, NewLeft, NewRight, NewBack, LastLeft = LeftEncoder.get_value()/360.0 *(2.75*M_PI), LastRight =  RightEncoder.get_value()/360.0 *(2.75*M_PI), LastBack = BackEncoder.get_value()/360.0 *(2.77*M_PI);
  double Theta = 0.0, Beta = 0.0, Alpha = 0.0;
  double RadiusR, RadiusB, h, h2;
  double Xx, Xy, Yy, Yx;
  double last_x = 0, last_y = 0, last_a = 0;
  double last_vel_l = 0, last_vel_r = 0, last_vel_b = 0;

  Position last_position; // last position of robot
  int last_velocity_time = 0;    // time of last velocity update
  int velocity_update_time = 0; // time SINCE last velocity update


  while(true){
    NewLeft = LeftEncoder.get_value()/360.0 *(2.75*M_PI);
    NewRight = RightEncoder.get_value()/360.0 *(2.75*M_PI);
    // NewBack = BackEncoder.get_value()/360.0 *(2.75*M_PI);

    // printf("l: %d, r: %d \n", NewLeft, NewRight);
    // printf("l: %d, r: %d \n", LeftEncoder.get_value(), RightEncoder.get_value());
    // printf("x: %.2lf, y: %.2lf, a: %.2lf\n", millis(), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));



    Left = NewLeft - LastLeft;
    Right = NewRight - LastRight;
    // Back = NewBack - LastBack;


    velocity_update_time = millis() - last_velocity_time;
    if(velocity_update_time > 20){  // velocity is updated every 20 ms
        tracking.l_velo = (NewLeft - last_vel_l) / velocity_update_time * 1000; // velocities are in inches per second
        tracking.r_velo = (NewRight - last_vel_r) / velocity_update_time * 1000;
        tracking.b_velo = (NewBack - last_vel_b) / velocity_update_time * 1000;

        last_vel_l = NewLeft;
        last_vel_r = NewRight;
        // last_vel_b = NewBack;

        // gives us linear velocity in inches per second, and angular velocity in radians per second
        tracking.g_velocity.x = (tracking.x_coord - last_position.x) / velocity_update_time * 1000;
        tracking.g_velocity.y = (tracking.y_coord - last_position.y) / velocity_update_time * 1000;
        tracking.g_velocity.angle = (tracking.global_angle - last_position.angle) / velocity_update_time * 1000;

        last_position = {tracking.x_coord, tracking.y_coord, tracking.global_angle};

        last_velocity_time = millis();
    }

    LastLeft = NewLeft;
    LastRight = NewRight;
    // LastBack = NewBack;

    Theta = (Left-Right)/DistanceLR;
    if (Theta != 0){
      RadiusR = Right/Theta;
      // RadiusB = Back/Theta;
      Beta = Theta/2.0;
      h = (RadiusR + DistanceLR/2) *2 *sin(Beta);
      // h2 = (RadiusB + DistanceB) *2 *sin(Beta);
    }
    else{
      h = Right;
      // h2 = Back;
      Beta =0.0;
    }
    Alpha = tracking.global_angle + Beta;

    // Xx = h2 * cos(Alpha);
    // Xy = h2 * -sin(Alpha);
    Yx = h * sin(Alpha);
    Yy = h * cos(Alpha);

    tracking.x_coord += Xx + Yx;
    tracking.y_coord += Yy + Xy;
    tracking.global_angle += Theta;


    tracking_data.print(&data_timer, 100, {
      [=](){return Data::to_char("%d || x: %.2lf, y: %.2lf, a: %.2lf\n", millis(), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));},
      // [=](){return Data::to_char("%d || GLOBAL VELOCITY| x: %.2f, y: %.2f a: %.2f\n", millis(), tracking.g_velocity.x, tracking.g_velocity.y, rad_to_deg(tracking.g_velocity.angle));},
      // [=](){return Data::to_char("%d || ENCODER L: %d, R: %d, B:%d \n", millis(), LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());},
      // [=](){return Data::to_char("%d || ENCODER VELO| l: %.2f, r: %.2f, b: %.2f\n", millis(), tracking.l_velo, tracking.r_velo, tracking.b_velo);}
    });



    // printf("\ntime: %d, TRACKING: %f, %f, %f \n", millis(), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
    // printf("time: %d, TOTAL: %f, %f, %f \n", millis(), total_x, total_y, rad_to_deg(total_a));
    // printf("%d pow_a: %.1f, pow_x: %.1f, pow_y: %.1f, total_pow: %.1f\n",millis(),  tracking.power_a, tracking.power_x, tracking.power_y, fabs(tracking.power_a) + fabs(tracking.power_x) + fabs(tracking.power_y));

    if(ptr->notify_handle())return;
    // delay(10);
    // printf("GLOBAL VELOCITY| x: %.2f, y: %.2f a: %.2f\n", tracking.g_velocity.x, tracking.g_velocity.y, rad_to_deg(tracking.g_velocity.angle));
    // printf("power| x: %.2f, y: % 2.f a: %.2f\n", tracking.power_x, tracking.power_y, tracking.power_a);

    // printf(" %f, %f, %f, %f\n",tracking.l_velo, tracking.r_velo, tracking.power_y, tracking.y_coord);
    // printf(" %f, %f, %f, %f\n",tracking.l_velo, tracking.r_velo, tracking.power_y, tracking.y_coord);
    delay(10);
  }
}


// #define exit_condition tracking.x_coord < target_x - 5.0 || tracking.y_coord > target_y + 5.0

void rush_goal(double target_x, double target_y, double target_a){
    double end_error = 0.5, end_error_a = 5.0, kp_x = 30.0, kp_a = 175.0, local_error_x, local_error_y, difference_a, error_a, error_d, max_power = 127, total_power, scale;
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
    double end_error = 0.5, end_error_a = 5.0, kp_x = 10.0, kp_a = 120.0, local_error_x, local_error_y, difference_a, error_a, error_d, max_power = 127.0, total_power, scale;
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



arc_params::arc_params(const Point start, Position target, const double radius, const bool positive, const double max_power, const bool angle_relative_to_arc, const double min_angle_percent, const bool brake, const double decel_dist, const double decel_speed):
  start{start}, target{target}, radius{radius}, positive{positive}, max_power{max_power}, angle_relative_to_arc{angle_relative_to_arc}, min_angle_percent{min_angle_percent}, brake{brake}, decel_dist{decel_dist}, decel_speed{decel_speed}{}

line_params::line_params(const Point start, Position target, const double max_power, const bool overshoot, const double min_angle_percent, const bool brake, const double decel_dist, const double decel_speed):
  start{start}, target{target}, max_power{max_power}, overshoot{overshoot}, min_angle_percent{min_angle_percent}, brake{brake}, decel_dist{decel_dist}, decel_speed{decel_speed}{}

line_old_params::line_old_params(double start_x, double start_y,double target_x, double target_y, double target_a, bool Brake, bool debug, int max_power, bool Overshoot, double end_error, double end_error_a, double p, double lpercent , double apercent):
  start_x{start_x}, start_y{start_x},target_x{target_x}, target_y{target_y}, target_a{target_a}, Brake{Brake}, debug{debug}, max_power{max_power}, Overshoot{Overshoot}, end_error{end_error}, end_error_a{end_error_a}, p{p}, lpercent{lpercent}, apercent{apercent}{}

point_params::point_params(const Position target, const double max_power, const bool overshoot, const double min_angle_percent, const bool brake, const double decel_dist, const double decel_speed, pid_const x_pid_const, pid_const y_pid_const, pid_const a_pid_const, const uint32_t time_out, double end_error, double end_error_a):
  target{target}, max_power{max_power}, overshoot{overshoot}, min_angle_percent{min_angle_percent}, brake{brake}, decel_dist{decel_dist}, decel_speed{decel_speed}, x_pid_const{x_pid_const}, y_pid_const{y_pid_const}, a_pid_const{a_pid_const}, time_out{time_out}, end_error{end_error}, end_error_a{end_error_a}{}

tank_arc_params::tank_arc_params(const Point start_pos, Position target, const double power, const double max_power, const bool brake):
  start_pos{start_pos}, target{target}, power{power}, max_power{max_power}, brake{brake}{}

tank_point_params::tank_point_params(const Position target, const bool turn_dir_if_0, const double max_power, const double min_angle_percent, const bool brake, double kp_y, double kp_a, double kd_a):
  target{target}, turn_dir_if_0{turn_dir_if_0}, max_power{max_power}, min_angle_percent{min_angle_percent}, brake{brake}, kp_y{kp_y}, kp_a{kp_a}, kd_a{kd_a}{}

turn_angle_params::turn_angle_params(const double target_a, const bool brake, bool near, double kp, double kd, double max_speed, int timeout, double min_power_a, double end_error):
  target_a{target_a},brake{brake}, near{near}, kp{kp}, kd{kd}, max_speed{max_speed}, timeout{timeout}, min_power_a{min_power_a}, end_error{end_error}{}

turn_point_params::turn_point_params(const Point target, const bool brake):
  target{target}, brake{brake}{}

// std::variant<arc_params, line_params, tank_arc_params, point_params, tank_point_params, turn_angle_params, turn_point_params> params
void move_start(move_types type, std::variant<arc_params, line_params, tank_arc_params, point_params, tank_point_params, turn_angle_params, turn_point_params, line_old_params> params, bool wait_for_comp){
  // params_g = params;
  switch(type){
    case move_types::arc:
      arc_params_g = std::get<arc_params>(params);
      move_t.rebind(move_on_arc);
    break;
    case move_types::line:
      line_params_g = std::get<line_params>(params);
      move_t.rebind(move_on_line);
    break;
    case move_types::point:
      point_params_g = std::get<point_params>(params);
      move_t.rebind(move_to_point);
    break;
    case move_types::tank_arc:
      tank_arc_params_g = std::get<tank_arc_params>(params);
      move_t.rebind(tank_move_on_arc);
    break;
    case move_types::tank_point:
      tank_point_params_g = std::get<tank_point_params>(params);
      move_t.rebind(tank_move_to_target);
    break;
    case move_types::turn_angle:
      turn_angle_params_g = std::get<turn_angle_params>(params);
      move_t.rebind(turn_to_angle);
    break;
    case move_types::turn_point:
      turn_point_params_g = std::get<turn_point_params>(params);
      move_t.rebind(turn_to_point);
    break;
    case move_types::line_old: 
      line_old_params_g = std::get<line_old_params>(params);
      move_t.rebind(move_on_line_old);
    break;
  }
  if(wait_for_comp)move_wait_for_complete();
}

bool move_wait_for_complete(){
  while(move_t.get_task_ptr()->get_state()!= 4)delay(10); //change to waitUntil(move_t.get_task_ptr()->get_state() == 4);
  return tracking.move_complete;
}

void move_wait_for_error(double error){ 
  while(fabs(tracking.drive_error) >error)delay(10);
}

void move_stop(bool brake){
  move_t.kill();
  if(brake)drivebase.brake();
  else drivebase.move(0,0,0);
}



void move_to_point(void* params){
    _Task* ptr = _Task::get_obj(params);
    // point_params* param_ptr = static_cast<point_params*>(_Task::get_params(params));
    // printf("before copy: %f\n", param_ptr->target.x);
    // Position target = param_ptr->target;
    // printf("in function: %f\n", target.x);
    // const double max_power = param_ptr->max_power;
    // const bool overshoot = param_ptr->overshoot;
    // const double min_angle_percent = param_ptr->min_angle_percent;
    // const bool brake= param_ptr->brake;
    // const double decel_dist = param_ptr->decel_dist, decel_speed = param_ptr->decel_speed;
    // pid_const x_pid_const = param_ptr->x_pid_const, y_pid_const = param_ptr->y_pid_const, a_pid_const = param_ptr->a_pid_const;
    // const uint32_t time_out = param_ptr->time_out;
    Position target = point_params_g.target;
    double max_power = point_params_g.max_power;
    bool overshoot = point_params_g.overshoot;
    double min_angle_percent = point_params_g.min_angle_percent;
    bool brake = point_params_g.brake;
    double decel_dist = point_params_g.decel_dist, decel_speed = point_params_g.decel_speed;
    pid_const x_pid_const = point_params_g.x_pid_const, y_pid_const = point_params_g.y_pid_const, a_pid_const = point_params_g.a_pid_const;
    const uint32_t time_out = point_params_g.time_out;
    double end_error = point_params_g.end_error;
    double end_error_a = point_params_g.end_error_a;

    tracking.move_complete= false;
    target.angle = deg_to_rad(target.angle);
    Position error;
    // Position tracking = {-2.0, 2.0, -20.0}, error;

    // Vector follow_line {target.x - start.x, target.y - start.y};
    // double line_angle = atan2(target.x - start.x, target.y - start.y); // angle of follow_line relative to the vertical

    Vector target_line = {target.x - tracking.x_coord, target.y - tracking.y_coord};    // line of current position to target
    // target_line.rotate(line_angle);    // displacement towards and along follow_line
    int orig_sgn_line_y = sgn(target_line.get_y());

    // power_scaling variables
    // scaling variables
    double pre_scaled_power_a = tracking.power_a;
    double d; // distance to target
    double angle_power_guarantee_xy_scale;
    double max_power_scale;
    double power_xy;
    double total_power;
    double min_power_a = max_power * min_angle_percent;
    // PID'S

    PID x_pid(x_pid_const.p,x_pid_const.i,x_pid_const.d, 0.0, true, 0.2, 3.0);
    PID y_pid(y_pid_const.p,y_pid_const.i,y_pid_const.d, 0.0, true, 0.2, 3.0);
    PID angle_pid(a_pid_const.p,a_pid_const.i,a_pid_const.d, 0.0, true, 0.0, 360.0);

    // decel variables
    double h; // magnitude of power vector
    double decel_power, decel_power_scale;
    double decel_start = sqrt(pow(x_pid.get_proportional(), 2) + pow(y_pid.get_proportional(), 2));  // theoretically when the deceleration starts
    Timer move_timer{"move_timer"};
    master.print(1,1,"y: %.3f",target.y);
    motion_i.print("%d | Move to point: (x: %.2f, y: %.2f, a: %.2f)  from: (x: %.2f, y: %.2f, a: %.2f)\n",millis(), target.x, target.y, rad_to_deg(target.angle), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
    while(true){
        // gets line displacements
        printf("x: %lf, y: %lf, a: %lf\n", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
        target_line.set_cartesian(target.x - tracking.x_coord, target.y - tracking.y_coord);
        target_line.rotate(tracking.global_angle);  // now represents local vectors to target
        d = target_line.get_magnitude();
        tracking.drive_error = d;

        tracking.power_x = x_pid.compute(-target_line.get_x(), 0.0);
        tracking.power_y = y_pid.compute(-target_line.get_y(), 0.0);
        tracking.power_a = angle_pid.compute(tracking.global_angle, near_angle(target.angle, tracking.global_angle) + tracking.global_angle);

        h = sqrt(pow(tracking.power_x, 2) + pow(tracking.power_y, 2));
        ////////////
        // deceleration code
        if(decel_dist){
          printf("DECEL\n");
          if(d < decel_dist){ // robot is past decel dist
            decel_power = constrain(map(d, decel_dist, max_power / decel_start, decel_speed, max_power), decel_speed, max_power);
            if (decel_power < decel_speed) decel_power = decel_speed;
            decel_power_scale = decel_power / h;
            tracking.power_x *= decel_power_scale;
            tracking.power_y *= decel_power_scale;
          }
          else{ // robot is before decel dist
            decel_power_scale = max_power / (fabs(tracking.power_x) + fabs(tracking.power_y));
            tracking.power_x *= decel_power_scale;
            tracking.power_y *= decel_power_scale;
          }
        }
        // else{
        //   // tracking.power_x = error.x * kp.x;
        //   // tracking.power_y = error.y * kp.y;
        // }
        // tracking.power_a = error.angle * kp.angle;
        /////////////


        // gives minimum move powers if requested
        if(fabs(tracking.power_x) < min_move_power_x)   tracking.power_x = sgn(tracking.power_x) * min_move_power_x;
        if(fabs(tracking.power_y) < min_move_power_y)   tracking.power_y = sgn(tracking.power_y) * min_move_power_y;
        if(fabs(tracking.power_a) < min_move_power_a)   tracking.power_a = sgn(tracking.power_a) * min_move_power_a;

        // printf("After: x: %lf, y: %lf\n", target_line.get_x(), target_line.get_y());


        // power scaling and angle power guarantee
        total_power = fabs(tracking.power_x) + fabs(tracking.power_y) + fabs(tracking.power_a);

        if(total_power > max_power){

            pre_scaled_power_a = tracking.power_a;
            angle_power_guarantee_xy_scale = 1.0;

            max_power_scale = max_power / total_power;
            tracking.power_x *= max_power_scale, tracking.power_y *= max_power_scale, tracking.power_a *= max_power_scale;
            // start of angle power guarantee

            power_xy = fabs(tracking.power_x) + fabs(tracking.power_y);

            if (fabs(pre_scaled_power_a) > min_power_a){
                if (fabs(tracking.power_a) < min_power_a)  tracking.power_a = min_power_a * sgn(tracking.power_a);  // power_a has been overshadowed
            }
            // angle gets the power it demanded if pre_scaled power_a was also less than min_power_a
            else    tracking.power_a = pre_scaled_power_a;
            if (power_xy > 0)  angle_power_guarantee_xy_scale = (max_power - fabs(tracking.power_a)) / power_xy;
            tracking.power_x *= angle_power_guarantee_xy_scale, tracking.power_y *= angle_power_guarantee_xy_scale;
            // printf("****power_a: %lf, x:%lf, y: %lf, pre_scaled: %lf, min: %lf\n", tracking.power_a, tracking.power_x , tracking.power_y, pre_scaled_power_a, min_power_a);
        }
        drivebase.move(tracking.power_x, tracking.power_y, tracking.power_a);

        motion_d.print("%d || x : %f, y : %f, a: %f, err: (%f, %f), px: %f, py: %f, pa: %f, ex: %f, dx: %f, ey: %f, dy: %f\n",millis(),tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle), target.x - tracking.x_coord, target.y - tracking.y_coord, tracking.power_x, tracking.power_y, tracking.power_a, x_pid.error, x_pid.derivative, y_pid.error, y_pid.derivative);

        if (overshoot && sgn(target_line.get_y()) != orig_sgn_line_y){
          if(brake) drivebase.brake();
          else drivebase.move(0.0, 0.0, 0.0);
          tracking.move_complete = true;
          motion_i.print("%d | Ending move to point: (X: %.2f Y: %.2f A: %.2f) at (X: %.2f Y: %.2f A: %.2f) \n", millis(), target.x, target.y, rad_to_deg(target.angle), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
          return;
        }
        else if(fabs(d) < end_error && fabs(rad_to_deg(angle_pid.get_error())) < end_error_a){
            if(brake) drivebase.brake();
            else drivebase.move(0.0, 0.0, 0.0);
            tracking.move_complete = true;
            motion_i.print("%d | Ending move to point: (X: %.2f Y: %.2f A: %.2f) at (X: %.2f Y: %.2f A: %.2f) \n", millis(), target.x, target.y, rad_to_deg(target.angle), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
            return;
        }
        else if(move_timer.get_time() > time_out && time_out > 0){
          if(brake) drivebase.brake();
          else drivebase.move(0.0, 0.0, 0.0);
          tracking.move_complete = true;
          motion_i.print("%d | TIMED OUT move to point: (X: %.2f Y: %.2f A: %.2f) at (X: %.2f Y: %.2f A: %.2f) \n", millis(), target.x, target.y, rad_to_deg(target.angle), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
          return;
        }

        if(ptr->notify_handle())return;
        delay(10);

    }
}

// this is 150-200 ms slower than tank move on arc on 24 radius 90 degree turn
void move_on_arc(void* params) {
  _Task* ptr = _Task::get_obj(params);
  // arc_params* param_ptr = static_cast<arc_params*>(_Task::get_params(params));
  const Point start = arc_params_g.start;
  Position target = arc_params_g.target;
  double radius = arc_params_g.radius;
  bool positive = arc_params_g.positive;
  double max_power= arc_params_g.max_power;
  bool angle_relative_to_arc = arc_params_g.angle_relative_to_arc;
  double min_angle_percent = arc_params_g.min_angle_percent;
  bool brake = arc_params_g.brake;
  double decel_dist = arc_params_g.decel_dist;
  double decel_speed = arc_params_g.decel_speed;
  tracking.move_complete = false;

  // Position error, kp = Position(30.0, 12.0, 175.0);

  Position error, kp = Position(12.0, 30.0, 175.0);

  target.angle = deg_to_rad(target.angle);
  // variable 'd' in diagram
  double chord_dist = sqrt(pow(target.x - start.x, 2) + pow(target.y - start.y, 2));
  printf("chord_dist = %f\n", chord_dist);
  double theta = atan2(target.y - start.y, target.x - start.x);
  printf("theta = %f\n", rad_to_deg(theta));
  double half_chord_dist = chord_dist / 2.0;
  printf("half_chord_dist: %lf\n", half_chord_dist);
  if(radius < half_chord_dist){
    printf("The radius needs to be at least %lf inches, you inputted %lf as a radius.\n", half_chord_dist, radius);
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

  // scaling variables
  double pre_scaled_power_a = tracking.power_a;
  double angle_power_guarantee_xy_scale;
  double max_power_scale;
  double power_xy;
  double total_power;
  double min_power_a = max_power * min_angle_percent;

  double decel_power, decel_power_scale;

  const double final_angle = atan2(target.y - arc_centre.y, target.x - arc_centre.x); // angle of final position to centre at end of move

  // positive turn_dir means cw movement about the arc
  int turn_dir = -sgn(rad_to_deg(near_angle(atan2(target.y - arc_centre.y, target.x - arc_centre.x), atan2(tracking.y_coord - arc_centre.y, tracking.x_coord - arc_centre.x))));
  printf("turn_dir: %d\n", turn_dir);

  motion_i.print("%d | Starting move on arc to: (x: %.2f, y: %.2f, a: %.2f) r: %.2f,  from: (x: %.2f, y: %.2f, a: %.2f)\n", millis(), target.x, target.y, rad_to_deg(target.angle), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
  while(true){
    arc_disp.angle = atan2(tracking.y_coord - arc_centre.y, tracking.x_coord - arc_centre.x);
    // printf("arc_disp.angle: %lf\n", arc_disp.angle);
    hypotenuse = sqrt(pow(tracking.x_coord - arc_centre.x, 2) + pow(tracking.y_coord - arc_centre.y, 2)); // dist from centre of arc to cur position
    arc_disp.x = radius - hypotenuse;
    arc_disp.y = radius * near_angle(angle_of_arc, arc_disp.angle);
    // printf("\n Arc displacements | x: %lf, y: %lf\n", arc_disp.x, arc_disp.y);

    // rotating arc displacements
    h = sqrt(pow(arc_disp.x, 2) + pow(arc_disp.y, 2));  // magnitude of displacement vector
    beta = atan2(arc_disp.y, arc_disp.x) + tracking.global_angle + arc_disp.angle;  // angle of displacement vector local to robot

    error.x = h * cos(beta);
    error.y = h * sin(beta);
    // printf("Local errors | x: %lf, y: %lf\n", error.x, error.y);

    if (angle_relative_to_arc){
        // error.angle = near_angle(turn_dir * arc_disp.angle, tracking.global_angle);
        error.angle = near_angle(target.angle - arc_disp.angle + (turn_dir == -1 ? 0 : M_PI), tracking.global_angle);

        // printf("angle target: %lf, angle_error: %lf\n", rad_to_deg(target.angle - arc_disp.angle + (turn_dir == -1 ? 0 : M_PI)), rad_to_deg(error.angle));
    }
    else  error.angle = near_angle(target.angle, tracking.global_angle);

    // deceleration code
    if(decel_dist){
      if(h < decel_dist){ // robot is past decel dist
        decel_power = constrain(map(h, decel_dist, max_power / kp.y, decel_speed, max_power), decel_speed, max_power);
        if (decel_power < decel_speed) decel_power = decel_speed;
        decel_power_scale = decel_power / h;
        tracking.power_x = decel_power_scale * error.x;
        tracking.power_y = decel_power_scale * error.y;
      }
      else{ // robot is before decel dist
        decel_power_scale = max_power / (fabs(error.x) * kp.x + fabs(error.y) * kp.y);
        tracking.power_x = error.x * kp.x * decel_power_scale;
        tracking.power_y = error.y * kp.y * decel_power_scale;
      }
    }
    else{
      tracking.power_x = error.x * kp.x;
      tracking.power_y = error.y * kp.y;
    }
    tracking.power_a = error.angle * kp.angle;

    // gives minimum move powers if not satisfied
    if (fabs(tracking.power_a) < min_move_power_a) tracking.power_a = sgn(error.angle) * min_move_power_a;
    if (fabs(tracking.power_x) < min_move_power_x) tracking.power_x = sgn(error.x) * min_move_power_x;
    if (fabs(tracking.power_y) < min_move_power_y) tracking.power_y = sgn(error.y) * min_move_power_y;

    printf("%lf,%lf\n", tracking.x_coord, tracking.y_coord);

    // MOVE ON ARC CODE
    total_power = fabs(tracking.power_x) + fabs(tracking.power_y) + fabs(tracking.power_a);

    if(total_power > max_power){

        pre_scaled_power_a = tracking.power_a;
        angle_power_guarantee_xy_scale = 1.0;

        max_power_scale = max_power / total_power;
        tracking.power_x *= max_power_scale, tracking.power_y *= max_power_scale, tracking.power_a *= max_power_scale;
        // start of angle power guarantee

        power_xy = fabs(tracking.power_x) + fabs(tracking.power_y);

        if (fabs(pre_scaled_power_a) > min_power_a){
            if (fabs(tracking.power_a) < min_power_a)  tracking.power_a = min_power_a * sgn(tracking.power_a);  // power_a has been overshadowed
        }
        // angle gets the power it demanded if pre_scaled power_a was also less than min_power_a
        else    tracking.power_a = pre_scaled_power_a;
        if (power_xy > 0)  angle_power_guarantee_xy_scale = (max_power - fabs(tracking.power_a)) / power_xy;
        tracking.power_x *= angle_power_guarantee_xy_scale, tracking.power_y *= angle_power_guarantee_xy_scale;
        // printf("****power_a: %lf, x:%lf, y: %lf, pre_scaled: %lf, min: %lf\n", tracking.power_a, tracking.power_x , tracking.power_y, pre_scaled_power_a, min_power_a);
    }
    // end of scaling
    // END OF MOVE ON ARC CODE
    drivebase.move(tracking.power_x, tracking.power_y, tracking.power_a);
    // waits for arc angle to become less than 1 degree
    if (rad_to_deg(fabs(near_angle(final_angle, atan2(tracking.y_coord - arc_centre.y, tracking.x_coord - arc_centre.x)))) < 1.0){
      motion_i.print("%d | Ending move on arc to target (X: %.2f Y: %.2f A: %.2f) at (X: %.2f Y: %.2f A: %.2f) \n", millis(), target.x, target.y, rad_to_deg(target.angle), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
      if(brake) drivebase.brake();
      else drivebase.move(0, 0, 0);
      printf("min_a: %lf\n", min_power_a);
      tracking.move_complete = true;
      return;
    }
    // printf("****power_a: %lf, x:%lf, y: %lf, pre_scaled: %lf, min: %lf, sum: %lf\n", tracking.power_a, tracking.power_x , tracking.power_y, pre_scaled_power_a, min_power_a, fabs(tracking.power_a) + fabs(tracking.power_x) + fabs(tracking.power_y));
    if(ptr->notify_handle())return;
    delay(10);
  }
}

void move_on_line(void* params){
    _Task* ptr = _Task::get_obj(params);
    // line_params* param_ptr = static_cast<line_params*>(_Task::get_params(params));

    const Point start = line_params_g.start;
    Position target = line_params_g.target;
    const double max_power = line_params_g.max_power;
    const bool overshoot = line_params_g.overshoot;
    const double min_angle_percent = line_params_g.min_angle_percent;
    const bool brake = line_params_g.brake;
    const double decel_dist = line_params_g.decel_dist, decel_speed = line_params_g.decel_speed;
    tracking.move_complete = false;
    target.angle = deg_to_rad(target.angle);
    Position error;
    // Position tracking = {-2.0, 2.0, -20.0}, error;

    Vector follow_line {target.x - start.x, target.y - start.y};
    double line_angle = atan2(target.x - start.x, target.y - start.y); // angle of follow_line relative to the vertical

    Vector target_line = {target.x - tracking.x_coord, target.y - tracking.y_coord};    // line of current position to target
    target_line.rotate(line_angle);    // displacement towards and along follow_line
    int orig_sgn_line_y = sgn(target_line.get_y());

    // power_scaling variables
    // scaling variables
    double pre_scaled_power_a = tracking.power_a;
    double d; // distance to target
    double angle_power_guarantee_xy_scale;
    double max_power_scale;
    double power_xy;
    double total_power;
    double min_power_a = max_power * min_angle_percent;
    const double x_multiplier = 2.0; // how much slower the robot strafes than moves forwards
    // PID'S

    PID x_line_pid(23.0, 0.000, 0.0, 0.0, true, 0.2, 3.0);
    PID y_line_pid(9.5, 0.0, 1000.0, 0.0, true, 0.2, 3.0);
    // PID angle_pid(175.0, 0.0, 0.0, 0.0, true, 0.0, 360.0);
    double kp_a = 125.0;
    // decel variables
    double h; // magnitude of power vector
    double decel_power, decel_power_scale;
    motion_i.print("%d | Starting move on line to: (x: %.2f, y: %.2f, a: %.2f)  from: (x: %.2f, y: %.2f, a: %.2f) with start: (%.2f, %.2f)\n", millis(), target.x, target.y, rad_to_deg(target.angle), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle), start.x, start.y);
    // motion_i.print("%d | ")
    while(true){
        // gets line displacements
        target_line.set_cartesian(target.x - tracking.x_coord, target.y - tracking.y_coord);
        target_line.rotate(tracking.global_angle);
        target_line.rotate(-tracking.global_angle);
        printf("Errors | x %lf, y: %lf\n", target_line.get_x(), target_line.get_y());

        d = target_line.get_magnitude();  // distance to target
        target_line.rotate(line_angle);    // target_line becomes displacement towards and along follow_line
        // printf("x: %lf, y: %lf\n", target_line.get_x(), target_line.get_y());
        // printf("%lf,%lf\n",tracking.x_coord, tracking.y_coord);

        // feeds line displacements into PID's
        x_line_pid.compute(-target_line.get_x(), 0.0);
        y_line_pid.compute(-target_line.get_y(), 0.0);
        // angle_pid.compute(tracking.global_angle, near_angle(target.angle, tracking.global_angle) + tracking.global_angle);

        target_line.set_cartesian(x_line_pid.get_output(), y_line_pid.get_output());

        // target_line now represents local vectors of robot to target, after PID transformation
        target_line.rotate(tracking.global_angle - line_angle);

        tracking.power_x = target_line.get_x() * x_multiplier;
        tracking.power_y = target_line.get_y();
        // tracking.power_a = angle_pid.get_output();
        tracking.power_a = kp_a * near_angle(target.angle, tracking.global_angle);
        // printf("output: %lf", angle_pid.get_output());
        printf("output | x: %lf, y: %lf, a: %lf\n", tracking.power_x, tracking.power_y, tracking.power_a);

        h = sqrt(pow(tracking.power_x, 2) + pow(tracking.power_y, 2));
        ////////////
        // deceleration code
        if(decel_dist){
          if(d < decel_dist){ // robot is past decel dist
            decel_power = constrain(map(d, decel_dist, max_power / y_line_pid.get_proportional(), decel_speed, max_power), decel_speed, max_power);
            if (decel_power < decel_speed) decel_power = decel_speed;
            decel_power_scale = decel_power / h;
            tracking.power_x *= decel_power_scale;
            tracking.power_y *= decel_power_scale;
          }
          else{ // robot is before decel dist
            decel_power_scale = max_power / (fabs(tracking.power_x) + fabs(tracking.power_y));
            tracking.power_x *= decel_power_scale;
            tracking.power_y *= decel_power_scale;
          }
        }
        // else{
        //   // tracking.power_x = error.x * kp.x;
        //   // tracking.power_y = error.y * kp.y;
        // }
        // tracking.power_a = error.angle * kp.angle;
        /////////////


        // gives minimum move powers if requested
        if(fabs(tracking.power_x) < min_move_power_x)   tracking.power_x = sgn(tracking.power_x) * min_move_power_x;
        if(fabs(tracking.power_y) < min_move_power_y)   tracking.power_y = sgn(tracking.power_y) * min_move_power_y;
        if(fabs(tracking.power_a) < min_move_power_a)   tracking.power_a = sgn(tracking.power_a) * min_move_power_a;

        // printf("After: x: %lf, y: %lf\n", target_line.get_x(), target_line.get_y());


        // power scaling and angle power guarantee
        total_power = fabs(tracking.power_x) + fabs(tracking.power_y) + fabs(tracking.power_a);

        if(total_power > max_power){

            pre_scaled_power_a = tracking.power_a;
            angle_power_guarantee_xy_scale = 1.0;

            max_power_scale = max_power / total_power;
            tracking.power_x *= max_power_scale, tracking.power_y *= max_power_scale, tracking.power_a *= max_power_scale;
            // start of angle power guarantee

            power_xy = fabs(tracking.power_x) + fabs(tracking.power_y);

            if (fabs(pre_scaled_power_a) > min_power_a){
                if (fabs(tracking.power_a) < min_power_a)  tracking.power_a = min_power_a * sgn(tracking.power_a);  // power_a has been overshadowed
            }
            // angle gets the power it demanded if pre_scaled power_a was also less than min_power_a
            else    tracking.power_a = pre_scaled_power_a;
            if (power_xy > 0)  angle_power_guarantee_xy_scale = (max_power - fabs(tracking.power_a)) / power_xy;
            tracking.power_x *= angle_power_guarantee_xy_scale, tracking.power_y *= angle_power_guarantee_xy_scale;
            // printf("****power_a: %lf, x:%lf, y: %lf, pre_scaled: %lf, min: %lf\n", tracking.power_a, tracking.power_x , tracking.power_y, pre_scaled_power_a, min_power_a);
        }
        printf("angle: %lf, error: %lf, power: x: %lf, y: %lf, a: %lf\n", rad_to_deg(tracking.global_angle), rad_to_deg(near_angle(target.angle, tracking.global_angle)),  tracking.power_x, tracking.power_y, tracking.power_a);

        drivebase.move(tracking.power_x, tracking.power_y, tracking.power_a);

        if (overshoot && sgn(target_line.get_y()) != orig_sgn_line_y){
          if(brake) drivebase.brake();
          else drivebase.move(0.0, 0.0, 0.0);
          tracking.move_complete = true;
          printf("Ending move on line to target X: %f Y: %f A: %f at X: %f Y: %f A: %f \n", target.x, target.y, rad_to_deg(target.angle), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
          return;
        }
        else if(fabs(d) < 0.5 && fabs(rad_to_deg(near_angle(target.angle, tracking.global_angle))) < 5.0){
            if(brake) drivebase.brake();
            else drivebase.move(0.0, 0.0, 0.0);
            tracking.move_complete = true;
            printf("Ending move on line to target X: %f Y: %f A: %f at X: %f Y: %f A: %f \n", target.x, target.y, rad_to_deg(target.angle), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
            return;
        }
        printf("sum:%lf\n", fabs(tracking.power_x) + fabs(tracking.power_y) + fabs(tracking.power_a));
        if(ptr->notify_handle())return;
        delay(10);

    }

}

// TANK MOVE ALGORITHMS

void tank_move_to_target(void* params){
    _Task* ptr = _Task::get_obj(params);
    // tank_point_params* param_ptr = static_cast<tank_point_params*>(_Task::get_params(params));
    const Position target = tank_point_params_g.target;
    const bool turn_dir_if_0 = tank_point_params_g.turn_dir_if_0;
    const double max_power = tank_point_params_g.max_power;
    const double min_angle_percent = tank_point_params_g.min_angle_percent;
    const bool brake = tank_point_params_g.brake;
    double kp_y = tank_point_params_g.kp_y;
    double kp_a =tank_point_params_g.kp_a;
    double kd_a = tank_point_params_g.kd_a;
    // Pid angle(kp.a);
    
    tracking.move_complete = false;

    Point local_error;
    Position error;

    double total_power, max_power_scale;
    int sgn_local_error_y, orig_sgn_line_y, sgn_line_y;
    double difference_a;
    double hypotenuse;

    // double kp_y = 11.5, kp_a = 150.0, kd_a = 0.0;
    // printf("Local errors | x: %lf, y: %lf \n", local_error.x, local_error.y);
    double min_power_a = max_power * min_angle_percent;
    double pre_scaled_power_a;
    double end_error = 0.5;

    // double deriv_a = 0.0;
    PID angle(kp_a, 0.0, kd_a, 0.0);

    // move on line variables
    Vector follow_line(target.y - tracking.y_coord, target.x - tracking.x_coord); // used to keep track of angle of follow_line relative to the vertical
    Vector line_disp(target.x - tracking.x_coord, target.y - tracking.y_coord);  // displacements relative to line
    double line_y_local_y; // local_y component of power along line
    int time = millis();
    motion_i.print("%d|| Starting tank move to point from (%.2f, %.2f, %.2f) to (%.2f, %.2f, %.2f)\n", millis(), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle),target.x, target.y, target.angle);
    while(true){
      // start of move_on_line stuff
      line_disp = Vector(target.x - tracking.x_coord, target.y - tracking.y_coord);  // displacements relative to line
      // line_disp.set_polar(line_disp.get_magnitude(), line_disp.get_angle() + follow_line.get_angle());  // rotates vector by line angle
      line_disp.rotate(follow_line.get_angle());  // rotates vector by line angle
      // line_y_local_y = line_disp.get_y() * cos(tracking.global_angle - follow_line.get_angle());
      sgn_line_y = sgn(line_disp.get_y());

      // end of move_on_line stuff
      difference_a = tracking.global_angle + atan2(target.y - tracking.y_coord, target.x - tracking.x_coord);
      hypotenuse = sqrt(pow(target.x - tracking.x_coord, 2.0) + pow(target.y - tracking.y_coord, 2.0));

      // local_error.x = cos(difference_a) * hypotenuse; // unused

      sgn_local_error_y = sgn(local_error.y);
      // printf("sgn_y: %d\n", sgn_local_error_y);
      if(sgn_local_error_y == 0){
        sgn_local_error_y = turn_dir_if_0 ? 1 : -1;
        // printf("triggered\n");
      }
      local_error.y = sin(difference_a) * hypotenuse;
      error.angle = near_angle(sgn_local_error_y * M_PI / 2, difference_a);
      // printf("Errors | y: %lf, a: %lf\n", local_error.y, rad_to_deg(error.angle));

      tracking.power_y = kp_y * local_error.y;
      // tracking.power_y = kp_y * line_y_local_y;
      // tracking.power_a = angle.compute(error.angle, 0.0);
      tracking.power_a = kp_a * error.angle;
      // gives min power to local y if that is not satisfied
      if (fabs(tracking.power_y) < min_move_power_y && fabs(local_error.y) > 0.5) tracking.power_y = min_move_power_y * sgn(local_error.y);

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
        // printf("****power_a: %lf, x:%lf, y: %lf, pre_scaled: %lf, min: %lf\n", tracking.power_a, tracking.power_x , tracking.power_y, pre_scaled_power_a, min_power_a);
        // end of angle power guarantee

      }
      // printf("Powers | y: %lf, a: %lf\n",tracking.power_y, tracking.power_a);
      // printf("time: %d, TRACKING: %f, %f, %f \n\n", millis(), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));

      error.x = target.x - tracking.x_coord;
      error.y = target.y - tracking.y_coord;

      if(fabs(line_disp.get_y()) < 0.5 && sgn_line_y != orig_sgn_line_y){
        if (brake) drivebase.brake();
        tracking.move_complete = true;
        motion_i.print("%d || Ending move to target X: %f Y: %f A: %f at X: %f Y: %f A: %f time: %d\n", millis(), target.x, target.y, target.angle, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle), millis()- time);
        //log_time("ending starting time: %d, delta time: %d X: %f Y: %f A: %f from X: %f Y: %f A: %f \n", millis(),millis() -starttime, target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
        // tracking.move_stop_task();
        break;
      }
      drivebase.move_tank(tracking.power_y, tracking.power_a);
      if(ptr->notify_handle())return;
      delay(10);
    }
}

void turn_to_angle(void* params){
  _Task* ptr = _Task::get_obj(params);
  // turn_angle_params* param_ptr = static_cast<turn_angle_params*>(_Task::get_params(params));
  double target_a = turn_angle_params_g.target_a;
  bool brake = turn_angle_params_g.brake;
  bool near = turn_angle_params_g.near;
  double kp = turn_angle_params_g.kp;
  double kd = turn_angle_params_g.kd;
  double max_speed = turn_angle_params_g.max_speed;
  int timeout = turn_angle_params_g.timeout;
  double min_power_a = turn_angle_params_g.min_power_a;
  double end_error = turn_angle_params_g.end_error;
  tracking.move_complete = false;

  PID angle_pid(kp, 0.0, kd, 0.0, true, 0.0, 360.0);
  motion_d.print("%d || Starting turn to angle %.2f from %.2f\n", millis(), target_a, rad_to_deg(tracking.global_angle));
  target_a = deg_to_rad(target_a);
  double new_target_a;
  double power;
  if(near){
    new_target_a = near_angle(target_a, tracking.global_angle) + tracking.global_angle;
  }
  else {
    new_target_a = target_a;
  }
  int start_time = millis();
  while(true){
    power =  angle_pid.compute(tracking.global_angle, new_target_a);
    if(fabs(power) > max_speed)power = max_speed*sgn(power);
    if(fabs(power) <min_power_a) power = sgn(power)*min_power_a;
    drivebase.move_tank(0,power);
    if(fabs(rad_to_deg(angle_pid.get_error())) < end_error){
      motion_d.print("%d || Ending turn to angle : %.2f at X: %.2f Y: %.2f A: %.2f, time: %d\n", millis(), rad_to_deg(target_a), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle), millis() - start_time);
      drivebase.move_tank(0, 0);
      if(brake) drivebase.brake();
      tracking.move_complete = true;
      return;
    }
    if(timeout != 0 && millis() - start_time > timeout){ 
      motion_d.print("%d|| TIMED OUT Ending turn to angle : %.2f at X: %.2f Y: %.2f A: %.2f, time: %d\n", millis(), rad_to_deg(target_a), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle), millis() - start_time);
      drivebase.move_tank(0, 0);
      if(brake) drivebase.brake();
      tracking.move_complete = true;
      return;
    }
    if(ptr->notify_handle())return;
    delay(10);
  }
}

//overload to handle being called within another motion
void turn_to_angle(double target_a, const bool brake, _Task* ptr){
  PID angle_pid(150.0, 0.0, 80.0, 0.0, true, 0.0, 360.0);
  target_a = deg_to_rad(target_a);
  tracking.move_complete = false;

  while(true){
    drivebase.move_tank(0, angle_pid.compute(tracking.global_angle, near_angle(target_a, tracking.global_angle) + tracking.global_angle));
    if(fabs(rad_to_deg(angle_pid.get_error())) < 5.0){
      motion_d.print("Ending turn to point with angle: %.2f at X: %.2f Y: %.2f A: %.2f \n", rad_to_deg(target_a), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
      drivebase.move_tank(0, 0);
      if(brake) drivebase.brake();
      tracking.move_complete = true;
      return;
    }
    if(ptr->notify_handle())return;
    delay(10);
  }

}


void turn_to_point(void* params){
  _Task* ptr = _Task::get_obj(params);
  // turn_point_params* param_ptr = static_cast<turn_point_params*>(_Task::get_params(params));
  const Point target = turn_point_params_g.target;
  const bool brake = turn_point_params_g.brake;
  motion_d.print("starting turn to point (%.2f, %.2f) from (%.2f, %.2f, %.2f) target angle: %.2f\n", target.x, target.y, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle),rad_to_deg(atan2(target.x - tracking.x_coord, target.y - tracking.y_coord)));
  turn_to_angle(rad_to_deg(atan2(target.x - tracking.x_coord, target.y - tracking.y_coord)), brake, ptr);
  motion_d.print("Ending turn to point function ending\n");
}


void tank_move_on_arc(void* params){
  _Task* ptr = _Task::get_obj(params);
  // tank_arc_params* param_ptr = static_cast<tank_arc_params*>(_Task::get_params(params));
  const Point start_pos = tank_arc_params_g.start_pos;
  Position target = tank_arc_params_g.target;
  const double power = tank_arc_params_g.power, max_power = tank_arc_params_g.max_power;
  const bool brake = tank_arc_params_g.brake;
  tracking.move_complete = false;

  Vector p2 = {start_pos.x, start_pos.y}, p1 = {target.x, target.y};  // p2 and p1 are switched because Nikhil messed up the math

  Vector disp = p2 - p1;
  target.angle = deg_to_rad(target.angle);
  // disp.set_polar(disp.get_magnitude(), disp.get_angle() + target.angle);
  disp.rotate(target.angle);  // rotates displacement by target's angle to obtain right triangles from isosceles triangle forming the arc
  double theta = 2.0 * atan2(disp.get_x(), disp.get_y());
  double radius = disp.get_magnitude() / 2.0 / sin(theta / 2.0);
  // printf("radius: %lf\n", radius);
  // printf("theta: %lf\n", rad_to_deg(theta));
  Point centre = {p1.get_x() + cos(target.angle) * radius, p1.get_y() - sin(target.angle) * radius};
  // printf("centre | x: %lf, y: %lf\n", centre.x, centre.y);
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
  const double pwm_to_vel = 0.0628319;
  const double kR = 30.0; // for ratio
  // const double kR = 0.75; // for difference

  const double kA = pwm_to_vel * 160.0, kB = 1 / pwm_to_vel, kP = 35.0, kD = 0.0;
  const double final_angle = atan2(target.y - centre.y, target.x - centre.x); // angle of final position to centre at end of move

  uint32_t last_d_update_time = millis();  // for derivative
  double last_angular_velocity = tracking.g_velocity.angle; // for derivative
  int orig_angle_error_sgn = sgn(target.angle - tracking.global_angle);
  double angle_diff;
  double radial_diff;
  radius = fabs(radius);
  // positive turn_dir means cw movement about the arc
  int turn_dir = -sgn(rad_to_deg(near_angle(atan2(target.y - centre.y, target.x - centre.x), atan2(tracking.y_coord - centre.y, tracking.x_coord - centre.x))));
  // printf("turn_dir: %d\n", turn_dir);
  motion_d.print("Starting tank move on arc from (%.2f, %.2f, %.2f) to (%.2f, %.2f, %.2f) on arc with r: %.2f and centre: (%.2f, %.2f)\n",tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle), target.x, target.y,target.angle, radius, centre.x, centre.y);
  while(true){
    g_position = {tracking.x_coord, tracking.y_coord};
    disp = g_position - centre;
    // printf("local_a: %lf\n", rad_to_deg(disp.get_angle()));

    arc_velocity.set_cartesian(tracking.g_velocity.x, tracking.g_velocity.y);
    arc_velocity.rotate(-disp.get_angle() + (turn_dir == -1 ? 0 : M_PI));

    // difference between target angle and current angle taking into account direction of movement and sign of power
    angle_diff = near_angle(-disp.get_angle() + (turn_dir == -1 ? 0 : M_PI), tracking.global_angle + (power < 0.0 ? M_PI : 0));

    linear_velocity = arc_velocity.get_y();

    // linear version of radial correction calcalution (instead of logarithmic or difference)
    radial_diff = disp.get_magnitude() / radius;
    // signs radial diff
    if (radial_diff < 1) radial_diff = -1/radial_diff + 1;
    else radial_diff = radial_diff -1;
    angular_velocity_target = turn_dir * (linear_velocity / disp.get_magnitude() + kR * radial_diff) + kA * angle_diff;
    // printf("radius:%lf, angular_vel: %lf, radial_error: %lf, radial_correction: %lf, angle_diff: %lf, angle_correction: %lf\n", radius, kB * turn_dir * linear_velocity / disp.get_magnitude(), radius - disp.get_magnitude(), kB * turn_dir * kR * log(disp.get_magnitude() / radius), rad_to_deg(angle_diff), kB * kA * angle_diff);
#ifndef xy_enable
    // radial ratio
    printf("radius:%lf, angular_vel: %lf, radial_error: %lf, radial_correction: %lf, angle_diff: %lf, angle_correction: %lf\n", radius, kB * turn_dir * linear_velocity / disp.get_magnitude(), radius - disp.get_magnitude(), kB * turn_dir * kR * log(disp.get_magnitude() / radius), rad_to_deg(angle_diff), kB * kA * angle_diff);
    // radial diff
    // printf("radius:%lf, disp_mag: %lf, angular_vel: %lf, radial_error: %lf, radial_correction: %lf, angle_diff: %lf, angle_correction: %lf\n", radius, disp.get_magnitude(), kB * turn_dir * linear_velocity / disp.get_magnitude(),  turn_dir * (disp.get_magnitude() - radius), kB * turn_dir * kR * (disp.get_magnitude() - radius), rad_to_deg(angle_diff), kB * kA * angle_diff);

#endif

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

    // waits for arc angle to become less than 1 degree
    if (rad_to_deg(fabs(near_angle(final_angle, atan2(tracking.y_coord - centre.y, tracking.x_coord - centre.x)))) < 1.0){

      // printf("ang: %lf\n", atan2(tracking.y_coord - centre.y, tracking.x_coord - centre.x));
      drivebase.move_tank(0.0, 0.0);
      if (brake)  drivebase.brake();
      tracking.move_complete = true;
      motion_d.print("Ending move on arc to target X: %.2f Y: %.2f A: %.2f at X: %.2f Y: %.2f A: %.2f \n", target.x, target.y, target.angle, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
      return;
    }
    printf("%lf,%lf\n", tracking.x_coord, tracking.y_coord);
#ifdef xy_enable
    printf("%lf,%lf\n", tracking.x_coord, tracking.y_coord);
#else
    printf("power_y:%lf, power_a: %lf\n",tracking.power_y, tracking.power_a);
#endif
    drivebase.move_tank(tracking.power_y, tracking.power_a);
    if(ptr->notify_handle())return;
    delay(10);
  }
}

/*
// Start of pwm to velocity relation testing
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

// power: 60, velocity: 110 | 1.83 // with weight: 95 // with goal: 90
// power: 75, velocity: 150	| 2.0  // with weight: 125 // with goal: 120
// below are all with weight
// power: 90, velocity: 145 | 1.6 // with goal: 140
// power: 100, velocity: 170 | 1.7  // with goal: 165
// power: 127, velocity: 235 | 1.8  // with goal: 230

// End of velocity relation testing
*/





void move_on_line_old(void* params){
  _Task* ptr = _Task::get_obj(params);
  // line_old_params* param_ptr = static_cast<line_old_params*>(_Task::get_params(params));
  double start_x = line_old_params_g.start_x, start_y = line_old_params_g.start_y;
  double target_x = line_old_params_g.target_x, target_y = line_old_params_g.target_y, target_a = deg_to_rad(line_old_params_g.target_a);
  double end_error = line_old_params_g.end_error, end_error_a = line_old_params_g.end_error_a;
  bool Brake = line_old_params_g.Brake, debug = line_old_params_g.debug;
  int max_power = line_old_params_g.max_power;
  bool Overshoot = line_old_params_g.Overshoot; //
  double lpercent= line_old_params_g.lpercent, apercent=line_old_params_g.apercent;
  //ki ly doesnt work but im lazy so fuck you
  double kp_lx =23.0, kp_ly =15.0, kp_dx = 23.0, kp_dy = 15.0, kp_a = 125.0, ki_lx = 0.0, ki_ly = 0.0, ki_dx = 0.0, ki_dy = 0.0, ki_a = 0.0, kd_lx = 0.0, kd_ly = 0.0, kd_dx = 0.0, kd_dy = 0.0, kd_a = 1000.0;
  double error_angle, error_line, error_d, error_x, error_y, error_a , error_tot;
  double power_line_x, power_line_y, power_d_x, power_d_y, total_power, line_total;
  double dif_a;
  double min_lx, min_ly, min_a, min_total;
  double integ_start = 5.0, integ_l, integ_d, integ_a;
  double deriv_l, deriv_d, deriv_a, last_error_l, last_error_d, last_error_a;
  uint32_t integ_last_time = millis(), deriv_time = millis(), start_time = millis();
  int last_time = 0;
  //log_time("starting time: %d X: %f Y: %f A: %f from X: %f Y: %f A: %f \n", millis(), target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
  motion_i.print("Starting move to target X: %f Y: %f A: %f from X: %f Y: %f A: %f\n", target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));

  tracking.move_complete = false;
  tracking.movetime = millis();
  double start_error_x = target_x - start_x, start_error_y = target_y - start_y;
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


      if(fabs(error_line) <=0.5)integ_l = 0.0;
      if(fabs(error_d) <= 0.5)integ_d = 0.0;

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




    if(millis() - last_time > 20){
      motion_i.print("pre sc lx: %f, ly: %f, dx: %f, dy: %f, a: %f\n",power_line_x,power_line_y,power_d_x,power_d_y,tracking.power_a);
    }

    total_power = fabs(power_line_x+power_d_x)+fabs(power_line_y+power_d_y)+fabs(tracking.power_a);
    if(total_power > max_power){
      line_total = fabs(power_line_x)+fabs(power_line_y);
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
      power_line_x =(((fabs(power_line_x)-min_lx)/(total_power-min_total))*(max_power-min_total)+min_lx)*sgn(power_line_x);
      power_line_y =(((fabs(power_line_y)-min_ly)/(total_power-min_total))*(max_power-min_total)+min_ly)*sgn(power_line_y);
      power_d_x =(power_d_x/(total_power-min_total))*(max_power-min_total);
      power_d_y =(power_d_y/(total_power-min_total))*(max_power-min_total);
      tracking.power_a =(((fabs(tracking.power_a)- min_a)/(total_power-min_total))*(max_power-min_total)+min_a)*sgn(tracking.power_a);
    }


    tracking.power_x = power_line_x + power_d_x;
    if(fabs(error_x) < end_error - 0.2) tracking.power_x = 0.0;
    else if(fabs(tracking.power_x) < min_move_power_x)tracking.power_x = sgn(tracking.power_x)*min_move_power_x;
    tracking.power_y = power_line_y + power_d_y;
    if(fabs(error_y) < end_error - 0.2) tracking.power_y = 0.0;
    else if(fabs(tracking.power_y) < min_move_power_y)tracking.power_y = sgn(tracking.power_y)*min_move_power_y;
    if(fabs(rad_to_deg(error_a)) < end_error_a - 1.0)tracking.power_a = 0.0;
    else if(fabs(tracking.power_a) < min_move_power_a)tracking.power_a = sgn(tracking.power_a)*min_move_power_a;




    drivebase.move(tracking.power_x,tracking.power_y,tracking.power_a);

    if(millis() - last_time > 20 && debug){
      motion_d.print("errors x: %.2f, y: %.2f, a: %.2f\n",error_x, error_y,rad_to_deg(error_a));
      motion_d.print("errors l: %.2f, d: %.2f\n",error_line, error_d);
      motion_d.print("powers x: %.2f, y: %.2f, a: %.2f\n",tracking.power_x, tracking.power_y, tracking.power_a);
      motion_d.print("post s lx: %f, ly: %f, dx: %f, dy: %f, a: %f\n",power_line_x,power_line_y,power_d_x,power_d_y,tracking.power_a);
      if(lpercent != 0.0 || apercent != 0.0)motion_d.print("mins  lx: %.2f, ly: %.2f, a: %.2f\n",min_lx,min_ly,min_a);
      if(ki_lx != 0.0 || ki_ly != 0.0 || ki_dx != 0.0 || ki_dy != 0.0 || ki_a != 0.0)motion_d.print("integ  l: %.2f, d: %.2f\n",integ_l,integ_d);
      if(kd_lx != 0.0 || kd_ly != 0.0 || kd_dx != 0.0 || kd_dy != 0.0 || kd_a != 0.0)motion_d.print("deriv  l: %.2f, d: %.2f a: %.2f\n",deriv_l, deriv_d, deriv_a);
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
      // brake();
      drivebase.brake();
      tracking.move_complete = true;
      motion_i.print("Ending move to target X: %f Y: %f A: %f at X: %f Y: %f A: %f, time: %d \n", target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle), millis() - start_time);
      //log_time("ending starting time: %d, delta time: %d X: %f Y: %f A: %f from X: %f Y: %f A: %f \n", millis(),millis() -starttime, target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
      // tracking.move_stop_task();
      return;
    }


    if(ptr->notify_handle())return;



    delay(10);
  }
}







//Gyro balance class

//Constructor
Gyro::Gyro(Imu& imu): inertial(imu){}

double Gyro::get_angle() {
  angle = GYRO_SIDE*inertial.GYRO_AXIS();
  return fabs(angle);
}

void Gyro::calibrate(){
  inertial.reset();
}

void Gyro::finish_calibrating(){
	while (inertial.is_calibrating()){
		motion_d.print("Calibrating Gyro...\n");
		delay(10);
	}
  motion_i.print("Done Calibrating Gyro\n");
}

void Gyro::climb_ramp(){
  drivebase.set_state(HIGH);
  finish_calibrating(); //Makes sure it's calibrated before starting (should already be)
  inertial.tare_roll();
  inertial.tare_pitch();

  drivebase.move_tank(127, 0);
  waitUntil(get_angle() > 22)
  double ramp_angle = get_angle();
  motion_i.print("ON RAMP: %f\n", ramp_angle);
  waitUntil(!inRange(r_reset_dist.get(), 0, 200));
  drivebase.move_tank(40, 0);
  cycleCheck(get_angle() < 20, 3, 10);
  drivebase.brake();
}

void Gyro::level(double kP, double kD){
	double last_angle=0;
	PID gyro_p(kP, 0, kD, 0);
  Timer gyro_steady ("Gyro", &motion_d);
  int speed;

	while(true){
    get_angle();
    speed = gyro_p.compute(-angle, 0);
		drivebase.move_tank(speed, 0);
    gyro_steady.print("Angle: %f | Speed: %d\n", angle, speed);
    
		if (fabs(angle-last_angle) > 0.6) gyro_steady.reset(); //Unsteady, Resets the steady timer
		else if (fabs(angle) < 6 && gyro_steady.get_time() > 450) break; //If within range to be level and steady on ramp

		if (master.interrupt(true, false)) return;

		last_angle = angle;
		delay(10);
	}

	motion_d.print("\nLevelled on ramp\n\n");
	drivebase.brake();
}