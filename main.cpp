#include "server.h"
#include "client.h"

#ifndef _WIN32
#include <errno.h>
#endif


#include <time.h>
#include <iostream>
using namespace std;


/*
This code in this file is for extremely small examples.
Linux works now.
*/

void printLastError() {
#ifdef _WIN32
	char buff[256];
	DWORD e = GetLastError();


	if(e != 0) { 
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), buff, sizeof(buff), NULL);
		cout << "[E] " << buff << " (0x" << std::hex << e << ")\n";
	}
#else
	cout << strerror(errno) << "\n";
#endif

	return;
}


/*
//Client Main():
int main() {
	Client cli("127.0.0.1", 555, true, false);
	char buff[128];
	std::string r;

	memset(buff, 0, 128);

	if(cli.sconnect()) {
		cout << "Connected\n";

		cli.hsend("Hello!\n");

		while(cli.hrecv(buff, sizeof(buff)) > 0) {
			cout << "Received: " << buff << "\n";
			
			r = "Received ";
			r.append(buff);

			cli.hsend(r);

			memset(buff, 0, sizeof(buff));
		}

	}
}*/




//Server Main(): TCP
int main() {
#ifndef _WIN32
	struct timespec timeVarSleep;
#endif
	int sleeptime;
	int b = 0;
	Server x(5556, false, true);

	x.startServer();

	while(1) { //or !x.isServerStopped
		x.serverTick();
		//messageAllClients(&x);
		b++;


		sleeptime = 50 + x.connectedClients();
		//never want sleep above 200ms. cheesy, but it works.
		if(sleeptime > 200)
			sleeptime = 200;
#ifdef _WIN32
		Sleep(sleeptime); //again, cheesy.
#else
		timeVarSleep.tv_sec = 0;
		timeVarSleep.tv_nsec = (1000000) * sleeptime;
		nanosleep(&timeVarSleep, NULL);
#endif

	}



	return 0;
}

//Server Main(): Single Threaded UDP (note: you will prob need to check the IP and source port in your protocol)
/*
int main() {
	Server x(5555, true, false);
	IPAddress ip;
	char buff[128];

	if(x.startServer())
		cout << "Server started (UDP)\n";
	else
		printLastError();


	//Server is now binded
	
	memset(buff, 0, sizeof(buff));

	while(x.hrecv(buff, sizeof(buff)) > 0) {
		cout << "Received: " << buff << "\n";
#ifdef _WIN32
		ip.compileIPLongToBytes(x.sockAddrInfo.sin_addr.S_un.S_addr);
#else
		ip.compileIPLongToBytes(x.sockAddrInfo.sin_addr.s_addr);
#endif
		ip.compileIPString();
		cout << "IP: " << ip.ipString << "\n";

		memset(buff, 0, sizeof(buff));
	}


	return 0;
}*/



































