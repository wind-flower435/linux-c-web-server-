#server:	main.cpp ./http_conn/http_conn.cpp	./http_conn/http_conn.h	./timer/timer.cpp ./timer/timer.h	./log/log.cpp ./log/log.h	./html_parse/html_parse.cpp  \
#./html_parse/html_parse.h ./misc/global.cpp	./threadpool/threadpool.h ./database/database.cpp ./database/database.h
#				g++ -std=gnu++0x -g -o server main.cpp ./http_conn/http_conn.cpp	./http_conn/http_conn.h ./timer/timer.cpp  \
#./timer/timer.h ./log/log.cpp ./log/log.h ./html_parse/html_parse.cpp  \
#./html_parse/html_parse.h	./misc/global.cpp	./misc/headers.h ./threadpool/threadpool.h ./database/database.cpp ./database/database.h -lpthread -lmysqlclient

#clean:
#	rm  -r server
CFLAGS=-g
obj=main.o ./http_conn/http_conn.o ./timer/timer.o ./database/database.o ./misc/global.o ./log/log.o ./html_parse/html_parse.o
server: $(obj)
	g++ -std=gnu++0x -o server $(obj) -lpthread -lmysqlclient

%.o:%.cpp
	g++ -std=gnu++0x -c $^ -o $@


clean:
	rm  -r server $(obj)