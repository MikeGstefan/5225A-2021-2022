#include "waypoint.hpp"

// tasks
Task *waypointTask = (nullptr);

// Coord constructor definition
Coord::Coord(double x, double y, double angle): x(x), y(y), angle(angle){}
Coord::Coord(): x(0.0), y(0.0), angle(0.0){}

// task manipulators
void waypointStartTask(const waypoint_target_params& waypoint_params){
    waypointTask = new Task(move_through_waypoints, new waypoint_target_params(waypoint_params));
}

void waypointStopTask(){
  if(waypointTask != nullptr){
    waypointTask->remove();
    delete waypointTask;
    waypointTask = nullptr;
  }
}

long waypoint::total_movement_time;
bool waypoint::movement_complete;
double waypoint::total_error_d, waypoint::error_d_next_waypoint, waypoint::total_error_x, waypoint::total_error_y;
int waypoint::next_waypoint, waypoint::path_size;

// move functions
void move_through_waypoints_sync(vector<waypoint> path, bool debug, brake_types brake_type, bool full_speed, int max_power){
  if(!waypoint::movement_complete) waypointStopTask();
  if(waypointTask != nullptr) waypointTask = nullptr;
  waypoint_target_params* waypoint_params = new waypoint_target_params{std::move(path), debug, brake_type, full_speed, max_power};

  waypoint::movement_complete = false;
  move_through_waypoints(waypoint_params);
}

void move_through_waypoints_async(vector<waypoint> path, bool debug, brake_types brake_type, bool full_speed, int max_power){
  if(!waypoint::movement_complete) waypointStopTask();
  waypoint_target_params waypoint_params{std::move(path),  debug, brake_type, full_speed, max_power};
  waypoint::movement_complete = false;
  printf("async task started %d \n", millis());
  waypointStartTask(waypoint_params);
  delay(10);
}

// waypoint class methods

// waypoint constructors
waypoint::waypoint(Coord coord, double end_error, double end_error_a, bool overshoot, bool angle_on_path, double min_power_a, double min_power_line_x):
    coord(coord), end_error(end_error), end_error_a(end_error_a), overshoot(overshoot), angle_on_path(angle_on_path), min_power_a(min_power_a)
    // min_power_x_line(min_power_x_line);
{}
waypoint::waypoint():   // constructor with ALL default args
    coord(0.0, 0.0, 0.0), end_error(0.5), end_error_a(5.0), overshoot(true), angle_on_path(true), min_power_a(20)
    // min_power_x_line(min_power_x_line);
{}

// getters and setters for private members
double waypoint::get_error_x() const{
    return this-> error_x;
}
void waypoint::set_error_x(double error_x){
    this-> error_x = error_x;
}
double waypoint::get_error_y() const{
    return this-> error_y;
}
void waypoint::set_error_y(double error_y){
    this-> error_y = error_y;
}
double waypoint:: get_error_d() const{
    return this-> error_d;
}
void waypoint::set_error_d(double error_d){
    this-> error_d = error_d;
}

double waypoint::get_move_time() const{
    return this-> move_time;
}

void waypoint::set_move_time(double time){
    this-> move_time = time;
}

double waypoint:: get_local_x() const{
    return this-> local_x;
}

void waypoint::set_local_x(double error_x){
    this-> local_x = error_x;
}

double waypoint:: get_local_y() const{
    return this-> error_y;
}

void waypoint::set_local_y(double local_y){
    this-> local_y = local_y;
}

double waypoint:: get_d_angle() const{
    return this-> d_angle;
}

void waypoint::set_d_angle(double d_angle){
    this-> d_angle = d_angle;
}
// WAYPOINT SPECIFIC FUNCTIONS

