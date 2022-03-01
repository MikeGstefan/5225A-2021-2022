#include "lift.hpp"

array<int, 6> f_lift_pos= {20, 150, 475, 630};
int f_lift_index = 0;
int f_lift_time = 0;

array<int, 4> b_lift_pos= {20,460, 580};
int b_lift_index = 0;
int b_lift_time = 0;
bool looking = false;
bool found = false;
int find_count = 0;


int up_press_time = 0;
int down_press_time = 0;

bool intk_state = false;

double b_error = 0.0;
double f_error = 0.0;

double b_power = 0.0;
double f_power = 0.0;

PID b_pid(5.0,0.0,0.0,0.0);
PID f_pid(5.0,0.0,0.0,0.0);

void intk_c(void* params){
  Timer intake_t ("intake jam", false);
		// intk.move(127);
		while(true){
      if(intk_state){ 
        if(intake_jam.get_new_press()) intake_t.reset(); //Start timer when pressed
			  else if(!intake_jam.get_value()) intake_t.reset(false); //End timer when unpressed
			  if(intake_t.get_time() > 1000){ //If pressed for more than 1 sec, reverse intk
				  intk.move(-127);
				  waitUntil(!intake_jam.get_value()); //Waits for unjam plus some time
				  delay(150);
				  intk.move(127);
			  }
      }
			
			delay(10);
		}
}

void f_lift_inc(){
  if(f_lift_index < f_lift_pos.size()-1){
    f_lift_index++;
    // f_lift.move_absolute(f_lift_pos[f_lift_index]);
  }
}

void f_lift_dec(){
  if(f_lift_index > 0){
    f_lift_index--;
     if(f_lift_index == 0){
        intk_state = 0;
        intk.move(127*intk_state);
        master.print(0,0,"INTAKE");
     }
    
    // f_lift.move_absolute(f_lift_pos[f_lift_index]);
  }
}

void b_lift_inc(){
  if(b_lift_index < b_lift_pos.size()-1){
    b_lift_index++;
    // b_lift.move_absolute(b_lift_pos[b_lift_index]);
  }
}

void b_lift_dec(){
  if(b_lift_index > 0){
    b_lift_index--;
    // b_lift.move_absolute(b_lift_pos[b_lift_index]);
   
  }
}

void handle_lifts(){
    if(master.get_digital_new_press(intake_button) || partner.get_digital_new_press(intake_button)){
        intk_state = !intk_state;
        intk.move(127*intk_state);
        master.print(0,0,"INTAKE");
      }



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
        }
        else{ 
            b_lift_index = b_lift_pos.size()-1;
            // b_lift.move_absolute(b_lift_pos[b_lift_index]);
        }
      }

      if(master.get_digital(lift_down_button) && !master.get_digital(lift_up_button) && millis() - down_press_time > 300){ //
        if(get_lift()){
            f_lift_index = 0;
            // f_lift.move_absolute(f_lift_pos[f_lift_index]);
        }
        else{ 
            b_lift_index = 0;
            // b_lift.move_absolute(b_lift_pos[b_lift_index]);
        }
      }

      if(master.get_digital(lift_up_button) && master.get_digital(lift_down_button) && (millis() - up_press_time > 300 || millis() - down_press_time > 300)){ //
        b_lift_index = 0;
        // b_lift.move_absolute(b_lift_pos[b_lift_index]);
        f_lift_index = 0;
        // f_lift.move_absolute(f_lift_pos[f_lift_index]);
        
      }

      if(master.get_digital_new_press(lift_both_down_button) || partner.get_digital_new_press(lift_down_button)){
          b_lift_index = 0;
        // b_lift.move_absolute(b_lift_pos[b_lift_index]);
        f_lift_index = 0;
        // f_lift.move_absolute(f_lift_pos[f_lift_index]);
        
      }

      if(partner.get_digital_new_press(lift_up_button)){ 
        b_lift_index = b_lift_pos.size() -1;
        // b_lift.move_absolute(b_lift_pos[b_lift_index]);
        f_lift_index = f_lift_pos.size() -1;
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
            // f_lift.move_absolute(f_lift_pos[f_lift_index]);
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



      b_error = b_lift_pos[b_lift_index] - b_lift.motor.get_position();
      if(fabs(b_error)  > 10){
        b_power = b_pid.compute(-b_error, 0.0);
        b_lift.move(b_power);
      }
      else{ //
        b_lift.motor.move_velocity(0);
      }


      f_error = f_lift_pos[f_lift_index] - f_lift.motor.get_position();
      if(fabs(f_error)  > 10){
        f_power = f_pid.compute(-f_error, 0.0);
        f_lift.move(f_power);
      }
      else{ //
        f_lift.motor.move_velocity(0);
      }

}

      


bool get_lift(){
    return (!drivebase.get_reverse() ==  master.get_analog(ANALOG_RIGHT_Y) > -1* drivebase.get_deadzone());
}