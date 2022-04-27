#include "tracking.hpp"
#include "Libraries/geometry.hpp"
#include "Libraries/printing.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "auton_util.hpp"
#include "Libraries/gui.hpp"
#include "Libraries/pid.hpp"
#include "drive.hpp"
#include "Libraries/util.hpp"
#include "Libraries/task.hpp"
#include "Subsystems/b_lift.hpp"
#include "Subsystems/f_lift.hpp"

Tracking tracking;

_Task update_t(update, "Tracking");
_Task move_t(move_on_arc, "Movement");
Gyro gyro(ramp_imu);

arc_params arc_params_g;
line_params line_params_g;
tank_arc_params tank_arc_params_g;
point_params point_params_g;
tank_point_params tank_point_params_g;
turn_angle_params turn_angle_params_g;
turn_point_params turn_point_params_g;
line_old_params line_old_params_g;
tank_rush_params tank_rush_params_g;

double point_to_angle(Point point){
  return rad_to_deg(Vector(tracking.coord(), point).get_angle());
}

Position polar_to_vector_point(double magnitude, double theta, double angle){
  return polar_to_vector_point(tracking.coord(), magnitude, theta, angle);
}

double Tracking::near_angle(double angle) const{
  return round((position.angle - angle) / (M_TWOPI)) * (M_TWOPI) + angle - position.angle;
}

double Tracking::near_angle(Position pos) const{
  return near_angle(pos.angle);
}

Position Tracking::coord() const{
  return rad_to_deg(position);
}

double Tracking::global_angle() const{
  return position.angle;
}

void Tracking::reset(double x, double y, double a){
  reset({x, y, a});
}

void Tracking::reset(Position position){
  update_t.data_update();
  tracking_imp.print("Resetting tracking from %.2f to %.2f", coord(), position);
  tracking.position = deg_to_rad(position);
  update_t.done_update();
  screen_flash::start("Reset", term_colours::GREEN);
}

double Tracking::get_dist(Position pos1, Position pos2){
  return Vector(pos2, pos1).get_magnitude();
}

double Tracking::get_dist(Position pos1){
  return get_dist(pos1, position);
}


//waits until the distance from current position to starting position is greater than the specified amount
void Tracking::wait_for_dist(double distance, int timeout){
  const Point start_pos = tracking.position;
  Timer dist_time{"Wait For Dist"};

  wait_until(get_dist(start_pos) > distance){
    if(timeout != 0 && dist_time.get_time() > timeout){
      tracking_imp.print("Timed out on waiting for %.2f inches at %.2f", distance, tracking.coord());
      break;
    }
  }
  tracking_imp.print("Completed %.2f inches at %.2f %.2f", distance, tracking.coord());
}

void update(void* params){
  _Task* ptr = _Task::get_obj(params);
  Timer data_timer{"tracking logs"};
  double Left, Right, Back, NewLeft, NewRight, NewBack, LastLeft = LeftEncoder.get_value() / 360.0 * (2.75 * M_PI), LastRight =  RightEncoder.get_value() / 360.0 * (2.75 * M_PI), LastBack = BackEncoder.get_value() / 360.0 * (2.75 * M_PI);
  double Theta = 0.0, Beta = 0.0, Alpha = 0.0;
  double RadiusR, RadiusB, h, h2;
  double Xx, Xy, Yy, Yx;
  double last_vel_l = 0, last_vel_r = 0, last_vel_b = 0;

  Position last_position; //last position of robot
  int last_velocity_time = 0; //time of last velocity update
  int velocity_update_time = 0; //time SINCE last velocity update

  while(true){
    NewLeft = LeftEncoder.get_value() / 360.0 * (2.75 * M_PI);
    NewRight = RightEncoder.get_value() / 360.0 * (2.75 * M_PI);
    NewBack = BackEncoder.get_value() / 360.0 * (2.75 * M_PI);

    Left = NewLeft - LastLeft;
    Right = NewRight - LastRight;
    Back = NewBack - LastBack;

    velocity_update_time = millis() - last_velocity_time;
    if(velocity_update_time > 10){ //velocity is updated every 20 ms
      tracking.l_velo = (NewLeft - last_vel_l) / velocity_update_time * 1000; //velocities are in inches per second
      tracking.r_velo = (NewRight - last_vel_r) / velocity_update_time * 1000;
      tracking.b_velo = (NewBack - last_vel_b) / velocity_update_time * 1000;

      last_vel_l = NewLeft;
      last_vel_r = NewRight;
      last_vel_b = NewBack;

      //gives us linear velocity in inches per second, and angular velocity in radians per second
      tracking.g_velocity = (tracking.position - last_position) / velocity_update_time * 1000;

      last_position = tracking.position;

      last_velocity_time = millis();
    }

    LastLeft = NewLeft;
    LastRight = NewRight;
    LastBack = NewBack;

    Theta = (Left-Right) / DistanceLR;
    if (Theta != 0){
      RadiusR = Right / Theta;
      RadiusB = Back / Theta;
      Beta = Theta / 2.0;
      h = 2 * (RadiusR + DistanceLR / 2) * sin(Beta);
      h2 = 2 * (RadiusB + DistanceB) * sin(Beta);
    }
    else{
      h = Right;
      h2 = Back;
      Beta = 0.0;
    }
    Alpha = tracking.position.angle + Beta;

    Xx = h2 * cos(Alpha);
    Xy = h2 * -sin(Alpha);
    Yx = h * sin(Alpha);
    Yy = h * cos(Alpha);

    tracking.position.x += Xx + Yx;
    tracking.position.y += Yy + Xy;
    tracking.position.angle += Theta;


    tracking_data.print(&data_timer, 10, {
      //[](){return sprintf2("Pos: %.2lf", tracking.coord());},
      //[](){return sprintf2("GLOBAL VELOCITY| x: %.2f, y: %.2f a: %.2f", tracking.g_velocity.x, tracking.g_velocity.y, rad_to_deg(tracking.g_velocity.angle));},
      //[](){return sprintf2("ENCODER L: %d, R: %d, B:%d", LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());},
      //[](){return sprintf2("ENCODER VELO| l: %.2f, r: %.2f, b: %.2f", tracking.l_velo, tracking.r_velo, tracking.b_velo);},
      [](){return sprintf2("%.2lf, L_v:%.2f, B_v:%.2f", tracking.coord(), tracking.l_velo, tracking.b_velo);},
    });

    if(ptr->notify_handle()) return;
    delay(10);
  }
}

void tank_rush_goal(void* params){
  _Task* ptr = _Task::get_obj(params);
  //tank_point_params* param_ptr = static_cast<tank_point_params*>(_Task::get_params(params));
  const Position target = tank_rush_params_g.target;
  const bool turn_dir_if_0 = tank_rush_params_g.turn_dir_if_0;
  const double max_pwr = tank_rush_params_g.max_pwr;
  const double min_angle_percent = tank_rush_params_g.min_angle_percent;
  const bool brake = tank_rush_params_g.brake;
  double kp_a = tank_rush_params_g.kp_a;
  double kd_a = tank_rush_params_g.kd_a;
  double dist_past = tank_rush_params_g.dist_past;
  //Pid angle(kp.a);
  
  tracking.move_complete = false;

  Point local_error;
  Position error;

  double total_pwr, max_pwr_scale;
  int sgn_local_error_y, orig_sgn_line_y, sgn_line_y;
  double difference_a;
  double hypotenuse;

  //double kp_y = 11.5, kp_a = 150.0, kd_a = 0.0;
  //printf2("Local errors | x: %f, y: %f ", local_error.x, local_error.y);
  double min_pwr_a = max_pwr * min_angle_percent;
  double pre_scaled_pwr_a;
  double end_error = 0.5;

  //double deriv_a = 0.0;
  PID angle(kp_a, 0.0, kd_a, 0.0);

  //move on line variables
  Vector follow_line = Vector(tracking.coord(), target).invert(); //used to keep track of angle of follow_line relative to the vertical
  Vector line_disp(tracking.coord(), target);  //displacements relative to line
  int time = millis();

  //MOTION FIX
    //global displacement of robot and target
    line_disp = Vector(tracking.coord(), target);
    hypotenuse = line_disp.get_magnitude(); //distance to target

    //rotates vector by line angle, now the vector represents the displacement RELATIVE TO THE LINE
    line_disp.rotate(follow_line.get_angle());
    orig_sgn_line_y = sgn(line_disp.get_y()); //used to calculate if the robot has overshot
    //END OF MOTION FIX

  motion_i.print("Starting tank rush goal to point from %.2f to %.2f", tracking.coord(), target);
  while(true){
    //global displacement of robot and target
    line_disp = Vector(tracking.coord(), target);
    hypotenuse = line_disp.get_magnitude(); //distance to target

    //rotates vector by line angle, now the vector represents the displacement RELATIVE TO THE LINE
    line_disp.rotate(follow_line.get_angle());
    sgn_line_y = sgn(line_disp.get_y() + dist_past); //used to calculate if the robot has overshot

    //difference in angle between angle to target and robot's local y axis
    difference_a = tracking.global_angle() + Vector(tracking.coord(), target).get_angle();

    
    local_error.y = sin(difference_a) * hypotenuse; //local y displacement to target
    sgn_local_error_y = sgn(local_error.y);
    //printf2("sgn_y: %d", sgn_local_error_y);
    if(sgn_local_error_y == 0){
      sgn_local_error_y = bool2sgn(turn_dir_if_0);
      //printf2("triggered");
    }
    //chooses nearest angle to turn to face target (back or front)
    error.angle = near_angle(sgn_local_error_y * M_PI / 2, difference_a);
    //printf2("Errors | y: %f, a: %f", local_error.y, rad_to_deg(error.angle));

    //tracking.pwr.y = kp_y * local_error.y;
    //tracking.pwr.y = kp_y * line_y_local_y;
    //tracking.pwr.angle = angle.compute(error.angle, 0.0);
    tracking.pwr.angle = kp_a * error.angle;
    //gives min pwr to local y if that is not satisfied
    //if (fabs(tracking.pwr.y) < min_move_pwr_y && fabs(local_error.y) > 0.5) tracking.pwr.y = min_move_pwr_y * sgn(local_error.y);

    //scales pwrs
    total_pwr = fabs(tracking.pwr.angle);
    if (total_pwr > max_pwr){
      tracking.pwr.y = 0;
      pre_scaled_pwr_a = tracking.pwr.angle;
      max_pwr_scale = max_pwr / total_pwr;
      tracking.pwr.angle *= max_pwr_scale;
    }
    else tracking.pwr.y = orig_sgn_line_y * (max_pwr - total_pwr);
    //printf2("pwrs | y: %f, a: %f", tracking.pwr.y, tracking.pwr.angle);

    motion_d.print("err_y: %.2f err_a : %.2f pow_y: %.2f, pow_a: %.2f\n ", local_error.y, rad_to_deg(error.angle), tracking.pwr.y, tracking.pwr.angle);
    graph.print("%d, %f\n", millis() - time, tracking.l_velo);

    if(f_dist.get() < 140){
      f_claw(1);
      if (brake) drivebase.brake();
      tracking.move_complete = true;
      motion_i.print("Ending tank rush goal target %f at %f | time: %d", target, tracking.coord(), millis() - time);
      break;
    }
    if(orig_sgn_line_y != sgn_line_y){
      if (brake) drivebase.brake();
      tracking.move_complete = true;
      motion_i.print(term_colours::ERROR, "MISSED GOAL tank rush goal target %f at %f time: %d", target, tracking.coord(), millis() - time);
      break;
    }
    drivebase.move(tracking.pwr.y, tracking.pwr.angle);
    if(ptr->notify_handle()) return;
    delay(10);
  }
}



