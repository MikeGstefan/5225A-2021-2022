#include "logging.hpp"
Task *printing = nullptr;

char queue[queue_size];
char* front = queue;
char* back = queue;
char buffer[256];

void print(const char* format,...){
  std::va_list args;
  va_start(args, format);
  vsprintf(buffer,format,args);
  //copying the string uses memcpy instead of strcpy or strncpy to avoid copying the terminating character

  //if the end of the buffer would be past the max of the queue array
  if(reinterpret_cast<uintptr_t>(back)+strlen(buffer) > reinterpret_cast<uintptr_t>(&queue)+queue_size-1){
    //copy the data that fills the queue
    memcpy(back, buffer, queue_size-1 - (reinterpret_cast<uintptr_t>(back) - reinterpret_cast<uintptr_t>(&queue)));
    //creates a ptr to the last character used to fill the back of the queue
    char* overflow_ptr = buffer+(queue_size-1 - (reinterpret_cast<uintptr_t>(back) - reinterpret_cast<uintptr_t>(&queue)));
    //fills the front of the queue with the remaining data from the buffer, marked by the overflow_ptr
    memcpy(queue,overflow_ptr,strlen(buffer)- (queue_size-1 - (reinterpret_cast<uintptr_t>(back) - reinterpret_cast<uintptr_t>(&queue))));
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
