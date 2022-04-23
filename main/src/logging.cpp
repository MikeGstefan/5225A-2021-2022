#include "logging.hpp"
#include "Libraries/printing.hpp"
const char* file_name= "/usd/data.txt";
const char* file_meta= "/usd/meta_data.txt";
char queue[queue_size];
char* front = queue;
char* back = queue;

ofstream file;
uintptr_t queue_start = reinterpret_cast<uintptr_t>(&queue);
vector<Data*> Data::obj_list;
_Task Data::log_t(queue_handle, "logging");

Data::Data(const char* obj_name, const char* id_code, log_types log_type_param, log_locations log_location_param, term_colours print_colour, int print_type){
  this->id = id_code;
  this->name = obj_name;
  this->log_type = log_type_param;
  this->log_location = log_location_param;
  this->print_colour = print_colour;
  this->print_type = print_type;
  obj_list.push_back(this);
}



Data task_log("tasks.txt","$01", general, log_locations::both);
Data controller_queue("controller.txt","$02", general,log_locations::none);
Data tracking_data("tracking.txt","$03",general,log_locations::sd);
Data tracking_imp("tracking.txt","$03",general,log_locations::both);
Data misc("misc.txt", "$04",general,log_locations::both);
Data drivers_data("driver.txt", "$05", general,log_locations::none);
Data motion_i("motion.txt","$06",general,log_locations::both);
Data motion_d("motion.txt", "$06", general,log_locations::both);
Data term("terminal.txt","$07",general,log_locations::t);
Data log_d("log.txt","$08",general,log_locations::both);
Data graph("graph.txt","$09",general,log_locations::sd);
Data events("events.txt", "$10", general,log_locations::both);
Data state_log("state.txt", "$11", general,log_locations::both);
Data ERROR("error.txt", "$12", error, log_locations::both);
Data skills_d("skills.txt", "$13", general, log_locations::both);
Data safety("safety.txt", "$14", general, log_locations::both);


vector<Data*> Data::get_objs(){
  return obj_list;
}


void Data::init(){
  file.open(file_meta,ofstream::trunc | ofstream::out);
  if(!file.is_open()){
    printf2(term_colours::RED, -1, "Log File not found");
    for(int i = 0; i< Data::obj_list.size(); i++){
      if(Data::obj_list[i]->log_location == log_locations::sd && int(Data::obj_list[i]->log_type) ==1)Data::obj_list[i]->log_location = log_locations::t;
      if(int(Data::obj_list[i]->log_type) ==2){
        if(Data::obj_list[i]->log_location == log_locations::both)Data::obj_list[i]->log_location= log_locations::t;
        else Data::obj_list[i]->log_type = off;
      }
    }
    return;
  }
  else{
    char meta_data[256];
    for(int i = 0; i< Data::obj_list.size(); i++){
      if((Data::obj_list[i]->log_location == log_locations::sd || Data::obj_list[i]->log_location == log_locations::both) && int(Data::obj_list[i]->log_type) !=0){
        strcat(meta_data,Data::obj_list[i]->name);
        strcat(meta_data,",");
        strcat(meta_data,Data::obj_list[i]->id);
        strcat(meta_data,",");
      }
    }
    file.write(meta_data,strlen(meta_data));
    file.close();
    file.open(file_name,ofstream::trunc);
    file.close();
    Data::log_t.start();

  }
}

void Data::print(Timer* tmr, int freq, std::vector<std::function<char*()>> str) const{
  if(tmr->get_time() > freq){
    for(int i = 0; i < str.size(); i++){
      char* buffer = str[i]();
      this->print(buffer);
      delete[] buffer;
    }
    tmr->reset();
  }
}

void Data::log_print(char* buffer, int buffer_len) const{
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
  _Task* ptr = _Task::get_obj(params);
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
    if(ptr->notify_handle())break;
  }


}

uintptr_t data_size(){//returns the number of characters needed to be printed from the queue
  if(reinterpret_cast<uintptr_t>(back) < reinterpret_cast<uintptr_t>(front))return(queue_size-1-( reinterpret_cast<uintptr_t>(front)-queue_start))+(reinterpret_cast<uintptr_t>(back)-queue_start); //Should be changeable to return (reinterpret_cast<uintptr_t>(back) - reinterpret_cast<uintptr_t>(front) + queue_size - 1);
  else return reinterpret_cast<uintptr_t>(back)- reinterpret_cast<uintptr_t>(front);
}


//cam be replaced with something from printing.hpp
char* Data::to_char(const char* fmt, ...){
  va_list args;
  va_start(args, fmt);
  char* buffer = new char[256];
  vsnprintf(buffer, 256, fmt, args);
  va_end(args);
  return buffer;
}
