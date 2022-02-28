#include "lift.hpp"

_Task lift(lift_pid, "lift");


array<int, 6> f_lift_pos= {20, 150, 300, 475, 630,  675};
int f_lift_index = 0;
int f_lift_time = 0;

array<int, 4> b_lift_pos= {20, 400, 580,  675};
int b_lift_index = 0;
int b_lift_time = 0;
bool looking = false;
bool found = false;
int find_count = 0;


int up_press_time = 0;
int down_press_time = 0;


double f_target = 100;

void f_lift_inc(){
  if(f_lift_index < f_lift_pos.size()-1){
    f_lift_index++;
    lift_pid_set_target(f_lift_index);
    // f_lift.move_absolute(f_lift_pos[f_lift_index]);
  }
}

void f_lift_dec(){
  if(f_lift_index > 0){
    f_lift_index--;
    lift_pid_set_target(f_lift_index);
    // f_lift.move_absolute(f_lift_pos[f_lift_index]);
  }
}

void b_lift_inc(){
  if(b_lift_index < b_lift_pos.size()-1){
    b_lift_index++;
    b_lift.move_absolute(b_lift_pos[b_lift_index]);
  }
}

void b_lift_dec(){
  if(b_lift_index > 0){
    b_lift_index--;
    b_lift.move_absolute(b_lift_pos[b_lift_index]);
  }
}

void handle_lifts(){
    


     if(master.get_digital_new_press(lift_up_button)){ 
        up_press_time = millis();
        if(get_lift()){
          f_lift_inc();
        }
        else{ 
          b_lift_inc();
        }
        
      }


      if(master.get_digital_new_press(lift_down_button)){ 
        down_press_time = millis();
        if(get_lift()){
          f_lift_dec();
        }
        else{ 
          b_lift_dec();
        }
      }

      if(master.get_digital(lift_up_button) && !master.get_digital(lift_down_button) && millis() - up_press_time > 300){ //
        if(get_lift()){
            f_lift_index = f_lift_pos.size()-1;
            // f_lift.move_absolute(f_lift_pos[f_lift_index]);
            lift_pid_set_target(f_lift_index);
        }
        else{ 
            b_lift_index = b_lift_pos.size()-1;
            b_lift.move_absolute(b_lift_pos[b_lift_index]);
        }
      }

      if(master.get_digital(lift_down_button) && !master.get_digital(lift_up_button) && millis() - down_press_time > 300){ //
        if(get_lift()){
            f_lift_index = 0;
            lift_pid_set_target(f_lift_index);
        }
        else{ 
            b_lift_index = 0;
            b_lift.move_absolute(b_lift_pos[b_lift_index]);
        }
      }

      if(master.get_digital(lift_up_button) && master.get_digital(lift_down_button) && (millis() - up_press_time > 300 || millis() - down_press_time > 300)){ //
        b_lift_index = 0;
        b_lift.move_absolute(b_lift_pos[b_lift_index]);
        f_lift_index = 0;
        // f_lift.move_absolute(f_lift_pos[f_lift_index]);
        lift_pid_set_target(f_lift_index);
        
      }

      if(master.get_digital_new_press(DIGITAL_A)){
          b_lift_index = 0;
        b_lift.move_absolute(b_lift_pos[b_lift_index]);
        f_lift_index = 0;
        // f_lift.move_absolute(f_lift_pos[f_lift_index]);
        lift_pid_set_target(f_lift_index);
        
      }


      if(master.get_digital_new_press(lift_release_button)){ 
        if(get_lift()){
          f_claw_p.set_value(0);
        }
        else{ 
          b_claw_p.set_value(0);
        }
        
      }

      if(f_lift_index == 0){ 
        if(f_touch.get_value() && millis() - f_lift_time > 750){
          f_claw_p.set_value(1);
          f_lift_time = millis();
          f_lift_index = 1;
            f_lift.move_absolute(f_lift_pos[f_lift_index]);
        }
      }


    //this needs to be fixed
      if(b_lift_index == 0){ 
        if(looking && (b_dist.get() > 75 && b_dist.get() < 90 && !found))find_count++;
        else {
          find_count = 0;
          looking = false;
        }

        if(find_count >= 2 && !found){
          b_claw_p.set_value(1);
          // looking = false;
          find_count = 0;
          found = true;
          b_lift_time =millis();
        }
        if(found && (b_dist.get() > 120 || millis() - b_lift_time > 2000)){
          looking = false;
          found = false;
          find_count = 0;
        }

        if(b_dist.get() < 70 && !looking && millis() - b_lift_time > 1000 && !found){
          looking = true;
        }
      }

}


bool get_lift(){
    return (!drivebase.get_reverse() ==  master.get_analog(ANALOG_RIGHT_Y) > -1* drivebase.get_deadzone());
}

void lift_pid_set_target(int target){ 
    lift.data_update();
    f_target = target;
    lift.done_update();
}

void lift_pid(void* params){ 
    _Task* ptr = _Task::get_obj(params);
    PID lift_pid_p(5.0,0.0,0.0,0.0);
    PID lift_pid_v(1.0,0.0,0.0,0.0);
    
    double error = f_target - f_lift.motor.get_position(), error_v;
    double power;
    double hold_power = 0;
    bool new_brake = true;
    bool brake = false;

    double end_error =5;
    while(true){ 
        error = f_lift_pos[f_target] - f_lift.motor.get_position();
        if(fabs(error) > end_error){
            if(brake && fabs(error)> 10){ //
            
            }
            else{ //
                power = lift_pid_p.compute(-error, 0.0);
            hold_power = 0;

            brake = false;
            f_lift.move(power);
            }
            

        }
        else{
            if(!brake){
                f_lift.motor.move_velocity(0);
                brake = true;
            }
            // error_v = -1*f_lift.motor.get_actual_velocity();
            // if(fabs(error_v) > 5.0){ 
            //     hold_power += 1*sgn(error_v);
            // }
            // // power = lift_pid_v.compute(-error_v, 0.0);
            // power = hold_power;
        }
        
        

        if(ptr->notify_handle())return;
        delay(10);
    }
}