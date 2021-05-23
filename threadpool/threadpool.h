#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <queue>
#include <exception>
#include <thread>
#include <condition_variable>
#include<iostream>
#include<functional>
#include<unistd.h>
using namespace std;



class threadpool
{
private:
  int min_num;
  int max_num;
  int live_num;
  int busy_num;
  int default_step;
  int init_num;
  int wait_exit_num;
  int default_time;
  std::thread arrange; 
  queue<function<void()> >m_workqueue;
  std::mutex m_mutex;
  std::mutex m_queuelock;
  std::condition_variable cv;
  bool m_stop;
public:
  threadpool(int _init_num,int _default_step, int _default_time):
  init_num(_init_num),min_num(_init_num),max_num(20),live_num(_init_num),
  busy_num(0),default_step(_default_step),wait_exit_num(0),default_time(_default_time), m_stop(false)
  {
    for(int i=0;i<init_num;i++)
    {
      thread(&threadpool::worker,this).detach();
    }
  }

   ~threadpool()
  {
    {
      lock_guard<mutex>lk(m_mutex);
      m_stop=true;
    }
    cv.notify_all();
  }

  bool open_arrange_threads()
  {
    arrange=thread(&threadpool::arrange_threads,this);
    arrange.detach();    
  }

  void arrange_threads()
  {
    while(!m_stop)
    {
      this_thread::sleep_for(chrono::milliseconds(default_time*1000));
      if(m_workqueue.size()>min_num && live_num<max_num)
        create_new_threads();
      else if(busy_num*2 < live_num && live_num>min_num)
        destory_threads();
    }    
  }

  void create_new_threads()
  {
    for(int i=0;i<default_step;i++)
    {
      thread(&threadpool::worker,this).detach();
      ++live_num;
    }
  }

  void destory_threads()
  {
    wait_exit_num=default_step;
    for(int i=0;i<default_step;i++)
    {      
      cv.notify_one();
    }
    wait_exit_num=0;
  }

  void worker()
  {
    unique_lock<mutex>lk(m_queuelock);
    while (!m_stop)
    {      
      if(m_workqueue.empty())
      {
        cv.wait(lk);
      }
      if(wait_exit_num>0)
      {
        --wait_exit_num;
        --live_num;
        break;
      }
      if(!m_workqueue.empty())
      {
        ++busy_num;
        function<void()>task=move(m_workqueue.front());
        m_workqueue.pop();
        lk.unlock();
        task();
        lk.lock();
        --busy_num;
      }    
    }
  }

  template<typename T>
  bool append(T&& request)
  {
    unique_lock<mutex>lk(m_mutex);
    m_workqueue.emplace(forward<T>(request));
    cv.notify_one();
    return true;
  }


};

#endif