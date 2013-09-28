#include "server.h"
#include "config.h"

#define SOCKADDR_LENGTH sizeof(struct sockaddr_in)

Server::Server(unsigned short lPort, bool lBlocking, bool lTCP) {//port, blocking, isTCP
	port = lPort;
	isBlocking = lBlocking;
	isTCP = lTCP;

	/*if(!isTCP) {
		cout << "UDP unimplemented. Sorry.\n";
		exit(0);
	}*/

	isServerStopped = true;

#ifdef WINDOWS
	if(!calledWSA) {
		WSADATA wsd;
		WSAStartup(0x0101, &wsd);
		calledWSA = true;
	}
#endif

}

static UINT WINAPI ThreadFunc(LPVOID param)
{
   Server* obj = (Server*)param;
   if(obj->isTCP)
	   obj->clientHandler(0); // call the member function, to do work in our new thread
   else
	   obj->UDPClientHandler(0);
                              
   return 0;
}



void Server::clientHandler(void * useless) {
	SOCKET clientSock;
	struct sockaddr_in clientSockAddr;
	struct CLIENT_DATA * clientData;

	std::vector<CLIENT_DATA*> serverSocksCopy;

	unsigned long iMode = 1;
	int addrLength = sizeof(clientSockAddr);

	while(isServerStopped == false) {

		clientSock = accept(serverSock, (struct sockaddr *) &clientSockAddr, 0);
		
		if(clientSock != INVALID_SOCKET) {
			ioctlsocket(clientSock, FIONBIO, &iMode); //non-blocking mode
			//untested Linux

			/*now we've gotten a client, let's give em a struct*/
			
			clientData = new CLIENT_DATA; /*this is unallocated in serverTick()*/
			{
				clientData->index = grabAvailableIndex();
				//clientData->isConnected = true; //un-necassary: if the client is not connected, they will not be in the list. 
				clientData->sock = clientSock;
				
				getpeername(clientSock, (sockaddr *)&clientSockAddr, &addrLength); /*grab updated client sockaddr_in structure*/
				clientData->ip.compileIPLongToBytes(clientSockAddr.sin_addr.S_un.S_addr); //update our IPAddress structure
				clientData->port = htons(clientSockAddr.sin_port); 


				serverSocks.push_back(clientData);



				//cout << "New client: \n" << "Sock: " << clientData->sock << "\nIP: " << clientData->ip.ipString << "\nIndex: " << clientData->index << "\n";
				cout << "(Server: " << port << ") [C] " << clientData->ip.ipString << ":" << clientData->port << " [S.I.:" << clientData->index << "] (" << connectedClients() << " clients connected)\n";
				//cout << "new connection\n";
			}

		} 
	}

}

void Server::UDPClientHandler(void * useless) {
	cout << "UDP server unimplemented. Sorry.\n";
	exit(0);
}

bool Server::startClientHandler() {

#ifdef WINDOWS
	if(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ThreadFunc, (LPVOID)this, NULL, NULL) == NULL) {
		return false;
	}
	return true;
#endif

#ifndef WINDOWS
		//IMPLEMENT PTHREAD
#endif
}

void Server::serverTick() {

	//cout << "serverTick(): called! (Your ALL-IN-ONE TCP Client/Server example!)\n";


	CLIENT_DATA * clientData;

	for(unsigned int x = 0; x < serverSocks.size(); x++) {
			//cout << __LINE__ << "\n";
			
			clientData = serverSocks[x];

			hnbsend("PING\n", clientData->index);
	}

}

unsigned int Server::connectedClients() {
	return serverSocks.size();
}

bool Server::startServer() {
	sockAddrInfo.sin_family = AF_INET;
	sockAddrInfo.sin_addr.S_un.S_addr = INADDR_ANY;
	sockAddrInfo.sin_port = htons(port);

	if(isTCP)
		serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	else
		serverSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	if(serverSock == INVALID_SOCKET)
		return false;

	if(bind(serverSock, (sockaddr *)&sockAddrInfo, sizeof(sockAddrInfo)) != 0)
		return false;
	
	if(isTCP)
		if(listen(serverSock, 128) != 0) //I don't care about backlog
			return false;

	isServerStopped = false;

	if(!isBlocking)
		return startClientHandler();
	
	return true;

}

