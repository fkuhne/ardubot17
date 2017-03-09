/*
 * ARDUBOT17 for Arduino Day 2017 at PUCRS
 *
 * Felipe Kühne (fkuhne at gmail dot com)
 * 03/2017
 *
 * THIS IS FREE SOFTWARE. CHOOSE THE LICENSE THAT BEST FITS FOR YOU.
 */

/* NRF24L01 comm based on https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-ExampleSketches#bm1 
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

 
#include <L298N.h>
#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>

/*
 * Global constant definitions:
 */

const int speedHisteresys = 100;

/* Pins used for motor 1: */
const int enable1_pin = 1;
const int inA1_pin = 2;
const int inB1_pin = 3;

/* Pins used for motor 2: */
const int enable2_pin = 4;
const int inA2_pin = 5;
const int inB2_pin = 6;

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

DCMotor motor1(enable1_pin, inA1_pin, inB1_pin);
DCMotor motor2(enable2_pin, inA2_pin, inB2_pin);
L298N l298n(motor1, motor2);
RF24 radio(ce_pin, csn_pin);

void setup()
{
  radio.begin(); /* Initialize the nRF24L01 Radio. */
  radio.setChannel(radioChannel); /* 2.508 Ghz - Above most Wifi channels. */
  radio.setDataRate(RF24_250KBPS); /* Fast enough, better range. */

  /* Set the Power Amplifier Level low to prevent power supply related issues. */
  radio.setPALevel(RF24_PA_LOW); /* RF24_PA_MAX */

  /* Open reading pipe. */
  radio.openReadingPipe(1, addresses[0]);

  /* Start the radio listening for data. */
  radio.startListening();
}

void loop()
{
  if(radio.available())
  {
    while(radio.available())
      radio.read(&joystick, sizeof(joystick));
	
	/* If joystick data is inside an hysteresis region, stop both motors. */
    if((joystick.y >= (1023/2 - speedHisteresys)) &&
       (joystick.y <= (1023/2 + speedHisteresys)))
      l298n.setState(STOP);
    
	else if(joystick.y < (1023/2 - speedHisteresys))
	{
	  l298n.setDirection(FW);
	  l298n.setSpeed(V1, W0);
	  l298n.setState(RUN);
	}
	else if(joystick.y > (1023/2 + speedHisteresys))
	{
	  l298n.setDirection(BW);
	  l298n.setSpeed(V1, W0);
	  l298n.setState(RUN);
	}
  }
}