arc_params::arc_params(const Point start, Position target, const double radius, const bool positive, const double max_pwr, const bool angle_relative_to_arc, const double min_angle_percent, const bool brake, const double decel_dist, const double decel_speed):
  start{start}, target{target}, radius{radius}, positive{positive}, max_pwr{max_pwr}, angle_relative_to_arc{angle_relative_to_arc}, min_angle_percent{min_angle_percent}, brake{brake}, decel_dist{decel_dist}, decel_speed{decel_speed}{}

line_params::line_params(const Point start, Position target, const double max_pwr, const bool overshoot, const double min_angle_percent, const bool brake, const double decel_dist, const double decel_speed):
  start{start}, target{target}, max_pwr{max_pwr}, overshoot{overshoot}, min_angle_percent{min_angle_percent}, brake{brake}, decel_dist{decel_dist}, decel_speed{decel_speed}{}

line_old_params::line_old_params(double start_x, double start_y, double target_x, double target_y, double target_a, bool Brake, bool debug, int max_pwr, bool Overshoot, double end_error, double end_error_a, double p, double lpercent , double apercent):
  start_x{start_x}, start_y{start_x}, target_x{target_x}, target_y{target_y}, target_a{target_a}, Brake{Brake}, debug{debug}, max_pwr{max_pwr}, Overshoot{Overshoot}, end_error{end_error}, end_error_a{end_error_a}, p{p}, lpercent{lpercent}, apercent{apercent}{}

point_params::point_params(const Position target, const double max_pwr, const bool overshoot, const double min_angle_percent, const bool brake, const double decel_dist, const double decel_speed, pid_const x_pid_const, pid_const y_pid_const, pid_const a_pid_const, const uint32_t time_out, double end_error, double end_error_a):
  target{target}, max_pwr{max_pwr}, overshoot{overshoot}, min_angle_percent{min_angle_percent}, brake{brake}, decel_dist{decel_dist}, decel_speed{decel_speed}, x_pid_const{x_pid_const}, y_pid_const{y_pid_const}, a_pid_const{a_pid_const}, time_out{time_out}, end_error{end_error}, end_error_a{end_error_a}{}

tank_arc_params::tank_arc_params(const Point start_pos, Position target, const double pwr, const double max_pwr, const bool brake, double decel_start, double decel_end, double decel_target_speed):
  start_pos{start_pos}, target{target}, pwr{pwr}, max_pwr{max_pwr}, brake{brake}, decel_start{decel_start}, decel_end{decel_end}, decel_target_speed{decel_target_speed}{}

tank_point_params::tank_point_params(const Position target, const bool turn_dir_if_0, const double max_pwr, const double min_angle_percent, const bool brake, double kp_y, double kp_a, double kd_a, int timeout, Point end_error, double min_pwr_y):
  target{target}, turn_dir_if_0{turn_dir_if_0}, max_pwr{max_pwr}, min_angle_percent{min_angle_percent}, brake{brake}, kp_y{kp_y}, kp_a{kp_a}, kd_a{kd_a}, timeout{timeout}, end_error{end_error}, min_pwr_y{min_pwr_y}{}

tank_rush_params::tank_rush_params(const Position target, const bool turn_dir_if_0, const double max_pwr, const double min_angle_percent, const bool brake, double kp_a, double kd_a, double dist_past): 
  target{target}, turn_dir_if_0{turn_dir_if_0}, max_pwr{max_pwr}, min_angle_percent{min_angle_percent}, brake{brake}, kp_a{kp_a}, kd_a{kd_a}, dist_past{dist_past}{}

turn_angle_params::turn_angle_params(const double target_a, const bool brake, bool near, double kp, double kd, double kp_v, double kd_v, double max_speed, int timeout, double min_pwr_a, double end_error):
  target_a{target_a}, brake{brake}, near{near}, kp{kp}, kd{kd}, kp_v{kp_v}, kd_v{kd_v}, max_speed{max_speed}, timeout{timeout}, min_pwr_a{min_pwr_a}, end_error{end_error}{}

turn_point_params::turn_point_params(const Point target, const bool brake, double max_pwr, int timeout):
  target{target}, brake{brake}, max_pwr{max_pwr}, timeout{timeout}{}

//std::variant<arc_params, line_params, tank_arc_params, point_params, tank_point_params, turn_angle_params, turn_point_params> params
void move_start(move_types type, std::variant<arc_params, line_params, tank_arc_params, point_params, tank_point_params, turn_angle_params, turn_point_params, line_old_params, tank_rush_params> params, bool wait_for_comp){
  //params_g = params;
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
    case move_types::tank_rush:
      tank_rush_params_g = std::get<tank_rush_params>(params);
      move_t.rebind(tank_rush_goal);
    break;
  }
  if(wait_for_comp) move_wait_for_complete();
}

bool move_wait_for_complete(){
  wait_until(!get_move_state());
  return tracking.move_complete;
}

bool get_move_state(){
  return move_t.get_task_ptr()->get_state() != 4;
}

void move_wait_for_error(double error){
  wait_until(fabs(tracking.drive_error) <= error);
}

void move_stop(bool brake){
  move_t.kill();
  if(brake) drivebase.brake();
  else drivebase.move(0, 0);
}


