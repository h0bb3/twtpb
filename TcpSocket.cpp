// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "stdafx.h"
#include "tcpsocket.h"

#pragma comment( lib, "wsock32.lib" )

namespace model
{

const bool TcpSocket::makeAddress(SOCKADDR_IN& anAddress, const char* anIpAddress, const unsigned short aPort)
{
	HOSTENT* hostEnt = NULL;

	//init structure (assume dotted address)
	anAddress.sin_family = PF_INET;
	anAddress.sin_port = htons((u_short)aPort);
	anAddress.sin_addr.s_addr = inet_addr(anIpAddress);

	//if invalid address, try as hostname
	if(anAddress.sin_addr.s_addr == INADDR_NONE)
	{
		//resolve hostname
		hostEnt = gethostbyname(anIpAddress);
		
		//if succeeded, extract address
		if(hostEnt)
		{
			//disgusting, the h_addr macro returns a char *
			anAddress.sin_addr.s_addr = *((unsigned long *)(hostEnt->h_addr));

			//success
			return true;
		}
		else
		{
			//couldn't resolve
			return false;
		}
	}

	//success
	return true;
}

void TcpSocket::echoWSAError(const char* aFunctionCall) const
{
	int error = WSAGetLastError();

	if(error == WSAEMSGSIZE)
	{
//		Debug::debugMessage("NetUtils::echoWSAError(): Got WSAEMSGSIZE, check max message size");
	}
	else if(error != WSAEWOULDBLOCK)
	{
//		Debug::debugMessage("NetUtils::echoWSAError(): %d while calling: %s", error, aFunctionCall);
	}
}

//constructor (NULL)
TcpSocket::TcpSocket()
:myWinsock(INVALID_SOCKET)
{
}


//destructor
TcpSocket::~TcpSocket()
{
	int err;
	const int BUFSIZE = 512;
	char buf[BUFSIZE];

	if(myWinsock != INVALID_SOCKET)
	{
		//done sending
		shutdown(myWinsock, 1);

		//recieve and discard all pending data
		//until error occurs or nothing left to recv
		do
		{
			err = recv(myWinsock, buf, BUFSIZE, 0);
		}
		while(err != SOCKET_ERROR && err != 0);

		//close socket
		closesocket(myWinsock);
		myWinsock = INVALID_SOCKET;
	}
}
	

//creation
TcpSocket* TcpSocket::create()
{
	TcpSocket* s(new TcpSocket());

	if(s && !s->init())
	{
		delete s;
		s = NULL;
	}

	return s;
}


TcpSocket* TcpSocket::create(const SOCKET& aConnectedWinsock, const SOCKADDR_IN& aRemoteAddress)
{
	TcpSocket* s(new TcpSocket());
	
	if(s && !s->init(aConnectedWinsock, aRemoteAddress))
	{
		delete s;
		s = NULL;
	}

	return s;
}


//default init
const bool TcpSocket::init()
{
	unsigned long nonBlocking = 1;

	//open socket
	//udp socket
	myWinsock = socket(PF_INET, SOCK_STREAM, 0);
	if(myWinsock == INVALID_SOCKET)
	{
		echoWSAError("socket()");
		return false;
	}

	//make socket nonblocking (nonzero "1" enables nonblocking)
	if(ioctlsocket(myWinsock, FIONBIO, &nonBlocking) == SOCKET_ERROR)
	{
		echoWSAError("ioctlsocket()");
		return false;
	}

	return true;
}


//make tcpsocket from a connected winsock (used by server)
const bool TcpSocket::init(const SOCKET& aConnectedWinsock, const SOCKADDR_IN& aRemoteAddress)
{
	myWinsock = aConnectedWinsock;
	myRemoteAddress = aRemoteAddress;
	return (myWinsock != INVALID_SOCKET);
}


//bind a socket (name it explicitly), and listen for clients
//only used for servers so that clients can find it via its name
const bool TcpSocket::bindAndListen(const unsigned short aPortNumber)
{
	SOCKADDR_IN address;
	
	assert(myWinsock != INVALID_SOCKET);

	//init structure
	address.sin_family = PF_INET;
	address.sin_port = htons((u_short)aPortNumber);
	address.sin_addr.s_addr = INADDR_ANY;

	//bind socket (name it)
	if(bind(myWinsock, (LPSOCKADDR)&address, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		echoWSAError("bind()");
		return false;
	}

	//listen for clients (maximum reasonable number of backlogged connections)
	if(listen(myWinsock, SOMAXCONN) == SOCKET_ERROR)
	{
		echoWSAError("listen()");
		return false;
	}

	return true;
}


//accept a possible pending connection request
/*
bool TcpSocket::AcceptConnection(SOCKET& aWinSock, SOCKADDR_IN& anAddress)
{
	int addySize = sizeof(SOCKADDR);
	
	assert(myWinsock != INVALID_SOCKET);

	//attempt to accept a connection
	aWinSock = accept(myWinsock, (LPSOCKADDR)&anAddress, &addySize);
	if(aWinSock == INVALID_SOCKET)
	{
//		NetUtils::echoWSAError("accept()");
		return false;
	}
	else
		return true;
}
*/


TcpSocket* TcpSocket::acceptConnection()
{
	SOCKADDR_IN remoteAddress;
	SOCKET remoteSocket;
	int addySize = sizeof(SOCKADDR);
	
	assert(myWinsock != INVALID_SOCKET);
	if(myWinsock != INVALID_SOCKET)
	{
		//attempt to accept a connection
		remoteSocket = accept(myWinsock, (LPSOCKADDR)&remoteAddress, &addySize);
		if(remoteSocket != INVALID_SOCKET)
		{
			//create new socket
			return create(remoteSocket, remoteAddress);
		}
		else
		{
//			NetUtils::echoWSAError("accept()");
		}
	}

	return NULL;
}



//connect to an address (set default sending address)
const bool TcpSocket::connect(const SOCKADDR_IN& anAddress)
{
	int wsaError;
	
	assert(myWinsock != INVALID_SOCKET);

	if(::connect(myWinsock, (LPSOCKADDR)&anAddress, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{	
		echoWSAError("connect()");

		//blocking errors are not fatal
		wsaError = WSAGetLastError();
		if(wsaError == WSAEWOULDBLOCK)
			return true;
		else
			return false;
	}
	else
		return true;
}


//send a raw data buffer
const bool TcpSocket::sendBuffer(char *aBuffer, const int aBufferLength) const
{
	int error = SOCKET_ERROR;
	int sentBytes = 0;
	int wsaError;

	assert(myWinsock != INVALID_SOCKET);
	if(myWinsock != INVALID_SOCKET)
	{
		//always loop until all data sent
		while(sentBytes < aBufferLength)
		{
			error = send(myWinsock, (aBuffer + sentBytes), aBufferLength - sentBytes, 0);

			if(error == SOCKET_ERROR)
			{
				echoWSAError("send()");

				//blocking errors are not fatal, but we MUST ALWAYS SEND DATA, so loop. :(
				wsaError = WSAGetLastError();
				if(wsaError != WSAEWOULDBLOCK)
					return false;
			}
			else
			{
				sentBytes += error;
			}
		}

		return true;
	}
	else
		return false;
}



//recieve to a raw data buffer
const bool TcpSocket::recvBuffer(char *aBuffer, const int aBufferLength, int& aNumBytesRead) const
{
	int wsaError;

	assert(aNumBytesRead == 0);
	assert(myWinsock != INVALID_SOCKET);
	if(myWinsock != INVALID_SOCKET)
	{
		aNumBytesRead = recv(myWinsock, aBuffer, aBufferLength, 0);
		if(aNumBytesRead == SOCKET_ERROR)
		{
			echoWSAError("recv()");

			//blocking errors are not fatal
			wsaError = WSAGetLastError();
			return wsaError == WSAEWOULDBLOCK;
		}
	}

	return aNumBytesRead != 0;
}

const bool TcpSocket::getLocalAddress(const SOCKADDR_IN& anAddress) const
{
	int SIZE = sizeof(SOCKADDR);
	
	if(myWinsock == INVALID_SOCKET)
		return false;

	//get local (bound) address of socket
	if(getsockname(myWinsock, (LPSOCKADDR)&anAddress, &SIZE) == SOCKET_ERROR)
	{
		echoWSAError("getsockname()");
		return false;
	}
	
	return true;
}


/*
void TcpSocket::NetUtils::echoWSAError(const char* aFunctionCall)
{
	int error = WSAGetLastError();
	
	//echo to console
	if(error != WSAEWOULDBLOCK)
	{
		Debug::debugMessage("WSAError: %d while calling: %s", error, aFunctionCall);
	}
}
*/

/*
//returns local host name
const char* TcpSocket::GetLocalHostName()
{
	static char temp[MAX_PATH];
	
	gethostname(temp, MAX_PATH);

	return temp;
}
*/


//check if connected
const bool TcpSocket::isConnected() const
{
	fd_set writeableSet;
	fd_set exceptSet;
	int err;
	timeval timeout;

	//clear
	FD_ZERO(&writeableSet);

	//one socket in set (my winsock)
	FD_SET(myWinsock, &writeableSet);

	//wait one second (this is probably not needed)
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	//check for sockets with writebility
	err = select(0,				//ignored, Berkely compatibility
				NULL,			//set to check for readability
				&writeableSet,	//set to check for writeability
				NULL,			//set to check for errors
				&timeout);		//timeout (NULL to block)

	if(err == 1 && FD_ISSET(myWinsock, &writeableSet))
	{
		FD_ZERO(&exceptSet);

		FD_SET(myWinsock, &exceptSet);

		err = select(0,		//ignored, Berkely compatibility
				NULL,		//set to check for readability
				NULL,		//set to check for writeability
				&exceptSet,	//set to check for errors
				&timeout);	//timeout (NULL to block)

		return (err != 1 && !FD_ISSET(myWinsock, &exceptSet));	//socket does not contain errors
	}
	else
		return false;
}

const bool TcpSocket::isTimedOut() const
{
	fd_set exceptSet;
	int err;
	timeval timeout;

	//clear
	FD_ZERO(&exceptSet);

	//one socket in set (my winsock)
	FD_SET(myWinsock, &exceptSet);

	//wait one second (this is probably not needed)
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	//check for sockets with writebility
	err = select(0,				//ignored, Berkely compatibility
				NULL,			//set to check for readability
				NULL,	//set to check for writeability
				&exceptSet,			//set to check for errors
				&timeout);		//timeout (NULL to block)

	return err == 1;
}

TcpSocket::Winsock::Winsock()
{
	WORD version(MAKEWORD(1, 1));

	myWSA.wVersion = 0xffff;
	WSAStartup(version, &myWSA);
	isInited();
}

TcpSocket::Winsock::~Winsock()
{
	WSACleanup();
}

const bool TcpSocket::Winsock::isInited() const
{
//	return LOBYTE(myWSA.wVersion) == 1 && HIBYTE(myWSA.wVersion) == 1;
	return myWSA.wVersion == 0x0101;
}

TcpSocket::Winsock TcpSocket::ourWinsock;

}
