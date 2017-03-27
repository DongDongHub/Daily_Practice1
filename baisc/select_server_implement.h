#include"basic_io_header.h"
#include<iostream>
using std::cout;
using std::endl;

#ifndef SELECT_IO_IMPLEMENT
#define SELECT_IO_IMPLEMENT

class SelectIoImplement 
{
public:
	 SelectIoImplement();
void loopFileDescriptorEvent(int nListenSocket ) ;

private:
void acceptNewClient( );
void recvClientMsg(int nFdIndex);

void removeInvalidFd(int nFd);
void addFd(int nFd);

private:
	int m_nListenSocket;
	int m_nCurrConnCount;
	int m_aConn[MAX_CONN];

	fd_set m_fReadSet;

};
#endif








