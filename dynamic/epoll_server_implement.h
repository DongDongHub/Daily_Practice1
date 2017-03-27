#include"basic_io_header.h"
#include<iostream>
#include<sys/epoll.h>
using std::cout;
using std::endl;

#ifndef EPOLL_IO_IMPLEMENT
#define EPOLL_IO_IMPLEMENT

class EpollIoImplement 
{
public:
	 EpollIoImplement();
void loopFileDescriptorEvent(int nListenSocket ) ;

private:
void acceptNewClient( );
void recvClientMsg(int nFdIndex);

bool  addFd(int nFd, int op , uint32_t nEvents);

private:
	int m_nListenSocket;
	int m_nCurrConnCount;
	int m_nEpFd;
	epoll_event m_epollEvent;
	epoll_event m_aEpollEvent[MAX_CONN];
};
#endif








