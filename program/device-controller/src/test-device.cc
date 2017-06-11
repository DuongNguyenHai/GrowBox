/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN takes care
  of use the correct LED pin whatever is the board used.
  If you want to know what pin the on-board LED is connected to on your Arduino model, check
  the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald

  modified 2 Sep 2016
  by Arturo Guadalupi
*/
#define PEL_HOT 4
#define PEL_COLD 5
#define FAN_BOX 2
#define FAN_OUTSIDE 3

#define VEN_WIN 15
#define VEN_FAN 19

#define HUMY_WIN 16
#define HUMY_FAN 18
#define HUMY 17

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(PEL_HOT, OUTPUT);
  pinMode(PEL_COLD, OUTPUT);
  pinMode(FAN_BOX, OUTPUT);
  pinMode(FAN_OUTSIDE, OUTPUT);
  pinMode(VEN_FAN, OUTPUT);
  pinMode(HUMY_FAN, OUTPUT);
  pinMode(HUMY, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(PEL_HOT, HIGH);
  digitalWrite(PEL_COLD, HIGH);
  digitalWrite(FAN_BOX, HIGH);
  digitalWrite(FAN_OUTSIDE, HIGH);
  digitalWrite(VEN_FAN, HIGH);
  digitalWrite(HUMY_FAN, HIGH);
  digitalWrite(HUMY, HIGH);
  delay(2000);                       // wait for a second
  digitalWrite(PEL_HOT, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(PEL_COLD, LOW);
  digitalWrite(FAN_BOX, LOW);
  digitalWrite(FAN_OUTSIDE, LOW);
  digitalWrite(VEN_FAN, LOW);
  digitalWrite(HUMY_FAN, LOW);
  digitalWrite(HUMY, LOW);
  delay(2000);                       // wait for a second
}
