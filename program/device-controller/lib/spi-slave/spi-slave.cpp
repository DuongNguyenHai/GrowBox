#include "spi-slave.h"

struct Buff {
    char c[BUFF_SIZE];
    unsigned char head;
    unsigned char tail;
};

struct Buff rv_buff = { {0}, 0, 0};

void SpiSlave::begin() {
    // turn on SPI in slave mode
	SPCR |= bit (SPE);
	// have to send on master in, *slave out*
	pinMode(MISO, OUTPUT);
	// now turn on interrupts
	SPI.attachInterrupt();
}

size_t SpiSlave::available() {
    return (unsigned char )(BUFF_SIZE + rv_buff.head - rv_buff.tail) % BUFF_SIZE;
}

int SpiSlave::read() {
	if(rv_buff.tail==rv_buff.head) {
		return NO_DATA;
	} else {
        unsigned char c = rv_buff.c[rv_buff.tail];
    	rv_buff.tail = (rv_buff.tail+1) % BUFF_SIZE;
    	return c;
    }
}

ISR (SPI_STC_vect) {
    rv_buff.c[rv_buff.head] = SPDR;
    rv_buff.head = (rv_buff.head + 1) % BUFF_SIZE;
}