void move_to_point(void* params){
  _Task* ptr = _Task::get_obj(params);

  Position target = point_params_g.target;
  double max_pwr = point_params_g.max_pwr;
  bool overshoot = point_params_g.overshoot;
  double min_angle_percent = point_params_g.min_angle_percent;
  bool brake = point_params_g.brake;
  double decel_dist = point_params_g.decel_dist, decel_speed = point_params_g.decel_speed;
  pid_const x_pid_const = point_params_g.x_pid_const, y_pid_const = point_params_g.y_pid_const, a_pid_const = point_params_g.a_pid_const;
  const uint32_t time_out = point_params_g.time_out;
  double end_error = point_params_g.end_error;
  double end_error_a = point_params_g.end_error_a;

  tracking.move_complete= false;
  target = deg_to_rad(target);
  Position error;
  //Position tracking = {-2.0, 2.0, -20.0}, error;

  //Vector follow_line {target.x - start.x, target.y - start.y};
  //double line_angle = Vector(start, target).get_angle(); //angle of follow_line relative to the vertical

  Vector target_line (tracking.coord(), target);    //line of current position to target
  //target_line.rotate(line_angle);    //displacement towards and along follow_line
  int orig_sgn_line_y = sgn(target_line.get_y());

  //pwr_scaling variables
  //scaling variables
  double pre_scaled_pwr_a = tracking.pwr.angle;
  double d; //distance to target
  double angle_pwr_guarantee_xy_scale;
  double max_pwr_scale;
  double pwr_xy;
  double total_pwr;
  double min_pwr_a = max_pwr * min_angle_percent;
  //PID'S

  PID x_pid(x_pid_const.p, x_pid_const.i, x_pid_const.d, 0.0, true, 0.2, 3.0);
  PID y_pid(y_pid_const.p, y_pid_const.i, y_pid_const.d, 0.0, true, 0.2, 3.0);
  PID angle_pid(a_pid_const.p, a_pid_const.i, a_pid_const.d, 0.0, true, 0.0, 360.0);

  //decel variables
  double h; //magnitude of pwr vector
  double decel_pwr, decel_pwr_scale;
  double decel_start = Vector::make_cartesian(x_pid.get_proportional(), y_pid.get_proportional()).get_magnitude();  //theoretically when the deceleration starts
  Timer move_timer{"move_timer"};
  master.print(1, 1, "y: %.3f", target.y);
  motion_i.print("Move to point: %.2f from: %.2f", rad_to_deg(target), tracking.coord());
  while(true){
    //gets line displacements
    printf2("Pos:%f", tracking.coord());
    target_line.set_cartesian(tracking.coord(), target);
    target_line.rotate(tracking.global_angle());  //now represents local vectors to target
    d = target_line.get_magnitude();
    tracking.drive_error = d;

    tracking.pwr.x = x_pid.compute(-target_line.get_x(), 0.0);
    tracking.pwr.y = y_pid.compute(-target_line.get_y(), 0.0);
    tracking.pwr.angle = angle_pid.compute(tracking.global_angle(), tracking.near_angle(target) + tracking.global_angle());

    h = Vector(tracking.pwr).get_magnitude();

    //deceleration code
    if(decel_dist){
      printf2("DECEL");
      if(d < decel_dist){ //robot is past decel dist
        decel_pwr = std::clamp(map(d, decel_dist, max_pwr / decel_start, decel_speed, max_pwr), decel_speed, max_pwr);
        if (decel_pwr < decel_speed) decel_pwr = decel_speed;
        decel_pwr_scale = decel_pwr / h;
        tracking.pwr *= decel_pwr_scale;
      }
      else{ //robot is before decel dist
        decel_pwr_scale = max_pwr / (fabs(tracking.pwr.x) + fabs(tracking.pwr.y));
        tracking.pwr *= decel_pwr_scale;
      }
    }

    //gives minimum move pwrs if requested
    if(fabs(tracking.pwr.x) < min_move_pwr_x) tracking.pwr.x = sgn(tracking.pwr.x) * min_move_pwr_x;
    if(fabs(tracking.pwr.y) < min_move_pwr_y) tracking.pwr.y = sgn(tracking.pwr.y) * min_move_pwr_y;
    if(fabs(tracking.pwr.angle) < min_move_pwr_a) tracking.pwr.angle = sgn(tracking.pwr.angle) * min_move_pwr_a;

    //printf2("After: x: %f, y: %f", target_line.get_x(), target_line.get_y());


    //pwr scaling and angle pwr guarantee
    total_pwr = fabs(tracking.pwr.x) + fabs(tracking.pwr.y) + fabs(tracking.pwr.angle);

    if(total_pwr > max_pwr){
      pre_scaled_pwr_a = tracking.pwr.angle;
      angle_pwr_guarantee_xy_scale = 1.0;

      max_pwr_scale = max_pwr / total_pwr;
      tracking.pwr *= max_pwr_scale;
      //start of angle pwr guarantee

      pwr_xy = fabs(tracking.pwr.x) + fabs(tracking.pwr.y);

      if (fabs(pre_scaled_pwr_a) > min_pwr_a){
        if (fabs(tracking.pwr.angle) < min_pwr_a)  tracking.pwr.angle = min_pwr_a * sgn(tracking.pwr.angle);  //pwr.angle has been overshadowed
      }
      //angle gets the pwr it demanded if pre_scaled pwr.angle was also less than min_pwr_a
      else tracking.pwr.angle = pre_scaled_pwr_a;
      if (pwr_xy > 0)  angle_pwr_guarantee_xy_scale = (max_pwr - fabs(tracking.pwr.angle)) / pwr_xy;
      tracking.pwr *= angle_pwr_guarantee_xy_scale;
      //printf2("****pwr.angle: %f, x:%f, y: %f, pre_scaled: %f, min: %f", tracking.pwr.angle, tracking.pwr.x , tracking.pwr.y, pre_scaled_pwr_a, min_pwr_a);
    }
    drivebase.move(tracking.pwr);

    motion_d.print("Pos:%f, err: (%f, %f), pow:%f, ex: %f, dx: %f, ey: %f, dy: %f", tracking.coord(), target.x - tracking.coord().x, target.y - tracking.coord().y, tracking.pwr, x_pid.error, x_pid.derivative, y_pid.error, y_pid.derivative);

    if (overshoot && sgn(target_line.get_y()) != orig_sgn_line_y){
      if(brake) drivebase.brake();
      else drivebase.move(0.0, 0.0, 0.0);
      tracking.move_complete = true;
      motion_i.print("Ending move to point: %.2f at %.2f", rad_to_deg(target), tracking.coord());
      return;
    }
    else if(fabs(d) < end_error && fabs(rad_to_deg(angle_pid.get_error())) < end_error_a){
      if(brake) drivebase.brake();
      else drivebase.move(0.0, 0.0, 0.0);
      tracking.move_complete = true;
      motion_i.print("Ending move to point: %.2f at %.2f", rad_to_deg(target), tracking.coord());
      return;
    }
    else if(move_timer.get_time() > time_out && time_out > 0){
      if(brake) drivebase.brake();
      else drivebase.move(0.0, 0.0, 0.0);
      tracking.move_complete = true;
      motion_i.print("TIMED OUT move to point: %.2f at %.2f", rad_to_deg(target), tracking.coord());
      return;
    }

    if(ptr->notify_handle()) return;
    delay(10);
  }
}

