#ifndef LOG_H
#define LOG_H
#include <queue>
#include <exception>
#include <thread>
#include <condition_variable>
#include<iostream>
#include<functional>
#include"headers.h"

using namespace std;


#define FILE_NAME 128
#define LINELEN 250

//

class log
{

private:
  
  static log* instance;
  static mutex m_mutex;
  mutex block_mutex;
  mutex rw_mutex;
  condition_variable cv;
  char log_name[FILE_NAME];
  char split_line[LINELEN];
  thread t;
  bool m_stop;
  queue<string >block_queue;
  FILE* f;
  int m_today;
  int m_count;
  log(){};
public:
  ~log();
  void start_work();
  void worker();
  void write_log(const char* level,const char *format, ...);
  void write_split(const char* format,...);
  void init();


  
  static log *get_instance()
  {
    if(instance==nullptr)
    {
      lock_guard<mutex> mk(m_mutex);
      if(instance==nullptr)
        instance=new log();      
    }
    return instance;
  }
  // static log* get_instance()
  // {
  //   static log instance;
  //   return &instance;
  // }

  


  
};



#endif
