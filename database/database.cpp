#include"database.h"

extern std::map<string, string>user_data;

void database::initresult(string url, string User, string PassWord, string DatabaseName, int Port)
{    
    conn = mysql_init(conn);
    if(conn==NULL)
      std::cout<<"create mysql connection failed"<<std::endl;
    conn=mysql_real_connect(conn,url.c_str(),User.c_str(),PassWord.c_str(),DatabaseName.c_str(),Port,NULL,0);
    
    if (mysql_query(conn, "SELECT username,passwd FROM user"))
    {std::cout<<"mysql init error"<<std::endl;}
    //从表中检索完整的结果集
    MYSQL_RES *result = mysql_store_result(conn);

    //返回结果集中的列数
    int num_fields = mysql_num_fields(result);

    //返回所有字段结构的数组
    MYSQL_FIELD *fields = mysql_fetch_fields(result);

    //从结果集中获取下一行，将对应的用户名和密码，存入map中
    while (MYSQL_ROW row = mysql_fetch_row(result))
    {
        string temp1(row[0]);
        string temp2(row[1]);
        user_data[temp1] = temp2;
    }
}

bool database::insertdata(string name,string passwd)
{
    std::lock_guard<std::mutex>lk(m_mutex);
    if(user_data.find(name)==user_data.end())
    {
      char *sql_insert = (char *)malloc(sizeof(char) * 200);
      strcpy(sql_insert, "INSERT INTO user(username, passwd) VALUES(");
      strcat(sql_insert, "'");
      strcat(sql_insert, name.c_str());
      strcat(sql_insert, "', '");
      strcat(sql_insert, passwd.c_str());
      strcat(sql_insert, "')");         
      
      user_data.insert({name,passwd});
      int res = mysql_query(conn, sql_insert);
      free (sql_insert);
      return res==0;//0-success
    }
    return false;
}

bool database::querydata(string name,string passwd )
{
    std::lock_guard<std::mutex>lk(m_mutex);
    if(user_data.find(name)!=user_data.end() && user_data.at(name)==passwd)
    { return true;}
        
    return false;

}

database :: ~database()
{mysql_close(conn);}