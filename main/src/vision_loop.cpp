#include "vision_loop.hpp"

Vision_States Vision_State = Vision_States::stable;
Vision_States Last_Vision_State;
vision left_eye;
vision right_eye;

void setVisionState(Vision_States state) {
	Last_Vision_State = Vision_State;
	Vision_State = state;
}



void vision_loop(){
  while (true) {
    if (abs(left_distance.get() - left_eye.last_distance) > 300 && left_eye.sus_spasm == 0){left_eye.sus_spasm = 1;}
    else if (abs(left_distance.get() - left_eye.last_distance) > 300 && left_eye.sus_spasm == 1){left_eye.spasm = 1;}
    else if (abs(left_distance.get() - left_eye.last_distance) <= 300 && left_eye.spasm == 1) {left_eye.sus_spasm = 1; left_eye.spasm = 0;}
    else if (abs(left_distance.get() - left_eye.last_distance) <= 300) {left_eye.sus_spasm = 0; left_eye.spasm = 0;}

    if (abs(right_distance.get() - right_eye.last_distance) > 300 && right_eye.sus_spasm == 0){right_eye.sus_spasm = 1;}
    else if (abs(right_distance.get() - right_eye.last_distance) > 300 && right_eye.sus_spasm == 1){right_eye.spasm = 1;}
    else if (abs(right_distance.get() - right_eye.last_distance) <= 300 && right_eye.spasm == 1) {right_eye.sus_spasm = 1; right_eye.spasm = 0;}
    else if (abs(right_distance.get() - right_eye.last_distance) <= 300) {right_eye.sus_spasm = 0; right_eye.spasm = 0;}

		if(abs(right_distance.get() - left_distance.get()) <= 50){setVisionState(Vision_States::stable);} // 50 is a test value
		else if (right_distance.get() > left_distance.get()){setVisionState(Vision_States::right_high);}
		else if (right_distance.get() < left_distance.get()){setVisionState(Vision_States::left_high);}

		if(right_distance.get() == 0){setVisionState(Vision_States::right_gone);}
		if(left_distance.get() == 0){setVisionState(Vision_States::left_gone);}

    if(left_eye.spasm == 1 && right_eye.spasm == 0){setVisionState(Vision_States::left_spasm);}
    if(left_eye.spasm == 0 && right_eye.spasm == 1){setVisionState(Vision_States::right_spasm);}
    if(left_eye.spasm == 1 && right_eye.spasm == 1){setVisionState(Vision_States::spasming);}
    printf("Left:%d Right: %d\n",left_distance.get(), right_distance.get());
    switch (Vision_State) {
      case Vision_States::stable:
        printf("stable\n");
        break;
      case Vision_States::left_spasm:
        printf("left eye is having a spasm\n");
        break;
      case Vision_States::right_spasm:
        printf("right eye is having a spasm\n");
        break;
      case Vision_States::spasming:
        printf("Both are spasming\n");
        break;
    }
    left_eye.last_distance = left_distance.get();
    right_eye.last_distance = right_distance.get();
    delay(33);
  }
}