void Server::stopServer() { //untested code but 98% sure it'll do.
	CLIENT_DATA * clientData;
	isServerStopped = true;

	if(serverSocks.size() > 0) {

		for(unsigned int x = 0; x < serverSocks.size(); x++) {
				clientData = serverSocks[x];
				deleteClient(clientData->index);
		}

		stopServer(); //recursive: we need to keep calling this until size() is zero.

	}

}

unsigned int Server::grabAvailableIndex() {
	unsigned int iTry;
	bool available;
	CLIENT_DATA* clientData;
	unsigned int x = 0;

	if(serverSocks.size() == 0)
		return 0;

grabAvailableIndexLoopTop:
	available = true;

	while(x < serverSocks.size()) {
		//cout << __LINE__ << "\n";
		iTry = x;
		clientData = serverSocks[x];

		x++;

		if(clientData->index == iTry) {
			available = false; //we found the same index
			break;
		}
		//iTry = x;
	}

	/*
	for(it = serverSocks.begin();
		it != serverSocks.end();
		it++) {
			cout << __LINE__ << "\n"; //PROBLEM LINE
			clientData = *it;
			if(clientData->index == iTry) {
				available = false; //we found the same index
				break;
			}

	}*/
	
	iTry = x; //incase our loop didn't get called. otherwise, we'll overwrite someone else's index. 

	if(available)
		return iTry;

	goto grabAvailableIndexLoopTop;

}

CLIENT_DATA * Server::grabClientData(unsigned int clientIndex) {
	CLIENT_DATA* clientData;

	for(unsigned int x = 0; x < serverSocks.size(); x++) {
			//cout << __LINE__ << "\n";
			clientData = serverSocks[x];
			if(clientData->index == clientIndex)
				return clientData;
	}

	return NULL;
}

void Server::deleteClient(unsigned int clientIndex) {
	CLIENT_DATA * clientData;

	for(unsigned int x = 0; x < serverSocks.size(); x++) {
			clientData = serverSocks[x];



			if(clientData->index == clientIndex) { //yo' chop dat shit
#ifdef WINDOWS
				closesocket(clientData->sock);
#endif
#ifndef WINDOWS
				close(clientData->sock);
#endif
				cout << "(Server: " << port << ") [C] " << clientData->ip.ipString << ":" << clientData->port << " [S.I.:" << clientData->index << "] (" << connectedClients() << " clients connected)\n";

				delete clientData;


				//deleteIt = serverSocks.erase(deleteIt);
				/*std::vector<CLIENT_DATA*>::iterator it;
				it = serverSocks.*/
				serverSocks.erase(serverSocks.begin()+x);

				

				//serverSocks' iterator got fucked. we still have a copy of it, though!
				//it = deleteIt; //w0w i regret writing this line of code. causes a huge bug.

				return;
			}


			//cout << __LINE__ << "\n";
	}

/*	int x = 0;
	for(it = serverSocks.begin();
		it != serverSocks.end();
		it++, x++) {

			clientData = *it;


			cout << "ID: "<< clientData->index << " Sock: " << clientData->sock << "\n";
			if(clientData->index == clientIndex) { //yo' chop dat shit
#ifdef WINDOWS
				closesocket(clientData->sock);
#endif
#ifndef WINDOWS
				close(clientData->sock);
#endif
				delete clientData;
				serverSocks.erase(serverSocks.begin() + x);
				return;
			}

	}*/
}

bool Server::haccept() {
	struct sockaddr_in clientSockAddr;

	sock = accept(serverSock, (struct sockaddr *) &clientSockAddr, 0);
	if(sock == INVALID_SOCKET)
		return false;

	return true;
}

void Server::disconnect() {
	/* needs to be adapted for blocking/non-blocking
#ifdef WINDOWS
	closesocket(sock);
#endif
#ifndef WINDOWS
	close(sock);
#endif
	isConnected = false;
	*/
}

unsigned int Server::hsend(char * sendData, unsigned int sendLength) {
	int ret = send(sock, (const char*)sendData, sendLength, 0);


	if(ret >= 1)
		bytesSent += ret;
	return ret;
}

unsigned int Server::hsend(string sendData) {
	int ret = send(sock, sendData.c_str(), sendData.length(), 0);
	if(ret >= 1)
		bytesSent += ret;
	return ret;
}

