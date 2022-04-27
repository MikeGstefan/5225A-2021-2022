// Right: get_roll, -1
// Left: get_roll, 1
// Back: get_pitch, 1
// Front: get_pitch, -1
#define GYRO_AXIS get_roll
constexpr int gyro_side = -1;

constexpr double DistanceLR = 9.91, DistanceB = 0.85; //From tracking update
constexpr double dist_back = 8.5;
constexpr double dist_front = 8.5;
constexpr double front_dist_dist = 3.85;