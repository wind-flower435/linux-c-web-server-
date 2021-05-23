
CFLAGS=-g
obj=main.o ./http_conn/http_conn.o ./timer/timer.o ./database/database.o ./misc/global.o ./log/log.o ./html_parse/html_parse.o
server: $(obj)
	g++ -std=gnu++0x -o server $(obj) -lpthread -lmysqlclient

%.o:%.cpp
	g++ -std=gnu++0x -c $^ -o $@


clean:
	rm  -r server $(obj)
