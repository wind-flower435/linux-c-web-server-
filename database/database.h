#ifndef DATABASE_H
#define DATABASE_H
#include<mysql/mysql.h>
#include <thread>
#include <condition_variable>
#include"../misc/headers.h"

using std::string;

class database
{
private:
  database(){}
  MYSQL* conn;
  std::mutex m_mutex;
public:
  static database* getinstance()
  {
    static database instance;
    return &instance;
  }
  void initresult(string url, string User, string PassWord, string DatabaseName, int Port);

  bool insertdata(string name,string passwd);

  bool querydata(string name, string passwd);
  ~database();
};

#endif