unsigned int Server::hrecv(char * recvBuff, unsigned int recvLength) {
	int ret;// = recv(sock, (char*)recvBuff, recvLength, 0);

	if(isTCP)
		ret = recv(sock, (char*)recvBuff, recvLength, 0);
	else {
		int sockaddrLength = SOCKADDR_LENGTH;
		
		ret = recvfrom(serverSock, recvBuff, recvLength, 0, (sockaddr *) &sockAddrInfo, &sockaddrLength);
		//serverSock, not sock!
		
		/*
		IPAddress ip;
		ip.compileIPLongToBytes(sockAddrInfo.sin_addr.S_un.S_addr);
		ip.compileIPString();
		cout << "IP: " << ip.ipString << "\n";*/
	}

	if(ret >= 1)
		bytesReceived += ret;
	return ret;
}


unsigned int Server::hrecv(string *recvString) {
	/*This is *really, REALLY* fucking cheap. We're just going to grab a single byte at a time from our network card.
	We need to do this because we can't predict where a null will be in the buffer, and we cannot "go in reverse." I do not want to build a network cache. I would rather have the OS handle this.
	
	NOTE: This function can return a PARTIAL STRING. Make sure you check if the connection was interrupted before processing data. I would feel uncomfortable with large (>65K) strings. 

	Whatever, I'll "optimize it in assembly" or something later. Just use the other function if you're actually worried about performance, but I wouldn't worry.
	*/
	char buff[2];
	
	memset(buff, 0, sizeof(buff)); //mad?
	recvString->assign(""); //that should do it

	if(!isTCP) {
		cout << "It\'s unsafe to use UDP with this function, because packets from different IPs will overlap.\n";
		exit(0);
	}

	while( (recv(sock, &buff[0], 1, 0) == 1) ) { //receive data until NULL data, bad connection, or blocked receive call
		
		if(buff[0] == '\x00') {
			bytesReceived++; break;
		}
		
		recvString->append(buff);
		bytesReceived++;
	}

	return recvString->length();
}

unsigned int Server::hnbsend(char * sendData, unsigned int sendLength, unsigned int clientIndex) {
	CLIENT_DATA* clientData = grabClientData(clientIndex);
	int ret;

	if(clientData == NULL)
		return false;

	ret = send(clientData->sock, (const char*)sendData, sendLength, 0);
	if(ret >= 1)
		bytesSent += ret;

	if(ret == SOCKET_ERROR)
		deleteClient(clientIndex);

	return ret;
}

unsigned int Server::hnbsend(string sendData, unsigned int clientIndex) {
	CLIENT_DATA* clientData = grabClientData(clientIndex);
	int ret;

	if(clientData == NULL)
		return false;

	ret = send(clientData->sock, sendData.c_str(), sendData.length(), 0);
	if(ret >= 1)
		bytesSent += ret;

	if(ret == SOCKET_ERROR)
		deleteClient(clientIndex);

	return ret;
}

unsigned int Server::hnbrecv(char * recvBuff, unsigned int recvLength, unsigned int clientIndex) {
	CLIENT_DATA* clientData = grabClientData(clientIndex);
	int ret;

	if(clientData == NULL)
		return false;

	ret = recv(clientData->sock, (char*)recvBuff, recvLength, 0);
	if(ret >= 1)
		bytesReceived += ret;

	if(ret == SOCKET_ERROR)
		deleteClient(clientIndex);

	return ret;
}

unsigned int Server::hnbrecv(string *recvString, unsigned int clientIndex) {
	CLIENT_DATA* clientData = grabClientData(clientIndex);
	char buff[2];
	int ret;
	
	memset(buff, 0, sizeof(buff)); //mad?
	recvString->assign(""); //that should do it

	while( (ret = recv(clientData->sock, &buff[0], 1, 0)) == 1)  { //receive data until NULL data, bad connection, or blocked receive call
		
		if(buff[0] == '\x00') {
			bytesReceived++; break;
		}
		
		recvString->append(buff);
		bytesReceived++;
	}

	if(ret == SOCKET_ERROR)
		deleteClient(clientIndex);

	return recvString->length();
}

Server::~Server() {
	if(!isServerStopped)
		stopServer();
}
