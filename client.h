#ifndef CLIENT_INCLUDE
#define CLIENT_INCLUDE

#include "config.h"

#include "ipfuncs.h"

#ifdef WINDOWS

#include <WinSock.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")


/*to work in MSVCPP, we need to enable Force File Output.
Prokect Properties->Linker->General
/FORCE
If this becomes a constant problem... http://msdn.microsoft.com/en-us/library/72zdcz6f%28VS.80%29.aspx
*/
bool calledWSAClient = false;
#endif


class Client {
	struct hostent * hostEntity;
	unsigned long Address;
	struct sockaddr_in sockAddrInfo;

	SOCKET sock;

	
	public:

		/*options*/
		string host;
		IPAddress hostIP;
		unsigned short port;
		
		bool isBlocking; 
		bool isTCP; //UDP to be implemented

		/*statistics*/
		unsigned int bytesSent;
		unsigned int bytesReceived;
		

		Client(string, unsigned short, bool, bool); //host, port, blocking, isTCP

		bool sconnect();
		void disconnect();

		unsigned int hsend(char *, unsigned int);
		unsigned int hsend(string);
		//see about templating??? class to string conversions?
		unsigned int hrecv(char *, unsigned int);
		unsigned int hrecv(string *);
		
		~Client(); //unimplemented

};

#endif
