#include"html_parse.h"


extern std::string doc_root;
extern std::map<std::string,std::string>users;

const unordered_set<string> html_parse::DEFAULT_HTML{
            "index", "register", "login",
             "welcome", "video", "picture", };

void html_parse::init()
{
  m_read_idx=0;
  m_write_idx=0;
  sign="";
  m_file_address="";
  m_content_type="*/*";
  m_method=0;
  m_read_buf="";
  data="";
  //memset(m_read_buf,0,sizeof(m_read_buf));
  memset(m_write_buf,0,sizeof(m_write_buf));

}

/*****************************************************************************************************/

bool html_parse::read_once()
{
  if(m_read_idx >=READ_BUFFER_SIZE) return false;
  char m_read_temp[READ_BUFFER_SIZE];
  int bytes_recved(0);
  while(1)
  {
    bytes_recved=recv(m_sockfd,m_read_temp + m_read_idx,READ_BUFFER_SIZE-m_read_idx,0);
    if(bytes_recved==-1 )
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      return false;
    }      
    if(bytes_recved==0)
    {
      log::get_instance()->write_log("warn ","read socketfd[%d] 0 bytes\n",m_sockfd);
      return false;
    }
    m_read_idx+=bytes_recved; 
  }
  m_read_buf=m_read_temp;
  return true;
}

/*****************************************************************************************************/

void html_parse::process_read()
{  
  // int count(0);
  // while(count<m_read_idx  && m_read_buf[count]!='\n')
  // {
  //   first_line.push_back(m_read_buf[count++]);
  // }
  // first_line.pop_back();//delete'\r'
  // int pos=first_line.find(' ');
  // if(first_line.substr(0,pos)=="GET")
  //   m_method=0;
  // else
  //   m_method=1;
  // count=pos+2;
  // while(count<m_read_idx && m_read_buf[count]!=' ')
  // {
  //   sign.push_back(m_read_buf[count++]);
  // }
  string first_line;
  int end_=m_read_buf.find("\r\n");
  first_line=m_read_buf.substr(0,end_);
  int method_pos=first_line.find(' ');

  if(first_line.substr(0,method_pos)=="GET")
    m_method=0;
  else
    m_method=1;

  ++method_pos;
  int sign_pos=first_line.find(' ',method_pos);
  ++method_pos;
  sign=first_line.substr(method_pos,sign_pos-method_pos);

  if(m_read_buf.find("keep-alive")==string::npos)
    m_linger=0;
  else
    m_linger=1;

  //process regist and login

  if(m_method==1)//1-post
  {
    int statusline_end=m_read_buf.find("\r\n\r\n");
    statusline_end+=4;
    data=m_read_buf.substr(statusline_end,(int)m_read_buf.size()-statusline_end);
  }
    

} 

/*****************************************************************************************************/
bool html_parse::process_register()
{

  if(data.empty()) return false;
  string name,passwd;
  int nameend=data.find("&");
  int namestart=data.find('=');
  ++namestart;
  name=data.substr(namestart,nameend-namestart);

  int passwdstart=data.find('=',nameend);
  ++passwdstart;
  passwd=data.substr(passwdstart,(int)data.size()-passwdstart);
  if(sign=="register")
  {
    return database::getinstance()->insertdata(name,passwd);
  }
  return false;
  
}

/*****************************************************************************************************/
bool html_parse::process_login()
{
  if(data.empty()) return false;
  string name,passwd;
  int nameend=data.find("&");
  int namestart=data.find('=');
  ++namestart;
  name=data.substr(namestart,nameend-namestart);

  int passwdstart=data.find('=',nameend);
  ++passwdstart;
  passwd=data.substr(passwdstart,(int)data.size()-passwdstart);
  if(sign=="login")
  {
    return database::getinstance()->querydata(name,passwd);
  }
  return false;

}
/*****************************************************************************************************/
void html_parse::process_write()
{

  if(m_method==1)
  {
    if(sign=="register")
    {
      m_file_address=process_register()?"login":"registerError";
      m_file_address=doc_root+m_file_address+".html";
    }
    else if(sign=="login")
    {
      m_file_address=process_login()?"fans":"loginError";
      m_file_address=doc_root+m_file_address+".html";
    }  
  }
  else if(sign.empty())
    m_file_address=doc_root+"welcome.html";
  else if(DEFAULT_HTML.find(sign)!=DEFAULT_HTML.end())
    m_file_address=doc_root + *DEFAULT_HTML.find(sign)+".html";
  else
    m_file_address=doc_root+sign;

  log::get_instance()->write_log("info ","the client socketfd[%d] request resource is: %s\n",m_sockfd, m_file_address.data());

  int fd=open((char*)m_file_address.data(),O_RDONLY);
  struct stat sb;
  stat((char*)m_file_address.data(),&sb);
  close(fd); 
  add_headers(sb.st_size);
}

/*****************************************************************************************************/

bool html_parse::add_response(char* format,...)
{
  if(m_write_idx>=WRITE_BUFFER_SIZE)
    return false;
  va_list arg_list;
  va_start(arg_list, format);
  int len =vsnprintf(m_write_buf+m_write_idx,WRITE_BUFFER_SIZE-m_write_idx-1,format,arg_list);
  m_write_idx+=len;
  va_end(arg_list);
  return true;
}

/*****************************************************************************************************/

bool html_parse::add_status_line()
{
  if(add_response("%s\n","HTTP/1.1 200 OK"))
    return true;
  return false;
}

/*****************************************************************************************************/

bool html_parse::add_time()
{
  time_t t;
  t=time(NULL);
  struct tm* tm_ptr;
  tm_ptr=gmtime(&t);
  char outstr[60];
  strcpy(outstr,"date: ");
  strftime(outstr+6,50-1-6,"%a, %d %b %Y %T ",tm_ptr);
  strcat(outstr,"GMT\n");
  add_response("%s",outstr);
  return true;
}

/*****************************************************************************************************/

bool html_parse::add_content_type()
{
  if(add_response("%s%s\n\n","content-type: ",m_content_type))
    return true;
  return false;
}

/*****************************************************************************************************/

bool html_parse::add_content_length(int content_length)
{
  if(add_response("%s%d\n","content-length: ",content_length))
    return true;
  return false;
}

/*****************************************************************************************************/

bool html_parse::add_connection()
{
  if(m_linger==1)
  {
    if(add_response("%s%s\n","connection: ","keep-alive"))
      return true;
    return false;
  }  
  return true;
}

/*****************************************************************************************************/

bool html_parse::add_headers(int content_length)
{
  add_status_line();
  add_time();
  add_connection();
  add_content_length(content_length);
  add_content_type();
  return true; 
}

/*****************************************************************************************************/
