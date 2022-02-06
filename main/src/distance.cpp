#include "vision_loop.hpp"
bool dis_end = false;
Vision_States Vision_State = Vision_States::stable;
Vision_States Last_Vision_State;
vision left_eye;
vision right_eye;

void setVisionState(Vision_States state) {
	Last_Vision_State = Vision_State;
	Vision_State = state;
}



void vision_loop(double distance){

	const Point start_pos = {tracking.x_coord, tracking.y_coord};
	double delta_dist = 0.0;
	while(distance >= delta_dist){
		delta_dist = sqrt(pow(tracking.x_coord -start_pos.x,2) + pow(tracking.y_coord - start_pos.y,2));
		printf("Delta Dist: %f\n", delta_dist);

    // if (abs(l_dis.get() - left_eye.last_distance) > 300 && left_eye.sus_spasm == 0){left_eye.sus_spasm = 1;}
    // else if (abs(l_dis.get() - left_eye.last_distance) > 300 && left_eye.sus_spasm == 1){left_eye.spasm = 1;}
    // else if (abs(l_dis.get() - left_eye.last_distance) <= 300 && left_eye.spasm == 1) {left_eye.sus_spasm = 1; left_eye.spasm = 0;}
    // else if (abs(l_dis.get() - left_eye.last_distance) <= 300) {left_eye.sus_spasm = 0; left_eye.spasm = 0;}
		//
    // if (abs(r_dis.get() - right_eye.last_distance) > 300 && right_eye.sus_spasm == 0){right_eye.sus_spasm = 1;}
    // else if (abs(r_dis.get() - right_eye.last_distance) > 300 && right_eye.sus_spasm == 1){right_eye.spasm = 1;}
    // else if (abs(r_dis.get() - right_eye.last_distance) <= 300 && right_eye.spasm == 1) {right_eye.sus_spasm = 1; right_eye.spasm = 0;}
    // else if (abs(r_dis.get() - right_eye.last_distance) <= 300) {right_eye.sus_spasm = 0; right_eye.spasm = 0;}

		if(abs(r_dis.get() - l_dis.get()) <= 50){setVisionState(Vision_States::stable);} // 50 is a test value
		else if (r_dis.get() > l_dis.get()){setVisionState(Vision_States::turn_left);}
		else if (r_dis.get() < l_dis.get()){setVisionState(Vision_States::turn_right);}

		if(r_dis.get() == 0){setVisionState(Vision_States::turn_left);}
		if(l_dis.get() == 0){setVisionState(Vision_States::turn_right);}
		if(r_dis.get() == 0 && l_dis.get() == 0){setVisionState(Vision_States::stable);}

    printf("Left:%d Right: %d\n",l_dis.get(), r_dis.get());
    switch (Vision_State) {
      case Vision_States::stable:
        printf("stable\n");
				drivebase.move_tank(-40,0);
				if(left_eye.last_distance == l_dis.get() && dis_end == true){break;}
				else if(left_eye.last_distance == l_dis.get()){dis_end = true;}
				else{dis_end = false;}
        break;
			case Vision_States::turn_left:
				printf("Turn left\n");
				drivebase.move_tank(-103, -30);
				break;
			case Vision_States::turn_right:
				drivebase.move_tank(-103, 30);
				printf("Turn right\n");
				break;
    }
    left_eye.last_distance = l_dis.get();
    right_eye.last_distance = r_dis.get();
    delay(33);
  }
}
