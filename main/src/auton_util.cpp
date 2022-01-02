#include "auton_util.hpp"

double get_filtered_output(ADIUltrasonic sensor, int check_count, uint16_t lower_bound, uint16_t upper_bound, int timeout){
  Timer timer{"Timer"};
  int success_count = 0;

  long total_input;
  uint16_t input;
  double filtered_output;
  while(timer.get_time() < timeout && success_count < check_count){
    input = sensor.get_value();
    if (lower_bound <= input <= upper_bound){

      total_input += input;
      printf("input: %d\n", input);
      success_count++;
    }
    else printf("FAILED! input: %d\n", input);
    delay(10);
  }
  filtered_output = total_input / success_count;
  printf("filtered output: %lf\n", filtered_output);
  return filtered_output;
}

// void flatten_against_wall(bool right){
//   bool front_done, back_done;
//   int flatten_power = 50, less_power = 30, greater_power = 70;
//
//
//   if(right){
//     drivebase.move(flatten_power, 0.0, 0.0);
//     while(!(front_done && back_done)){
//
//
//
//       if(front_done){
//
//       }
//       if (back_done){
//
//
//       }
//
//
//     }
//   }
//   else{
//
//
//   }
//
//
//
// }
