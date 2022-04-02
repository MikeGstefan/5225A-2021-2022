#include "lift.hpp"
#include "config.hpp"
#include "controller.hpp"
#include "pid.hpp"
#include "util.hpp"
#include "drive.hpp"

//4-bar
#if game
array<int, 5> f_lift_pos= {20, 200, 475, 630, 800};
#else
array<int, 4> f_lift_pos= {20, 200, 475, 630};
#endif
int f_lift_index = 0;
int f_lift_time = 0;
bool f_claw_state = false;
int partner_f_time = 0;
bool f_lift_manual = false;

//6bar
#if game
array<int, 5> b_lift_pos= {20,460, 600, 675, 950};
#else
array<int, 4> b_lift_pos= {20, 200, 475, 630};
#endif

int b_lift_index = 0;
int b_lift_time = 0;
int partner_b_time = 0;
bool looking = false;
bool found = false;
int find_count = 0;


int up_press_time = 0;
int down_press_time = 0;

bool intk_state = false;
bool reverse_state = false;
bool intk_pos = false;
bool started_pos = false;
int intk_pos_time = millis();

double b_error = 0.0;
double f_error = 0.0;

double b_power = 0.0;
double f_power = 0.0;

PID b_pid(5.0,0.0,0.0,0.0);
PID f_pid(5.0,0.0,0.0,0.0);

int timer = millis();

