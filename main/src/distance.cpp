#include "distance.hpp"
bool dis_end = false;
Distance_States Distance_State = Distance_States::stable;
Distance_States Last_Distance_State;
cDistance left_eye;
cDistance right_eye;
double averageleft = 0;
double averageright = 0;


void setVisionState(Distance_States state) {
	Last_Distance_State = Distance_State;
	Distance_State = state;
}


void average(int time){
	double total_left = 0, total_right = 0;
	int cycle_count = 0;
	int start_time = millis();
	vector <int> left;
	vector <int> right;
	misc.print("Start Time: %d\n", start_time);
	while(start_time+time >= millis()){
		printf("%d| Left:%d Right: %d\n", millis(), back_left_dist.get(), back_right_dist.get());
		left.push_back(back_left_dist.get());
		right.push_back(back_right_dist.get());
		// total_left += back_left_dist.get();
		// total_right += back_right_dist.get();
		cycle_count +=1;
		delay(33);
	}
	sort(left.begin(), left.end());
	sort(right.begin(), right.end());
	averageleft = left.at(left. size() / 2);
	averageright = right.at(right.size() / 2);
	printf("Average: Left:%f Right:%f\n", averageleft, averageright);
	misc.print("End Time: %d\n", millis());
}

void distance_reset(int time){
	double d = 315;	
	average(time);
	double diff = averageleft-averageright;
	double angle = atan(diff/d);
	angle = rad_to_deg(angle);
	misc.print("Angle Reset to: %f\n", angle);
}

void distance_loop(double distance){
	double y_speed = 40;
	double a_speed = 30;
	int count = 0;
	const Point start_pos = {tracking.x_coord, tracking.y_coord};
	double delta_dist = 0.0;

	while(distance >= delta_dist){
		delta_dist = sqrt(pow(tracking.x_coord -start_pos.x,2) + pow(tracking.y_coord - start_pos.y,2));
		printf("Delta Dist: %f\n", delta_dist);

		if(abs(back_left_dist.get() - back_right_dist.get()) <= 50) setVisionState(Distance_States::stable); // 50 is a test value
		else if (back_left_dist.get() > back_right_dist.get()) setVisionState(Distance_States::turn_left); 
		else if (back_left_dist.get() < back_right_dist.get()) setVisionState(Distance_States::turn_right); 

		if(back_left_dist.get() == 0) setVisionState(Distance_States::turn_left);
		if(back_right_dist.get() == 0) setVisionState(Distance_States::turn_right);
		if(back_left_dist.get() == 0 && back_right_dist.get() == 0) setVisionState(Distance_States::stable);

		printf("Left:%d Right: %d\n",back_right_dist.get(), back_left_dist.get());

		switch (Distance_State) {
			case Distance_States::stable:
				printf("stable\n");
				drivebase.move_tank(-y_speed,0);
				if(left_eye.last_distance == back_right_dist.get()) count++;
				if(count == 4) break;
				else count = 0;
				break;
			case Distance_States::turn_left:
				printf("Turn left\n");
				drivebase.move_tank(-y_speed, -a_speed);
				break;
			case Distance_States::turn_right:
				drivebase.move_tank(-y_speed, a_speed);
				printf("Turn right\n");
				break;
		}

		left_eye.last_distance = back_right_dist.get();
		right_eye.last_distance = back_left_dist.get();
		delay(33);
  	}
}
