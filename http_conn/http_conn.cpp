#include"http_conn.h"


using namespace std;


extern void mod_fd(int epollfd,int fd,int e,bool et);

extern string doc_root;


 
void http_conn::init()
{
  pos=0;
  bytes_have_send=0;
}

bool http_conn::m_write()
{
  int fd=open(m_html_parse.m_file_address.data(),O_RDONLY);
  struct stat sb;
  stat((char*)m_html_parse.m_file_address.data(),&sb);
  if(bytes_have_send==0)
    write(m_socketfd,m_html_parse.m_write_buf,m_html_parse.m_write_idx);
  int temp=0;
  
  while(1)
  {
    temp=sendfile(m_socketfd,fd,&pos,sb.st_size);
    if(temp<0 )
    {
      if(errno==11)
      {
        log::get_instance()->write_log("warn ","send socketfd[%d] %s have a errno[%d],send again\n",m_socketfd,m_html_parse.m_file_address.data(), errno);
        mod_fd(m_epollfd,m_socketfd,EPOLLOUT|EPOLLONESHOT,true);
        return true;
      }
      else
      {
        log::get_instance()->write_log("error","send socketfd[%d] %s have a error, errno[%d]\n",m_socketfd,m_html_parse.m_file_address.data(),errno);
        close(fd);
        init();
        m_html_parse.init();
        return false;
      }      
    }      
    else if(temp==0)
      break; 
    else
      bytes_have_send+=temp;
         
  }
  close(fd);
  log::get_instance()->write_log("info ","send socketfd[%d] %s:%d bytes\n",m_socketfd,m_html_parse.m_file_address.data(),bytes_have_send);
  init();
  m_html_parse.init();
  if(m_html_parse.m_linger==1)
  {
    mod_fd(m_epollfd,m_socketfd,EPOLLIN|EPOLLONESHOT,true);  
    return true;
  }
  return false;
}

void http_conn::process()
{
  m_html_parse.process_read();
  m_html_parse.process_write();
  mod_fd(m_epollfd,m_socketfd,EPOLLOUT|EPOLLONESHOT,true);//buffer full,than epollshot not chufa,so set EPOLLOUT
  //cout<<"thread id: "<<std::this_thread::get_id()<<" finish task"<<endl;  
}















