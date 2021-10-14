#ifndef HTTP_CONN_H
#define HTTP_CONN_H
#include"headers.h"
#include"html_parse.h"
#include"log.h"
/**************************************
  bool read_once();
  void process_read();
  void process_write();
  
  bool add_status_line();
  bool add_headers(int content_length);
  bool add_time();
  bool add_content_type();
  bool add_content_length(int content_length);
  bool add_connection();
  bool add_response(char* format,...);
  std::string m_file_address;
  std::string first_line;
**************************************/
class http_conn
{
public:
  html_parse m_html_parse;
  http_conn(){}
  http_conn(int epollfd,int socketfd):pos(0), bytes_have_send(0),m_html_parse(socketfd), m_epollfd(epollfd),m_socketfd(socketfd)
  {}
  ~http_conn()
  {
    // if(m_html_parse)
    //   delete(m_html_parse);
    // m_html_parse=NULL;
  }
  void process();
  void init();
  bool m_write();
  
private:
  // char m_read_buf[READ_BUFFER_SIZE];
  // char m_write_buf[WRITE_BUFFER_SIZE];
  // int m_read_idx;
  // int m_write_idx;
  // char* m_keep_status="keep-alive";
  // int m_method;//0-get,1-post

  int m_epollfd;
  int m_socketfd;
  int bytes_have_send;   
  off_t pos;
  



};

#endif