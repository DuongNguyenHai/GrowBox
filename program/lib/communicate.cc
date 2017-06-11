#include "communicate.h"

namespace BONE {

// int Socket::sendMessage(const char* msg) {}
static unsigned char CRC8(const unsigned char *data, unsigned char len) {
  unsigned char crc = 0x00;
  while (len--) {
    unsigned char extract = *data++;
    for (unsigned char tempI = 8; tempI; tempI--) {
      unsigned char sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) {
        crc ^= 0x8C;
      }
      extract >>= 1;
    }
  }
  return crc;
}

Socket::Socket() {}
Socket::~Socket() {}

void Socket::creatSocket(unsigned short port) {
	servSock_ = createTCPServerSocket(port);
	clntSock_ = acceptTCPConnection(servSock_);
}

int Socket::readMessage(char* msg) {
	int num = recv(clntSock_ , msg, BUFFSIZE, 0);
	if ( num == -1 ) 
        BONE_WARN << "ERROR reading from socket";
    else if( num > 0 ){
    	// success
    	msg[num] = '\0';
    }
    else {
        // BONE_VLOG << "Client disconnect !\n";
        clntSock_ = acceptTCPConnection(servSock_);
    }
    return num;
}


ControlDevice::ControlDevice() {}
ControlDevice::~ControlDevice() {}

bool ControlDevice::begin() {
    pinMode(SS0, OUTPUT);
    digitalWrite(SS0, 1);
    if( (wiringPiSPISetup(chanel, speed)) <0 ){
        BONE_WARN << "SPI Setup failed: " << strerror(errno);
        return false;
    } else {
        fd_ = wiringPiSPIGetFd(0);
        return ( fd_!=-1 ? true:false );
    }
}

bool ControlDevice::sendCommand(unsigned char cmd[], unsigned int length) {
    // Add byte crc at the end
    cmd[length-1] = CRC8(cmd+1, length-2);
    digitalWrite(SS0, 0);
    write (fd_, cmd, length);
    digitalWrite(SS0, 1);
    delay(20);
    return true;
}

bool ControlDevice::switchDevice(unsigned char device, bool state) {
    unsigned char cmd[] = {SIGNAL_START, device, state, 0};
    return sendCommand(cmd, BUFF_SIZE);
}

void ControlDevice::shutdown() {
    unsigned char cmd[] = {SIGNAL_START, SHUTDOWN, 0, 0};
    sendCommand(cmd, BUFF_SIZE);
}

}