#include"timer.h"


extern int timeslot;
//class node;

node::node(int _sockfd=0,int _epollfd=0):sockfd(_sockfd),epollfd(_epollfd),prev(nullptr),next(nullptr)
{expire=time(NULL)+3*timeslot;}
node::node(int _sockfd,int _epollfd,node* _prev,node* _next):sockfd(_sockfd),epollfd(_epollfd),prev(_prev),next(_next)
{expire=time(NULL)+3*timeslot;}
node::~node(){}

arrange::arrange()
{
  dummy_head=new node(-1,-1);
  dummy_end=new node(-2,-2);
  dummy_head->next=dummy_end;
  dummy_end->prev=dummy_head;
}

arrange::~arrange()
{
  node* tmp=dummy_head;
  while(tmp!=nullptr)
  {
    dummy_head=tmp->next;
    delete(tmp);
    tmp=dummy_head;
  }
}

void arrange::append(node* n)
{
  n->next=dummy_end;
  n->prev=dummy_end->prev;
  dummy_end->prev->next=n;
  dummy_end->prev=n;    
}

void arrange::del_node(int sockfd)
{
  node* cur=dummy_head->next;
  while(cur!=nullptr &&cur->next!=nullptr)
  {
    if(cur->sockfd==sockfd)
    {
      cur->prev->next=cur->next;
      cur->next->prev=cur->prev;
      log::get_instance()->write_log("info ","------------timer process,initiative delete sockfd[%d]------------\n",cur->sockfd);
      delete(cur);
      cur=nullptr;
      break;
    }
    cur=cur->next;
  }
}


void arrange::refresh(int sockfd)
{
  node* cur=dummy_head->next;
  while(cur!=nullptr && cur->next!=nullptr)
  {    
    if(cur->sockfd==sockfd)
    {
      refresh(cur);
      break;
    }
    cur=cur->next;     
  }
}

void arrange::del_node()
{
  time_t t=time(NULL);
  node* cur=dummy_head->next;
  while(cur!=nullptr && cur->expire<=t && cur->next!=nullptr)
  {
    node* _next=cur->next;
    cur->prev->next=_next;
    cur->next->prev=cur->prev;
    log::get_instance()->write_log("info ","------------timer process,delete expire sockfd[%d]------------\n",cur->sockfd);
    epoll_ctl(cur->epollfd,EPOLL_CTL_DEL,cur->sockfd,0);
    close(cur->sockfd);
    delete(cur);
    cur=_next;
  }
}

void arrange::refresh(node* n)
{
  time_t t=time(NULL)+3*timeslot;
  n->expire=t;

  node* start=n->next;
  while(start!=nullptr &&start->next!=nullptr && start->expire<t)
  {
    start=start->next;
  }
  n->prev->next=n->next;
  n->next->prev=n->prev;
  if(start->next==nullptr)
  {
    n->next=start;
    n->prev=start->prev;
    start->prev->next=n;
    start->prev=n;
  }
  else
  {
    n->next=start->next;
    n->prev=start;
    start->next->prev=n;
    start->next=n;
  }  

}