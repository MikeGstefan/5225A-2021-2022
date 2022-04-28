//Right: get_roll, -1
//Left: get_roll, 1
//Back: get_pitch, 1
//Front: get_pitch, -1
#define GYRO_AXIS get_roll
constexpr int gyro_side = -1;

constexpr double
  DistanceLR = 9.91, DistanceB = 0.85, //From tracking update
  dist_back = 8.5,
  dist_front = 8.5,
  front_dist_dist = 3.85,
  left_track_diameter = 2.75,
  right_track_diameter = 2.75,
  back_track_diameter = 2.75;