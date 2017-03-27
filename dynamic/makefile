basic_server:basic_server.o libserver_listen_initial.so libselect_server_implement.a libpoll_server_implement.a libepoll_server_implement.a
	g++ -o basic_server basic_server.o  -g -L. -lserver_listen_initial -lselect_server_implement -lpoll_server_implement -lepoll_server_implement

basic_server.o:basic_server.cpp
	g++ -c basic_server.cpp -std=c++11 -g

libserver_listen_initial.so:server_listen_initial.o
	g++ -shared   -o libserver_listen_initial.so server_listen_initial.o

server_listen_initial.o:server_listen_initial.h server_listen_initial.cpp
	g++ -c -fPIC  server_listen_initial.h server_listen_initial.cpp -std=c++11 -g


libselect_server_implement.a:select_server_implement.o
	ar cr libselect_server_implement.a select_server_implement.o
select_server_implement.o:select_server_implement.h select_server_implement.cpp
	g++ -c select_server_implement.h select_server_implement.cpp -std=c++11 -g

libpoll_server_implement.a:poll_server_implement.o
	ar cr libpoll_server_implement.a poll_server_implement.o

poll_server_implement.o:poll_server_implement.h poll_server_implement.cpp
	g++ -c  poll_server_implement.h poll_server_implement.cpp -std=c++11 -g

libepoll_server_implement.a:epoll_server_implement.o
	ar cr libepoll_server_implement.a epoll_server_implement.o

epoll_server_implement.o:epoll_server_implement.h epoll_server_implement.cpp
	g++ -c epoll_server_implement.h epoll_server_implement.cpp -std=c++11 -g

clean:
	rm  basic_server *.o *.gch *.a
