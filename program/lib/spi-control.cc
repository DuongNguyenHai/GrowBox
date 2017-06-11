#include "spi-control.h"

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

ControlDevice::ControlDevice() {}

ControlDevice::~ControlDevice() {}

bool ControlDevice::begin() {
    pinMode(SS0, OUTPUT);
    digitalWrite(SS0, 1);
    if( (wiringPiSPISetup(chanel, speed)) <0){
        printf("SPI Setup failed: %s\n", strerror (errno));
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
