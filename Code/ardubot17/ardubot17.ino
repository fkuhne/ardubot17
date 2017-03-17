/*
 * ARDUBOT17 for Arduino Day 2017 at PUCRS
 *
 * Felipe Kühne (fkuhne at gmail dot com)
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

const int speedHisteresys = 10;

/* Pins used for motor 1: */
const int enable1_pin = 9;
const int inA1_pin = 2;
const int inB1_pin = 3;

/* Pins used for motor 2: */
const int enable2_pin = 10;
const int inA2_pin = 4;
const int inB2_pin = 5;

/*
 * Global stuff:
 */

/* Connect the HC-05 TX to Arduino pin 2 RX and HC-05 RX to Arduino pin 3 TX
 * through a voltage divider: 5V---( 1k )--[RX]--(2k)---GND */
SoftwareSerial BTserial(btSerialRX_pin, btSerialTX_pin);

DCMotor motor1(enable1_pin, inA1_pin, inB1_pin);
DCMotor motor2(enable2_pin, inA2_pin, inB2_pin);
L298N l298n(motor1, motor2);

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup()
{
  Serial.begin(9600);
  
  BTSerial.begin(9600);
}

void waitCompleteSentence()
{
  while(stringComplete == false && BTSerial.available()) 
  {
    /* Get the new byte and add it to the inputString. */
    inputString += (char)BTSerial.read();
	
    /* If the incoming character is a newline, set a flag so the main loop can
	 * do something about it. Also, remove the trailing new line. */
    if(inputString[inputString.length()] == '\n')
	{
	  inputString[inputString.length()] == '\0';
      stringComplete = true;
	}
  }
}

void loop()
{
  waitCompleteSentence();
  
  char *token = strtok(inputString.c_str(), ",");
  int digitalX = atoi(token);
  token = strtok(NULL, ",");
  int digitalY = atoi(token);
  token = strtok(NULL, ",");
  int switch = atoi(token);
  
  /* Debug: */
  Serial.print("digitalX = "); Serial.println(digitalX);
  Serial.print("digitalY = "); Serial.println(digitalY);
  Serial.print("switch = "); Serial.println(switch);
}

 
void parseData() {

    // split the data into its parts
    
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(receivedChars,",");      // get the first part - the string
  strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC
  
  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  integerFromPC = atoi(strtokIndx);     // convert this part to an integer
  
  strtokIndx = strtok(NULL, ","); 
  floatFromPC = atof(strtokIndx);     // convert this part to a float

}


void showParsedData() {
 Serial.print("Message ");
 Serial.println(messageFromPC);
 Serial.print("Integer ");
 Serial.println(integerFromPC);
 Serial.print("Float ");
 Serial.println(floatFromPC);
}