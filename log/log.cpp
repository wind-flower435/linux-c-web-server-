#include"log.h"

mutex log::m_mutex;
log* log::instance=nullptr;

log::~log()
{
  if(f!=nullptr)
    fclose(f);
  m_stop=true;
  cv.notify_one();
}

void log::start_work()
{
  t=thread(&log::worker,this);
  t.detach();
}

void log::worker()
{
  unique_lock<mutex>lk(block_mutex);
  while(!m_stop)
  {    
    if(block_queue.empty())
    {cv.wait(lk);}
    if(!block_queue.empty())
    {
        string task = (block_queue.front());
        block_queue.pop();
        fputs(task.data(),f);
        fflush(f);
    }   
  }

}

void log::write_log(const char* level,const char *format, ...)
{
  time_t t=time(NULL);
  struct tm* st=localtime(&t);
  int len(0);
  int line_idx(0);

  rw_mutex.lock();
  len=snprintf(split_line+line_idx,LINELEN-line_idx-1,"%02d-%02d-%02d",st->tm_hour,st->tm_min,st->tm_sec);
  line_idx+=len;
  len=snprintf(split_line+line_idx,LINELEN-line_idx-1,"[%s]: ",level);
  line_idx+=len;    
  rw_mutex.unlock();

  va_list arg_list;
  va_start(arg_list, format);

  rw_mutex.lock();
  len=vsnprintf(split_line+line_idx,LINELEN-line_idx-1,format,arg_list);
  line_idx+=len;
  block_queue.push(split_line);
  rw_mutex.unlock();

  cv.notify_one();

}

void log::write_split(const char* format,...)
{
  int line_idx=0;
  int len=0;
  va_list arg_list;
  va_start(arg_list, format);

  rw_mutex.lock();
  len=vsnprintf(split_line+line_idx,LINELEN-line_idx-1,format,arg_list);
  line_idx+=len;
  block_queue.push(split_line);
  rw_mutex.unlock();

  cv.notify_one();
}

void log::init()
{
  m_count=0;
  m_stop=false;
  time_t t=time(NULL);
  struct tm* st=localtime(&t);
  if(m_count==0)
  {      
    snprintf(log_name,FILE_NAME,"%d_%02d_%02d_Log",st->tm_year+1900,st->tm_mon+1,st->tm_mday);
    f=fopen(log_name,"a");
  }
}
