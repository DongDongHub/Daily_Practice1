#include"poll_server_implement.h"

PollIoImplement::PollIoImplement()
{
	m_nListenSocket = -1;
	m_nCurrConnCount = 0;
	bzero(m_fPollFd, sizeof(m_fPollFd));
	for( int i = 0; i < MAX_CONN; i++ )	
	{
		m_fPollFd[i].fd = -1;
	}		
} 

void PollIoImplement::loopFileDescriptorEvent(int nListenSocket )
{

	m_nListenSocket = nListenSocket;
	int nMaxFd;
	m_fPollFd[0].fd = nListenSocket;
	m_fPollFd[0].events = POLLIN;
	m_nCurrConnCount++;
	
	while(1)
	{
		int nRet = poll(m_fPollFd, MAX_CONN , 50);
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
			if( m_fPollFd[0].revents & POLLIN  )
			{
				acceptNewClient();
				continue;		
			}
			else
			{
				for( int  i = 1; i < MAX_CONN; i++ )
				{
					if( m_fPollFd[i].revents & POLLIN  )
					{
						recvClientMsg(i);	
					}
				}
			}
		}
	}
}

void PollIoImplement::acceptNewClient()
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

void PollIoImplement::recvClientMsg(int nFdIndex)
{
	const int BUF_SIZE = 256;
	char buf[BUF_SIZE];
	bzero(buf, BUF_SIZE);
	int nLen = recv(m_fPollFd[nFdIndex].fd, buf, BUF_SIZE, 0);
	if( nLen < 0 )
	{
		cout<<errno<<" "<<strerror(errno)<<endl;
		cout<<m_fPollFd[nFdIndex].fd<<" recv  system call   err !"<<endl;
		removeInvalidFd(nFdIndex);
		return;

	}
	else if( nLen == 0 )
	{
		cout<<m_fPollFd[nFdIndex].fd<<" recv  system call peer close connection   !"<<endl;
		removeInvalidFd(nFdIndex);
		return;
	}
	else
	{
		cout<<"recv : "<<buf;
		int nLen1 = send(m_fPollFd[nFdIndex].fd, buf, nLen + 1 , 0);

		if( nLen1 != nLen + 1 )
		{
			cout<<errno<<" "<<strerror(errno)<<endl;
			cout<<m_fPollFd[nFdIndex].fd<<" send  system call   err !"<<endl;
			close(m_fPollFd[nFdIndex].fd);
			removeInvalidFd(nFdIndex);
		}
	}	
}

void PollIoImplement::removeInvalidFd(int nFdIndex)
{
	m_nCurrConnCount--;
	bzero(&m_fPollFd[nFdIndex], sizeof(pollfd) );
	m_fPollFd[ nFdIndex ].fd = -1;
	cout<<"The current fd set count: "<<m_nCurrConnCount<<endl;
}

void  PollIoImplement::addFd(int nFd)
{
	int i = 0;
	while( m_fPollFd[i].fd != -1 )
	{
		i++;
	}
	m_fPollFd[i].fd = nFd;
	m_fPollFd[i].events = POLLIN;
	m_nCurrConnCount++;	
	cout<<"The current fd set count: "<<m_nCurrConnCount<<endl;
}

