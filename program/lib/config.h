#ifndef CONFIG_H
#define CONFIG_H

#define NUM_OF_DS18B 2

const unsigned char ds18b_1_rom[8] = {0x28, 0xff, 0xc6, 0x13, 0x50, 0x16, 0x04, 0x1a};
const unsigned char ds18b_2_rom[8] = {0x28, 0xff, 0x02, 0x08, 0x44, 0x16, 0x03, 0xdc};
// const unsigned char ds18b_3_rom[8] = {0x28, 0xff, 0xbd, 0xb7, 0x44, 0x16, 0x03, 0xf7};

#define NUM_OF_DHT 1

#define PIN_LAMP 19				// connecto Lamp
#define PIN_PUMP 26				// connect to pump
#define PIN_DS1 4				// onewire to ds18b20, slot 1
#define PIN_DS2 17				// onewire to ds18b20, slot 2
#define PIN_DHT1 27				// onewire to DHT22, slot 1
#define PIN_DHT2 22				// onewire to DHT22, slot 2

// name of collection in database
#define COLL_TEMPT "logTempt"
#define COLL_HUMY "logHumidity"
#define COLL_LAMP "logLamp"
#define COLL_PROGRESS_TEMPT "logProgressTemperature"
#define COLL_PROGRESS_HUMY "logProgressHumidity"
#define COLL_SETUP "setup"

// command for control device
#define CMD_HOT 0x01
#define CMD_COLD 0x02
#define CMD_FAN_BOX 0x03
#define CMD_FAN_OUTSIDE 0x04
#define CMD_VEN 0x05
#define CMD_HUMY 0x06
#define CMD_VENT_WIN_ONLY 0x07
#define CMD_SHUTDOWN 0x79

#define HOT CMD_HOT
#define COLD CMD_COLD
#define FAN_BOX CMD_FAN_BOX
#define FAN_OUTSIDE CMD_FAN_OUTSIDE
#define VENTILATION CMD_VEN
#define HUMIDITY CMD_HUMY
#define VENTILATION_WIN CMD_VENT_WIN_ONLY
#define SHUTDOWN CMD_SHUTDOWN

#endif