/*
 * ARDUBOT17 for Arduino Day 2017 at PUCRS
 *
 * Felipe Kuhne (fkuhne at gmail dot com)
 * 03/2017
 *
 * THIS IS FREE SOFTWARE. CHOOSE THE LICENSE THAT BEST FITS FOR YOU.
 */

/* NRF24L01 comm based on https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-ExampleSketches#js1 
 * and RF24 lib (https://github.com/TMRh20/RF24).
 * Connections: (http://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo)
 *  1: GND
 *  2: VCC 3.3V !!! NOT 5V
 *  3: CE to Arduino pin 7
 *  4: CSN to Arduino pin 8
 *  5: SCK to Arduino pin 13
 *  6: MOSI to Arduino pin 11
 *  7: MISO to Arduino pin 12
 *  8: UNUSED
 */

#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

/*
 * Global constant definitions:
 */

/* Pins: */
const int joystickX_pin = A0; //14
const int joystickY_pin = A1; //15
const int joystickSwitch_pin = 16; //A2

/*
 * Radio:
 */

const int ce_pin = 7;
const int csn_pin = 8;
const int radioChannel = 108;
byte addresses[][6] = {"1Node", "2Node"}; // These will be the names of the "Pipes"
typedef struct {
  int x;
  int y;
  bool sw;
} joystickData;

joystickData joystick{0, 0, 0};

/*
 * Global variables:
 */

RF24 radio(ce_pin, csn_pin);

void setup()
{
  /* Initialize hardware. */
  pinMode(joystickX_pin, INPUT);
  pinMode(joystickY_pin, INPUT);
  pinMode(joystickSwitch_pin, INPUT_PULLUP);
  
  /* Initialize radio. */
  radio.begin(); /* Initialize the nRF24L01 Radio. */
  radio.setChannel(radioChannel); /* 2.508 Ghz - Above most Wifi channels. */
  radio.setDataRate(RF24_250KBPS); /* Fast enough, better range. */

  /* Set the Power Amplifier Level low to prevent power supply related issues. */
  radio.setPALevel(RF24_PA_LOW); /* RF24_PA_MAX */

  /* Open reading pipe. */
  radio.openWritingPipe(addresses[0]);

  /* Start the radio listening for data. */
  radio.startListening();
}

void loop()
{
  joystick.x = analogRead(joystickX_pin);
  joystick.y = analogRead(joystickY_pin);
  joystick.sw  = !digitalRead(joystickSwitch_pin);  // Invert the pulldown switch
  
  radio.write(&joystick, sizeof(joystick));
  
  delay(250);
}
