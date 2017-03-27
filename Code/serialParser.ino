String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup()
{
  Serial.begin(9600);
}

void waitCompleteSentence()
{
  while(stringComplete == false && Serial.available()) 
  {
    /* Get the new byte and add it to the inputString. */
    inputString += (char)Serial.read();
	
    /* If the incoming character is a newline, set a flag so the main loop can
	 * do something about it. Also, remove the trailing new line. */
    if(inputString[inputString.length()] == '\n')
	{
	  inputString[inputString.length()] == '\0';
      stringComplete = true;
      Serial.println("OK");
	}
  }
}

void loop()
{
//  waitCompleteSentence();

  //Serial.println(inputString);
    
/*  if(stringComplete == true)
  {
    int firstSeparator = inputString.indexOf(',');
    int digitalY = inputString.substring(firstSeparator).toInt();
    int secondSeparator = inputString.indexOf(',', firstSeparator + 1);
    int digitalX = inputString.substring(firstSeparator + 1, secondSeparator).toInt();
    int switchButton = inputString.substring(secondSeparator + 1, inputString.length()).toInt();

    stringComplete = false;
    inputString.remove(0, inputString.length());
    
    Serial.print("digitalX = "); Serial.println(digitalX);
    Serial.print("digitalY = "); Serial.println(digitalY);
    Serial.print("switch = "); Serial.println(switchButton);
  }  
*/
  
  /* Based on https://www.arduino.cc/en/Tutorial/ReadASCIIString. */
  while(Serial.available() > 0)
  {
    int digitalY = Serial.parseInt();
    int digitalX = Serial.parseInt();
    int switchButton = Serial.parseInt();
    if (Serial.read() == '\n') {
      Serial.print("digitalX = "); Serial.println(digitalX);
      Serial.print("digitalY = "); Serial.println(digitalY);
      Serial.print("switch = "); Serial.println(switchButton);
    }
  }
}