/*  //  This function is UNTESTED, it is currently not called. Work must be done later on this
void set_offsets(vector<waypoint>& path){
    double x_dif, y_dif, hypotenuse, theta;
    // original position, before movement starts
    waypoint orig_pos{{tracking.x_coord, tracking.y_coord, tracking.global_angle}};
    waypoint prev_waypoint{{}};

    for (int next_waypoint = 0; next_waypoint < path.size(); next_waypoint++){
        // makes prev_waypoint = last waypoint if next_waypoint is not 0, otherwise makes it orig_pos
        prev_waypoint = next_waypoint? path[next_waypoint - 1] : orig_pos;
        x_dif = path[next_waypoint].coord.x - prev_waypoint.coord.x;
        y_dif = path[next_waypoint].coord.y - prev_waypoint.coord.y;
        theta = atan2(y_dif, x_dif);
        hypotenuse = sqrt(pow(x_dif, 2.0) + pow(y_dif, 2.0));
        // path[next_waypoint].coord.angle = M_PI/2 - theta;
        path[next_waypoint].coord.x =  prev_waypoint.coord.x + (hypotenuse + path[next_waypoint].offset) * cos(theta);
        path[next_waypoint].coord.y = prev_waypoint.coord.y + (hypotenuse + path[next_waypoint].offset) * sin(theta);
    }
}
*/
void set_errors(vector<waypoint>& path){
  for (int waypoint = 1; waypoint < path.size(); waypoint++){
    path[waypoint].set_error_x(path[waypoint].coord.x - path[waypoint - 1].coord.x);
    path[waypoint].set_error_y(path[waypoint].coord.y - path[waypoint - 1].coord.y);
    path[waypoint].set_error_d(sqrt(pow(path[waypoint].get_error_x(), 2.0) + pow(path[waypoint].get_error_y(), 2.0)));
    path[waypoint].set_d_angle(atan2(path[waypoint].get_error_x(),path[waypoint].get_error_y())-path[waypoint-1].coord.angle);
    path[waypoint].set_local_x(path[waypoint].get_error_d() * sin(path[waypoint].get_d_angle()));
    path[waypoint].set_local_y(path[waypoint].get_error_d() * cos(path[waypoint].get_d_angle()));
  }
}


double set_error_d_waypoints_left(vector<waypoint>& path){
    double error_d_waypoints_left;
    for (int waypoint = 1; waypoint < path.size(); waypoint++)error_d_waypoints_left += path[waypoint].get_error_d();

    return error_d_waypoints_left;
}

double set_error_x_waypoints_left(vector<waypoint>& path){
    double error_x_waypoints_left;
    for (int waypoint = 1; waypoint < path.size(); waypoint++)error_x_waypoints_left += path[waypoint].get_local_x();
    return error_x_waypoints_left;
}

double set_error_y_waypoints_left(vector<waypoint>& path){
  double error_y_waypoints_left;
  for (int waypoint = 1; waypoint < path.size(); waypoint++)error_y_waypoints_left += path[waypoint].get_local_y();
  return error_y_waypoints_left;
}

void set_angles(vector<waypoint>& path){
    double x_dif, y_dif;
    // original position, before movement starts
    waypoint orig_pos{{tracking.x_coord, tracking.y_coord, tracking.global_angle}};
    waypoint prev_waypoint{{}};

    for (int next_waypoint = 0; next_waypoint < path.size(); next_waypoint++){
        // makes prev_waypoint =  last waypoint if next_waypoint is not 0, otherwise makes it orig_pos
        prev_waypoint = next_waypoint? path[next_waypoint - 1] : orig_pos;
        if (path[next_waypoint].angle_on_path){  // if global_angle (of waypoint) needs to be altered
            x_dif = path[next_waypoint].coord.x - prev_waypoint.coord.x;
            y_dif = path[next_waypoint].coord.y - prev_waypoint.coord.y;
            path[next_waypoint].coord.angle = M_PI/2 - atan2(y_dif, x_dif);
        }
        else    path[next_waypoint].coord.angle = deg_to_rad(path[next_waypoint].coord.angle);
    }
}

void waypoint::wait_for_complete(){
    while(!waypoint::movement_complete) delay(10);
}

