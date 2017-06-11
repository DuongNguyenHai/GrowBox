// Nguyen Hai Duong
// June 31 2017
// Test passed
// g++ -std=c++11 -Wall onfire.cc lib/spi-control.cc -o onfire -lwiringPi

#include <stdio.h>
#include <signal.h>	
#include "lib/spi-control.h"

#define PIN_PUMP 26
void cancel();

ControlDevice control;

int main() {
	if (signal(SIGINT, (void (*)(int)) cancel) == SIG_ERR) {
		perror("Unable to catch SIGINT");
		return -1;
	}
	wiringPiSetupGpio();		// u can ignore calling this function 
	pinMode(PIN_PUMP, OUTPUT);
	control.begin();

	control.switchDevice(HOT, OFF);
	control.switchDevice(COLD, OFF);
	control.switchDevice(FAN_BOX, ON);
	control.switchDevice(FAN_OUTSIDE, ON);
	digitalWrite(PIN_PUMP, LOW);
	control.switchDevice(VENTILATION, ON);

	while(1) {
		delay(1000);	// must be wait for arduino finish the previous job. its take more than 6 seconds
	}
	
	return 0;
}

void cancel() {
	// control.switchDevice(HOT, OFF);
	control.shutdown();
	digitalWrite(PIN_PUMP, HIGH);
	pinMode(PIN_PUMP, INPUT);

	exit(1);
}