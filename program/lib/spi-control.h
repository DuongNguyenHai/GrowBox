// Nguyen Hai Duong
// June 31 2017
// Test passed

#ifndef SPI_CONTROL_H
#define SPI_CONTROL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define chanel 0
#define speed 500000
#define	SS0	8
#define BUFF_SIZE 4
#define SIGNAL_START 0x78

#define CMD_HOT 0x01
#define CMD_COLD 0x02
#define CMD_FAN_BOX 0x03
#define CMD_FAN_OUTSIDE 0x04
#define CMD_VEN 0x05
#define CMD_HUMY 0x06
#define CMD_SHUTDOWN 0x79

#define HOT CMD_HOT
#define COLD CMD_COLD
#define FAN_BOX CMD_FAN_BOX
#define FAN_OUTSIDE CMD_FAN_OUTSIDE
#define VENTILATION CMD_VEN
#define HUMIDITY CMD_HUMY
#define SHUTDOWN CMD_SHUTDOWN
#define ON 0x01
#define OFF 0x00

class ControlDevice {

public:
	ControlDevice();
	~ControlDevice();
	bool begin();
	bool switchDevice(unsigned char device, bool state);
	bool sendCommand(unsigned char cmd[], unsigned int length);
	void shutdown();		// turn off all device
private:
	int fd_;
};

#endif