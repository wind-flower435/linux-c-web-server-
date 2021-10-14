#ifndef TIMER1_H
#define TIMER1_H

#include"headers.h"
#include"log.h"

class node
{
public:
  int sockfd;
  int epollfd;
  time_t expire;
  node* prev;
  node* next;
public:
~node();
node(int _sockfd,int _epollfd);
node(int _sockfd,int _epollfd,node* _prev,node* _next);

};




class arrange
{public:
  node* dummy_head;
  node* dummy_end;
  
  arrange();
  ~arrange();
  void append(node* n);
  void del_node();
  void del_node(int sockfd);
  void refresh(node* n);
  void refresh(int sockfd);
};


// { 
//   time_t t=time(NULL);
//   node* cur=dummy_head->next;
//   node* pre=dummy_head;
//   while(cur!=nullptr)
//   {
//     if(cur->expire <= t)
//     { node* _next=cur->next;
//       pre->next=_next;
//       if(_next!=nullptr)
//           _next->prev=pre;
//       node* tmp=cur;
//       cur=cur->next;
//       cout<<"delete node->sockfd:"<<tmp->sockfd<<endl;
//       delete(tmp);
//       tmp=nullptr;
//       continue;
//     }
//     else
//       break;
    
//     cur=cur->next;
//     pre=pre->next;
//   }
// }

// void arrange::refresh(node* n)
// {
//   time_t t=time(NULL)+2*timeslot;
//   n->expire=t;

//   node* pre=n->prev;
//   node* start=n->next;
  
//   // delete(n);
//   // n=nullptr;
  
//   while(start!=nullptr && start->expire<t)
//   {
//     start=start->next;
//     pre=pre->next;
//   }

//   node* _next=n->next;
//   n->prev->next=_next;
//   if(_next!=nullptr)
//       _next->prev=n->prev;

//   if(start!=nullptr)
//   {
//     n->next=start;
//     n->prev=start->prev;
//     start->prev->next=n;
//     start->prev=n;
//   }
//   else
//   {
//     pre->next=n;
//     n->prev=pre;
//   }
  
// }

#endif
