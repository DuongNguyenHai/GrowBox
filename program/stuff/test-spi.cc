// Nguyen Hai Duong
// June 31 2017
// Test passed
// g++ -std=c++11 -Wall test-spi.cc lib/spi-control.cc -o spi -lwiringPi

#include <stdio.h>
#include "lib/spi-control.h"

int main() {

	ControlDevice control;

	wiringPiSetupGpio();		// u can ignore calling this function 

	control.begin();

	while(1) {
		
		control.switchDevice(HOT, ON);
		control.switchDevice(COLD, ON);
		control.switchDevice(FAN_BOX, ON);
		control.switchDevice(FAN_OUTSIDE, ON);

		control.switchDevice(VENTILATION, ON);
		delay(3000);		// must be wait for arduino finish the previous job. take time > 2s
		control.switchDevice(HUMIDITY, ON);
		delay(3000);		// must be wait for arduino finish the previous job. take time > 2s

		control.switchDevice(HOT, OFF);
		control.switchDevice(COLD, OFF);
		control.switchDevice(FAN_BOX, OFF);
		control.switchDevice(FAN_OUTSIDE, OFF);
		control.switchDevice(VENTILATION, OFF);
		delay(3000);	// must be wait for arduino finish the previous job. take time > 2s
		control.switchDevice(HUMIDITY, OFF);
		delay(3000);	// must be wait for arduino finish the previous job. take time > 2s

		control.shutdown();
		delay(7000);	// must be wait for arduino finish the previous job. its take more than 6 seconds
	}
	
	return 0;
}