//this is 150-200 ms slower than tank move on arc on 24 radius 90 degree turn
void move_on_arc(void* params){
  _Task* ptr = _Task::get_obj(params);
  //arc_params* param_ptr = static_cast<arc_params*>(_Task::get_params(params));
  const Point start = arc_params_g.start;
  Position target = arc_params_g.target;
  double radius = arc_params_g.radius;
  bool positive = arc_params_g.positive;
  double max_pwr= arc_params_g.max_pwr;
  bool angle_relative_to_arc = arc_params_g.angle_relative_to_arc;
  double min_angle_percent = arc_params_g.min_angle_percent;
  bool brake = arc_params_g.brake;
  double decel_dist = arc_params_g.decel_dist;
  double decel_speed = arc_params_g.decel_speed;
  tracking.move_complete = false;

  //Position error, kp = Position(30.0, 12.0, 175.0);

  Position error, kp = Position(12.0, 30.0, 175.0);

  target = deg_to_rad(target);
  //variable 'd' in diagram
  double chord_dist = Vector(start, target).get_magnitude();
  printf2("chord_dist = %f", chord_dist);
  double theta = Vector(start, target).get_angle();
  printf2("theta = %f", rad_to_deg(theta));
  double half_chord_dist = chord_dist / 2.0;
  printf2("half_chord_dist: %f", half_chord_dist);
  if(radius < half_chord_dist){
    printf2("The radius needs to be at least %f inches, you inputted %f as a radius.", half_chord_dist, radius);
    return;
  }
  double alpha = acos(half_chord_dist / radius);
  printf2("alpha: %f", rad_to_deg(alpha));
  theta += positive ? alpha : -alpha;

  printf2("new_theta: %f", rad_to_deg(theta));

  Point arc_centre = {start.x + cos(theta) * radius, start.y + sin(theta) * radius};
  printf2("Arc centre: x: %f, y: %f", arc_centre.x, arc_centre.y);

  Position arc_disp;
  double hypotenuse, h;
  double angle_of_arc = Vector(arc_centre, target).get_angle();
  double beta; //angle relative to horizontal of h + robot angle + arc_disp.angle

  //move on arc variables
  double pwr_x_scale, pwr_y_scale;

  //scaling variables
  double pre_scaled_pwr_a = tracking.pwr.angle;
  double angle_pwr_guarantee_xy_scale;
  double max_pwr_scale;
  double pwr_xy;
  double total_pwr;
  double min_pwr_a = max_pwr * min_angle_percent;

  double decel_pwr, decel_pwr_scale;

  const double final_angle = Vector(arc_centre, target).get_angle(); //angle of final position to centre at end of move

  //positive turn_dir means cw movement about the arc
  int turn_dir = -sgn(rad_to_deg(near_angle(Vector(arc_centre, target).get_angle(), Vector(arc_centre, tracking.coord()).get_angle())));
  printf2("turn_dir: %d", turn_dir);

  motion_i.print("Starting move on arc to %.2f from %.2f", rad_to_deg(target), tracking.coord());
  while(true){
    arc_disp.angle = Vector(arc_centre, tracking.coord()).get_angle();
    //printf2("arc_disp.angle: %f", arc_disp.angle);
    hypotenuse = Vector(arc_centre, tracking.coord()).get_magnitude(); //dist from centre of arc to cur position
    arc_disp.x = radius - hypotenuse;
    arc_disp.y = radius * near_angle(angle_of_arc, arc_disp.angle);
    //printf2("\n Arc displacements | x: %f, y: %f", arc_disp.x, arc_disp.y);

    //rotating arc displacements
    h = Vector(arc_disp).get_magnitude();  //magnitude of displacement vector
    beta = Vector(arc_disp).get_angle() + tracking.global_angle() + arc_disp.angle;  //angle of displacement vector local to robot

    error.x = Vector::make_polar(h, beta).get_x();
    error.y = Vector::make_polar(h, beta).get_y();
    //printf2("Local errors | x: %f, y: %f", error.x, error.y);

    if (angle_relative_to_arc){
      //error.angle = near_angle(turn_dir * arc_disp.angle, tracking.position.angle);
      error.angle = near_angle(target.angle - arc_disp.angle + (turn_dir == -1 ? 0 : M_PI), tracking.global_angle());

      //printf2("angle target: %f, angle_error: %f", rad_to_deg(target.angle - arc_disp.angle + (turn_dir == -1 ? 0 : M_PI)), rad_to_deg(error.angle));
    }
    else  error.angle = tracking.near_angle(target);

    //deceleration code
    if(decel_dist){
      if(h < decel_dist){ //robot is past decel dist
        decel_pwr = std::clamp(map(h, decel_dist, max_pwr / kp.y, decel_speed, max_pwr), decel_speed, max_pwr);
        if (decel_pwr < decel_speed) decel_pwr = decel_speed;
        decel_pwr_scale = decel_pwr / h;
        tracking.pwr.x = error.x;
        tracking.pwr.y = error.y;
        tracking.pwr *= decel_pwr_scale;
      }
      else{ //robot is before decel dist
        decel_pwr_scale = max_pwr / (fabs(error.x) * kp.x + fabs(error.y) * kp.y);
        tracking.pwr.x = error.x * kp.x;
        tracking.pwr.y = error.y * kp.y;
        tracking.pwr *= decel_pwr_scale;
      }
    }
    else{
      tracking.pwr.x = error.x * kp.x;
      tracking.pwr.y = error.y * kp.y;
    }
    tracking.pwr.angle = error.angle * kp.angle;

    //gives minimum move pwrs if not satisfied
    if (fabs(tracking.pwr.angle) < min_move_pwr_a) tracking.pwr.angle = sgn(error.angle) * min_move_pwr_a;
    if (fabs(tracking.pwr.x) < min_move_pwr_x) tracking.pwr.x = sgn(error.x) * min_move_pwr_x;
    if (fabs(tracking.pwr.y) < min_move_pwr_y) tracking.pwr.y = sgn(error.y) * min_move_pwr_y;

    printf2("%f, %f", tracking.coord().x, tracking.coord().y);

    //MOVE ON ARC CODE
    total_pwr = fabs(tracking.pwr.x) + fabs(tracking.pwr.y) + fabs(tracking.pwr.angle);

    if(total_pwr > max_pwr){

        pre_scaled_pwr_a = tracking.pwr.angle;
        angle_pwr_guarantee_xy_scale = 1.0;

        max_pwr_scale = max_pwr / total_pwr;
        tracking.pwr *= max_pwr_scale;
        //start of angle pwr guarantee

        pwr_xy = fabs(tracking.pwr.x) + fabs(tracking.pwr.y);

        if (fabs(pre_scaled_pwr_a) > min_pwr_a){
            if (fabs(tracking.pwr.angle) < min_pwr_a)  tracking.pwr.angle = min_pwr_a * sgn(tracking.pwr.angle);  //pwr.angle has been overshadowed
        }
        //angle gets the pwr it demanded if pre_scaled pwr.angle was also less than min_pwr_a
        else    tracking.pwr.angle = pre_scaled_pwr_a;
        if (pwr_xy > 0)  angle_pwr_guarantee_xy_scale = (max_pwr - fabs(tracking.pwr.angle)) / pwr_xy;
        tracking.pwr *= angle_pwr_guarantee_xy_scale;
        //printf2("****pwr.angle: %f, x:%f, y: %f, pre_scaled: %f, min: %f", tracking.pwr.angle, tracking.pwr.x , tracking.pwr.y, pre_scaled_pwr_a, min_pwr_a);
    }
    //end of scaling
    //END OF MOVE ON ARC CODE
    drivebase.move(tracking.pwr);
    //waits for arc angle to become less than 1 degree
    if (rad_to_deg(fabs(near_angle(final_angle, Vector(arc_centre, tracking.coord()).get_angle()))) < 1.0){
      motion_i.print("Ending move on arc: %.2f at %.2f", rad_to_deg(target), tracking.coord());
      if(brake) drivebase.brake();
      else drivebase.move(0, 0, 0);
      printf2("min_a: %f", min_pwr_a);
      tracking.move_complete = true;
      return;
    }
    //printf2("****pwr.angle: %f, x:%f, y: %f, pre_scaled: %f, min: %f, sum: %f", tracking.pwr.angle, tracking.pwr.x , tracking.pwr.y, pre_scaled_pwr_a, min_pwr_a, fabs(tracking.pwr.angle) + fabs(tracking.pwr.x) + fabs(tracking.pwr.y));
    if(ptr->notify_handle()) return;
    delay(10);
  }
}

void move_on_line(void* params){
  _Task* ptr = _Task::get_obj(params);
  //line_params* param_ptr = static_cast<line_params*>(_Task::get_params(params));

  const Point start = line_params_g.start;
  Position target = line_params_g.target;
  const double max_pwr = line_params_g.max_pwr;
  const bool overshoot = line_params_g.overshoot;
  const double min_angle_percent = line_params_g.min_angle_percent;
  const bool brake = line_params_g.brake;
  const double decel_dist = line_params_g.decel_dist, decel_speed = line_params_g.decel_speed;
  tracking.move_complete = false;
  target = deg_to_rad(target);
  Position error;
  //Position tracking = {-2.0, 2.0, -20.0}, error;

  //Vector follow_line (start, target);
  double line_angle = Vector(start, target).get_angle(); //angle of follow_line relative to the vertical

  Vector target_line (tracking.coord(), target);    //line of current position to target
  target_line.rotate(line_angle);    //displacement towards and along follow_line
  int orig_sgn_line_y = sgn(target_line.get_y());

  //pwr_scaling variables
  //scaling variables
  double pre_scaled_pwr_a = tracking.pwr.angle;
  double d; //distance to target
  double angle_pwr_guarantee_xy_scale;
  double max_pwr_scale;
  double pwr_xy;
  double total_pwr;
  double min_pwr_a = max_pwr * min_angle_percent;
  constexpr double x_multiplier = 2.0; //how much slower the robot strafes than moves forwards
  //PID'S

  PID x_line_pid(23.0, 0.000, 0.0, 0.0, true, 0.2, 3.0);
  PID y_line_pid(9.5, 0.0, 1000.0, 0.0, true, 0.2, 3.0);
  //PID angle_pid(175.0, 0.0, 0.0, 0.0, true, 0.0, 360.0);
  double kp_a = 125.0;
  //decel variables
  double h; //magnitude of pwr vector
  double decel_pwr, decel_pwr_scale;
  motion_i.print("Starting move on line to %.2f from %.2f", rad_to_deg(target), tracking.coord());
  //motion_i.print("%d | ")
  while(true){
    //gets line displacements
    target_line.set_cartesian(tracking.coord(), target);
    target_line.rotate(tracking.global_angle()); //Don't these lines just undo each other?
    target_line.rotate(-tracking.global_angle());
    printf2("Errors | x %f, y: %f", target_line.get_x(), target_line.get_y());

    d = target_line.get_magnitude();  //distance to target
    target_line.rotate(line_angle);    //target_line becomes displacement towards and along follow_line
    //printf2("x: %f, y: %f", target_line.get_x(), target_line.get_y());
    //printf2("%f, %f", tracking.position.x, tracking.position.y);

    //feeds line displacements into PID's
    x_line_pid.compute(-target_line.get_x(), 0.0);
    y_line_pid.compute(-target_line.get_y(), 0.0);
    //angle_pid.compute(tracking.position.angle, near_angle(target.angle, tracking.position.angle) + tracking.position.angle);

    target_line.set_cartesian(x_line_pid.get_output(), y_line_pid.get_output());

    //target_line now represents local vectors of robot to target, after PID transformation
    target_line.rotate(tracking.global_angle() - line_angle);

    tracking.pwr.x = target_line.get_x() * x_multiplier;
    tracking.pwr.y = target_line.get_y();
    //tracking.pwr.angle = angle_pid.get_output();
    tracking.pwr.angle = kp_a * tracking.near_angle(target);
    //printf("output: %f", angle_pid.get_output());
    printf2("output | pow: %f", tracking.pwr);

    h = Vector(tracking.pwr).get_magnitude();
    ////////////
    //deceleration code
    if(decel_dist){
      if(d < decel_dist){ //robot is past decel dist
        decel_pwr = std::clamp(map(d, decel_dist, max_pwr / y_line_pid.get_proportional(), decel_speed, max_pwr), decel_speed, max_pwr);
        if (decel_pwr < decel_speed) decel_pwr = decel_speed;
        decel_pwr_scale = decel_pwr / h;
        tracking.pwr *= decel_pwr_scale;
      }
      else{ //robot is before decel dist
        decel_pwr_scale = max_pwr / (fabs(tracking.pwr.x) + fabs(tracking.pwr.y));
        tracking.pwr *= decel_pwr_scale;
      }
    }
    //else{
    // //tracking.pwr.x = error.x * kp.x;
    // //tracking.pwr.y = error.y * kp.y;
    //}
    //tracking.pwr.angle = error.angle * kp.angle;
    /////////////


    //gives minimum move pwrs if requested
    if(fabs(tracking.pwr.x) < min_move_pwr_x)   tracking.pwr.x = sgn(tracking.pwr.x) * min_move_pwr_x;
    if(fabs(tracking.pwr.y) < min_move_pwr_y)   tracking.pwr.y = sgn(tracking.pwr.y) * min_move_pwr_y;
    if(fabs(tracking.pwr.angle) < min_move_pwr_a)   tracking.pwr.angle = sgn(tracking.pwr.angle) * min_move_pwr_a;

    //printf2("After: x: %f, y: %f", target_line.get_x(), target_line.get_y());


    //pwr scaling and angle pwr guarantee
    total_pwr = fabs(tracking.pwr.x) + fabs(tracking.pwr.y) + fabs(tracking.pwr.angle);

    if(total_pwr > max_pwr){
      pre_scaled_pwr_a = tracking.pwr.angle;
      angle_pwr_guarantee_xy_scale = 1.0;

      max_pwr_scale = max_pwr / total_pwr;
      tracking.pwr *= max_pwr_scale;
      //start of angle pwr guarantee

      pwr_xy = fabs(tracking.pwr.x) + fabs(tracking.pwr.y);

      if (fabs(pre_scaled_pwr_a) > min_pwr_a){
        if (fabs(tracking.pwr.angle) < min_pwr_a)  tracking.pwr.angle = min_pwr_a * sgn(tracking.pwr.angle);  //pwr.angle has been overshadowed
      }
      //angle gets the pwr it demanded if pre_scaled pwr.angle was also less than min_pwr_a
      else tracking.pwr.angle = pre_scaled_pwr_a;
      if (pwr_xy > 0) angle_pwr_guarantee_xy_scale = (max_pwr - fabs(tracking.pwr.angle)) / pwr_xy;
      tracking.pwr *= angle_pwr_guarantee_xy_scale;
      //printf2("****pwr.angle: %f, x:%f, y: %f, pre_scaled: %f, min: %f", tracking.pwr.angle, tracking.pwr.x , tracking.pwr.y, pre_scaled_pwr_a, min_pwr_a);
    }
    printf2("angle: %f, error: %f, pwr: x: %f, y: %f, a: %f", (tracking.coord().angle), rad_to_deg(tracking.near_angle(target)),  tracking.pwr.x, tracking.pwr.y, tracking.pwr.angle);

    drivebase.move(tracking.pwr);

    if (overshoot && sgn(target_line.get_y()) != orig_sgn_line_y){
      if(brake) drivebase.brake();
      else drivebase.move(0.0, 0.0, 0.0);
      tracking.move_complete = true;
      printf2("Ending move on line: %.2f at %.2f", rad_to_deg(target), tracking.coord());
      return;
    }
    else if(fabs(d) < 0.5 && fabs(rad_to_deg(tracking.near_angle(target))) < 5.0){
      if(brake) drivebase.brake();
      else drivebase.move(0.0, 0.0, 0.0);
      tracking.move_complete = true;
      printf2("Ending move on line: %.2f at %.2f", rad_to_deg(target), tracking.coord());
      return;
    }
    printf2("sum:%f", fabs(tracking.pwr.x) + fabs(tracking.pwr.y) + fabs(tracking.pwr.angle));
    if(ptr->notify_handle()) return;
    delay(10);
  }
}

