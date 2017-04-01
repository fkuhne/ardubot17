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

const int serialFrameSize = 13;

/* Pins: */
const int joystickX_pin = A1;
const int joystickY_pin = A0;
const int joystickButton_pin = 4;
const int btSerialRX_pin = 2;
const int btSerialTX_pin = 3;

/* Connect the HC-05 TX to Arduino pin 2 RX and HC-05 RX to Arduino pin 3 TX
 * through a voltage divider: 5V---( 1k )--[RX]--(2k)---GND */
SoftwareSerial BTSerial(btSerialRX_pin, btSerialTX_pin);

void setup()
{
  /* Initialize hardware. */
  pinMode(joystickX_pin, INPUT);
  pinMode(joystickY_pin, INPUT);
  pinMode(joystickButton_pin, INPUT);

  /* Open both serial ports. */
  Serial.begin(9600); /* This is for debug only. */
  BTSerial.begin(9600); /* Bluetooth communication. */
}

void loop()
{
  /* Invert X to compensate for an error in hardware. :) */
  int digitalX = map(analogRead(joystickX_pin), 1023, 0, 0, 1023);
  /* Invert Y to compensate for an error in hardware. :) */
  int digitalY = map(analogRead(joystickY_pin), 1023, 0, 0, 1023);
  /* Button state. +1 to avoid data equal to zero. */
  int button = digitalRead(joystickButton_pin) + 1;

  char sendBuffer[serialFrameSize] = "";
  
  /* Format the numbers with four digits, so we can fix the frame size that
   *  will be transmitted. */
  snprintf(sendBuffer, serialFrameSize, "%04d,%04d,%1d\n", digitalX, digitalY, button);

  BTSerial.print(sendBuffer); /* Send through bluetooth. */

  Serial.print(sendBuffer); /* This is for debug only. */

  delay(100);
}

