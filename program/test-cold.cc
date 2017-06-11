// Nguyen Hai Duong
// June 31 2017
// Test passed
// g++ -std=c++11 -Wall test-cold.cc lib/spi-control.cc -o cold -lwiringPi

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>	
#include "lib/spi-control.h"

#define PIN_PUMP 26
void cancel();
ControlDevice control;

int main() {

	if (wiringPiSetupGpio() == -1)
		return -1;

	if (signal(SIGINT, (void (*)(int)) cancel) == SIG_ERR) {
		perror("Unable to catch SIGINT");
		return -1;
	}

	pinMode(PIN_PUMP, OUTPUT);
	
	control.begin();
	
	control.switchDevice(FAN_BOX, ON);
	control.switchDevice(FAN_OUTSIDE, ON);
	digitalWrite(PIN_PUMP, LOW);
	delay(500);
	control.switchDevice(COLD, ON);

	

	while(1) {
		sleep(1000);
	}
	
	return 0;
}

void cancel() {
	control.switchDevice(COLD, OFF);
	control.switchDevice(FAN_BOX, OFF);
	control.switchDevice(FAN_OUTSIDE, OFF);
	digitalWrite(PIN_PUMP, HIGH);
	pinMode(PIN_PUMP, INPUT);
	exit(1);
}