//TANK MOVE ALGORITHMS

void tank_move_to_target(void* params){
  _Task* ptr = _Task::get_obj(params);
  //tank_point_params* param_ptr = static_cast<tank_point_params*>(_Task::get_params(params));
  const Position target = tank_point_params_g.target;
  const bool turn_dir_if_0 = tank_point_params_g.turn_dir_if_0;
  const double max_pwr = tank_point_params_g.max_pwr;
  const double min_angle_percent = tank_point_params_g.min_angle_percent;
  const bool brake = tank_point_params_g.brake;
  double kp_y = tank_point_params_g.kp_y;
  double kp_a =tank_point_params_g.kp_a;
  double kd_a = tank_point_params_g.kd_a;
  int timeout = tank_point_params_g.timeout;
  Point end_error = tank_point_params_g.end_error;
  double min_pwr_y = tank_point_params_g.min_pwr_y;
  //Pid angle(kp.a);
  
  tracking.move_complete = false;

  Point local_error;
  Position error;

  double total_pwr, max_pwr_scale;
  int sgn_local_error_y, orig_sgn_line_y, sgn_line_y;
  double difference_a;
  double hypotenuse;

  //double kp_y = 11.5, kp_a = 150.0, kd_a = 0.0;
  //printf2("Local errors | x: %f, y: %f ", local_error.x, local_error.y);
  double min_pwr_a = max_pwr * min_angle_percent;
  double pre_scaled_pwr_a;
  //double end_error = 0.5, end_error_x = 0.5;
  double end_dist;

  //double deriv_a = 0.0;
  PID angle(kp_a, 0.0, kd_a, 0.0);
  PID y_pid(kp_y, 0.0, 350.0, 0.0);

  //move on line variables
  Vector follow_line = Vector(tracking.coord(), target).invert(); //used to keep track of angle of follow_line relative to the vertical
  Vector line_disp(tracking.coord(), target);  //displacements relative to line
  int time = millis();    

  //MOTION FIX
  //global displacement of robot and target
  line_disp = Vector(tracking.coord(), target);
  hypotenuse = line_disp.get_magnitude(); //distance to target

  //rotates vector by line angle, now the vector represents the displacement RELATIVE TO THE LINE
  line_disp.rotate(follow_line.get_angle());
  orig_sgn_line_y = sgn(line_disp.get_y()); //used to calculate if the robot has overshot
  //END OF MOTION FIX

  int start_time = millis();
  bool last_state= false;
  motion_i.print("Starting tank move to point: %.2f at %.2f", rad_to_deg(target), tracking.coord());
  while(true){
    //global displacement of robot and target
    line_disp = Vector(tracking.coord(), target);
    hypotenuse = line_disp.get_magnitude(); //distance to target

    //rotates vector by line angle, now the vector represents the displacement RELATIVE TO THE LINE
    line_disp.rotate(follow_line.get_angle());
    sgn_line_y = sgn(line_disp.get_y()); //used to calculate if the robot has overshot

    //difference in angle between angle to target and robot's local y axis
    difference_a = tracking.global_angle() + Vector(tracking.coord(), target).get_angle();

    sgn_local_error_y = sgn(local_error.y);
    //printf2("sgn_y: %d", sgn_local_error_y);
    if(sgn_local_error_y == 0){
      sgn_local_error_y = turn_dir_if_0 ? 1 : -1;
      //printf2("triggered");
    }
    //orig_sgn_line_y = sgn_local_error_y;
    local_error.y = sin(difference_a) * hypotenuse; //local y displacement to target
    //chooses nearest angle to turn to face target (back or front)
    error.angle = near_angle(sgn_local_error_y * M_PI / 2, difference_a);
    //printf2("Errors | y: %f, a: %f", local_error.y, rad_to_deg(error.angle));

    end_dist = sqrt(pow(hypotenuse, 2) + pow(local_error.y, 2) - (2 * hypotenuse * local_error.y * cos(tracking.global_angle() - Vector(tracking.coord(), target).get_angle())));  
    motion_d.print("end_dist: %.2f, hypotenuse: %.2f, local %.2f, angle: %.2f", end_dist, hypotenuse, local_error.y, tracking.coord().angle - rad_to_deg(Vector(tracking.coord(), target).get_angle()));

    //tracking.pwr.y = kp_y * local_error.y;
    tracking.pwr.y = y_pid.compute(-local_error.y, 0.0);
    //tracking.pwr.y = kp_y * line_y_local_y;
    //tracking.pwr.angle = angle.compute(error.angle, 0.0);
    if(fabs(end_dist) > end_error.x && fabs(local_error.y) > 4.0 ){

      if(!last_state){
        last_state = true;
        master.print(0, 0, "correcting");
      }
      motion_d.print("correcting");
      tracking.pwr.angle = kp_a * error.angle;
    }
    else{
      if(last_state){
        last_state = false;
        master.print(0, 0, "vibing");
      }
      motion_d.print("vibing");
      tracking.pwr.angle = 0.0;
    }
    
    //gives min pwr to local y if that is not satisfied
    if (fabs(tracking.pwr.y) < min_pwr_y && fabs(local_error.y) > 0.5) tracking.pwr.y = min_pwr_y * sgn(local_error.y);

    //scales pwrs
    total_pwr = fabs(tracking.pwr.y) + fabs(tracking.pwr.angle);
    if (total_pwr > max_pwr){

      pre_scaled_pwr_a = tracking.pwr.angle;

      max_pwr_scale = max_pwr / total_pwr;
      tracking.pwr.y *= max_pwr_scale;
      tracking.pwr.angle *= max_pwr_scale;


      //start of angle pwr guarantee
      if (fabs(pre_scaled_pwr_a) > min_pwr_a){
        if (fabs(tracking.pwr.angle) < min_pwr_a)  tracking.pwr.angle = min_pwr_a * sgn(tracking.pwr.angle);  //pwr.angle has been overshadowed
      }
      //angle gets the pwr it demanded if pre_scaled pwr.angle was also less than min_pwr_a
      else tracking.pwr.angle = pre_scaled_pwr_a;
      tracking.pwr.y = sgn(local_error.y) * (max_pwr - fabs(tracking.pwr.angle));
      //printf2("****pwr.angle: %f, x:%f, y: %f, pre_scaled: %f, min: %f", tracking.pwr.angle, tracking.pwr.x , tracking.pwr.y, pre_scaled_pwr_a, min_pwr_a);
      //end of angle pwr guarantee

    }
    //printf2("pwrs | y: %f, a: %f", tracking.pwr.y, tracking.pwr.angle);

    motion_d.print("error y : %.2f error a : %.2f, end con: %.2f, end clause %.2f, end dist: %.2f, pow y : %.2f, pow a : %.2f", local_error.y, rad_to_deg(error.angle), fabs(line_disp.get_y()), end_error.y, end_dist, tracking.pwr.y, tracking.pwr.angle);
    motion_d.print("sgn: %d, orig sgn: %d", sgn_line_y, orig_sgn_line_y);
    graph.print("%d, %f\n", millis() - time, tracking.l_velo);
    //exits movement once the target has been overshot (if the sign of y error along the line has flipped)
    //if(fabs(line_disp.get_y()) < end_error.y || sgn_line_y != orig_sgn_line_y){
    if((fabs(local_error.y) < end_error.y && fabs(line_disp.get_y()) < 4) || sgn_line_y != orig_sgn_line_y){
      if (brake) drivebase.brake();
      tracking.move_complete = true;
      motion_i.print("Ending move to target: %.2f at %.2f", rad_to_deg(target), tracking.coord());
      //tracking.move_stop_task();
      break;
    }
    if(timeout != 0 && millis() - start_time > timeout){
      if (brake) drivebase.brake();
      tracking.move_complete = true;
      motion_i.print("TIME OUT move to target: %.2f at %.2f | time: %d", rad_to_deg(target), tracking.coord(), millis() - time);
      break;
    }
    drivebase.move(tracking.pwr.y, tracking.pwr.angle);
    
    if(ptr->notify_handle()) return;
    delay(10);
  }
}

