
#include"http_conn/http_conn.h"
#include"threadpool/threadpool.h"
#include"timer/timer.h"
#include"log/log.h"
#include"database/database.h"

#include<unordered_map>


#define MAX_EVENT_NUMBER 100
   
using namespace std;

extern void setnonblocking(int fd);
extern void mod_fd(int epollfd,int fd,int e,bool et);
extern void addfd(int epollfd, int fd, bool one_shot, bool et);
extern void sig_handler(int sig);
extern void add_sig();
void setpipeignore();

extern int pipefd[2];
extern int timeslot;

static arrange timing_arrrange;


int main(int argc,char* argv[])
{
    int listenfd,connfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);
    int port=atoi(argv[1]);

    
    struct sockaddr_in address;
    memset(&address,0,sizeof(struct sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);//0.0.0.0
    address.sin_port = htons(port);

    int flag = 1;
    int ret=0;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    ret=bind(listenfd, (struct sockaddr *)&address, sizeof(address));
    ret = listen(listenfd, 5);

    //database setting
    string username="root";
    string passwd="961231520lf";
    string dbname="htmldb";
    int db_port=3306;
    database::getinstance()->initresult("localhost",username,passwd,dbname,db_port);

    //创建内核事件表
    struct epoll_event events[MAX_EVENT_NUMBER];
    int  epollfd = epoll_create(50);
    assert(epollfd != -1);
    addfd(epollfd,listenfd,false,false);

    //log setting
    log::get_instance()->init();
    log::get_instance()->start_work();    

    //pipe setting
    add_sig();
    addfd(epollfd,pipefd[1],false,true);
    setnonblocking(pipefd[0]);
    char time_recv_buf[10];
    alarm(timeslot);
    setpipeignore();
    bool timeout(false); 
        
    //create threadpool object    
    unordered_map<int,http_conn>user;  
    threadpool pool(8,2,3);//init threads,default_step,default_time
    pool.open_arrange_threads();

    while(1)
    {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if (errno != EINTR && number<0)
        {
           cout<<"errno:"<<errno<<endl;
           log::get_instance()->write_log("error","epollfd errno: %d\n",errno);
           break;
        }
        for(int i=0;i<number;i++)
        {
            int socketfd=events[i].data.fd;
            if(socketfd==listenfd)
            {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
                connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
                addfd(epollfd,connfd,true,true);
                //user[connfd]=http_conn(epollfd,connfd);
                user.insert({connfd,http_conn(epollfd,connfd)});
                timing_arrrange.append(new node(connfd,epollfd));
                log::get_instance()->write_log("info ","shake hands three times with sockfd[%d] done\n",connfd);
            }
            else if (events[i].events&EPOLLIN && socketfd==pipefd[1])
            {
                
                int ret=recv(pipefd[1],time_recv_buf,sizeof(time_recv_buf),0);
                if(ret==-1||ret==0) continue;
                int sig(0);
                for(int i=0;i<ret;i++)
                {
                    sig=sig*10+time_recv_buf[i]-'0';
                }
                switch (sig)
                {
                    case SIGALRM:
                    {
                      timeout=true;
                      break;
                    }
                    case SIGINT:
                    {break;}
                    default: break;
                }    
            }     
            else if (events[i].events & (EPOLLRDHUP| EPOLLHUP | EPOLLERR))
            {
                timing_arrrange.del_node(socketfd);
                log::get_instance()->write_log("info ","the other side have closed,delete socketfd[%d]\n",socketfd);
                epoll_ctl(epollfd,EPOLL_CTL_DEL,socketfd,0);
                user.erase(socketfd);
                close(socketfd);
            }       
            else if(events[i].events & EPOLLIN)
            {
               
                if(user.at(socketfd).m_html_parse.read_once())
                {                   
                    log::get_instance()->write_log("info ","recved requests from sockfd:%d\n",socketfd); 
                    bool flag=pool.append(bind(&http_conn::process,&user.at(socketfd) ));
                    if(!flag) break;
                    timing_arrrange.refresh(socketfd);
                }       
                else
                {                       
                    timing_arrrange.del_node(socketfd);
                    log::get_instance()->write_log("warn ","the socketfd[%d] read error,delete socketfd\n",socketfd);
                    epoll_ctl(epollfd,EPOLL_CTL_DEL,socketfd,0);
                    user.erase(socketfd);
                    close(socketfd);                    
                   
                }             
            }            
            else if(events[i].events & EPOLLOUT)
            {    
                if(user.at(socketfd).m_write())
                {
                    timing_arrrange.refresh(socketfd);
                    log::get_instance()->write_split("\n\n","");
                }
                else
                {
                    timing_arrrange.del_node(socketfd);
                    epoll_ctl(epollfd,EPOLL_CTL_DEL,socketfd,0);
                    user.erase(socketfd);
                    close(socketfd);
                    log::get_instance()->write_split("\n\n","");
                }                
                
            }
            

        }

        if(timeout)
        {
            timing_arrrange.del_node();
            alarm(timeslot);
            timeout=false;
            log::get_instance()->write_log("info ","------------timer process,the numbers of clients: %d------------\n",(int)user.size());
        }
    
    }

    return 0;
}

