

#ifndef IPFUNCS_INCLUDE
#define IPFUNCS_INCLUDE

#include <iostream>
#include <string>
using namespace std;

class IPAddress {

public:
	std::string ipString;

	unsigned char b1;
	unsigned char b2;
	unsigned char b3;
	unsigned char b4;

	IPAddress();
	IPAddress(char *); //127.0.0.1
	IPAddress(unsigned long);
	IPAddress(unsigned char, unsigned char, unsigned char, unsigned char);

	void increment(); //for things like portscanning, etc. (only on LAN :)
	void decrement(); //


	void compileIPString();
	void compileIPLongToBytes(unsigned long);
	void compileIPBytes();

	unsigned long getLongIP();

	bool isIPSet();

};

#endif

