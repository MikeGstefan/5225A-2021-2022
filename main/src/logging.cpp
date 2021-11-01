#include "logging.hpp"
Task *logging_task = nullptr;
const char* file_name= "/usd/data.txt";
const char* file_meta= "/usd/meta_data.txt";
char queue[queue_size];
char* front = queue;
char* back = queue;
// char buffer[256];
ofstream file;
uintptr_t queue_start = reinterpret_cast<uintptr_t>(&queue);
vector<Data*> Data::obj_list;

Data::Data(const char* obj_name, const char* id_code, log_types log_type_param, log_locations log_location_param){
  this->id = id_code;
  this->name = obj_name;
  this->log_type = log_type_param;
  this->log_location = log_location_param;
  obj_list.push_back(this);
}



Data task_log("tasks.txt","$01", general, log_locations::sd);



vector<Data*> Data::get_objs(){
  return obj_list;
}

void logging_task_start(){
  logging_task = new Task(queue_handle);
}
void logging_task_stop(){
  if(logging_task != nullptr){
    logging_task->remove();
    delete logging_task;
    logging_task = nullptr;
  }
}

void Data::log_init(){
  file.open(file_meta,ofstream::trunc | ofstream::out);
  if(!file.is_open()){
    printf("Log File not found\n");
    for(int i = 0; i< Data::obj_list.size(); i++){
      if(Data::obj_list[i]->log_location == log_locations::sd && int(Data::obj_list[i]->log_type))Data::obj_list[i]->log_location = log_locations::t;
    }
    return;
  }
  else{
    char meta_data[256];
    for(int i = 0; i< Data::obj_list.size(); i++){
      if((Data::obj_list[i]->log_location == log_locations::sd || Data::obj_list[i]->log_location == log_locations::both) && int(Data::obj_list[i]->log_type)){
        strcat(meta_data,Data::obj_list[i]->name);
        strcat(meta_data,",");
        strcat(meta_data,Data::obj_list[i]->id);
        strcat(meta_data,",");
      }
    }
    file.write(meta_data,strlen(meta_data));
    file.close();
    file.open(file_name,ofstream::app);
    file.close();
    logging_task_start();

  }
}


void Data::print(const char* format,...){
  char buffer[256];
  std::va_list args;
  va_start(args, format);
  int buffer_len = vsnprintf(buffer,256,format,args) + 3;
  va_end(args);
  // printf("%s, %d\n",this->name,this->log_type);
  if(int(this->log_type)){
    switch(log_location){
      case log_locations::t:
        printf("%s",buffer);
      break;
      case log_locations::sd:
        this->log_print(buffer, buffer_len);
      break;
      case log_locations::none:
      break;
      case log_locations::both:
        printf("%s",buffer);
        this->log_print(buffer, buffer_len);
      break;
    }
  }

}

void Data::log_print(char* buffer, int buffer_len){
  memcpy(buffer+buffer_len -3, this->id,3);
  //copying the string uses memcpy instead of strcpy or strncpy to avoid copying the terminating character

  //if the end of the buffer would be past the max of the queue array
  if(reinterpret_cast<uintptr_t>(back)+buffer_len > queue_start+queue_size-1){
    //copy the data that fills the queue
    memcpy(back, buffer, queue_size-1 - (reinterpret_cast<uintptr_t>(back) - queue_start));
    //creates a ptr to the last character used to fill the back of the queue
    char* overflow_ptr = buffer+(queue_size-1 - (reinterpret_cast<uintptr_t>(back) - queue_start));
    //fills the front of the queue with the remaining data from the buffer, marked by the overflow_ptr
    memcpy(queue,overflow_ptr,buffer_len- (queue_size-1 - (reinterpret_cast<uintptr_t>(back) - queue_start)));
    //moves back to the back of the data
    back =  buffer_len-queue_size+1 + back;
  }
  else{
    //copies data to the queue and moves the pointer down
    memcpy(back,buffer,buffer_len);
    back +=buffer_len;
  }

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
