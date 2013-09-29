#include "ipfuncs.h"

#include <cstdlib>
#include <string.h>

#include <stdio.h>

IPAddress::IPAddress() {
	ipString = "0.0.0.0";
	b1 = 0; b2 = 0; b3 = 0; b4 = 0;
}

IPAddress::IPAddress(char * inputIPString) {
	ipString = inputIPString;

	compileIPBytes();

}

IPAddress::IPAddress(unsigned long inputIPBytes) {
	compileIPLongToBytes(inputIPBytes);
}

IPAddress::IPAddress(unsigned char s1, unsigned char s2, unsigned char s3, unsigned char s4) {
	b1 = s1; b2 = s2; b3 = s3; b4 = s4;

	compileIPString();
}

void IPAddress::increment() {

	b4++;

	if(b4 == 255) {
		b4 = 0;
		if(b3 != 255)
			b3++;
	}
	if(b3 == 255) {
		b3 = 0;
		if(b2 != 255)
			b2++;
	}
	if(b2 == 255) {
		b2 = 0;
		if(b1 != 255)
			b1++;
	}
	if(b1 == 255) {
		b1 = 0;
	}
	
	compileIPString();
}

void IPAddress::decrement() {
	
	b4--;

	if(b4 == 0) {
		b4 = 255;
		if(b3 != 0)
			b3--;
	}
	if(b3 == 0) {
		b3 = 255;
		if(b2 != 0)
			b2--;
	}
	if(b2 == 0) {
		b2 = 255;
		if(b1 != 0);
			b1--;
	}
	if(b1 == 0) {
		b1 = 255;
	}

	compileIPString();
}

void IPAddress::compileIPString() { //bytes to string
	char s1[8];
	char s2[8];
	char s3[8];
	char s4[8];

#ifdef _WIN32
	itoa( (int) b1, s1, 10);
	itoa( (int) b2, s2, 10);
	itoa( (int) b3, s3, 10);
	itoa( (int) b4, s4, 10);
#else
	snprintf(s1, sizeof(s1), "%d", b1);
        snprintf(s2, sizeof(s2), "%d", b2);
        snprintf(s3, sizeof(s3), "%d", b3);
        snprintf(s4, sizeof(s4), "%d", b4);
#endif
	ipString = s1;
	ipString += ".";
	ipString += s2;
	ipString += ".";
	ipString += s3;
	ipString += ".";
	ipString += s4;
}

void IPAddress::compileIPLongToBytes(unsigned long inputIPBytes) {
	unsigned char * abyte = (unsigned char*)&inputIPBytes;

	b1 = abyte[0];
	b2 = abyte[1];
	b3 = abyte[2];
	b4 = abyte[3];
	//now that we have bytes, call string function
	compileIPString();
}

void IPAddress::compileIPBytes() { //string to bytes
	char localCopy[16];
	char s1[4];
	char s2[4];
	char s3[4];
	char s4[4];

	int ipLength = ipString.length();
	int periodCount = 0;
	int i = 0, x = 0;

	memset(localCopy, 0, sizeof(localCopy));
	memset(s1, 0, 4); memset(s2, 0, 4); memset(s3, 0, 4); memset(s4, 0, 4); 
#ifdef _WIN32
	strncpy_s(localCopy, sizeof(localCopy), ipString.c_str(), ipLength);
#else
	strncpy(localCopy, ipString.c_str(), ipLength);
#endif

top:
	while(i < ipLength) {

		if(localCopy[i] == '.') {
			periodCount++;
		} else {
			if(periodCount == 0) {
				for(x = 0; (localCopy[i] >= 0x30) && (localCopy[i] <= 0x39) && (i < ipLength); x++, i++) { /*I thought of how hilarious this would look under a disassembler.*/
					s1[x] = localCopy[i];
				}
				goto top;
			}
			if(periodCount == 1) {
				for(x = 0; (localCopy[i] >= 0x30) && (localCopy[i] <= 0x39) && (i < ipLength); x++, i++) {
					s2[x] = localCopy[i];
				}
				goto top;
			}
			if(periodCount == 2) {
				for(x = 0; (localCopy[i] >= 0x30) && (localCopy[i] <= 0x39) && (i < ipLength); x++, i++) {
					s3[x] = localCopy[i];
				}
				goto top;
			}
			if(periodCount == 3) {
				for(x = 0; (localCopy[i] >= 0x30) && (localCopy[i] <= 0x39) && (i < ipLength); x++, i++) {
					s4[x] = localCopy[i];
				}
				goto top;
			}
		}


		i++;
	}

	b1 = atoi(s1);
	b2 = atoi(s2);
	b3 = atoi(s3);
	b4 = atoi(s4);

	return;

}

unsigned long IPAddress::getLongIP() {
	unsigned long ret;
	//b4 b3 b2 b1

	ret = b4 << 6;

	return ret;
}

bool IPAddress::isIPSet() {

	if( (b1 == 0) && (b2 == 0) && (b3 == 0) && (b4 == 0) && (ipString == "0.0.0.0") ) {
		return false;
	} else {
		if( (b1==0) && (b2==0) && (b3==0) && (b4==0) && (ipString != "0.0.0.0") ) { //oops, someone forgot to set the bytes when we got the string!
			compileIPBytes();
			return true;
		} else {
			/*ok, let's just try this and see if it works*/
			compileIPString();
			if(ipString != "0.0.0.0") //checks for the opposite: if someone forgot to set the string when bytes were received
				return true;

			return false;
		}
		
	}
	/*b1-b4 isn't 0.0.0.0 and neither is ipString! I don't think we'll ever reach here*/
	return true;
}