void intk_c(void* params){
  Timer intake_t ("intake jam", false);
		// intk.move(127);
		while(true){
      if(intk_state){
        if(intake_jam.get_new_press()) intake_t.reset(); //Start timer when pressed
        else if(!intake_jam.get_value()) intake_t.reset(false); //End timer when unpressed
        if(intake_t.get_time() > 500){ //If pressed for more than 0.5 sec, reverse intk
				  intk.move(-127);
				  wait_until(!intake_jam.get_value()); //Waits for unjam plus some time
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
        intk_pos = false;
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
/*
    // doesn't turn on intake if it's at the bottom
    if(f_lift_index != 0 && (master.get_digital_new_press(intake_button) || partner.get_digital_new_press(partner_intk_on))){
        intk_state = !intk_state;
        intk.move(127*intk_state);
        master.print(0,0,"INTAKE");
      }

    if(partner.get_digital_new_press(partner_intk_reverse) && f_lift_index != 0){
      intk_state = 0;
      // reverse_state = !reverse_state;
      intk.move(-127);
    }

    

     if(master.get_digital_new_press(lift_up_button)){
        up_press_time = millis();
        if(get_lift()){
          f_lift_inc();
        }
        else{
          b_lift_inc();
        }
        f_lift_manual = false;

      }


      if(master.get_digital_new_press(lift_down_button)){
        down_press_time = millis();
        if(get_lift()){
          f_lift_dec();
        }
        else{
          b_lift_dec();
        }
        f_lift_manual = false;
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
        up_press_time = millis();
        f_lift_manual = false;
      }
      // lowers lift to bottom height when down button is held
      if(master.get_digital(lift_down_button) && !master.get_digital(lift_up_button) && millis() - down_press_time > 300){ //
        if(get_lift()){
          // turns off intake when f lift lowers to bottom
            intk_state = 0;
            intk.move(127*intk_state);
            f_lift_index = 0;
            intk_pos = false;
            // f_lift.move_absolute(f_lift_pos[f_lift_index]);
        }
        else{
            b_lift_index = 0;
            // b_lift.move_absolute(b_lift_pos[b_lift_index]);
        }
        f_lift_manual = false;
        down_press_time = millis();
      }

      if(master.get_digital(lift_up_button) && master.get_digital(lift_down_button) && (millis() - up_press_time > 300 || millis() - down_press_time > 300)){ //
        intk_pos = false;
        b_lift_index = 0;
        // b_lift.move_absolute(b_lift_pos[b_lift_index]);
        f_lift_index = 0;
        // f_lift.move_absolute(f_lift_pos[f_lift_index]);
        intk_state = 0;
        intk.move(127*intk_state);
        f_lift_manual = false;
        up_press_time = millis();
        down_press_time = millis();
      }

      if(master.get_digital_new_press(lift_both_down_button) || (!game && partner.get_digital_new_press(lift_down_button))){
        // turns intake off when f lift lowers to bottom
        intk_state = 0;
        intk.move(127*intk_state);
        intk_pos = false;
          b_lift_index = 0;
        // b_lift.move_absolute(b_lift_pos[b_lift_index]);
        f_lift_index = 0;
        f_lift_manual = false;
        // f_lift.move_absolute(f_lift_pos[f_lift_index]);

      }

      if(game){
        if(partner.get_digital_new_press(partner_f_up)){
          f_lift_inc();
          partner_f_time = millis();
          f_lift_manual = false;
        }
        if(partner.get_digital_new_press(partner_f_down)){
          f_lift_dec();
          partner_f_time = millis();
          f_lift_manual = false;
        }
        if(partner.get_digital_new_press(partner_b_up)){
          b_lift_inc();
          partner_b_time = millis();
          f_lift_manual = false;
        }
        if(partner.get_digital_new_press(partner_b_down)){
          b_lift_dec();
          partner_b_time = millis();
          f_lift_manual = false;
        }

        if(partner.get_digital(partner_f_up) &&!partner.get_digital(partner_f_down) && millis() - partner_f_time > 300){
          f_lift_index = f_lift_pos.size()-1;
          f_lift_manual = false;
        }
        if(partner.get_digital(partner_f_down) &&!partner.get_digital(partner_f_up) && millis() - partner_f_time > 300){
          f_lift_index = 0;
          f_lift_manual = false;
        }
        if(partner.get_digital(partner_b_up) &&!partner.get_digital(partner_b_down) && millis() - partner_b_time > 300){
          b_lift_index = b_lift_pos.size()-1;
          f_lift_manual = false;
        }
        if(partner.get_digital(partner_b_down) &&!partner.get_digital(partner_b_up) && millis() - partner_b_time > 300){
          b_lift_index = 0;
          f_lift_manual = false;
        }
      }

      if(!game && partner.get_digital_new_press(lift_up_button) ){
        b_lift_index = b_lift_pos.size() -1;
        // b_lift.move_absolute(b_lift_pos[b_lift_index]);
        f_lift_index = f_lift_pos.size() -1;
        f_lift_manual = false;
      }


      if(master.get_digital_new_press(lift_release_button)){
        if(get_lift()){
          f_claw_p.set_value(0);
          f_claw_state = false;
           f_lift_time = millis();
        }
        else{
          b_claw_p.set_value(0);
        }

      }

      if(f_lift_index == 0){
        if(f_touch.get_value() && millis() - f_lift_time > 1000 && !f_claw_state){
          f_claw_p.set_value(1);
          f_lift_time = millis();
          f_lift_index = 1;
          f_claw_state = true;
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


      if(fabs(partner.get_analog(ANALOG_LEFT_Y)) > 20){
        f_lift_manual = true;
      }
      if(!f_lift_manual){
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
      else{ 
        if(fabs(partner.get_analog(ANALOG_LEFT_Y)) > 20){
          if((partner.get_analog(ANALOG_LEFT_Y) > 0 && f_lift.motor.get_position() < 900)|| (partner.get_analog(ANALOG_LEFT_Y) < 0 && f_lift.motor.get_position() >10 ))f_lift.move(partner.get_analog(ANALOG_LEFT_Y));
          else f_lift.motor.move_velocity(0);
        }
        else f_lift.motor.move_velocity(0);
      }


      

      if(!intk_pos && f_lift_index == 0 && fabs(f_error)  < 10 && !started_pos){
        intk.move(-20);
        intk_state = 0;
        intk_pos_time = millis();
        // intk_pos = true;
        started_pos = true;
      }
      if(!intk_pos && f_lift_index == 0 && fabs(f_error)  < 10 && millis() - intk_pos_time > 500 && started_pos){
        intk.move(0);
        intk_state = 0;
        // intk_pos = false;
        started_pos = false;
        // intk_pos_time = millis();
        intk_pos = true;

      }


      if(millis() - timer > 50){
        master.print(0,0,"f %d, b %d\n", f_lift_index, b_lift_index);
        timer = millis();
      }

*/
}




bool get_lift(){
  return (drivebase.get_reverse() != (master.get_analog(ANALOG_RIGHT_Y) > -drivebase.get_deadzone()));
}
