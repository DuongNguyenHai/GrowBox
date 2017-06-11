#include "DS18B20.h"

DS18B20::~DS18B20() {}

void DS18B20::setAddr(const uint8_t rom[8]) {
	for(int i=0; i<8; i++){
	    addr_[i] = rom[i];
	}
}

float DS18B20::tempt() {
	for(int j=0; j<7; j++) { // trying to get data in 7 seven times
	   	OneWire::reset();
		OneWire::select(addr_);
		OneWire::write(0x44);	// send command to request convert tempt
		
		switch (mode_) {
	       	case 9:
	        	delay(94);
	        	break;
	       	case 10:
	        	delay(188);
	         	break;
	        case 11: 
	        	delay(375);
	        	break;
	        default:			// default is 12 bit resolution
	        	delay(750);
	   	}

		OneWire::reset();
		OneWire::select(addr_);
		OneWire::write(0xBE);	// read rempt from Scratchpad
		uint8_t data[9];

		for (int i = 0; i < 9; i++) {
			data[i] = OneWire::read();
		}
		if (OneWire::crc8(data, 8) != data[8]) {
			// printf("crc is wrong, data is not valid\n");
		} else {
			return (float)((data[1] << 8) | data[0]) / 16.0; // the unit of temperature is celsius
		}
	}
	
	return VAL_FAIL;
}

void DS18B20::resolution(uint8_t mode) {
	mode_ = mode;
	uint8_t data[4];
	OneWire::reset();
	OneWire::select(addr_);
	OneWire::write(0xBE);
	
	for (int i = 0; i < 4; i++) {
		data[i] = OneWire::read();
	}

	OneWire::reset();
	OneWire::select(addr_);
	OneWire::write(0x4E);                           // Write to Scratchpad
   	OneWire::write(data[2]);                        // TH register
   	OneWire::write(data[3]);                        // TL register

   	// configuration register
   	switch (mode_) {
       	case 9:
        	OneWire::write(0x1F);
        	break;
       	case 10:
        	OneWire::write(0x3F);
         	break;
        case 11: 
        	OneWire::write(0x5F);
        	break;
        case 12:
        	OneWire::write(0x7F);
   	}
}

void DS18B20::readAddr(uint8_t rom[8]) {
	for(int i=0; i<8; i++)
	    rom[i] = addr_[i]; 
}