// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "stdafx.h"
#include "httprequest.h"

#include "tcpsocket.h"

namespace model
{

const char* HttpRequest::getHost(const char* anUrl)
{
	static cmn::String host;
	size_t i;

	host = anUrl;
	if(host.find("http://") == 0)
	{
		host = host.substr(7, host.length() - 7);
		i = host.find('/');
		if(i > 0)
		{
			host = host.substr(0, i);
			return host.c_str();
		}
	}

	return NULL;
}

HttpRequest::HttpRequest()
:mySocket(NULL)
,myRequestSent(false)
,myBytesRead(0)
,myMethod(GET)
{
	myBuffer = NULL;
}

HttpRequest::~HttpRequest()
{
	delete [] myBuffer;
	cleanup();
}

const bool HttpRequest::request(const Method aMethod, const char* aRequest)
{
	const char* host(getHost(aRequest));
	SOCKADDR_IN addr;

	cleanup();

	if(!host)
		return false;

	//create socket
	mySocket = TcpSocket::create();
	if(!mySocket)
		return false;

	//connect
	TcpSocket::makeAddress(addr, host, 80);
	if(!mySocket->connect(addr))
	{
		cleanup();
		return false;
	}

	myMethod = aMethod;
	myRequest = aRequest;

	return true;
}

const bool HttpRequest::update()
{
	if(mySocket && mySocket->isConnected())
	{
		if(myRequestSent)
			receive();
		else
			return sendRequest();
	}

	return true;
}

const int HttpRequest::getState() const
{
	if(mySocket)
	{
		if (mySocket->isTimedOut()) {
			return OFFLINE;
		} else if(mySocket->isConnected()) {
			return AWAITING_RESPONSE;
		}
		else
		{
			return CONNECTING;
		}
	}
	else
	{
		return IDLE;
	}
}

//private implementation
const bool HttpRequest::sendRequest()
{
	static const char* METHOD[] = 
	{
		"HEAD",
		"GET",
		"POST",
	};
	char buffer[4096];
	SOCKADDR_IN localaddr;

	assert(mySocket->isConnected());

	mySocket->getLocalAddress(localaddr);

	//request
	sprintf_s(buffer, sizeof(buffer), "%s %s HTTP/1.0\r\nHost: %s\r\nAccept: text/*\r\n\r\n", METHOD[myMethod], myRequest.c_str(), getHost(myRequest.c_str()));
	if(!mySocket->sendBuffer(buffer, (int)strlen(buffer)))
	{
		cleanup();
		return false;
	}

	myRequestSent = true;
	return true;
}

void HttpRequest::receive()
{
	int bytesRead(0);
	static char buffer[BUFFER_SIZE];

	//keep receiving until there is an error, as TCP is stream based
	if(mySocket->recvBuffer(buffer, sizeof(buffer), bytesRead))
	{
		if(bytesRead > 0)
		{
			char *tmpBuffer = new char[myBytesRead + bytesRead + 1];
			memcpy(tmpBuffer, myBuffer, myBytesRead);
			memcpy(&tmpBuffer[myBytesRead], buffer, bytesRead);
			delete [] myBuffer;
			myBuffer = tmpBuffer;

			myBytesRead += bytesRead;
			/*if(myBytesRead >= BUFFER_SIZE) {
				myBytesRead = BUFFER_SIZE - 1;
			}*/
			myBuffer[myBytesRead] = 0;
		}
	}
	else
	{
		cleanup();
	}
}

void HttpRequest::cleanup()
{
	delete mySocket;
	mySocket = NULL;
	myRequestSent = false;
	myBytesRead = 0;
	//don't clear the buffer, external clients need to read it
}

}

