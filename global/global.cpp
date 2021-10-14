#include"headers.h"

using std::string;
using std::to_string;


/*************global variable***************/
//string doc_root="./root/";                      
string doc_root="./resources/";             //
int pipefd[2];                              //
int timeslot=8;                             //
std::map<string,string>user_data;      //
/*************global variable***************/



/****************************global function****************************/


void setnonblocking(int fd)
{
    int flags=fcntl(fd,F_GETFL);
    flags|=O_NONBLOCK;
    fcntl(fd,F_SETFL,flags);
}

void addfd(int epollfd, int fd, bool one_shot, bool et )
{
    struct epoll_event ev;
    ev.data.fd=fd;
    ev.events = EPOLLIN | EPOLLRDHUP;   

    if (one_shot==true)
        ev.events |= EPOLLONESHOT;
    if(et==true)
        ev.events |=EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
    setnonblocking(fd);
}

void mod_fd(int epollfd,int fd,int e,bool et)
{
    struct epoll_event ev;
    ev.data.fd=fd;
    ev.events = e;
    if(et==true)
      ev.events|=EPOLLET;
    //epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
    setnonblocking(fd);
}

void sig_handler(int sig)
{
    int tmp=errno;
    string msg=to_string(sig);
    send(pipefd[0],msg.data(),msg.size(),0);
    errno=tmp;
}

void setpipeignore()
{
    struct sigaction sa;
    sigset_t mask;
    sigfillset(&mask);

    sa.sa_mask=mask;
    sa.sa_handler=SIG_IGN;
    sigaction(SIGPIPE,&sa,NULL);
}

void add_sig()
{
    struct sigaction sa;
    sigset_t mask;
    sigfillset(&mask);

    sa.sa_mask=mask;
    sa.sa_handler=sig_handler;
    sa.sa_flags=SA_RESTART;

    socketpair(AF_UNIX,SOCK_STREAM,0,pipefd);
    sigaction(SIGALRM,&sa,NULL);
}
/****************************global function****************************/