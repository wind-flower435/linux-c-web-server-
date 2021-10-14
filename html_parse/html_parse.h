#ifndef HTML_PARSE_H
#define HTML_PARSE_H
#include"headers.h"
#include"log.h"
#include"database.h"

#include<unordered_set>


class html_parse
{
public:
  static const int FILENAME_LEN = 200;
  static const int READ_BUFFER_SIZE = 2048;
  static const int WRITE_BUFFER_SIZE = 1024;
  static const std::unordered_set<std::string> DEFAULT_HTML;
private:
  char* m_content_type=(char*)"*/*";
  int m_method=1;//0-get,1-post
  

  std::string m_read_buf;
  int m_read_idx=0;
  
  int m_sockfd;
  std::string sign;
  std::string data;
 
  
  
public:
  std::string m_file_address;
  char m_write_buf[WRITE_BUFFER_SIZE];
  int m_write_idx=0;
  int m_linger=1;//0-no keep alive,1-keep-alive;
public:
  html_parse():m_sockfd(-1){}
  html_parse(int _sockfd):m_sockfd(_sockfd){}
  void init();
  bool read_once();
  void process_read();
  void process_write();
  bool add_response(char* format,...);
  bool add_status_line();
  bool add_time();
  bool add_content_type();
  bool add_content_length(int content_length);
  bool add_connection();
  bool add_headers(int content_length);
  bool process_register();
  bool process_login();
};

#endif


