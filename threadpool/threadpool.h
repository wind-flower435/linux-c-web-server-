#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <queue>
//#include <cstdio>
#include <exception>
#include <thread>
#include <condition_variable>
#include<iostream>
#include<functional>
#include"../http_conn/http_conn.h"

using namespace std;


class threadpool
{private:
  int m_thread_number;
  //int m_max_requests;
  std::vector<std::thread> m_pool;
  queue<function<void()> >m_workqueue;
  std::mutex m_mutex;
  std::mutex m_queuelock;
  std::condition_variable cv;
  bool m_stop;
public:
  threadpool(int _m_thread_number):m_thread_number(_m_thread_number),m_stop(false)
  {
    for(int i=0;i<m_thread_number;i++)
    {
      //m_pool.push_back(thread(&threadpool::worker,this));
      m_pool.emplace_back(move(thread(&threadpool::worker,this)));
      m_pool.at(i).detach();
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

  // void worker()
  // {
  //   while (!m_stop)
  //   {
  //     unique_lock<mutex>lk(m_queuelock);
  //     if(m_workqueue.empty())
  //     {
  //       cv.wait(lk);
  //       //continue;
  //     }
  //     if(!m_workqueue.empty())
  //     {
  //       http_conn* request=move(m_workqueue.front());
  //       m_workqueue.pop();
  //       lk.unlock();
  //       request->process();
  //       lk.lock();
  //     }    
  //   }  
  // }
  void worker()
  {
    unique_lock<mutex>lk(m_queuelock);
    while (!m_stop)
    {
      
      if(m_workqueue.empty())
      {
        cv.wait(lk);
      }
      if(!m_workqueue.empty())
      {
        function<void()>task=move(m_workqueue.front());
        m_workqueue.pop();
        lk.unlock();
        task();
        lk.lock();
      }    
    }
  }
  // bool append(http_conn* request)
  // {
  //   //lock_guard<mutex>lk(m_mutex);
  //   unique_lock<mutex>lk(m_mutex);
  //   if(m_workqueue.size()>=m_max_requests)
  //   {/*cv.wait(lk);*/return false;}
  //   m_workqueue.push(request);
  //   cv.notify_one();
  //   return true;
  // }
  template<typename T>
  bool append(T&& request)
  {
    unique_lock<mutex>lk(m_mutex);
    //if(m_workqueue.size()>=m_max_requests)
    //{return false;}
    m_workqueue.emplace(forward<T>(request));
    cv.notify_one();
    return true;
  }
};




#endif