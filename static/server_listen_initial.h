#include"basic_io_header.h"
#include<iostream>
#include<stdio.h>

using std::cout;
using std::endl;


#ifndef INITIAL_LISTENING_STATUS
#define INITIAL_LISTENING_STATUS
class InitialListeningStatus
{
	public:
		InitialListeningStatus( const char* ipAddr, int nPort );
		bool startListening(int &nFd, int nListenLength);
;

	private:
		void makeListeningSocket();
		bool setSocketAddrReuse( int nFd);
		bool  bindSockAddr( int nFd);
		bool listenSockAddr( int nFd, int nLength );


	private:  //use to store listen address and port.
		sockaddr_in m_sListenAddr;
		int			m_nPort;
		const char *m_pIpstr;
};

#endif