void turn_to_angle(void* params){
  _Task* ptr = _Task::get_obj(params);
  //turn_angle_params* param_ptr = static_cast<turn_angle_params*>(_Task::get_params(params));
  double target_a = turn_angle_params_g.target_a;
  bool brake = turn_angle_params_g.brake;
  bool near = turn_angle_params_g.near;
  double kp = turn_angle_params_g.kp;
  double kd = turn_angle_params_g.kd;
  double kp_v = turn_angle_params_g.kp_v;
  double kd_v = turn_angle_params_g.kd_v;
  double max_speed = turn_angle_params_g.max_speed;
  int timeout = turn_angle_params_g.timeout;
  double min_pwr_a = turn_angle_params_g.min_pwr_a;
  double end_error = turn_angle_params_g.end_error;
  tracking.move_complete = false;

  PID angle_pid(kp, 0.0, kd, 0.0, true, 0.0, 360.0);
  PID angle_velocity_pid(kp_v, 0.0, kd_v, 0.0, true, 0.0, 360.0);
  double target_velocity;
  double kB = 17.2; //ratio of motor pwr to target velocity (in radians) i.e. multiply vel by this to get motor pwr

  motion_i.print("Starting turn to angle %.2f from %.2f", target_a, tracking.coord().angle);
  target_a = deg_to_rad(target_a);
  double new_target_a;
  double pwr;
  if(near){ //picks the closest angle to turn to
    new_target_a = near_angle(target_a, tracking.global_angle()) + tracking.global_angle();
  }
  else { //otherwise pick the angle passed in
    new_target_a = target_a;
  }
  int start_time = millis();
  while(true){
    target_velocity = angle_pid.compute(tracking.global_angle(), new_target_a);
    //target_velocity = new_target_a - tracking.position.angle;

    pwr = kB * target_velocity + angle_velocity_pid.compute(tracking.g_velocity.angle, target_velocity); 
    motion_d.print("target_velocity[deg]: %f, acc vel: %f, vel_diff: %f, pwr:%f", rad_to_deg(target_velocity), rad_to_deg(tracking.g_velocity.angle), rad_to_deg(target_velocity - tracking.g_velocity.angle), pwr);
    if(fabs(pwr) > max_speed) pwr = max_speed*sgn(pwr);
    if(fabs(pwr) <min_pwr_a) pwr = sgn(pwr) * min_pwr_a;
    drivebase.move(0, pwr);
    if(fabs(rad_to_deg(angle_pid.get_error())) < end_error){
      motion_i.print("Ending turn to angle: %.2f at %.2f, time: %d", rad_to_deg(target_a), tracking.coord(), millis() - start_time);
      drivebase.move(0, 0);
      if(brake) drivebase.brake();
      tracking.move_complete = true;
      return;
    }
    if(timeout != 0 && millis() - start_time > timeout){
      motion_i.print("TIMED OUT Ending turn to angle: %.2f at %.2f, time: %d", rad_to_deg(target_a), tracking.coord(), millis() - start_time);
      drivebase.move(0, 0);
      if(brake) drivebase.brake();
      tracking.move_complete = true;
      return;
    }
    if(ptr->notify_handle()) return;
    delay(10);
  }
}

//overload to handle being called within another motion
void turn_to_angle(double target_a, const bool brake, double max_pwr, int timeout, _Task* ptr){
  PID angle_pid(150.0, 0.0, 80.0, 0.0, true, 0.0, 360.0);
  target_a = deg_to_rad(target_a);
  tracking.move_complete = false;
  double pwr;
  int start_time = millis();
  while(true){
    pwr = angle_pid.compute(tracking.global_angle(), near_angle(target_a, tracking.global_angle()) + tracking.global_angle());
    if(fabs(pwr)  > max_pwr) pwr = sgn(pwr) * max_pwr;
    if(fabs(pwr) < min_move_pwr_a) pwr = sgn(pwr) * min_move_pwr_a;
    drivebase.move(0, pwr);
    if(fabs(rad_to_deg(angle_pid.get_error())) < 5.0){
      motion_i.print("Ending turn to point with angle: %.2f at %.2f", rad_to_deg(target_a), tracking.coord());
      drivebase.move(0, 0);
      if(brake) drivebase.brake();
      tracking.move_complete = true;
      return;
    }
    if(timeout != 0 && millis() - start_time > timeout){
      motion_i.print(" TIMED OUT turn to point with angle: %.2f at %.2f", rad_to_deg(target_a), tracking.coord());
      drivebase.move(0, 0);
      if(brake) drivebase.brake();
      tracking.move_complete = true;
      return;
    }
    if(ptr->notify_handle()) return;
    delay(10);
  }

}


void turn_to_point(void* params){
  _Task* ptr = _Task::get_obj(params);
  //turn_point_params* param_ptr = static_cast<turn_point_params*>(_Task::get_params(params));
  const Point target = turn_point_params_g.target;
  const bool brake = turn_point_params_g.brake;
  double max_pwr = turn_point_params_g.max_pwr;
  int timeout = turn_point_params_g.timeout;
  motion_i.print("starting turn to point %.2f from %.2f target angle: %.2f", target, tracking.coord(), rad_to_deg(Vector(tracking.coord(), target).get_angle()));
  turn_to_angle(rad_to_deg(Vector(tracking.coord(), target).get_angle()), brake, max_pwr, timeout, ptr);
  motion_i.print("Ending turn to point function ending");
}

