#ifndef CLIENT_INCLUDE
#define CLIENT_INCLUDE

#include "ipfuncs.h"

#include <string.h>
#include <string>

using namespace std;

#ifdef _WIN32

#include <WinSock.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")


/*to work in MSVCPP, we need to enable Force File Output.
Prokect Properties->Linker->General
/FORCE
If this becomes a constant problem... http://msdn.microsoft.com/en-us/library/72zdcz6f%28VS.80%29.aspx
*/
bool calledWSAClient = false;
#else

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
//#include <netinet6/in6.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>

#define INVALID_SOCKET -1
typedef int SOCKET;
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