void waypoint::wait_for_distance_from_waypoint(double distance, int waypoint){
    if (waypoint < waypoint::path_size){
        distance = fabs(distance); // makes abs in case user is dumb
        while(waypoint::next_waypoint < waypoint) delay(10); // wait until we hit the previous waypoint
        printf("\n\nWAIT FOR WAYPOINT DONE LOOP 1 \n\n");
        delay(50); // waits for next_waypoint to update DO NOT TAKE OUT
        while(waypoint::error_d_next_waypoint > distance) {
          printf("%d, err: %f, d %f\n", millis(),waypoint::error_d_next_waypoint, distance );
          delay(10); // waits for distance away from waypoint
        }
        printf("\n\nWAIT FOR WAYPOINT DONE LOOP 2 \n\n");
    }
    else    printf("\n\n You requested waiting on a waypoint that is more than the max length.\n\n");
}

void waypoint::wait_for_waypoint(int waypoint){
    if (waypoint < waypoint::path_size){
        while(waypoint::next_waypoint <= waypoint) delay(10);
    }
    else    printf("\n\n You requested waiting on a waypoint that is more than the max length.\n\n");
}

void move_through_waypoints(void* waypoint_params){

    waypoint_target_params* params = (waypoint_target_params*)waypoint_params;
    vector<waypoint> path{std::move(params->path)};
    bool debug = params->debug;
    brake_types brake_type = params->brake_type;
    bool full_speed = params->full_speed;
    int max_power = params->max_power;
    delete params;

    // creates shorter aliases for static waypoint variables
    long& total_movement_time = waypoint::total_movement_time;
    bool& movement_complete = waypoint::movement_complete;
    double& total_error_d = waypoint::total_error_d;
    int& next_waypoint = waypoint::next_waypoint;
    int& path_size = waypoint::path_size;
    double& error_d_next_waypoint = waypoint::error_d_next_waypoint;  // creates alias that updates by VALUE every cycle

    double pre_scaled_power_d, pre_scaled_power_a, scale, pre_scaled_power_x, pre_scaled_power_y;

    double error_a = 0.0, error_d_waypoints_left, local_x_waypoints_left, local_y_waypoints_left, orig_pos_angle, cur_pos_angle;
    next_waypoint = 0;
    path_size = path.size();

    double local_error_x = 0.0, local_error_y = 0.0, difference_a = 0.0;
    double kp = 7.5, kp_a = 125.0, kI_d = 0.00;
    int lasttime = 0, starttime = millis(), integral_last_time = millis(), last_waypoint_time = millis();
    double total_power;
    double integral_x, integral_y;
    double brake_hypotenuse, brake_theta, brake_scalar = 10.0, arrival_time, brake_time_threshold = 0.100;    // variables concerning powered brake code
    double arrival_time_angle, brake_scalar_angle = 10.0;  // variables concerning angle brake code
    // //log_time("starting time: %d X: %f Y: %f A: %f from X: %f Y: %f A: %f \n", millis(), target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
    double last_pow_x, last_pow_y;

    double d_angle;


    movement_complete = false;
    total_movement_time = millis();
    bool quad_3_4;
    orig_pos_angle = atan2(tracking.y_coord - path[next_waypoint].coord.y, tracking.x_coord - path[next_waypoint].coord.x);
    // change to this
    // orig_pos_angle = atan2(path[next_waypoint].coord.y - tracking.y_coord, path[next_waypoint].x_coord - tracking.x_coord);

    int orig_pos_sgn = sgn(orig_pos_angle), cur_pos_sgn;
    if (fabs(orig_pos_angle) > M_PI/2) quad_3_4 = true;
    else quad_3_4 = false;

    // custom waypoint setup CODE
    // set_offsets(path);
    set_errors(path);
    set_angles(path);
    error_d_waypoints_left = set_error_d_waypoints_left(path);
    local_x_waypoints_left = set_local_x_waypoints_left(path);
    local_y_waypoints_left = set_local_y_waypoints_left(path);

    path[next_waypoint].set_error_x(path[next_waypoint].coord.x - tracking.x_coord);
    path[next_waypoint].set_error_y(path[next_waypoint].coord.y - tracking.y_coord);
    path[next_waypoint].set_error_d(sqrt(pow(path[next_waypoint].get_error_x(), 2.0) + pow(path[next_waypoint].get_error_y(), 2.0)));
    path[next_waypoint].set_d_angle(atan2(path[next_waypoint].get_error_x(),path[next_waypoint].get_error_y()));

    printf("\nStarting move through waypoint X: %f Y: %f A: %f from X: %f Y: %f A: %f  Time %d\n", path[path.size() - 1].coord.x, path[path.size() - 1].coord.y, rad_to_deg(path[path.size() - 1].coord.angle), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle), starttime);

    while (true){

          // update error_d for next waypoint
          path[next_waypoint].set_error_x(path[next_waypoint].coord.x - tracking.x_coord);
          path[next_waypoint].set_error_y(path[next_waypoint].coord.y - tracking.y_coord);
          path[next_waypoint].set_error_d(sqrt(pow(path[next_waypoint].get_error_x(), 2.0) + pow(path[next_waypoint].get_error_y(), 2.0)));

          // update error_a
          error_a = path[next_waypoint].coord.angle - tracking.global_angle;

// error_a = leastAngle(path[next_waypoint].coord.angle, tracking.global_angle) - tracking.global_angle;
          // update total_error_d
          error_d_next_waypoint = path[next_waypoint].get_error_d();    // global var, don't mess with it
          total_error_d = error_d_waypoints_left + error_d_next_waypoint;

          tracking.drive_error = total_error_d; // drive error and movetime need to be added as static vars to waypoint class
          difference_a = tracking.global_angle + atan2(path[next_waypoint].get_error_y(), path[next_waypoint].get_error_x());
          local_error_x = cos(difference_a) * path[next_waypoint].get_error_d();
          local_error_y = sin(difference_a) * path[next_waypoint].get_error_d();

          if(fabs(local_error_x) > 0.3){
            tracking.power_x = cos(difference_a) * total_error_d * kp;
            if (fabs(tracking.power_x) < min_power && max_power > min_power) tracking.power_x = min_power * sgn(local_error_x);
          }
          else tracking.power_x = 0.0;

          if(fabs(local_error_y) > 0.3){
            tracking.power_y = sin(difference_a) * total_error_d * kp;
            if (fabs(tracking.power_y) < min_power && max_power > min_power) tracking.power_y = min_power * sgn(local_error_y);
          }
          else tracking.power_y = 0.0;

          if(fabs(error_a) > deg_to_rad(path[next_waypoint].end_error_a)){
            tracking.power_a = error_a * kp_a;
            if (fabs(tracking.power_a) < min_power_a) tracking.power_a = min_power_a * sgn(error_a);
          }
          else tracking.power_a = 0.0;
          if(millis() - integral_last_time > 50){   // integral is UNUSED currently
            if (fabs(local_error_x) < 5.0){
              integral_x += fabs(local_error_x) * (millis() - integral_last_time);
            }
            if (fabs(local_error_y) < 5.0){
              integral_y += fabs(local_error_y) * (millis() - integral_last_time);
            }

            if(fabs(local_error_x) <= 0.9) {
              integral_x = 0;
            }
            if(fabs(local_error_y) <= 0.9) {
              integral_y = 0;
            }
          }
          tracking.power_x += sgn(tracking.power_x) * integral_x * kI_d;
          //tracking.power_y+= sgn(tracking.power_y) * integral_y* kI_d;


// START OF SCALING
    // if (total_error_d < path[path.size() - 1].end_error) tracking.power_x = 0.0; tracking.power_y = 0.0;

    total_power = fabs(tracking.power_x) + fabs(tracking.power_y) + fabs(tracking.power_a);
    pre_scaled_power_d = tracking.power_x + tracking.power_y, pre_scaled_power_a = tracking.power_a;
    pre_scaled_power_x = tracking.power_x; pre_scaled_power_y = tracking.power_y;

    if(total_power > max_power){
        tracking.power_x = tracking.power_x / total_power * max_power;
        tracking.power_y = tracking.power_y / total_power * max_power;
        tracking.power_a = tracking.power_a / total_power * max_power;

        // if (fabs(tracking.power_x) + fabs(tracking.power_y) > 0.0){ // gets rid of division of 0 error
            if (fabs(pre_scaled_power_a) > path[next_waypoint].min_power_a){  //  (added fabs here)
                if (fabs(tracking.power_a) < path[next_waypoint].min_power_a){ // power_a has been overshadowed  (added fabs here)
                        tracking.power_a = sgn(tracking.power_a) * path[next_waypoint].min_power_a;
                        scale = (fabs(max_power) - fabs(tracking.power_a)) / (fabs(tracking.power_x)  + fabs(tracking.power_y));
                        tracking.power_x *= scale;
                        tracking.power_y *= scale;
                }
            }

            else {  // angle gets the power it demanded, if pre_scaled power_a was also less than min_power_a
                tracking.power_a = pre_scaled_power_a;
                scale = (fabs(max_power) - fabs(tracking.power_a)) / (fabs(tracking.power_x)  + fabs(tracking.power_y));
                tracking.power_x *= scale;
                tracking.power_y *= scale;
            }
        // }
    }

// BRAKE CODE
    if(next_waypoint == path.size() - 1 && brake_type == brake_types::harsh_Stop){   // robot is on the way to the final waypoint
        brake_hypotenuse = sqrt(pow(tracking.g_velocity.x, 2.0) + pow(tracking.g_velocity.y, 2.0));    // finds linear displacement
        arrival_time = path[next_waypoint].get_error_d() / brake_hypotenuse;
        if (arrival_time < brake_time_threshold){    // applies brake 100 milliseconds before reaching target
            printf("*************BRAKE arrival_time: %.5f\n", arrival_time);
            double theta = atan2(-tracking.g_velocity.y, -tracking.g_velocity.x) + tracking.global_angle;    // angle of linear displacement
            tracking.power_x = brake_hypotenuse * cos(theta) * brake_scalar;
            tracking.power_y = brake_hypotenuse * sin(theta) * brake_scalar;
        }
        /* // This is angle braking code, incomplete

        arrival_time_angle = error_a/tracking.g_velocity.angle;
        printf("*************BRAKE ANGLE arrival_time: %.5f\n", arrival_time_angle);

        if (arrival_time_angle < 0.080) tracking.power_a = -tracking.g_velocity.angle * brake_scalar_angle;
        */
    }

    cur_pos_angle = atan2(tracking.y_coord - path[next_waypoint].coord.y, tracking.x_coord - path[next_waypoint].coord.x);
    // change to this LATER
    // cur_pos_angle = atan2(path[next_waypoint].coord.y - tracking.y_coord, path[next_waypoint].coord.x - tracking.x_coord);
    cur_pos_sgn = sgn(cur_pos_angle);
// END OF SCALING

    // allocates all power not used by angle PID to the distance
    if (full_speed && next_waypoint == path_size - 1 && fabs(tracking.power_x) + fabs(tracking.power_y) > 0.0){
        // printf("power x and y before %f, %f", tracking.power_x, tracking.power_y);
        tracking.power_x = tracking.power_x / (fabs(tracking.power_x) + fabs(tracking.power_y)) * (max_power - fabs(tracking.power_a));
        tracking.power_y = tracking.power_y / (fabs(tracking.power_x) + fabs(tracking.power_y)) * (max_power - fabs(tracking.power_a));
        // printf(" power x and y after %f, %f \n", tracking.power_x, tracking.power_y);
    }



// This section handles reaching the next waypoint

          if (path[next_waypoint].overshoot){
              // printf("OVERSHOOT\n");

              if (quad_3_4 && cur_pos_sgn != orig_pos_sgn)   cur_pos_angle += orig_pos_sgn * (M_PI * 2);    // cur_pos_angle needs to be altered
              if (fabs(orig_pos_angle - cur_pos_angle) > M_PI/2 || path[next_waypoint].get_error_d() < path[next_waypoint].end_error){ // overshoot has occurred or distance is satisfied
                  // printf("\n overshoot!");
                  tracking.power_x = 0.0, tracking.power_y = 0.0;   // overshoot occurred, but angle is not satisfied yet
// PREVIOUSLY HANDLE_EXIT
                if(fabs(error_a) < deg_to_rad(path[next_waypoint].end_error_a)){  // reached waypoint
                    printf("\n\nTime: %d, Ending move to waypoint %d X: %f, Y: %f, A: %f At X: %f, Y: %f, A: %f\n", millis(), next_waypoint, path[next_waypoint].coord.x, path[next_waypoint].coord.y, rad_to_deg(path[next_waypoint].coord.angle), tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
                    //log_time("\n\nEnding move to waypoint %d X: %f, Y: %f, A: %f \n", next_waypoint, path[next_waypoint].coord.x, path[next_waypoint].coord.y, rad_to_deg(path[next_waypoint].coord.angle));

                    //log_time("At X: %f, Y: %f, A: %f, Time: %d", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle), millis());

                    last_waypoint_time = millis();
                    // //log_time("ending starting time: %d, delta time: %d X: %f Y: %f A: %f from X: %f Y: %f A: %f \n", millis(),millis() -starttime, target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
                    waypoint::next_waypoint++;
                    next_waypoint = waypoint::next_waypoint;

                    if (next_waypoint >= path.size()){  // last waypoint has been reached
                        printf("\n FINAL WAYPOINT REACHED! Time is: %d, time since start of move %d \n\n\n", millis(), millis()  - starttime);
                        printf("GLOBAL VELOCITY| x: %.2f, y: %.2f a: %.2f\n", tracking.g_velocity.x, tracking.g_velocity.y, rad_to_deg(tracking.g_velocity.angle));

                        //log_time("\n FINAL WAYPOINT REACHED! Time is: %d, time since start of move %d \n\n\n", millis(), millis()  - starttime);

                        if (brake_type != brake_types::none)  printf("\n\n ***************BRAKE");    brake();
                        movement_complete = true;
                        path_size = 0;
                        waypointStopTask();
                        total_movement_time = 0;
                        return;
                    }
                    else { // if last waypoint has NOT been reached
                        error_d_waypoints_left -= path[next_waypoint].get_error_d();
                        orig_pos_angle = atan2(path[next_waypoint - 1].coord.y - path[next_waypoint].coord.y, path[next_waypoint - 1].coord.x - path[next_waypoint].coord.x);
                        // change to this
                        // orig_pos_angle = atan2(path[next_waypoint].coord.y - path[next_waypoint - 1].coord.y, path[next_waypoint].coord.x - path[next_waypoint - 1].coord.x);

                        orig_pos_sgn = sgn(orig_pos_angle);   // every waypoint
                        if (fabs(orig_pos_angle) > M_PI/2) quad_3_4 = true; // every waypoint
                        else quad_3_4 = false;  // every waypoint
                    }
                }
// END OF HANDLE_EXIT
              }
          }
          // if NOT overshoot
          else if (path[next_waypoint].get_error_d() < path[next_waypoint].end_error){
              // printf("not overshoot\n");
// PREVIOUSLY HANDLE_EXIT
            if(fabs(error_a) < deg_to_rad(path[next_waypoint].end_error_a)){  // reached waypoint
                printf("\n\nyoEnding move to waypoint X: %f, Y: %f, A: %f \n", path[next_waypoint].coord.x, path[next_waypoint].coord.y, rad_to_deg(path[next_waypoint].coord.angle));
                printf("At X: %f, Y: %f, A: %f, Time: %d", tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle), millis() - last_waypoint_time);

                // //log_time("ending starting time: %d, delta time: %d X: %f Y: %f A: %f from X: %f Y: %f A: %f \n", millis(),millis() -starttime, target_x, target_y, target_a, tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
                waypoint::next_waypoint++;
                next_waypoint = waypoint::next_waypoint;
                last_waypoint_time = millis();

                if (next_waypoint >= path.size()){  // last waypoint has been reached
                    printf("\n FINAL WAYPOINT REACHED! Time: %d \n\n\n", millis()  - starttime);
                    printf("GLOBAL VELOCITY| x: %.2f, y: %.2f a: %.2f\n", tracking.g_velocity.x, tracking.g_velocity.y, rad_to_deg(tracking.g_velocity.angle));

                    if (brake_type != brake_types::none)  printf("\n\n *************** UNPOWERED BRAKE");     brake();
                    movement_complete = true;
                    path_size = 0;
                    total_movement_time = 0;
                    waypointStopTask();
                    return;
                }
                else error_d_waypoints_left -= path[next_waypoint].get_error_d();
            }
// END OF HANDLE_EXIT
          }
// END OF handle_reaching_next_waypoint

        // UPDATES LAST POWERS
        last_pow_x = tracking.power_x;
        last_pow_y = tracking.power_y;
        move(tracking.power_x, tracking.power_y, tracking.power_a);

// debug

// BRAKE DATA
/*            if (tracking.power_x != last_pow_x || tracking.power_y != last_pow_y){
                printf("POWER CHANGE| time: %d ", millis());
                printf("error_d: %f, pow_a: %.1f, pow_x: %.1f, pow_y: %.1f, total_pow: %.1f ", error_d_next_waypoint, tracking.power_a, tracking.power_x, tracking.power_y, fabs(tracking.power_a) + fabs(tracking.power_x) + fabs(tracking.power_y));
                printf("GLOBAL VELOCITY| x: %.2f, y: %.2f a: %.2f\n", tracking.g_velocity.x, tracking.g_velocity.y, rad_to_deg(tracking.g_velocity.angle));

            }
*/
        if(debug == true && millis() - lasttime > 50){
            printf("%d, Err x: %.1f, Err y: %.1f, Err a: %.1f, t_err_d: %.1f\n ", millis(), path[next_waypoint].get_error_x(), path[next_waypoint].get_error_y(), rad_to_deg(error_a), total_error_d);
            printf("err_d: %.f, pow_x: %.1f, pow_y: %.1f, pow_a: %.1f, total_pow: %.1f\n", error_d_next_waypoint, tracking.power_x, tracking.power_y, tracking.power_a, fabs(tracking.power_a) + fabs(tracking.power_x) + fabs(tracking.power_y));

            // printf("", power_a, power);
            // printf("pt_a: %.1f, cpt_a: %.1f, dif_a: %1.f, err %.1f, t_e: %.1f, e_a: %.1f, p_a: %.1f, pow_x: %.1f, p_y: %.1f, e_x: %.1f, e_y: %.1f ", rad_to_deg(orig_pos_angle), rad_to_deg(cur_pos_angle), rad_to_deg(fabs(orig_pos_angle - cur_pos_angle)), path[next_waypoint].get_error_d(), total_error_d, rad_to_deg(error_a), tracking.power_a, tracking.power_x, tracking.power_y, local_error_x, local_error_y);
            // printf("err_A %f\n", rad_to_deg(error_a));

            // printf("tar_x: %f, tar_Y: %f, tar_A: %f ", path[next_waypoint].coord.x, path[next_waypoint].coord.y, path[next_waypoint].coord.angle);
            // printf("GX: %.1f, GY: %.1f, GA: %.1f \n",tracking.x_coord, tracking.y_coord, rad_to_deg(tracking.global_angle));
            // printf("integ X: %f, integ Y: %f \n",integral_x, integral_y);
            lasttime = millis();
        }

        delay(10);

        if (movement_complete) return; // entire movement is complete
    }
}
