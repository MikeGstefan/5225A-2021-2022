#include "distance.hpp"
#include "config.hpp"
#include "Libraries/logging.hpp"
#include "auton_util.hpp"
#include "Libraries/util.hpp"
#include "drive.hpp"
#include "tracking.hpp"


Distance_States Distance_State = Distance_States::stable;
Distance_States Last_Distance_State;
cDistance left_eye;
cDistance right_eye;
bool ram = false;


double d_offset = 0.0;//depth of the funny wall thing

void setVisionState(Distance_States state){
	Last_Distance_State = Distance_State;
	Distance_State = state;
}

Position distance_reset_left(int cycles){
	double dist_corner = 6; //Front Distance sensor to side of robot
	double side_length = 86; //Front distance sensor to side distance sensor (Parrallel to the robot not the h value)
	double dist_to_centre = 196; //Side distance sensor to tracking centre
	double local_y = 0, local_x = 0, angle = 0;
	double averageleft = 0, averageright = 0, averageside = 0;
	double dist_sensor = 429; //Distance between Front Sensors
	double l_average, r_average, s_average;
	int start_time = millis();
	std::vector <int> left;
	std::vector <int> right;
	std::vector <int> side;
	ram = false;
	int side_dist = l_dist.get();

	int error_count = 0;

	misc.print("Start Time: %d", start_time);
	for(int i = 0; i < cycles; i++){
		side_dist = l_dist.get();
		misc.print("Left:%d Right: %d, Side: %d", l_reset_dist.get(), r_reset_dist.get(), side_dist);
		if(l_reset_dist.get() != 0) left.push_back(l_reset_dist.get()); else error_count++;
		if(r_reset_dist.get() != 0)right.push_back(r_reset_dist.get());else error_count++;
		if(side_dist != 0) side.push_back(side_dist); else error_count++;
		delay(33);
	}

	l_average = std::accumulate(left.begin(), left.end(), 0.0)/left.size();
	r_average = std::accumulate(right.begin(), right.end(), 0.0)/right.size();
	s_average = std::accumulate(side.begin(), side.end(), 0.0)/side.size();

	// l_average = l_average/left.size();
	// r_average = r_average/right.size();
	// l_side_average /= l_side.size();
	// r_side_average /= r_side.size();
	misc.print("average collected");
	for(int i = 0; i< cycles; i++){
		if(i < left.size() && fabs(left.at(i) - l_average) > 50) left.erase(left.begin()+i);
		if(i < right.size() && fabs(right.at(i) - r_average) > 50) right.erase(right.begin()+i);
		if(i < side.size() && fabs(side.at(i) - s_average) > 50) side.erase(side.begin()+i);
	}


	if(error_count < 8){
		sort(left.begin(), left.end());
		sort(right.begin(), right.end());
		sort(side.begin(), side.end());

		averageleft = left.at(left. size() / 2);
		averageright = right.at(right.size() / 2);
		averageside = side.at(side.size() / 2);

		printf2("Average: Left:%f Right:%f", averageleft, averageright);
		misc.print("End Time: %d", millis());
	}
	else{
		misc.print(term_colours::ERROR, "Reset Failed Error Count is %d", error_count);
		ram = true;
	}
	if(ram == 0){
		int diff = averageleft-averageright;
		// angle = atan(diff/dist_sensor);
		// angle = near_angle(2*atan2(sqrt(pow(diff,2.0) + pow(dist_sensor, 2.0)- pow(d_offset, 2.0)) - dist_sensor, diff - d_offset),0.0);
		angle = near_angle(2*atan2(sqrt(pow(dist_sensor,2.0) - pow(d_offset,2.0) + pow(averageleft,2.0) + pow(averageright,2.0) -( 2*averageleft*averageright))-dist_sensor, diff + d_offset), 0.0);

		misc.print("Angle Reset to: %f", rad_to_deg(angle));
		local_y = ((averageleft/25.4) - tan(angle) + (dist_to_centre/25.4) * tan(angle) + (side_length/25.4)) * cos(angle);
		local_x = ((averageside/25.4) + (dist_to_centre/25.4)) * cos(angle);

		misc.print("Front: %f, Side: %f, Side sensor: %f", local_y, local_x, averageside);
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
	double dist_corner = 6; //Front Distance sensor to side of robot
	double side_length = 86; //Front distance sensor to side distance sensor (Parrallel to the robot not the h value)
	double dist_to_centre = 196; //Side distance sensor to tracking centre
	double local_y = 0, local_x = 0, angle = 0;
	double averageleft = 0, averageright = 0, averageside = 0;
	double dist_sensor = 429; //Distance between Front Sensors
	double l_average, r_average, s_average;
	int start_time = millis();
	std::vector <int> left;
	std::vector <int> right;
	std::vector <int> side;
	ram = false;
	int side_dist = r_dist.get();

	int error_count = 0;

	misc.print("Start Time: %d", start_time);
	for(int i = 0; i < cycles; i++){
		side_dist = r_dist.get();
		misc.print("Left:%d Right: %d, Side: %d", l_reset_dist.get(), r_reset_dist.get(), side_dist);
		if(l_reset_dist.get() != 0) left.push_back(l_reset_dist.get()); else error_count++;
		if(r_reset_dist.get() != 0)right.push_back(r_reset_dist.get());else error_count++;
		if(side_dist != 0) side.push_back(side_dist); else error_count++;
		delay(33);
	}

	l_average = std::accumulate(left.begin(), left.end(), 0.0)/left.size();
	r_average = std::accumulate(right.begin(), right.end(), 0.0)/right.size();
	s_average = std::accumulate(side.begin(), side.end(), 0.0)/side.size();

	// l_average = l_average/left.size();
	// r_average = r_average/right.size();
	// l_side_average /= l_side.size();
	// r_side_average /= r_side.size();
	misc.print("average collected");
	for(int i = 0; i< cycles; i++){
		if(i < left.size() && fabs(left.at(i) - l_average) > 50) left.erase(left.begin()+i);
		if(i < right.size() && fabs(right.at(i) - r_average) > 50) right.erase(right.begin()+i);
		if(i < side.size() && fabs(side.at(i) - s_average) > 50) side.erase(side.begin()+i);
	}

	if(error_count < 8){
		sort(left.begin(), left.end());
		sort(right.begin(), right.end());
		sort(side.begin(), side.end());

		averageleft = left.at(left. size() / 2);
		averageright = right.at(right.size() / 2);
		averageside = side.at(side.size() / 2);

		printf2("Average: Left:%f Right:%f", averageleft, averageright);
		misc.print("End Time: %d", millis());
	}
	else{
		misc.print(term_colours::ERROR, "Reset Failed Error Count is %d", error_count);
		ram = true;
	}
	if(ram == 0){
		int diff = averageleft-averageright;
		// angle = atan(diff/dist_sensor);
		angle = near_angle(2*atan2(sqrt(pow(diff,2.0) + pow(dist_sensor, 2.0)- pow(d_offset, 2.0)) - dist_sensor, diff - d_offset),0.0);

		misc.print("Angle Reset to: %f", rad_to_deg(angle));
		local_y = ((averageright/25.4) - tan(angle) + (dist_to_centre/25.4) * tan(angle) + (side_length/25.4)) * cos(angle);
		local_x = ((averageside/25.4) + (dist_to_centre/25.4)) * cos(angle);

		misc.print("Front: %f, Side: %f, Side sensor: %f", local_y, local_x, averageside);
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


Position distance_reset_center(int cycles){
		double dist_corner = -25; //Front Distance sensor to side of robot
		double side_length = 15; //Front distance sensor to side distance sensor (Parrallel to the side of the robot not the h value)
		double dist_to_centre = 180; //Side distance sensor to tracking centre
		double dist_sensor = 400; //Distance between Front Sensors
		double local_y = 0, l_local_x = 0, r_local_x = 0, angle = 0, local_x;
		double averageleft = 0, averageright = 0, average_l_side = 0, average_r_side = 0;
		double l_average, r_average, l_side_average, r_side_average;
		int start_time = millis();
		std::vector <int> left;
		std::vector <int> right;
		std::vector <int> r_side;
		std::vector <int> l_side;
		ram = false;
		int left_low = l_reset_dist.get();
		int right_low = r_reset_dist.get();
		int r_side_low = r_dist.get();
		int l_side_low = l_dist.get();


		misc.print("Start Time: %d", start_time);
		for(int i = 0; i < cycles; i++){
			misc.print("Left:%d Right: %d, r_Side: %d, l_side: %d", l_reset_dist.get(), r_reset_dist.get(), r_dist.get(), l_dist.get());

			if(l_reset_dist.get() != 0) left.push_back(l_reset_dist.get());
			if(r_reset_dist.get() != 0)right.push_back(r_reset_dist.get());
			if(r_dist.get() != 0) r_side.push_back(r_dist.get());
			if(l_dist.get() != 0) l_side.push_back(l_dist.get());
			delay(33);
		}
		misc.print("values collected");
		// for(int i = 0; i < cycles; i++){
		// 	l_average += left.at(i);
		// 	r_average += right.at(i);
		// 	l_side_average += l_side.at(i);
		// 	r_side_average += r_side.at(i);
		// }
		l_average = std::accumulate(left.begin(), left.end(), 0.0)/left.size();
		r_average = std::accumulate(right.begin(), right.end(), 0.0)/right.size();
		l_side_average = std::accumulate(l_side.begin(), l_side.end(), 0.0)/l_side.size();
		r_side_average = std::accumulate(r_side.begin(), r_side.end(), 0.0)/r_side.size();

		// l_average = l_average/left.size();
		// r_average = r_average/right.size();
		// l_side_average /= l_side.size();
		// r_side_average /= r_side.size();
		misc.print("average collected");
		for(int i = 0; i< cycles; i++){
			if(i < left.size() && fabs(left.at(i) - l_average) > 50) left.erase(left.begin()+i);
			if(i < right.size() && fabs(right.at(i) - r_average) > 50) right.erase(right.begin()+i);
			if(i < l_side.size() && fabs(l_side.at(i) - l_side_average) > 50) l_side.erase(l_side.begin()+i);
			if(i < r_side.size() && fabs(r_side.at(i) - r_side_average) > 50) r_side.erase(r_side.begin()+i);
		}

			sort(left.begin(), left.end());
			sort(right.begin(), right.end());
			sort(r_side.begin(), r_side.end());
			sort(l_side.begin(), l_side.end());

			averageleft = left.at(int(left. size() / 2));
			averageright = right.at(int(right.size() / 2));
			average_r_side = r_side.at(int(r_side.size() / 2));
			average_l_side = l_side.at(int(l_side.size() / 2));

			printf2("Average: Left:%f Right:%f", averageleft, averageright);
			printf2("Average Side: Left:%f Right:%f", average_l_side, average_r_side);
			misc.print("End Time: %d", millis());

			int diff = averageleft-averageright;
			// angle = atan(diff/dist_sensor);
			angle = near_angle(2*atan2(sqrt(pow(diff,2.0) + pow(dist_sensor, 2.0)- pow(d_offset, 2.0)) - dist_sensor, diff - d_offset),0.0);

			misc.print("Angle Reset to: %f", rad_to_deg(angle));
			local_y = ((averageright/25.4) - tan(angle) + (dist_to_centre/25.4) * tan(angle) + (side_length/25.4)) * cos(angle);
			l_local_x = ((average_l_side/25.4) + (dist_to_centre/25.4)) * cos(angle);
			r_local_x = 141.0-(((average_r_side/25.4) + (dist_to_centre/25.4)) * cos(angle));
			local_x = (r_local_x + l_local_x)/2;

			misc.print("local_y: %f, l_Side: %f, r_Side: %f, local_x: %f", local_y, l_local_x, r_local_x, local_x);

		// master.print(0,0," %.3f, %.3f", local_y, l_local_x, r_local_x);
 //delete expression
		Position Reset (local_x, local_y, rad_to_deg(angle));
		return Reset;
}


//15
void ramp_distance_line_up(double distance, int timeout){
	double y_speed = 40;
	double a_speed = 30;
	int count = 0;
	const Point start_pos = {tracking.x_coord, tracking.y_coord};
	double delta_dist = 0.0;
	int start_time = millis();

	while(distance >= delta_dist){
		delta_dist = sqrt(pow(tracking.x_coord -start_pos.x,2) + pow(tracking.y_coord - start_pos.y,2));
		misc.print("Delta Dist: %f", delta_dist);

		if(abs(l_reset_dist.get() - r_reset_dist.get()) <= 50) setVisionState(Distance_States::stable); // 50 is a test value
		else if (l_reset_dist.get() > r_reset_dist.get()) setVisionState(Distance_States::turn_left);
		else if (l_reset_dist.get() < r_reset_dist.get()) setVisionState(Distance_States::turn_right);

		if(l_reset_dist.get() == 0) setVisionState(Distance_States::turn_left);
		if(r_reset_dist.get() == 0) setVisionState(Distance_States::turn_right);
		if(l_reset_dist.get() == 0 && r_reset_dist.get() == 0) setVisionState(Distance_States::stable);

		misc.print("Left:%d Right: %d",r_reset_dist.get(), l_reset_dist.get());

		switch (Distance_State){
			case Distance_States::stable:
				printf2("stable");
				drivebase.move(-y_speed,0);
				if(left_eye.last_distance == r_reset_dist.get()) count++;
				if(count == 4) break;
				else count = 0;
				break;
			case Distance_States::turn_left:
				printf2("Turn left");
				drivebase.move(-y_speed, -a_speed);
				break;
			case Distance_States::turn_right:
				drivebase.move(-y_speed, a_speed);
				printf2("Turn right");
				break;
		}
		if(timeout != 0 && millis() - start_time > timeout)break;
		left_eye.last_distance = r_reset_dist.get();
		right_eye.last_distance = l_reset_dist.get();
		delay(33);
  	}
}

void goal_line_up(int timeout){
	double y_speed = 40;
	double a_speed = 30;
	const Point start_pos = {tracking.x_coord, tracking.y_coord};
	double delta_dist = 0.0;
	int start_time = millis();

	while(true){ //When grabs goal idk figure this shit out
		if(abs(b_dist.get() - r_goal_dist.get()) <= 75) setVisionState(Distance_States::stable); // 50 is a test value
		else if (b_dist.get() > r_goal_dist.get()) setVisionState(Distance_States::turn_left);
		else if (b_dist.get() < r_goal_dist.get()) setVisionState(Distance_States::turn_right);

		if(b_dist.get() == 0) setVisionState(Distance_States::turn_left);
		if(r_goal_dist.get() == 0) setVisionState(Distance_States::turn_right);
		if(b_dist.get() == 0 && r_goal_dist.get() == 0) setVisionState(Distance_States::stable);

		misc.print("Left:%d Right: %d",r_goal_dist.get(), b_dist.get());

		switch (Distance_State) {
			case Distance_States::stable:
				printf2("stable");
				drivebase.move(-y_speed,0);
				break;
			case Distance_States::turn_left:
				printf2("Turn left");
				drivebase.move(-y_speed, -a_speed);
				break;
			case Distance_States::turn_right:
				drivebase.move(-y_speed, a_speed);
				printf2("Turn right");
				break;
		}
		if(timeout != 0 && millis() - start_time > timeout)break;
		left_eye.last_distance = r_goal_dist.get();
		right_eye.last_distance = b_dist.get();
		delay(33);
  	}
}
