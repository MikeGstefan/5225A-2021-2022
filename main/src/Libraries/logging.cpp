#include "logging.hpp"
#include "task.hpp"

static const std::string file_name= "/usd/data.txt";
static const std::string file_meta= "/usd/meta_data.txt";
char queue[queue_size];
char* front = queue;
char* back = queue;

std::ofstream file;
uintptr_t queue_start = reinterpret_cast<uintptr_t>(&queue);
std::vector<Data*> Data::obj_list;
_Task Data::log_t(queue_handle, "logging");

Data::Data(std::string obj_name, int id_code, log_types log_type_param, log_locations log_location_param, term_colours print_colour, int print_type){
  id = '$' + std::to_string(id_code);
  id.insert(1, 3-id.length(), '0');
  this->name = obj_name;
  this->log_type = log_type_param;
  this->log_location = log_location_param;
  this->print_colour = print_colour;
  this->print_type = print_type;
  obj_list.push_back(this);
}

Data task_log("tasks.txt", 1, general, log_locations::both);
Data controller_queue("controller.txt", 2, general, log_locations::none);
Data tracking_data("tracking.txt", 3, general, log_locations::sd);
Data tracking_imp("tracking.txt", 3, general, log_locations::both);
Data misc("misc.txt", 4, general, log_locations::both);
Data drivers_data("driver.txt", 5, general, log_locations::none);
Data motion_i("motion.txt", 6, general, log_locations::both);
Data motion_d("motion.txt", 6, general, log_locations::both);
Data term("terminal.txt", 7, general, log_locations::t);
Data log_d("log.txt", 8, general, log_locations::both);
Data graph("graph.txt", 9, general, log_locations::sd);
Data state_log("state.txt", 10, general, log_locations::both);
Data ERROR("error.txt", 11, error, log_locations::both, term_colours::ERROR);
Data skills_d("skills.txt", 12, general, log_locations::both);
Data safety("safety.txt", 13, general, log_locations::both);

std::vector<Data*> Data::get_objs(){
  return obj_list;
}

void Data::init(){
  file.open(file_meta, std::ofstream::trunc | std::ofstream::out);
  if(!file.is_open()){
    printf2(term_colours::ERROR, "Log File not found");
    for(int i = 0; i < Data::obj_list.size(); i++){
      if(Data::obj_list[i]->log_location == log_locations::sd && int(Data::obj_list[i]->log_type) ==1) Data::obj_list[i]->log_location = log_locations::t;
      if(int(Data::obj_list[i]->log_type) ==2){
        if(Data::obj_list[i]->log_location == log_locations::both) Data::obj_list[i]->log_location = log_locations::t;
        else Data::obj_list[i]->log_type = off;
      }
    }
    return;
  }
  else{
    std::string meta_data;
    for(int i = 0; i < Data::obj_list.size(); i++){
      if((Data::obj_list[i]->log_location == log_locations::sd || Data::obj_list[i]->log_location == log_locations::both) && static_cast<int>(Data::obj_list[i]->log_type) != 0){
        meta_data = Data::obj_list[i]->name + ',' + Data::obj_list[i]->id + ',';
      }
    }
    file.write(meta_data.c_str(), meta_data.length());
    file.close();
    file.open(file_name, std::ofstream::trunc);
    file.close();
    Data::log_t.start();
  }
}

void Data::print(Timer* tmr, int freq, std::vector<std::function<std::string()>> str) const{
  if(tmr->get_time() > freq){
    for(int i = 0; i < str.size(); i++){
      this->print(str[i]());
    }
    tmr->reset();
  }
}

void Data::log_print(char* buffer, int buffer_len) const{
  memcpy(buffer + buffer_len-3, this->id.c_str(), 3);
  //copying the string uses memcpy instead of strcpy or strncpy to avoid copying the terminating character

  //if the end of the buffer would be past the max of the queue array
  if(reinterpret_cast<uintptr_t>(back) + buffer_len > queue_start + queue_size-1){
    //copy the data that fills the queue
    memcpy(back, buffer, queue_size-1 - (reinterpret_cast<uintptr_t>(back) - queue_start));
    //creates a ptr to the last character used to fill the back of the queue
    char* overflow_ptr = buffer + (queue_size-1 - (reinterpret_cast<uintptr_t>(back) - queue_start));
    //fills the front of the queue with the remaining data from the buffer, marked by the overflow_ptr
    memcpy(queue, overflow_ptr, buffer_len- (queue_size-1 - (reinterpret_cast<uintptr_t>(back) - queue_start)));
    //moves back to the back of the data
    back =  buffer_len-queue_size+1 + back;
  }
  else{
    //copies data to the queue and moves the pointer down
    memcpy(back, buffer, buffer_len);
    back +=buffer_len;
  }
}

void queue_handle(void* params){
  _Task* ptr = _Task::get_obj(params);
  Timer logging_tmr{"Logging Tmr"};
  char * temp_back;
  while(true){
    if(data_size() > print_point || logging_tmr.get_time() > print_max_time){
      temp_back = back;
      //does the queue rollover?
      if(reinterpret_cast<uintptr_t>(back) < reinterpret_cast<uintptr_t>(front)){
        file.open(file_name, std::ofstream::app);
        //print from the front to the end of the queue
        file.write(front, queue_size-1-( reinterpret_cast<uintptr_t>(front) - queue_start));
        //print from the start to the end of the data
        file.write(queue, reinterpret_cast<uintptr_t>(temp_back) - queue_start);
        file.close();
        front = queue + (reinterpret_cast<uintptr_t>(temp_back) - queue_start);
      }
      else{
        file.open(file_name, std::ofstream::app);
        //print the data marked to be printed
        file.write(front, reinterpret_cast<uintptr_t>(temp_back) -  reinterpret_cast<uintptr_t>(front));
        file.close();
        front += reinterpret_cast<uintptr_t>(temp_back) -  reinterpret_cast<uintptr_t>(front);
      }
      logging_tmr.reset();
    }
    delay(10);
    if(ptr->notify_handle()) break;
  }
}

uintptr_t data_size(){ //returns the number of characters needed to be printed from the queue
  uintptr_t size = reinterpret_cast<uintptr_t>(back) - reinterpret_cast<uintptr_t>(front);
  return size < 0 ? size + queue_size - 1 : size;
}