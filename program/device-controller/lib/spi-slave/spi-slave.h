#ifndef SPI_SLAVE_H
#define SPI_SLAVE_H

#include <SPI.h>
#define BUFF_SIZE 32
#define NO_DATA -1

class SpiSlave {

public:
    SpiSlave() {};
    ~SpiSlave() {};
    void begin();
    size_t available();
    int read();
private:

};

#endif
