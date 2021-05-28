#include "logging.hpp"
Task *printing = nullptr;

int open_slot = 0, last_point = 0;
string id_array [queue_size];
string type = "NULL";

vector<std::string> Queue(queue_size);



Data tracking(Levels::wire, "usd/test.txt");
// Data pos(Levels::both);

void Data::print(const char* format, ...){
    char output_char[256];
    va_list args;
    va_start(args, format);
    vsprintf(output_char, format, args);
    va_end(args);
    std::string output =output_char;
    // printf("got here %s\n", output_char);
    // std::cout << output <<endl;
    switch(this->level){
        case Levels::both:
            printf("%s\n",output.c_str());
            this->logging(output);
        break;
        case Levels::wire:
            printf("%s\n",output.c_str());
        break;
        case Levels::card:
          this->logging(output);
        break;
        case Levels::none:

        break;
    }
}


void Data::logging(std::string output){
  id_array[open_slot] = this->file;
  Queue[open_slot] = output;
  open_slot++;
  if(open_slot > queue_size-1){
    open_slot = 0;
  }
}

void L_print(){
  //data can be printed every 4-6 ms
  //avoid printing more often then that
  printf("task started\n");
  std::ofstream file;
  while(true){
    // printf("open: %d, last: %d\n",open_slot,last_point);
    if(last_point != open_slot || !Queue[last_point].empty()){
      file.open(id_array[last_point], ios::app);
      file<<Queue[last_point];
      cout<<millis()<< " "<<id_array[last_point]<<" "<<Queue[last_point]<<endl;
      //this clears the string in the array rather than erase the index of the vector
      Queue[last_point].clear();
      id_array[last_point].clear();
      file << endl;
      file.close();
      last_point +=1;
      if(last_point > queue_size-1){
        last_point = 0;
      }
    }
    delay(1);
  }
}

void print_task(){
  printing = new Task(L_print);
}

void printStopTask(){
  if(printing != nullptr){
    printing->remove();
    delete printing;
    printing = nullptr;
  }
}
