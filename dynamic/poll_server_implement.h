#include"basic_io_header.h"
#include<iostream>
#include<sys/poll.h>
using std::cout;
using std::endl;

#ifndef SELECT_IO_IMPLEMENT
#define SELECT_IO_IMPLEMENT

class PollIoImplement 
{
public:
	 PollIoImplement();
void loopFileDescriptorEvent(int nListenSocket ) ;

private:
void acceptNewClient( );
void recvClientMsg(int nFdIndex);

void removeInvalidFd(int nFd);
void addFd(int nFd);

private:
	int m_nListenSocket;
	int m_nCurrConnCount;
	pollfd m_fPollFd[MAX_CONN];
};
#endif








