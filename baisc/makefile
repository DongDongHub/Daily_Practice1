basic_server:basic_server.o server_listen_initial.o select_server_implement.o poll_server_implement.o epoll_server_implement.o
	g++ -o basic_server basic_server.o server_listen_initial.o select_server_implement.o poll_server_implement.o epoll_server_implement.o  -g

basic_server.o:basic_server.cpp
	g++ -c basic_server.cpp -std=c++11 -g

server_listen_initial.o:server_listen_initial.h server_listen_initial.cpp
	g++ -c server_listen_initial.h server_listen_initial.cpp -std=c++11 -g

select_server_implement.o:select_server_implement.h select_server_implement.cpp
	g++ -c select_server_implement.h select_server_implement.cpp -std=c++11 -g

poll_server_implement.o:poll_server_implement.h poll_server_implement.cpp
	g++ -c  poll_server_implement.h poll_server_implement.cpp -std=c++11 -g

epoll_server_implement.o:epoll_server_implement.h epoll_server_implement.cpp
	g++ -c epoll_server_implement.h epoll_server_implement.cpp -std=c++11 -g

clean:
	rm basic_server.o basic_server server_listen_initial.o *.gch select_server_implement.o poll_server_implement.o
