#include "logging.hpp"
Task *logging_task = nullptr;
const char* file_name= "/usd/test.txt";
char queue[queue_size];
char* front = queue;
char* back = queue;
char buffer[256];
ofstream file;

uintptr_t queue_start = reinterpret_cast<uintptr_t>(&queue);

void logging_task_start(){
  file.open(file_name,ofstream::app);
  file.close();
  logging_task = new Task(queue_handle);

}
void logging_task_stop(){
  if(logging_task != nullptr){
    logging_task->remove();
    delete logging_task;
    logging_task = nullptr;
  }
}

void log_print(const char* format,...){
  std::va_list args;
  va_start(args, format);
  vsprintf(buffer,format,args);
  //copying the string uses memcpy instead of strcpy or strncpy to avoid copying the terminating character

  //if the end of the buffer would be past the max of the queue array
  if(reinterpret_cast<uintptr_t>(back)+strlen(buffer) > queue_start+queue_size-1){
    //copy the data that fills the queue
    memcpy(back, buffer, queue_size-1 - (reinterpret_cast<uintptr_t>(back) - queue_start));
    //creates a ptr to the last character used to fill the back of the queue
    char* overflow_ptr = buffer+(queue_size-1 - (reinterpret_cast<uintptr_t>(back) - queue_start));
    //fills the front of the queue with the remaining data from the buffer, marked by the overflow_ptr
    memcpy(queue,overflow_ptr,strlen(buffer)- (queue_size-1 - (reinterpret_cast<uintptr_t>(back) - queue_start)));
    //moves back to the back of the data
    back =  strlen(buffer)-queue_size+1 + back;
  }
  else{
    //copies data to the queue and moves the pointer down
    memcpy(back,buffer,strlen(buffer));
    back +=strlen(buffer);
  }
  va_end(args);
}

void queue_handle(void* params){
  Timer logging_tmr{"logging_tmr"};
  char * temp_back;
  while(true){
    if(data_size() > print_point || logging_tmr.get_time() > print_max_time){
      temp_back = back;
      //does the queue rollover?
      if(reinterpret_cast<uintptr_t>(back) < reinterpret_cast<uintptr_t>(front)){
        file.open(file_name,ofstream::app);
        //print from the front to the end of the queue
        file.write(front, queue_size-1-( reinterpret_cast<uintptr_t>(front)-queue_start));
        //print from the start to the end of the data
        file.write(queue,reinterpret_cast<uintptr_t>(temp_back)-queue_start);
        file.close();
        front = queue +(reinterpret_cast<uintptr_t>(temp_back)-queue_start);
      }
      else{
        file.open(file_name,ofstream::app);
        //print the data marked to be printed
        file.write(front, reinterpret_cast<uintptr_t>(temp_back)- reinterpret_cast<uintptr_t>(front));
        file.close();
        front += reinterpret_cast<uintptr_t>(temp_back)- reinterpret_cast<uintptr_t>(front);
      }
      logging_tmr.reset();
    }
    delay(10);
  }


}

uintptr_t data_size(){//returns the number of characters needed to be printed from the queue
  if(reinterpret_cast<uintptr_t>(back) < reinterpret_cast<uintptr_t>(front))return(queue_size-1-( reinterpret_cast<uintptr_t>(front)-queue_start))+(reinterpret_cast<uintptr_t>(back)-queue_start);
  else return reinterpret_cast<uintptr_t>(back)- reinterpret_cast<uintptr_t>(front);
}
