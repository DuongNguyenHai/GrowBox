#include <Servo.h>
#include <SPI.h>

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
#define CMD_SHUTDOWN 0xff

#define MAX_DATA 4
#define ON 0x01
#define OFF 0x00

Servo venWin;        // servo open/close window of ventilation
Servo humWin;        // servo open/close humidity door

#define CLOSE 0
#define OPEN 90

bool flag_ready = true;
bool flag_has_data = false;
uint8_t spi[MAX_DATA];
uint8_t spi_pos = 0;
uint8_t data[3];
uint8_t dt_num = 0;

bool flag_venWin = false;
bool flag_humWin = false;

uint8_t CRC8(const uint8_t *data, uint8_t len);

void setup()
{
   Serial.begin (115200);   // debugging

   venWin.attach(VEN_WIN);
   humWin.attach(HUMY_WIN);
   venWin.write(CLOSE);
   humWin.write(CLOSE);
   venWin.detach();
   humWin.detach();

	// turn on SPI in slave mode
	SPCR |= bit (SPE);
	// have to send on master in, *slave out*
	pinMode(MISO, OUTPUT);
	// now turn on interrupts
	SPI.attachInterrupt();

    pinMode(PEL_HOT, OUTPUT);
    pinMode(PEL_COLD, OUTPUT);
    pinMode(FAN_BOX, OUTPUT);
    pinMode(FAN_OUTSIDE, OUTPUT);
    pinMode(VEN_FAN, OUTPUT);
    pinMode(HUMY_FAN, OUTPUT);
    pinMode(HUMY, OUTPUT);
}

// SPI interrupt routine
ISR (SPI_STC_vect) {
   if(flag_ready) {
      spi[spi_pos] = SPDR;
      spi_pos++;
   }
}

bool spiAvailable() {
   if(spi_pos>0)
      return true;
   else
      return false;
}

void loop()
{
   if(spiAvailable()) {
      dt_num = spi[0];
      if(spi_pos>=(dt_num+2)) {
         for (size_t i = 0; i < dt_num; i++) {
            data[i] = spi[i+1];
         }
         if(spi[dt_num+1]==CRC8(data, dt_num)) {
            flag_ready = false;
            flag_has_data = true;
            spi_pos = 0;
         }
      }
   }

   if(flag_has_data) {

      switch (data[0]) {
         case CMD_HOT: {
            digitalWrite(PEL_HOT, data[1]);
            Serial.print("switch peltier hot: ");
            if(data[1])
                Serial.println("ON");
            else
                Serial.println("OFF");
         } break;
         case CMD_COLD: {
            digitalWrite(PEL_COLD, data[1]);
            Serial.print("switch peltier cold: ");
            if(data[1])
                Serial.println("ON");
            else
                Serial.println("OFF");
         } break;
         case CMD_FAN_BOX: {
            digitalWrite(FAN_BOX, data[1]);
            Serial.print("switch fan box: ");
            if(data[1])
                Serial.println("ON");
            else
                Serial.println("OFF");
         } break;
         case CMD_FAN_OUTSIDE: {
            digitalWrite(FAN_OUTSIDE, data[1]);
            Serial.print("switch fan outside: ");
            if(data[1])
                Serial.println("ON");
            else
                Serial.println("OFF");
         } break;
         case CMD_VEN: {
            // use servo to open ventilation door
            if(data[1]) {
               switchWindow(venWin, VEN_WIN, OPEN);
               digitalWrite(VEN_FAN, HIGH);
               flag_venWin = true;
            } else {
               digitalWrite(VEN_FAN, LOW);
               switchWindow(venWin, VEN_WIN, CLOSE);
               flag_venWin = false;
            }
            Serial.print("switch ventilation: ");
            if(data[1])
                Serial.println("ON");
            else
                Serial.println("OFF");
         } break;
         case CMD_HUMY: {
            // use servo to open humidity door
            if(data[1]) {
               switchWindow(humWin, HUMY_WIN, OPEN);
               digitalWrite(HUMY, HIGH);
               digitalWrite(HUMY_FAN, HIGH);
               flag_humWin = true;
            } else {
               digitalWrite(HUMY, LOW);
               digitalWrite(HUMY_FAN, LOW);
               switchWindow(humWin, HUMY_WIN, CLOSE);
               flag_humWin = false;
            }
            Serial.print("switch humidity: ");
            if(data[1])
                Serial.println("ON");
            else
                Serial.println("OFF");
         } break;
         case CMD_SHUTDOWN: {
            digitalWrite(PEL_HOT, LOW);
            digitalWrite(PEL_COLD, LOW);
            digitalWrite(FAN_BOX, LOW);
            digitalWrite(FAN_OUTSIDE, LOW);
            digitalWrite(VEN_FAN, LOW);
            digitalWrite(HUMY, LOW);
            digitalWrite(HUMY_FAN, LOW);
            if(flag_venWin)
               switchWindow(venWin, VEN_WIN, CLOSE);
            if(flag_humWin)
               switchWindow(humWin, HUMY_WIN, CLOSE);

            Serial.println("Shutdown system");
         } break;
         default: ;
      }

      flag_ready = true;
      flag_has_data = false;
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

uint8_t CRC8(const uint8_t *data, uint8_t len) {
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
