#include "distance.hpp"
Distance_States Distance_State = Distance_States::stable;
Distance_States Last_Distance_State;
cDistance left_eye;
cDistance right_eye;
bool ram = false;

void setVisionState(Distance_States state) {
	Last_Distance_State = Distance_State;
	Distance_State = state;
}

Position distance_reset_left(int cycles){
	double dist_corner = 72; //Distance sensor to corner
	double side_length = 135; //Corner of the robot to side distance sensor
	double dist_to_centre = 190; //Side distance sensor to tracking centre
	double local_y = 0, local_x = 0, angle = 0;
	double averageleft = 0, averageright = 0, averageside = 0;
	double dist_sensor = 429; //Distance between Sensors
	int start_time = millis();
	vector <int> left;
	vector <int> right;
	vector <int> side;
	ram = false;
	int left_low = l_reset_dist.get();
	int right_low = r_reset_dist.get();
	int side_low = l_dist.get();

	int error_count = 0;

	misc.print("Start Time: %d\n", start_time);
	for(int i = 0; i < cycles; i++){
		misc.print("%d| Left:%d Right: %d, Side: %d\n", millis(), l_reset_dist.get(), r_reset_dist.get(), l_dist.get());

		if(l_reset_dist.get() <= left_low) left_low = l_reset_dist.get();
		if(r_reset_dist.get() <= right_low) right_low = r_reset_dist.get();
		if(l_dist.get() <= side_low) side_low = l_dist.get();


		if(l_reset_dist.get() < left_low + 20) left.push_back(l_reset_dist.get()); else error_count++;
		if(r_reset_dist.get() < right_low + 20) right.push_back(r_reset_dist.get()); else error_count++;
		if(l_dist.get() < side_low + 20) side.push_back(l_dist.get()); else error_count++;

		delay(33);
	}
	if(error_count < 5){
		sort(left.begin(), left.end());
		sort(right.begin(), right.end());
		sort(side.begin(), side.end());

		averageleft = left.at(left. size() / 2);
		averageright = right.at(right.size() / 2);
		averageside = side.at(side.size() / 2);

		printf("Average: Left:%f Right:%f\n", averageleft, averageright);
		misc.print("End Time: %d\n", millis());
	}
	else{
		misc.print("Reset Failed Error Count is %d", error_count);
		ram = true;
	}
	if(ram == 0){
		int diff = averageleft-averageright;
		angle = atan(diff/dist_sensor);

		misc.print("Angle Reset to: %f\n", rad_to_deg(angle));
		local_y = ((averageleft/25.4) - tan(angle) + (dist_to_centre/25.4) * tan(angle) + (side_length/25.4)) * cos(angle);
		local_x = ((averageside/25.4) + (dist_to_centre/25.4)) * cos(angle);

		misc.print("Front: %f, Side: %f, Side sensor: %f\n", local_y, local_x, averageside);
		Position Reset (local_x, local_y, angle);
		return Reset;
	}else{
		//Make it ram into the wall or just throw a flag to another program
		flatten_against_wall();
		Position Reset(local_x,local_y,angle);
		return Reset;
	}
}

Position distance_reset_right(int cycles){
	double dist_corner = 72; //Distance sensor to corner
	double side_length = 135; //Corner of the robot to side distance sensor
	double dist_to_centre = 190; //Side distance sensor to tracking centre
	double local_y = 0, local_x = 0, angle = 0;
	double averageleft = 0, averageright = 0, averageside = 0;
	double dist_sensor = 429; //Distance between Sensors
	int start_time = millis();
	vector <int> left;
	vector <int> right;
	vector <int> side;
	ram = false;
	int left_low = l_reset_dist.get();
	int right_low = r_reset_dist.get();
	int side_low = r_dist.get();

	int error_count = 0;

	misc.print("Start Time: %d\n", start_time);
	for(int i = 0; i < cycles; i++){
		misc.print("%d| Left:%d Right: %d, Side: %d\n", millis(), l_reset_dist.get(), r_reset_dist.get(), r_dist.get());

		if(l_reset_dist.get() <= left_low) left_low = l_reset_dist.get();
		if(r_reset_dist.get() <= right_low) right_low = r_reset_dist.get();
		if(r_dist.get() <= side_low) side_low = r_dist.get();


		if(l_reset_dist.get() < left_low + 20) left.push_back(l_reset_dist.get()); else error_count++;
		if(r_reset_dist.get() < right_low + 20) right.push_back(r_reset_dist.get()); else error_count++;
		if(r_dist.get() < side_low + 20) side.push_back(r_dist.get()); else error_count++;

		delay(33);
	}
	if(error_count < 5){
		sort(left.begin(), left.end());
		sort(right.begin(), right.end());
		sort(side.begin(), side.end());

		averageleft = left.at(left. size() / 2);
		averageright = right.at(right.size() / 2);
		averageside = side.at(side.size() / 2);

		printf("Average: Left:%f Right:%f\n", averageleft, averageright);
		misc.print("End Time: %d\n", millis());
	}
	else{
		misc.print("Reset Failed Error Count is %d", error_count);
		ram = true;
	}
	if(ram == 0){
		int diff = averageleft-averageright;
		angle = atan(diff/dist_sensor);

		misc.print("Angle Reset to: %f\n", rad_to_deg(angle));
		local_y = ((averageleft/25.4) - tan(angle) + (dist_to_centre/25.4) * tan(angle) + (side_length/25.4)) * cos(angle);
		local_x = ((averageside/25.4) + (dist_to_centre/25.4)) * cos(angle);

		misc.print("Front: %f, Side: %f, Side sensor: %f\n", local_y, local_x, averageside);
		Position Reset (local_x, local_y, angle);
		return Reset;
	}else{
		//Make it ram into the wall or just throw a flag to another program
		flatten_against_wall();
		Position Reset(local_x,local_y,angle);
		return Reset;
	}
}


//15
void distance_loop(double distance){
	double y_speed = 40;
	double a_speed = 30;
	int count = 0;
	const Point start_pos = {tracking.x_coord, tracking.y_coord};
	double delta_dist = 0.0;

	while(distance >= delta_dist){
		delta_dist = sqrt(pow(tracking.x_coord -start_pos.x,2) + pow(tracking.y_coord - start_pos.y,2));
		misc.print("Delta Dist: %f\n", delta_dist);

		if(abs(l_reset_dist.get() - r_reset_dist.get()) <= 50) setVisionState(Distance_States::stable); // 50 is a test value
		else if (l_reset_dist.get() > r_reset_dist.get()) setVisionState(Distance_States::turn_left);
		else if (l_reset_dist.get() < r_reset_dist.get()) setVisionState(Distance_States::turn_right);

		if(l_reset_dist.get() == 0) setVisionState(Distance_States::turn_left);
		if(r_reset_dist.get() == 0) setVisionState(Distance_States::turn_right);
		if(l_reset_dist.get() == 0 && r_reset_dist.get() == 0) setVisionState(Distance_States::stable);

		misc.print("Left:%d Right: %d\n",r_reset_dist.get(), l_reset_dist.get());

		switch (Distance_State) {
			case Distance_States::stable:
				printf("stable\n");
				drivebase.move_tank(-y_speed,0);
				if(left_eye.last_distance == r_reset_dist.get()) count++;
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

		left_eye.last_distance = r_reset_dist.get();
		right_eye.last_distance = l_reset_dist.get();
		delay(33);
  	}
}
