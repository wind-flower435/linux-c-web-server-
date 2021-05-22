


# linux c++ tiny web server

* 该项目使用 **线程池** + **非阻塞socket** + **epoll(ET)** 的技术实现。
* 实现**异步日志系统**，记录服务器运行状态。
* 可以访问服务器数据库，实现web端用户注册、登录功能，可以请求服务器图片和视频文件，请求开启摄像头。
* 经过Webbench测试可以实现**上万的并发连接**数据交换
#### 运行

1.环境  
ubuntu:18.04  
Mysql:5.7.33  

2.运行前准备  
//建立db  
`create database yourdb;`  

// 创建user表  
`USE yourdb;
CREATE TABLE user(
    username char(50) NULL,
    passwd char(50) NULL
)ENGINE=InnoDB;`  



//修改main.cpp文件中对应的信息,数据库登录名,密码,库名，修改为你主机上mysql的用户名和密码，以下为范例：  
`string username = "username";`  
`string passwd = "passwd";`  
`string dbname = "dbname";`    

3.运行  
` make`   
` ./server 9907`   //9907为示例端口 

4.webbench压力测试


4.演示图片  
![welcome](https://user-images.githubusercontent.com/79500699/114193993-a0a60d80-9981-11eb-9844-bb4b9172d105.png)  

![select](https://user-images.githubusercontent.com/79500699/114194127-c3382680-9981-11eb-8827-4d4863f4a086.png)
