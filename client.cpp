
#include "client.h"
#include "config.h"





Client::Client(string lHost, unsigned short lPort, bool lBlocking, bool lTCP) {//host, port, blocking, isTCP
	host = lHost;
	port = lPort;
	isBlocking = lBlocking;
	isTCP = lTCP;

	/*if(!isTCP) {
		cout << "UDP unimplemented. Sorry.\n";
		exit(0);
	}*/

#ifdef WINDOWS
	if(!calledWSAClient) { //fuck WSA
		WSADATA wsd;
		WSAStartup(0x0101, &wsd);
		calledWSAClient = true;
	}
#endif

}

bool Client::sconnect() {
	u_long iMode = 1;

	if(!hostIP.isIPSet()) { //we need to do a lookup and setup shit
		hostEntity = gethostbyname(host.c_str());

		//printLastError();

		if(hostEntity == NULL)
			return false;


		sockAddrInfo.sin_addr.S_un.S_addr = *((unsigned long*)hostEntity->h_addr_list[0]); //set socket structure to use dns ip
		hostIP.compileIPLongToBytes(*((unsigned long*)hostEntity->h_addr_list[0]));		   //set our structure to use this ip as well
		sockAddrInfo.sin_family = AF_INET;
		sockAddrInfo.sin_port = htons(port);

	} 

	if(isTCP) {
		if( (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
			return false;
	} else {
		if( (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
			return false;
	}
	

	if(connect(sock, (struct sockaddr*) &sockAddrInfo, sizeof(sockaddr_in)))
		return false;


	if(!isBlocking)
		ioctlsocket(sock, FIONBIO, &iMode); //put in non-blocking mode
											//i am *pretty* sure this has 0 effect on connect()
											//untested Linux

	return true;
}

void Client::disconnect() {
#ifdef WINDOWS
	closesocket(sock);
#endif
#ifndef WINDOWS
	close(sock);
#endif
	//isConnected = false;
}

unsigned int Client::hsend(char * sendData, unsigned int sendLength) {
	int ret = send(sock, (const char*)sendData, sendLength, 0);
	if(ret >= 1)
		bytesSent += ret;
	return ret;
}

unsigned int Client::hsend(string sendData) {
	int ret = send(sock, sendData.c_str(), sendData.length(), 0);
	if(ret >= 1)
		bytesSent += ret;
	return ret;
}

unsigned int Client::hrecv(char * recvBuff, unsigned int recvLength) {
	int ret = recv(sock, (char*)recvBuff, recvLength, 0);
	if(ret >= 1)
		bytesReceived += ret;
	return ret;
}

unsigned int Client::hrecv(string *recvString) {
	/*This is *really, REALLY* fucking cheap. We're just going to grab a single byte at a time from our network card.
	We need to do this because we can't predict where a null will be in the buffer, and we cannot "go in reverse." I do not want to build a network cache. I would rather have the OS handle this.
	
	NOTE: This function can return a PARTIAL STRING. Make sure you check if the connection was interrupted before processing data. I would feel uncomfortable with large (>65K) strings. 

	Whatever, I'll "optimize it in assembly" or something later. Just use the other function if you're actually worried about performance, but I wouldn't worry.
	*/
	char buff[2];
	
	memset(buff, 0, sizeof(buff)); //mad?
	recvString->assign(""); //that should do it

	while( (recv(sock, &buff[0], 1, 0) == 1) ) { //receive data until NULL data, bad connection, or blocked receive call
		
		if(buff[0] == '\x00') {
			bytesReceived++; break;
		}
		
		recvString->append(buff);
		bytesReceived++;
	}

	return recvString->length();
}

Client::~Client() {
	if(sock != INVALID_SOCKET) {
#ifdef WINDOWS
		closesocket(sock);
#endif
#ifndef WINDOWS
		close(sock);
#endif

	}

}
