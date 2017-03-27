#include"basic_io_header.h"
#include<iostream>
#include<stdio.h>
#include"server_listen_initial.h"
#include"poll_server_implement.h"
#include"select_server_implement.h"
#include"epoll_server_implement.h"

using std::cout;
using std::endl;


/*
void makeListeningSocket(const char* ip, int nPort, sockaddr_in * addr ,int *nListenSocket);
void setSocketAddrReuse( int nFd);
void bindSockAddr( int nFd, sockaddr_in* addr );
void listenSockAddr( int nFd, int nLength );
*/
/*
void loopFileDescriptorEvent(int nListenSocket );
void acceptNewClient( int nFd, int *nConnCount, int *a );
void recvClientMsg(int nFd, int * nConnCount, int *a);


void removeInvalidFd(int nFd, int *nConnCount, int *a);
bool  addFd(int nFd, int *nConnCount, int *a);
*/
int main(int argc ,char ** argv)
{

	const char* localIpAddress="192.168.1.102";
	int nListenSocket;
	InitialListeningStatus initListenStatus(localIpAddress, DEFAULT_PORT);
	if (! initListenStatus.startListening(nListenSocket, 10 ) )
	{
		exit(-1);
	}
/*
	makeListeningSocket(localIpAddress, DEFAULT_PORT, &listenSockaddr, &nListenSocket);
	bindSockAddr(nListenSocket, &listenSockaddr);
	listenSockAddr(nListenSocket, MAX_CONN);
*/
//	loopFileDescriptorEvent(nListenSocket);
//	SelectIoImplement selectIoImplemt;
//	selectIoImplemt.loopFileDescriptorEvent(nListenSocket);
//	PollIoImplement pollIoImplement;
//	pollIoImplement.loopFileDescriptorEvent(nListenSocket);
	EpollIoImplement epollIoImplement;
	epollIoImplement.loopFileDescriptorEvent(nListenSocket);	
}
/*
void setSocketAddrReuse( int nFd)
{
	int nTake = 1; 
	if(setsockopt(nFd, SOL_SOCKET , SO_REUSEADDR , &nTake, sizeof(nTake)) == -1)
	{
		cout<<"reset user port reuse flag  err !"<<endl;
		exit(-1);
	}
}


void makeListeningSocket(const char* ip, int nPort, sockaddr_in * addr ,int *nListenSocket)
{
	addr->sin_port = htons(nPort);
	addr->sin_family = AF_INET;
	inet_pton(AF_INET, ip, (void*)&addr->sin_addr);

	if( ( *nListenSocket = socket(PF_INET, SOCK_STREAM, 0) ) < 0)
	{
		cout<<"create socket fd err !"<<endl;
		exit(-1);
	}
	setSocketAddrReuse(*nListenSocket);
}



void bindSockAddr( int nFd, sockaddr_in* addr )
{
	if( bind(nFd, (const sockaddr*)addr, sizeof(sockaddr_in)) == -1)
	{
		cout<<errno<<" "<<strerror(errno)<<endl;
		cout<<"bind listening socket address err !"<<endl;
		exit(-1);
	}
}

void listenSockAddr( int nFd, int nLength )
{
	if( listen(nFd, nLength ) == -1 )
	{
		cout<<errno<<" "<<strerror(errno)<<endl;
		cout<<" listen socket address err !"<<endl;
		exit(-1);
	}
}

void loopFileDescriptorEvent(int nListenSocket)
{
	int nConnCount = 0; //calc curr connected socket number
	int nMaxFd = nListenSocket;
	int nFdArray[5] = {};	

	fd_set readSet;
	while(1)
	{
		FD_ZERO(&readSet);
		FD_SET( nListenSocket, &readSet);
		for( auto i = 0; i < MAX_CONN; i++ )
		{
			FD_SET(nFdArray[i], &readSet);
			if( nMaxFd < nFdArray[i] )
			{
				nMaxFd =nFdArray[i];
			}			
		}

		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec =2000;
		int nRet = select(nMaxFd + 1, &readSet, NULL, NULL, &tv );
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
			if( FD_ISSET(nListenSocket, &readSet )  )
			{
				acceptNewClient(nListenSocket, &nConnCount, nFdArray);
				continue;		
			}
			else
			{
				for( auto i = 0; i < MAX_CONN; i++ )
				{
					if( FD_ISSET(nFdArray[i], &readSet) )
					{
						recvClientMsg(nFdArray[i], &nConnCount, nFdArray);	
					}
				}
			}
		}
	}


}


void acceptNewClient( int nFd , int* nConnCount, int *a )
{
	sockaddr_in clientSockaddr;
	socklen_t nLen = sizeof(sockaddr_in );	
	bzero(&clientSockaddr,  sizeof(clientSockaddr));


	int nClientSocket = accept(nFd,( struct sockaddr* ) &clientSockaddr, &nLen);  
	if( nClientSocket == -1 )	
	{
		cout<<errno<<" "<<strerror(errno)<<endl;
		cout<<" accept system call   err !"<<endl;
		return;
	}
	if( *nConnCount < MAX_CONN )
	{	addFd(nClientSocket, nConnCount, a);
		char strClient[INET_ADDRSTRLEN + 1];
		bzero(strClient, INET_ADDRSTRLEN + 1);
		int nPort = ntohs( clientSockaddr.sin_port );
		inet_ntop(AF_INET, &clientSockaddr.sin_addr, strClient, INET_ADDRSTRLEN );
		cout<<"client connection from "<<strClient<<" : "<<nPort<<endl;

	}
	else
	{
		cout<<refusedMsg<<endl;
		send(nClientSocket, refusedMsg, strlen(refusedMsg)+1, 0);
		close(nClientSocket);
	}	
} 

void recvClientMsg(int nFd, int *nConnCount,  int *a)
{
	const int BUF_SIZE = 256;
	char buf[BUF_SIZE];
	bzero(buf, BUF_SIZE);
	int nLen = recv(nFd, buf, BUF_SIZE, 0);
	if( nLen < 0 )
	{
		cout<<errno<<" "<<strerror(errno)<<endl;
		cout<<nFd<<" recv  system call   err !"<<endl;
		removeInvalidFd(nFd, nConnCount, a);
		return;

	}
	else if( nLen == 0 )
	{
		cout<<nFd<<" recv  system call peer close connection   !"<<endl;
		removeInvalidFd(nFd, nConnCount, a);
		return;
	}
	else
	{
		cout<<"recv : "<<buf;
		int nLen1 = send(nFd, buf, nLen + 1 , 0);
		if( nLen1 != nLen + 1 )
		{
			cout<<errno<<" "<<strerror(errno)<<endl;
			cout<<nFd<<" send  system call   err !"<<endl;
			close(nFd);
			removeInvalidFd(nFd, nConnCount, a);
		}
	}	
}

void removeInvalidFd(int nFd, int *nConnCount, int *a)
{
	int i = 0;
	while(a[i] != nFd)
	{
		i++;
	}
	if( i < MAX_CONN &&  a[i] == nFd )
	{	a[i] = 0;
		*nConnCount = *nConnCount - 1;
		cout<<"remove invalid socket fd success !"<<endl;
	}
	else
	{
		cout<<"remove invalid socket fd fail !"<<endl;
	}
	cout<<"The current fd set count: "<<*nConnCount<<endl;
}

bool  addFd(int nFd, int *nConnCount, int *a)
{
	if( *nConnCount == MAX_CONN )
	{
		cout<<"Already arrive max connection ! "<<endl;
		return false;
	}
	int i = 0;
	while( a[i] )
	{
		i++;
	}
	a[i] = nFd;
	*nConnCount = *nConnCount + 1;
	cout<<"The current fd set count: "<<*nConnCount<<endl;
	return true;

}
*/
