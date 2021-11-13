// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#ifndef HUBCOMMON_HTTPREQUEST_H
#define HUBCOMMON_HTTPREQUEST_H

#include <common/stringdef.h>

namespace model
{

class TcpSocket;

class HttpRequest
{
public:

	enum Method
	{
		HEAD,
		GET,
		POST,
	};

	static const char* getHost(const char* anUrl);
	HttpRequest();
	~HttpRequest();
	const bool request(const Method aMethod, const char* aRequest);
	const bool update();
	const int getState() const;

	enum State
	{
		IDLE,
		CONNECTING,
		AWAITING_RESPONSE,
		OFFLINE
	};

	enum
	{
		BUFFER_SIZE = (1 << 17),
	};

	//members
	char *myBuffer;

private:

	void cleanup();
	const bool sendRequest();
	void receive();

	//members
	TcpSocket* mySocket;
	bool myRequestSent;
	cmn::String myRequest;
	unsigned int myBytesRead;
	Method myMethod;
};

}

#endif