void tank_move_on_arc(void* params){
  _Task* ptr = _Task::get_obj(params);
  //tank_arc_params* param_ptr = static_cast<tank_arc_params*>(_Task::get_params(params));
  const Point start_pos = tank_arc_params_g.start_pos;
  Position target = tank_arc_params_g.target;
  const double pwr = tank_arc_params_g.pwr, max_pwr = tank_arc_params_g.max_pwr;
  const bool brake = tank_arc_params_g.brake;
  double decel_start = tank_arc_params_g.decel_start, decel_end = tank_arc_params_g.decel_end, decel_target_speed = tank_arc_params_g.decel_target_speed;
  tracking.move_complete = false;

  Vector p2 (start_pos), p1 (target);  //p2 and p1 are switched because Nikhil messed up the math

  Vector disp (p2 - p1);
  target = deg_to_rad(target);
  disp.rotate(target.angle);  //rotates displacement by target's angle to obtain right triangles from isosceles triangle forming the arc
  double theta = 2.0 * atan2(disp.get_x(), disp.get_y()); //'span' of the arc (in radians)
  double radius = disp.get_magnitude() / 2.0 / sin(theta / 2.0);
  //printf2("radius: %f", radius);
  //printf2("theta: %f", rad_to_deg(theta));
  Point centre = {p1.get_x() + cos(target.angle) * radius, p1.get_y() - sin(target.angle) * radius};
  //printf2("centre | x: %f, y: %f", centre.x, centre.y);
  //finished calculating centre coord

  //init variables
  Point g_position;
  Vector arc_velocity;
  double linear_velocity, angular_velocity_target;
  double total_pwr, max_pwr_scale;
  //robot travels at 220 degrees / second while pwr is 127; kp = 175
  //from pwr to velocity * 1.6
  //from velocity to pwr / 1.6 or * 0.625
  //const double pwm_to_vel = 1.6;
  constexpr double pwm_to_vel = 0.05814;
  constexpr double kR = 30.0; //multiplier on radial error

  constexpr double kA = pwm_to_vel * 90.0, kB = 1 / pwm_to_vel, kP = 50.0, kD = 30.0;
  const double final_angle = Vector(centre, target).get_angle(); //angle of final position to centre at end of move

  uint32_t last_d_update_time = millis();  //for derivative
  double last_angular_velocity = tracking.g_velocity.angle; //for derivative
  int orig_angle_error_sgn = sgn(target.angle - tracking.global_angle());
  double angle_diff;
  double radial_diff;
  radius = fabs(radius);
  //positive turn_dir means cw movement about the arc
  int turn_dir = -sgn(rad_to_deg(near_angle(Vector(centre, target).get_angle(), Vector(centre, tracking.coord()).get_angle())));
  //printf2("turn_dir: %d", turn_dir);
  double dist;


  motion_d.print("Starting tank move on arc: %.2f from %.2f on arc with r: %.2f and centre: %.2f", target, tracking.coord(), radius, centre);
  while(true){
    g_position = tracking.coord();
    disp = Vector(centre, g_position); //global displacement of robot and arc's centra
    //printf2("local_a: %f", rad_to_deg(disp.get_angle()));

    //global velocity of the robot
    arc_velocity.set_cartesian(tracking.g_velocity);
    //veloicty of the robot relative to the arc
    arc_velocity.rotate(-disp.get_angle() + (turn_dir == -1 ? 0 : M_PI));

    //difference between target angle and current angle taking into account direction of movement and sign of pwr
    angle_diff = near_angle(-disp.get_angle() + (turn_dir == -1 ? 0 : M_PI), tracking.global_angle() + (pwr < 0.0 ? M_PI : 0));

    //linear velocity along the arc is the y component of the robot's velocity relative to the arc
    linear_velocity = arc_velocity.get_y();

    radial_diff = disp.get_magnitude() / radius; //ratio of actual radius to target radius
    //signs radial diff
    if (radial_diff < 1) radial_diff = -1 / radial_diff + 1;
    else radial_diff = radial_diff -1;
    //target angular velocity (for use in a cascading PID)
    angular_velocity_target = turn_dir * (linear_velocity / disp.get_magnitude() + kR * radial_diff) + kA * angle_diff;
    //printf2("radius:%f, angular_vel: %f, radial_error: %f, radial_correction: %f, angle_diff: %f, angle_correction: %f", radius, kB * turn_dir * linear_velocity / disp.get_magnitude(), radius - disp.get_magnitude(), kB * turn_dir * kR * log(disp.get_magnitude() / radius), rad_to_deg(angle_diff), kB * kA * angle_diff);
    if(!xy_enable){
      //radial ratio
      printf2("radius:%f, angular_vel: %f, radial_error: %f, radial_correction: %f, angle_diff: %f, angle_correction: %f", radius, kB * turn_dir * linear_velocity / disp.get_magnitude(), radius - disp.get_magnitude(), kB * turn_dir * kR * log(disp.get_magnitude() / radius), rad_to_deg(angle_diff), kB * kA * angle_diff);
      //radial diff
      //printf2("radius:%f, disp_mag: %f, angular_vel: %f, radial_error: %f, radial_correction: %f, angle_diff: %f, angle_correction: %f", radius, disp.get_magnitude(), kB * turn_dir * linear_velocity / disp.get_magnitude(),  turn_dir * (disp.get_magnitude() - radius), kB * turn_dir * kR * (disp.get_magnitude() - radius), rad_to_deg(angle_diff), kB * kA * angle_diff);
    }
    //output of cascading PID (the error being the difference in target velocity and actual velocity)
    tracking.pwr.angle = kB * angular_velocity_target + kP * (angular_velocity_target - tracking.g_velocity.angle) + kD * (tracking.g_velocity.angle - last_angular_velocity) / (millis() - last_d_update_time);

    //printf2("p:%f, d:%f",  kP * (angular_velocity_target - tracking.g_velocity.angle), kD * (tracking.g_velocity.angle - last_angular_velocity));
    //used for derivative
    last_d_update_time = millis();
    last_angular_velocity = tracking.g_velocity.angle;
    dist = fabs(near_angle(Vector(centre, target).get_angle(), Vector(centre, tracking.coord()).get_angle())) * radius;

    tracking.pwr.y = sgn(pwr) * map_set(dist, 0.0, std::numeric_limits<double>::max(), static_cast<double>(min_move_pwr_y), fabs(pwr),
      decel_end, decel_target_speed,
      decel_start, fabs(pwr), 
      std::numeric_limits<double>::max(), fabs(pwr)
    );
    //printf("Angular displacement velocity: %f", rad_to_deg(tracking.position.angle + disp.get_angle()));
    //local y pwr is constant, whatever the user supplied the function
    //tracking.pwr.y = pwr;
    //pwr scaling
    total_pwr = fabs(tracking.pwr.y) + fabs(tracking.pwr.angle);
    if (total_pwr > max_pwr){
      max_pwr_scale = max_pwr / total_pwr;
      tracking.pwr.y *= max_pwr_scale;
      tracking.pwr.angle *= max_pwr_scale;
    }
    motion_d.print("ARC pwr: y: %.2f, a: %.2f", tracking.pwr.y, tracking.pwr.angle);
    //exits movement after arc angle becomes less than 1 degree
    if (rad_to_deg(fabs(near_angle(final_angle, Vector(centre, tracking.coord()).get_angle()))) < 1.0){

      //printf2("ang: %f", Vector(centre, tracking.coord()).get_angle());
      drivebase.move(0.0, 0.0);
      if (brake) drivebase.brake();
      tracking.move_complete = true;
      motion_d.print("Ending move on arc to target X: %.2f at %.2f ", target, tracking.coord());
      return;
    }
    if(xy_enable) printf2("%f", tracking.coord());
    else printf2("pwr.y: %f, pwr.angle: %f", tracking.pwr.y, tracking.pwr.angle);
    drivebase.move(tracking.pwr.y, tracking.pwr.angle);
    if(ptr->notify_handle()) return;
    delay(10);
  }
}


