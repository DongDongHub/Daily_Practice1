#include"server_listen_initial.h"

InitialListeningStatus::InitialListeningStatus( const char* ipAddr, int nPort)
{
	m_pIpstr = ipAddr;
	m_nPort = nPort;
}

bool InitialListeningStatus::startListening(int& nFd, int nListenLength)
{
	makeListeningSocket();
	if( ( nFd = socket(PF_INET, SOCK_STREAM, 0) ) < 0)
	{
		cout<<"create socket fd err !"<<endl;
		return false;
	}	
	return setSocketAddrReuse(nFd) && bindSockAddr(nFd) && listenSockAddr(nFd, nListenLength );
}

void InitialListeningStatus::makeListeningSocket()
{
	m_sListenAddr.sin_port = htons(m_nPort);
	m_sListenAddr.sin_family = AF_INET;
	inet_pton(AF_INET, m_pIpstr, (void*)&m_sListenAddr.sin_addr);
}

bool InitialListeningStatus::setSocketAddrReuse(int nFd)
{
	int nTake = 1; 
	if(setsockopt(nFd, SOL_SOCKET , SO_REUSEADDR , &nTake, sizeof(nTake)) == -1)
	{
		cout<<"reset user port reuse flag  err !"<<endl;
		return false;
	}
	return true;
}

bool InitialListeningStatus::bindSockAddr( int nFd )
{
		if( bind(nFd, (const sockaddr*)&m_sListenAddr, sizeof(sockaddr_in)) == -1)
	{
		cout<<errno<<" "<<strerror(errno)<<endl;
		cout<<"bind listening socket address err !"<<endl;
		return false;
	}
	return true;
}

bool InitialListeningStatus::listenSockAddr( int nFd, int nLength )
{
	if( listen(nFd, nLength ) == -1 )
	{
		cout<<errno<<" "<<strerror(errno)<<endl;
		cout<<" listen socket address err !"<<endl;
		return false;
	}
	return true;
}




