// Nguyen Hai Duong
// June 31 2017
// Test passed
// g++ -std=c++11 -Wall test-spi2.cc -o spi2 -lwiringPi

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

unsigned char CRC8(const unsigned char *data, unsigned char len) {
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

int main() {
	
	unsigned char buff[] = {2, 0x01, 1, 0};
	buff[3] = CRC8(buff+1, 2);

	pinMode(SS0, OUTPUT);
	digitalWrite(SS0, 1);

	if( (wiringPiSPISetup(chanel, speed)) <0){
		fprintf (stderr, "SPI Setup failed: %s\n", strerror (errno));
		return 0;
	}

	int fd = wiringPiSPIGetFd(0);

	while(1) {

		for(int i = 0; i<4; i++)
     		printf("0x%02x ", buff[i]);
     	printf("\n");
		fflush(stdout);
		digitalWrite(SS0, 0);
		write (fd, buff, 4) ;
		digitalWrite(SS0, 1);
		delay(1000);
	}
	return 0;
}