void move_on_line_old(void* params){
  _Task* ptr = _Task::get_obj(params);
  //line_old_params* param_ptr = static_cast<line_old_params*>(_Task::get_params(params));
  double start_x = line_old_params_g.start_x, start_y = line_old_params_g.start_y;
  double target_x = line_old_params_g.target_x, target_y = line_old_params_g.target_y, target_a = deg_to_rad(line_old_params_g.target_a);
  double end_error = line_old_params_g.end_error, end_error_a = line_old_params_g.end_error_a;
  bool Brake = line_old_params_g.Brake, debug = line_old_params_g.debug;
  int max_pwr = line_old_params_g.max_pwr;
  bool Overshoot = line_old_params_g.Overshoot; //
  double lpercent= line_old_params_g.lpercent, apercent=line_old_params_g.apercent;
  //ki ly doesnt work but im lazy so fuck you
  double kp_lx =23.0, kp_ly =15.0, kp_dx = 23.0, kp_dy = 15.0, kp_a = 125.0, ki_lx = 0.0, ki_ly = 0.0, ki_dx = 0.0, ki_dy = 0.0, ki_a = 0.0, kd_lx = 0.0, kd_ly = 0.0, kd_dx = 0.0, kd_dy = 0.0, kd_a = 1000.0;
  double error_angle, error_line, error_d, error_x, error_y, error_a , error_tot;
  double pwr_line_x, pwr_line_y, pwr_d_x, pwr_d_y, total_pwr, line_total;
  double dif_a;
  double min_lx, min_ly, min_a, min_total;
  double integ_start = 5.0, integ_l, integ_d, integ_a;
  double deriv_l, deriv_d, deriv_a, last_error_l, last_error_d, last_error_a;
  uint32_t integ_last_time = millis(), deriv_time = millis(), start_time = millis();
  int last_time = 0;
  motion_i.print("Starting move to target : %.2f at %.2f", Position{target_x, target_y, target_a}, tracking.coord());

  tracking.move_complete = false;
  tracking.movetime = millis();
  double start_error_x = target_x - start_x, start_error_y = target_y - start_y;
  double d_angle = atan2(start_error_x, start_error_y);


  double orig_point_angle = atan2(error_x, error_y);
  double cur_point_angle;
  int perpendicular = M_PI / 2;

  while(true){
    error_x = target_x - tracking.coord().x;
    error_y = target_y - tracking.coord().y;
    error_a = target_a - tracking.global_angle();
    error_tot = Vector::make_cartesian(error_x, error_y).get_magnitude();

    error_angle = atan2(error_x, error_y) - d_angle;
    error_line = sin(error_angle) * error_tot;
    error_d = cos(error_angle) * error_tot;
    dif_a = d_angle - tracking.global_angle();


    if (millis() - integ_last_time > 50){
      if(fabs(error_line) < integ_start) integ_l += fabs(error_line) * (millis() - integ_last_time);
      if(fabs(error_d) <integ_start) integ_d += fabs(error_d) * (millis() - integ_last_time);


      if(fabs(error_line) <=0.5) integ_l = 0.0;
      if(fabs(error_d) <= 0.5) integ_d = 0.0;

      integ_last_time = millis();
    }

    if(millis() - deriv_time > 0){
      deriv_l = (error_line-last_error_l) / (millis() - deriv_time);
      deriv_d = (error_d-last_error_d) / (millis() - deriv_time);
      deriv_a = (error_a-last_error_a) / (millis() - deriv_time);

      last_error_l = error_line;
      last_error_d = error_d;
      last_error_a = error_a;
      deriv_time = millis();
    }

    pwr_line_x = cos(dif_a) * error_line * kp_lx;
    pwr_line_y = -sin(dif_a) * error_line * kp_ly;
    pwr_d_x = sin(dif_a) * error_d * kp_dx;
    pwr_d_y = cos(dif_a) * error_d * kp_dy;
    tracking.pwr.angle = error_a * kp_a;

    pwr_line_x += cos(dif_a) *integ_l  * ki_lx;
    pwr_line_y += -sin(dif_a) *integ_l * ki_ly;
    pwr_d_x += sin(dif_a) * integ_d * ki_dx;
    pwr_d_y += cos(dif_a) * integ_d * ki_dy;

    pwr_line_x += cos(dif_a) * deriv_l * kd_lx;
    pwr_line_y += -sin(dif_a) * deriv_l * kd_ly;
    pwr_d_x += sin(dif_a) * deriv_d * kd_dx;
    pwr_d_y += cos(dif_a) * deriv_d * kd_dy;
    tracking.pwr.angle += deriv_a*kd_a;

    if(millis() - last_time > 20){
      motion_i.print("pre sc lx: %f, ly: %f, dx: %f, dy: %f, a: %f", pwr_line_x, pwr_line_y, pwr_d_x, pwr_d_y, tracking.pwr.angle);
    }

    total_pwr = fabs(pwr_line_x + pwr_d_x) + fabs(pwr_line_y + pwr_d_y) + fabs(tracking.pwr.angle);
    if(total_pwr > max_pwr){
      line_total = fabs(pwr_line_x) + fabs(pwr_line_y);
      if(line_total > lpercent*max_pwr){
        min_lx = (fabs(pwr_line_x) / line_total) * lpercent*max_pwr;
        min_ly = (fabs(pwr_line_y) / line_total) * lpercent*max_pwr;
      }
      else{
        min_lx = fabs(pwr_line_x);
        min_ly = fabs(pwr_line_y);
      }
      if(fabs(tracking.pwr.angle) > apercent*max_pwr) min_a = apercent*max_pwr;
      else min_a = fabs(tracking.pwr.angle);
      min_total = min_lx + min_ly + min_a;
      pwr_line_x =(((fabs(pwr_line_x) - min_lx) / (total_pwr-min_total)) * (max_pwr-min_total) + min_lx) *sgn(pwr_line_x);
      pwr_line_y =(((fabs(pwr_line_y) - min_ly) / (total_pwr-min_total)) * (max_pwr-min_total) + min_ly) *sgn(pwr_line_y);
      pwr_d_x =(pwr_d_x / (total_pwr-min_total)) * (max_pwr-min_total);
      pwr_d_y =(pwr_d_y / (total_pwr-min_total)) * (max_pwr-min_total);
      tracking.pwr.angle =(((fabs(tracking.pwr.angle) -  min_a) / (total_pwr-min_total)) * (max_pwr-min_total) + min_a) *sgn(tracking.pwr.angle);
    }


    tracking.pwr.x = pwr_line_x + pwr_d_x;
    if(fabs(error_x) < end_error - 0.2) tracking.pwr.x = 0.0;
    else if(fabs(tracking.pwr.x) < min_move_pwr_x) tracking.pwr.x = sgn(tracking.pwr.x) * min_move_pwr_x;
    tracking.pwr.y = pwr_line_y + pwr_d_y;
    if(fabs(error_y) < end_error - 0.2) tracking.pwr.y = 0.0;
    else if(fabs(tracking.pwr.y) < min_move_pwr_y) tracking.pwr.y = sgn(tracking.pwr.y) * min_move_pwr_y;
    if(fabs(rad_to_deg(error_a)) < end_error_a - 1.0) tracking.pwr.angle = 0.0;
    else if(fabs(tracking.pwr.angle) < min_move_pwr_a) tracking.pwr.angle = sgn(tracking.pwr.angle) * min_move_pwr_a;


    drivebase.move(tracking.pwr);

    if(millis() - last_time > 20 && debug){
      motion_d.print("errors x: %.2f, y: %.2f, a: %.2f", error_x, error_y, rad_to_deg(error_a));
      motion_d.print("errors l: %.2f, d: %.2f", error_line, error_d);
      motion_d.print("pwr: %.2f", tracking.pwr);
      motion_d.print("post s lx: %f, ly: %f, dx: %f, dy: %f, a: %f", pwr_line_x, pwr_line_y, pwr_d_x, pwr_d_y, tracking.pwr.angle);
      if(lpercent || apercent) motion_d.print("mins  lx: %.2f, ly: %.2f, a: %.2f", min_lx, min_ly, min_a);
      if(ki_lx || ki_ly || ki_dx || ki_dy || ki_a) motion_d.print("integ  l: %.2f, d: %.2f", integ_l, integ_d);
      if(kd_lx || kd_ly || kd_dx || kd_dy || kd_a) motion_d.print("deriv  l: %.2f, d: %.2f a: %.2f", deriv_l, deriv_d, deriv_a);
      last_time = millis();
    }

    if(fabs(error_x) < end_error && fabs(error_y) < end_error && fabs(rad_to_deg(error_a)) < end_error_a){
      drivebase.brake();
      tracking.move_complete = true;
      motion_i.print("Ending move to target %f at %f, time: %d ", Position{target_x, target_y, target_a}, tracking.coord(), millis() - start_time);
      return;
    }

    if(ptr->notify_handle()) return;
    delay(10);
  }
}







//Gyro balance class

//Constructor
Gyro::Gyro(Imu& imu): inertial(imu){}

//Updates last / cur angle
double Gyro::get_angle(){
  angle = gyro_side*inertial.GYRO_AXIS();
  return fabs(angle);
}

//Updates last / cur angle
double Gyro::get_angle_dif(){
  double da = angle - last_angle;
  int dt = millis() - time;
  time = millis();
  last_angle = angle;
  return da / dt;
}

void Gyro::calibrate(){
  inertial.reset();
}

void Gyro::finish_calibrating(){
  if(inertial.is_calibrating()){
    wait_until(!inertial.is_calibrating()) motion_d.print("Calibrating Gyro...");
    motion_i.print("Done Calibrating Gyro");
  }
  else motion_i.print("Already Calibrated Gyro");
}

void Gyro::climb_ramp(){
  drivebase.set_state(HIGH);
  finish_calibrating(); //Makes sure it's calibrated before starting (should already be)
  inertial.tare_roll();
  inertial.tare_pitch();

  drivebase.move(-gyro_side*127, 0.0);
  Task([this](){
    wait_until(false){
      get_angle();
      //motion_i.print("Angle:%f Angle_v:%f, L_v:%f, R_v:%f, Dist:%f", angle, get_angle_dif(), tracking.l_velo, tracking.r_velo, tracking.position.x);
      //motion_i.print("Angle:%f", angle);
    }
  });
  b_lift.set_state(b_lift_states::move_to_target, 2);
  wait_until(angle > 22);
  drivebase.brake();
  f_lift.set_state(f_lift_states::move_to_target, 1, 100);
  motion_i.print("ON RAMP: %f", angle);
  screen_flash::start("On Ramp", term_colours::NOTIF);
  //master.wait_for_press(DIGITAL_R1);

  tracking.reset();

  drivebase.move(-gyro_side*90, 0.0); //Can probably get rid of this
  tracking.wait_for_dist(18);
}

void Gyro::level(double kP, double kD){
	PID gyro_p(kP, 0, kD, 0);
  Timer gyro_steady ("Gyro", &motion_i);
  double speed;
  bool lowered = false;
  bool neg = false;
  constexpr double min_pwr = 30.0;

  screen_flash::start("PID", term_colours::NOTIF);

	while(!(gyro_steady.get_time() > 500 || master.interrupt(true, true))){
    speed = gyro_p.compute(-angle, 0);

    if(!lowered && fabs(angle) < 21.0){
      f_lift.set_state(f_lift_states::move_to_target, 0);
      lowered = true;
    }
    if(!neg && in_range(speed, 10.0, min_pwr)) speed = min_pwr*sgn(speed);
    if(speed < 0) neg = true;
    if(in_range(fabs(speed), 0.0, 10.0)) speed = 0;

    drivebase.move(speed, 0.0);
    motion_i.print("In:%.3f Sens:%.3f Out:%.3f, Speed:%.3f, P:%.3f, D:%.3f", -angle, gyro.inertial.GYRO_AXIS(), gyro_p.get_output(), speed, gyro_p.proportional, gyro_p.derivative);
    drivebase.move(speed, 0.0);
    
		if (fabs(angle) > 6 || get_angle_dif() > 0.06) gyro_steady.reset();
    delay(11);
  }

	motion_i.print("\nLevelled on ramp\n");
  front_l.set_brake_mode(E_MOTOR_BRAKE_HOLD);
  front_r.set_brake_mode(E_MOTOR_BRAKE_HOLD);
  back_l.set_brake_mode(E_MOTOR_BRAKE_HOLD);
  back_r.set_brake_mode(E_MOTOR_BRAKE_HOLD);
  center_l.set_brake_mode(E_MOTOR_BRAKE_HOLD);
  center_r.set_brake_mode(E_MOTOR_BRAKE_HOLD);
  drivebase.velo_brake();

  screen_flash::start("Braked", term_colours::NOTIF);
}