#include "auton_util.hpp"
#include "logging.hpp"



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



// double get_front_dist(){
//   double avg = 0.0;
//   int count = 0;
//   while(true){ 
//     // for(int i = 0; i < 3; i++){
//     if(front_dist.get() > 10){
//       avg+= front_dist.get();
//       count++;
//     }
//     if(count > 2){
//       avg/=count;
//       break;
//     }
//     delay(34);
//   }
//   return ((avg/25.4) +front_dist_dist);
  
// }

void detect_goal(){ 
  // cycleCheck(b_dist.get() > 80 && b_dist.get() < 90, 5, 33);
  while(b_dist.get() > 70){ 
    misc.print("looking for edge: %d\n", b_dist.get());
    delay(33);
  }
  int successCount = 0;
    while (successCount < 2){
        if (b_dist.get() > 75 && b_dist.get() < 90) successCount++;
        else successCount = 0;
        misc.print("looking: %d\n", b_dist.get());
        delay(33);
    }
  misc.print("Detected %d\n", b_dist.get());
  b_claw_p.set_value(1);
}