// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#ifndef HUBCOMMON_TCPSOCKET_H
#define HUBCOMMON_TCPSOCKET_H

namespace model
{

class TcpSocket
{
public:

	//creation
	static const bool makeAddress(SOCKADDR_IN& anAddress, const char* anIpAddress, const unsigned short aPort);
	static TcpSocket* create();
	static TcpSocket* create(const SOCKET& aConnectedWinsock, const SOCKADDR_IN& aRemoteAddress);
	
	//destructor
	~TcpSocket();
	
	//bind the socket (name it with a port number), and listen for clients
	//only used for servers
	const bool bindAndListen(const unsigned short aPortNumber);

	//accept a possible pending connection
	TcpSocket* acceptConnection();
	
	//connect to an address
	const bool connect(const SOCKADDR_IN& anAddress);
	
	//send a raw data buffer
	const bool sendBuffer(char* aBuffer, const int aBufferLength) const;
	
	//recieve to a raw data buffer
	const bool recvBuffer(char* aBuffer, const int aBufferLength, int& aNumBytesRead) const;

	//get bound address of this socket
	const bool getLocalAddress(const SOCKADDR_IN& anAdress) const;

	//get remote address that this socket is connected to
	const SOCKADDR_IN& getRemoteAddress() const	{return myRemoteAddress;}

	//check if connected
	const bool isConnected() const;

	// check if a socket times out
	const bool isTimedOut() const;

private:

	TcpSocket();
	const bool init();
	const bool init(const SOCKET& aConnectedWinsock, const SOCKADDR_IN& aRemoteAddress);
	void echoWSAError(const char* aFunctionCall) const;

	class Winsock
	{
	public:

		Winsock();
		~Winsock();
		const bool isInited() const;

	private:

		//members
		WSADATA myWSA;
	};

	//members
	static Winsock ourWinsock;
	SOCKET myWinsock;
	SOCKADDR_IN myRemoteAddress;
};

#endif
}
