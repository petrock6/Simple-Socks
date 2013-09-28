#ifndef SERVER_INCLUDE
#define SERVER_INCLUDE

#include "config.h"
#include "ipfuncs.h"

#include <vector>

#ifdef WINDOWS

#include <WinSock.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")


/*to work in MSVCPP, we need to enable Force File Output.
Prokect Properties->Linker->General
/FORCE
If this becomes a constant problem... http://msdn.microsoft.com/en-us/library/72zdcz6f%28VS.80%29.aspx
*/
bool calledWSA = false;

#endif


struct CLIENT_DATA {
	unsigned int index;
	SOCKET sock;
	
	IPAddress ip;
	unsigned short port;
	
};


/*NOTE:
NON-BLOCKING:
For Server, non-blocking sockets are shoved in a CLIENT_DATA vector list. That should make coding a non-blocking server fairly easy.
YOU NEVER USE HACCEPT. NEVER. ALL OF THAT WORK IS DONE FOR YOU.

BLOCKING:
startServer();
haccept();
then continue with your coding!*/

class Server {
	


	SOCKET serverSock;
	SOCKET sock;

	bool isServerStopped;


	
	//unsigned int serverIndex;

	public:
	
		struct sockaddr_in sockAddrInfo;

		/*options*/
		unsigned short port;
		
		bool isBlocking; 
		bool isTCP; //UDP to be implemented

		/*statistics*/
		unsigned int bytesSent;
		unsigned int bytesReceived;
		
		/*if the server is non-blocking, we need to support multiple clients*/
		std::vector<CLIENT_DATA*> serverSocks;
		//std::vector<CLIENT_DATA*>::iterator it;
		void serverTick(); //call this in your non-blocking server thread. otherwise clients will not be freed upon dc. 
		unsigned int connectedClients();

		Server(unsigned short, bool, bool); //port, blocking, isTCP

		bool startServer(); //will create it's own thread once completed!
		void stopServer(); //will stop server

		unsigned int grabAvailableIndex();
		CLIENT_DATA * grabClientData(unsigned int);
		void deleteClient(unsigned int); 

		void clientHandler(void *); //the thread itself
		void UDPClientHandler(void *);
		bool startClientHandler(); //non-blocking sockets: after we've started listen(), we need a thread to do our accept(). this non-blocking call creates that thread.

		bool haccept(); //for blocking sockets ONLY. This is the accept() call to get a socket. Socket is inside of Server.

		void disconnect(); /*blocking*/
		void disconnect(unsigned int); /*non-blocking*/

		/*Blocking:*/
		unsigned int hsend(char *, unsigned int);
		unsigned int hsend(string);
		unsigned int hrecv(char *, unsigned int);
		unsigned int hrecv(string *);
		
		/*Non-blocking: these functions call the unallocation procedure, as the only way to tell if we are connected is to try sending data*/
		unsigned int hnbsend(char *, unsigned int, unsigned int); //buff, len, index
		unsigned int hnbsend(string, unsigned int);
		unsigned int hnbrecv(char *, unsigned int, unsigned int);
		unsigned int hnbrecv(string *, unsigned int);

		~Server(); //unimplemented

};

#endif
