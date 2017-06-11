#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include <stdio.h>
#include <sys/un.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <sys/socket.h>
#include "config.h"
#include "TCP-IP.h"
#include "bone-log.h"

// #define PORT 8888
#define chanel 0
#define speed 500000
#define	SS0	8
#define BUFF_SIZE 4
#define SIGNAL_START 0x78

namespace BONE {

// Test passed: June 31 2017
class Socket {
public:
	Socket();
	~Socket();
	void creatSocket(unsigned short port);
	int sendMessage(const char*msg);
	int readMessage(char *msg);

private:
	int servSock_;
	int clntSock_;
};

// Test passed: June 31 2017
class ControlDevice {

public:
	ControlDevice();
	~ControlDevice();
	bool begin();
	bool switchDevice(unsigned char device, bool state);
	bool sendCommand(unsigned char cmd[], unsigned int length);
	void shutdown(); // turn off all device
private:
	int fd_;
};

}
#endif

