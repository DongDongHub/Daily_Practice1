#include"select_server_implement.h"

SelectIoImplement::SelectIoImplement()
{
	m_nListenSocket = -1;
	m_nCurrConnCount = 0;
	bzero(m_aConn, sizeof(m_aConn ));
	FD_ZERO(&m_fReadSet);
} 

void SelectIoImplement::loopFileDescriptorEvent(int nListenSocket )
{
	m_nListenSocket = nListenSocket;
	int nMaxFd;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec =2000;

	while(1)
	{
		FD_ZERO(&m_fReadSet);
		FD_SET( m_nListenSocket, &m_fReadSet);
		nMaxFd = m_nListenSocket;
		for( auto i = 0; i < MAX_CONN; i++ )
		{
			FD_SET(m_aConn[i], &m_fReadSet);
			if( nMaxFd < m_aConn[i] )
			{
				nMaxFd =m_aConn[i];
			}			
		}

		int nRet = select(nMaxFd + 1, &m_fReadSet, NULL, NULL, &tv );
		if( nRet < 0 )
		{
			cout<<errno<<" "<<strerror(errno)<<endl;
			cout<<"select system call  err !"<<endl;
			exit(-1);
		}
		else if( nRet == 0 )
		{
			continue;
		}
		else
		{
			if( FD_ISSET(m_nListenSocket, &m_fReadSet )  )
			{
				acceptNewClient();
				continue;		
			}
			else
			{
				for( auto i = 0; i < MAX_CONN; i++ )
				{
					if( FD_ISSET(m_aConn[i], &m_fReadSet) )
					{
						recvClientMsg(i);	
					}
				}
			}
		}
	}
}

void SelectIoImplement::acceptNewClient()
{
	sockaddr_in clientSockaddr;
	socklen_t nLen = sizeof(sockaddr_in );	
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
		addFd(nClientSocket);
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

void SelectIoImplement::recvClientMsg(int nFdIndex)
{
	const int BUF_SIZE = 256;
	char buf[BUF_SIZE];
	bzero(buf, BUF_SIZE);
	int nLen = recv(m_aConn[nFdIndex], buf, BUF_SIZE, 0);
	if( nLen < 0 )
	{
		cout<<errno<<" "<<strerror(errno)<<endl;
		cout<<m_aConn[nFdIndex]<<" recv  system call   err !"<<endl;
		removeInvalidFd(nFdIndex);
		return;

	}
	else if( nLen == 0 )
	{
		cout<<m_aConn[nFdIndex]<<" recv  system call peer close connection   !"<<endl;
		removeInvalidFd(nFdIndex);
		return;
	}
	else
	{
		cout<<"recv : "<<buf;
		int nLen1 = send(m_aConn[nFdIndex], buf, nLen + 1 , 0);
		if( nLen1 != nLen + 1 )
		{
			cout<<errno<<" "<<strerror(errno)<<endl;
			cout<<m_aConn[nFdIndex]<<" send  system call   err !"<<endl;
			close(m_aConn[nFdIndex]);
			removeInvalidFd(nFdIndex);
		}
	}	
}

void SelectIoImplement::removeInvalidFd(int nFdIndex)
{
	m_nCurrConnCount--;
	m_aConn[ nFdIndex ] = 0;
	cout<<"The current fd set count: "<<m_nCurrConnCount<<endl;
}

void  SelectIoImplement::addFd(int nFd)
{
	int i = 0;
	while( m_aConn[i] )
	{
		i++;
	}
	m_aConn[i] = nFd;
	m_nCurrConnCount++;	
	cout<<"The current fd set count: "<<m_nCurrConnCount<<endl;
}

