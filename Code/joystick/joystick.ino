/*
 * ARDUBOT17 for Arduino Day 2017 at PUCRS
 *
 * Felipe Kuhne (fkuhne at gmail dot com)
 * 03/2017
 *
 * THIS IS FREE SOFTWARE. CHOOSE THE LICENSE THAT BEST FITS FOR YOU.
 */

/* A good source of information about bluetooth modules with Arduino is here:
 * http://www.martyncurrey.com/connecting-2-arduinos-by-bluetooth-using-a-hc-05-and-a-hc-06-pair-bind-and-link/
 */

#include <SoftwareSerial.h>

/*
 * Global constant definitions:
 */

/* Pins: */
const int joystickX_pin = A1; //14
const int joystickY_pin = A0; //15
const int joystickSwitch_pin = 4;
const int btSerialRX_pin = 2;
const int btSerialTX_pin = 3;

/* Connect the HC-05 TX to Arduino pin 2 RX and HC-05 RX to Arduino pin 3 TX
 * through a voltage divider: 5V---( 1k )--[RX]--(2k)---GND */
SoftwareSerial BTserial(btSerialRX_pin, btSerialTX_pin);


void setup()
{
  /* Initialize hardware. */
  pinMode(joystickX_pin, INPUT);
  pinMode(joystickY_pin, INPUT);
  pinMode(joystickSwitch_pin, INPUT);

  Serial.begin(9600);
  BTserial.begin(9600);
}

void loop()
{
  int digitalX = analogRead(joystickX_pin);
  /* Invert Y to compensate for an error in hardware. :) */
  digitalY = map(analogRead(joystickY_pin), 1023, 0, 0, 1023);
  switch = digitalRead(joystickSwitch_pin);

  BTSerial.print(digitalX);
  BTserial.print(",");
  BTserial.print(digitalY);
  BTserial.print(",");
  BTserial.println(switch);

  /* This is for debug only. */
  Serial.print(digitalX);
  Serial.print(",");
  Serial.print(digitalY);
  Serial.print(",");
  Serial.println(switch);

  delay(100);
}
