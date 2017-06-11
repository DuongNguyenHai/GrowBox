// Nguyen Hai Duong
// June 10 2017
// version 1.0
#include <Servo.h>
#include <spi-slave.h>

#define SIGNAL_START 0x78
#define PEL_HOT 4
#define PEL_COLD 5
#define FAN_BOX 2
#define FAN_OUTSIDE 3

#define VEN_WIN 15
#define VEN_FAN 19

#define HUMY_WIN 16
#define HUMY_FAN 18
#define HUMY 17

#define CMD_HOT 0x01
#define CMD_COLD 0x02
#define CMD_FAN_BOX 0x03
#define CMD_FAN_OUTSIDE 0x04
#define CMD_VEN 0x05
#define CMD_HUMY 0x06
#define CMD_VENT_WIN_ONLY 0x07
#define CMD_SHUTDOWN 0x79

#define MAX_DATA 4
#define ON 0x01
#define OFF 0x00

Servo venWin;        // servo open/close window of ventilation
Servo humWin;        // servo open/close humidity door

#define CLOSE 0
#define OPEN 90

unsigned char data[2];
bool flag_venWin = false;
bool flag_humWin = false;

uint8_t CRC8(const uint8_t *data, uint8_t len);
bool hasCommand();

SpiSlave spi;

void setup()
{
    Serial.begin (115200);   // debugging

    venWin.attach(VEN_WIN);
    humWin.attach(HUMY_WIN);
    venWin.write(CLOSE);
    humWin.write(CLOSE);
    venWin.detach();
    humWin.detach();

    spi.begin();

    pinMode(PEL_HOT, OUTPUT);
    pinMode(PEL_COLD, OUTPUT);
    pinMode(FAN_BOX, OUTPUT);
    pinMode(FAN_OUTSIDE, OUTPUT);
    pinMode(VEN_FAN, OUTPUT);
    pinMode(HUMY_FAN, OUTPUT);
    pinMode(HUMY, OUTPUT);
}

void loop()
{
    if(hasCommand()) {
        // Serial.print(data[0], HEX);
        // Serial.print(":");
        // Serial.println(data[1], HEX);
        switch (data[0]) {
           case CMD_HOT: {
              digitalWrite(PEL_HOT, data[1]);
           } break;
           case CMD_COLD: {
              digitalWrite(PEL_COLD, data[1]);
           } break;
           case CMD_FAN_BOX: {
              digitalWrite(FAN_BOX, data[1]);
           } break;
           case CMD_FAN_OUTSIDE: {
              digitalWrite(FAN_OUTSIDE, data[1]);
           } break;
           case CMD_VEN: {
              // use servo to open ventilation door
              if(data[1]) {
                 switchWindow(venWin, VEN_WIN, OPEN);
                 digitalWrite(VEN_FAN, HIGH);
                //  flag_venWin = true;
              } else {
                 digitalWrite(VEN_FAN, LOW);
                 switchWindow(venWin, VEN_WIN, CLOSE);
                //  flag_venWin = false;
              }
           } break;
           case CMD_HUMY: {
              // use servo to open humidity door
              if(data[1]) {
                 switchWindow(humWin, HUMY_WIN, OPEN);
                 digitalWrite(HUMY, HIGH);
                 digitalWrite(HUMY_FAN, HIGH);
                //  flag_humWin = true;
              } else {
                 digitalWrite(HUMY, LOW);
                 digitalWrite(HUMY_FAN, LOW);
                 switchWindow(humWin, HUMY_WIN, CLOSE);
                //  flag_humWin = false;
              }
           } break;
           case CMD_VENT_WIN_ONLY: {
              if(data[1]) {
                 switchWindow(venWin, VEN_WIN, OPEN);
              } else {
                 switchWindow(venWin, VEN_WIN, CLOSE);
              }
           }
           case CMD_SHUTDOWN: {
                digitalWrite(PEL_HOT, LOW);
                digitalWrite(PEL_COLD, LOW);
                digitalWrite(FAN_BOX, LOW);
                digitalWrite(FAN_OUTSIDE, LOW);
                digitalWrite(VEN_FAN, LOW);
                digitalWrite(HUMY, LOW);
                digitalWrite(HUMY_FAN, LOW);
                // if(flag_venWin)
                   switchWindow(venWin, VEN_WIN, CLOSE);
                // if(flag_humWin)
                   switchWindow(humWin, HUMY_WIN, CLOSE);

           } break;
           default:;
        }
    }
}

void switchWindow(Servo &servo, unsigned char win, unsigned char set) {
   servo.attach(win);
   unsigned char start = set;
   if(win==HUMY_WIN)
      start = OPEN - set;

   unsigned char pos = start;
   unsigned char k = ( pos==OPEN ? -1:1 );
   do {
      pos += k;
      servo.write(pos);
      delay(30);
   } while(pos!=(OPEN-start));
   servo.detach();
}

bool hasCommand() {
    if(spi.available()>3) {
        if(spi.read()==SIGNAL_START) {
            data[0] = spi.read();
            data[1] = spi.read();
            if(spi.read()==CRC8(data, 2))
                return true;
            else
                return false;
        }
    }
    return false;
}

uint8_t CRC8(const uint8_t data[], uint8_t len) {
   uint8_t crc = 0x00;
   while (len--) {
      uint8_t extract = *data++;
      for (uint8_t tempI = 8; tempI; tempI--) {
         uint8_t sum = (crc ^ extract) & 0x01;
         crc >>= 1;
         if (sum) {
            crc ^= 0x8C;
         }
         extract >>= 1;
      }
   }
   return crc;
}
