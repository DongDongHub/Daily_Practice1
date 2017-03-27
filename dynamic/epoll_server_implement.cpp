#include"epoll_server_implement.h"

EpollIoImplement::EpollIoImplement()
{
	m_nEpFd = -1;
	m_nListenSocket = -1;
	m_nCurrConnCount = 0;
	bzero(&m_epollEvent, sizeof(m_epollEvent));
	bzero(m_aEpollEvent, sizeof( m_epollEvent ) * MAX_CONN ); 
} 

void EpollIoImplement::loopFileDescriptorEvent(int nListenSocket )
{

	m_nListenSocket = nListenSocket;
	m_nEpFd = epoll_create(MAX_CONN);
	if( !addFd(m_nListenSocket, EPOLL_CTL_ADD, EPOLLIN))
	{
		exit(-1);
	} 

	while(1)
	{
		int nRet = epoll_wait(m_nEpFd, m_aEpollEvent, MAX_CONN, 100); 
		if( nRet < 0 )
		{
			cout<<errno<<" "<<strerror(errno)<<endl;
			cout<<"epoll_wait system call  err !"<<endl;
			exit(-1);
		}
		else if( nRet == 0 )
		{
			continue;
		}
		else
		{
			for( auto i = 0; i < nRet; i++ )
			{
				if( m_aEpollEvent[i].events & EPOLLIN  )
				{
					if( m_aEpollEvent[i].data.fd == m_nListenSocket )
					{
						acceptNewClient();
						continue;
					}
					recvClientMsg(i);	
				}
				else
				{
					cout<<" unhandle event : "<<m_aEpollEvent[i].events<<endl;
				}
			}
		}
	}
}
void EpollIoImplement::acceptNewClient()
{
	sockaddr_in clientSockaddr;
	socklen_t nLen = sizeof( sockaddr_in );	
	bzero(&clientSockaddr,  sizeof(clientSockaddr));


	int nClientSocket = accept(m_nListenSocket,( struct sockaddr* ) &clientSockaddr, &nLen);  
	if( nClientSocket == -1 )	
	{
		cout<<errno<<" "<<strerror(errno)<<endl;
		cout<<" accept system call   err !"<<endl;
		return;
	}
	if( m_nCurrConnCount < MAX_CONN )
	{	
		addFd(nClientSocket, EPOLL_CTL_ADD, EPOLLIN);
		char strClient[INET_ADDRSTRLEN + 1];
		bzero(strClient, INET_ADDRSTRLEN + 1);
		int nPort = ntohs( clientSockaddr.sin_port );
		inet_ntop(AF_INET, &clientSockaddr.sin_addr, strClient, INET_ADDRSTRLEN );
		cout<<"client connection from "<<strClient<<" : "<<nPort<<endl;
	}
	else
	{

		const char* refusedMsg = "The server connection is already overload !";
		cout<<refusedMsg<<endl;
		send(nClientSocket, refusedMsg, strlen(refusedMsg)+1, 0);
		close(nClientSocket);
	}
}

void EpollIoImplement::recvClientMsg(int nFdIndex)
{
	const int BUF_SIZE = 256;
	char buf[BUF_SIZE];
	bzero(buf, BUF_SIZE);
	int nLen = recv(m_aEpollEvent[nFdIndex].data.fd, buf, BUF_SIZE, 0);
	if( nLen < 0 )
	{
		cout<<errno<<" "<<strerror(errno)<<endl;
		cout<<m_aEpollEvent[nFdIndex].data.fd<<" recv  system call   err !"<<endl;
		addFd(m_aEpollEvent[nFdIndex].data.fd, EPOLL_CTL_DEL, EPOLLIN); 
		return;

	}
	else if( nLen == 0 )
	{
		cout<<m_aEpollEvent[nFdIndex].data.fd<<" recv  system call peer close connection   !"<<endl;
		addFd(m_aEpollEvent[nFdIndex].data.fd, EPOLL_CTL_DEL, EPOLLIN); 
		return;
	}
	else
	{
		cout<<"recv : "<<buf;
		int nLen1 = send(m_aEpollEvent[nFdIndex].data.fd, buf, nLen + 1 , 0);
		if( nLen1 != nLen + 1 )
		{
			cout<<errno<<" "<<strerror(errno)<<endl;
			cout<<" send  system call   err !"<<endl;
			addFd(m_aEpollEvent[nFdIndex].data.fd, EPOLL_CTL_DEL, EPOLLIN); 
			close( m_aEpollEvent[nFdIndex].data.fd  );
		}
	}	
}


bool  EpollIoImplement::addFd(int nFd, int op , uint32_t nEvents)
{
	m_epollEvent.events = nEvents;
	m_epollEvent.data.fd = nFd;
	if(	epoll_ctl(m_nEpFd, op, nFd, &m_epollEvent) == -1)
	{			
		cout<<m_epollEvent.events<<" send  system call   err !"<<endl;
		return false;
	}

	if( op == EPOLL_CTL_ADD  )
	{
		m_nCurrConnCount++;	
		cout<<"The current fd set count: "<<m_nCurrConnCount<<endl;
	}
	else if( op == EPOLL_CTL_DEL )
	{
		m_nCurrConnCount--;	
		cout<<"The current fd set count: "<<m_nCurrConnCount<<endl;
	}
	else if( op == EPOLL_CTL_MOD ) 
	{	
		cout<<" epoll modify the data :" <<op<<endl;
	}
	return true;
}

