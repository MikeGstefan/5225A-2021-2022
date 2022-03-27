#include "distance.hpp"
Distance_States Distance_State = Distance_States::stable;
Distance_States Last_Distance_State;
cDistance left_eye;
cDistance right_eye;
bool ram = false;


double d_offset = 0.0;//depth of the funny wall thing

void setVisionState(Distance_States state) {
	Last_Distance_State = Distance_State;
	Distance_State = state;
}

Position distance_reset_left(int cycles){
	double dist_corner = 6; //Distance sensor to corner
	double side_length = 86; //Corner of the robot to side distance sensor
	double dist_to_centre = 196; //Side distance sensor to tracking centre
	double local_y = 0, local_x = 0, angle = 0;
	double averageleft = 0, averageright = 0, averageside = 0;
	double dist_sensor = 429; //Distance between Sensors
	double l_average, r_average, s_average;
	int start_time = millis();
	vector <int> left;
	vector <int> right;
	vector <int> side;
	ram = false;
	int side_dist = l_dist.get();

	int error_count = 0;

	misc.print("Start Time: %d\n", start_time);
	for(int i = 0; i < cycles; i++){
		side_dist = l_dist.get();
		misc.print("%d| Left:%d Right: %d, Side: %d\n", millis(), l_reset_dist.get(), r_reset_dist.get(), side_dist);
		if(l_reset_dist.get() != 0) left.push_back(l_reset_dist.get()); else error_count++;
		if(r_reset_dist.get() != 0)right.push_back(r_reset_dist.get());else error_count++;
		if(side_dist != 0) side.push_back(side_dist); else error_count++;
		delay(33);
	}

	for(int i = 0; i < cycles; i++){
		l_average += left.at(i);
		r_average += right.at(i);
		s_average += side.at(i);
	}

	l_average = l_average/cycles;
	r_average = r_average/cycles;
	s_average = s_average/cycles;

	for(int i = 0; i< cycles; i++){
		if(abs(left.at(i) - l_average) > 50){
			left.erase(left.begin()+i); error_count++;
		}
		if(abs(right.at(i) - r_average) > 50){
			right.erase(right.begin()+i);error_count++;
		}
		if(abs(side.at(i) - s_average) > 50){
			side.erase(side.begin()+i);error_count++;
		}
	}


	if(error_count < cycles/3){
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
		// angle = atan(diff/dist_sensor);
		// angle = near_angle(2*atan2(sqrt(pow(diff,2.0) + pow(dist_sensor, 2.0)- pow(d_offset, 2.0)) - dist_sensor, diff - d_offset),0.0);
		angle = near_angle(2*atan2(sqrt(pow(dist_sensor,2.0) - pow(d_offset,2.0) + pow(averageleft,2.0) + pow(averageright,2.0) -( 2*averageleft*averageright))-dist_sensor, diff + d_offset), 0.0);

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
	double dist_corner = 6; //Distance sensor to corner
	double side_length = 86; //Corner of the robot to side distance sensor
	double dist_to_centre = 196; //Side distance sensor to tracking centre
	double local_y = 0, local_x = 0, angle = 0;
	double averageleft = 0, averageright = 0, averageside = 0;
	double dist_sensor = 429; //Distance between Sensors
	double l_average, r_average, s_average;
	int start_time = millis();
	vector <int> left;
	vector <int> right;
	vector <int> side;
	ram = false;
	int side_dist = r_dist.get();

	int error_count = 0;

	misc.print("Start Time: %d\n", start_time);
	for(int i = 0; i < cycles; i++){
		side_dist = r_dist.get();
		misc.print("%d| Left:%d Right: %d, Side: %d\n", millis(), l_reset_dist.get(), r_reset_dist.get(), side_dist);
		if(l_reset_dist.get() != 0) left.push_back(l_reset_dist.get()); else error_count++;
		if(r_reset_dist.get() != 0)right.push_back(r_reset_dist.get());else error_count++;
		if(side_dist != 0) side.push_back(side_dist); else error_count++;
		delay(33);
	}

	for(int i = 0; i < cycles; i++){
		l_average += left.at(i);
		r_average += right.at(i);
		s_average += side.at(i);
	}

	l_average = l_average/cycles;
	r_average = r_average/cycles;
	s_average = s_average/cycles;

	for(int i = 0; i< cycles; i++){
		if(abs(left.at(i) - l_average) > 50){
			left.erase(left.begin()+i); error_count++;
		}
	}
	for(int i = 0; i< cycles; i++){
		if(abs(right.at(i) - r_average) > 50){
			right.erase(right.begin()+i);error_count++;
		}
	}
	for(int i = 0; i< cycles; i++){
		if(abs(side.at(i) - s_average) > 50){
			side.erase(side.begin()+i);error_count++;
		}
	}

	if(error_count < cycles/3){
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
		// angle = atan(diff/dist_sensor);
		angle = near_angle(2*atan2(sqrt(pow(diff,2.0) + pow(dist_sensor, 2.0)- pow(d_offset, 2.0)) - dist_sensor, diff - d_offset),0.0);

		misc.print("Angle Reset to: %f\n", rad_to_deg(angle));
		local_y = ((averageright/25.4) - tan(angle) + (dist_to_centre/25.4) * tan(angle) + (side_length/25.4)) * cos(angle);
		local_x = ((averageside/25.4) + (dist_to_centre/25.4)) * cos(angle);

		misc.print("Front: %f, Side: %f, Side sensor: %f\n", local_y, local_x, averageside);
		master.print(0,0," %.3f, %.3f", local_y, local_x);
		Position Reset (local_x, local_y, rad_to_deg(angle));
		return Reset;
	}else{
		//Make it ram into the wall or just throw a flag to another program
		flatten_against_wall();
		Position Reset(local_x,local_y,angle);
		return Reset;
	}
}


//15
void distance_loop(double distance, int timeout){
	double y_speed = 40;
	double a_speed = 30;
	int count = 0;
	const Point start_pos = {tracking.x_coord, tracking.y_coord};
	double delta_dist = 0.0;
	int start_time = millis();

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
				drivebase.move(-y_speed,0);
				if(left_eye.last_distance == r_reset_dist.get()) count++;
				if(count == 4) break;
				else count = 0;
				break;
			case Distance_States::turn_left:
				printf("Turn left\n");
				drivebase.move(-y_speed, -a_speed);
				break;
			case Distance_States::turn_right:
				drivebase.move(-y_speed, a_speed);
				printf("Turn right\n");
				break;
		}
		if(timeout != 0 && millis() - start_time > timeout)break;
		left_eye.last_distance = r_reset_dist.get();
		right_eye.last_distance = l_reset_dist.get();
		delay(33);
  	}
}
