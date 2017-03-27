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
#include <L298N.h>

/*
 * Global constant definitions:
 */

const int serialFrameSize = 13;
unsigned long timeReceived = 0;

const float deadBand = 20.0;

/* Pins used for motor 1: */
const int enable1_pin = 9;
const int inA1_pin = 2;
const int inB1_pin = 3;

/* Pins used for motor 2: */
const int enable2_pin = 10;
const int inA2_pin = 5;
const int inB2_pin = 4;

const int btSerialRX_pin = 7;
const int btSerialTX_pin = 8;

const int buzzer_pin = 6;
/*
 * Global stuff:
 */

/* Connect the HC-05 TX to Arduino pin 2 RX and HC-05 RX to Arduino pin 3 TX
 * through a voltage divider: 5V---( 1k )--[RX]--(2k)---GND */
SoftwareSerial BTSerial(btSerialRX_pin, btSerialTX_pin);

DCMotor motor1(enable1_pin, inA1_pin, inB1_pin);
DCMotor motor2(enable2_pin, inA2_pin, inB2_pin);
L298N l298n(motor1, motor2);

void setup()
{
  /* Debug serial port. */
  Serial.begin(9600);

  /* Bluetooth serial port. */
  BTSerial.begin(9600);
}

/* This function was based on the example from
 * https://www.arduino.cc/en/Tutorial/ReadASCIIString. */
int waitCompleteSentence(int *digitalX, int *digitalY, int *button)
{
  /* Wait for an entire frame to be received. */
  if(BTSerial.available() < serialFrameSize)
    return -1;

  /* Once the frame is completed, we can parse it. */
  *digitalX = BTSerial.parseInt();
  *digitalY = BTSerial.parseInt();
  *button = BTSerial.parseInt();

  if(BTSerial.read() == '\n')
  {
    BTSerial.flush(); /* Clean up spurious data. */
    /* button equals 0 means an invalid frame. */
    if(button > 0)
      return 0;
  }

  return -1;
}

/* Read the digital Y and X values from the joystick and map them to linear
 * (tangential) speed and angular (rotational) speeds, respectively.
 *
 * digitalX and digitalY are on the range 0 ~ 1023. When the joystick is at
 * rest, their values are in the middle (1023/2), but can vary a little bit, so
 * let us consider a small deadband, and if the X/Y values are within it,
 * keep the robot still. We have to consider this for both axis.
 *
 * If we are outside the deadband area, we have to balance the X and Y
 * signals in order to generate linear and angular speeds, which will later be
 * transformed in PWM signals. The speeds will be mapped to percentage values,
 * from -100% to +100%, and then these values will be mapped to the logical
 * signals needed by the L298N H-bridge (enable signal (PWM) and inA/inB
 * (direction). This algorithm is based on
 *
 */
void applyControlSignals(int digitalX, int digitalY)
{
  const float deadbandLowerLimit = 1023.0 / 2.0 - deadBand;
  const float deadbandUpperLimit = 1023.0 / 2.0 + deadBand;

  /* If it's inside the deadband area, turn off the motors and return. */
  if(digitalX > deadbandLowerLimit &&
     digitalX < deadbandUpperLimit &&
     digitalY > deadbandLowerLimit &&
     digitalY < deadbandUpperLimit)
  {
    l298n.setState(STOP);
    return;
  }

  /* Here we are outside the deadband area. */

  float vPercentage = 0.0; /* Linear velocity in percentage level. */
  float wPercentage = 0.0; /* Angular velocity in percentage level. */

  /* Let's compute the angular velocity first. For the left, that is, between
   * 1023 and ((1023/2)+deadband), it is positive. For the right, between
   * ((1023/2)-deadband) and 0, it is negative. Let us then map this and
   * translate it to a percentage level, that is, translate from 1023~0 to
   * -100~100. */
  if(digitalX >= deadbandUpperLimit)
  {
    wPercentage = map(digitalX, deadbandUpperLimit, 1023.0, 0.0, 100.0);
  }
  else if(digitalX <= deadbandLowerLimit)
  {
    wPercentage = map(digitalX, deadbandLowerLimit, 0.0, 0.0, -100.0);
  }

  /* Let us now compute the linear velocity. The same scheme as above will
   * be applied, that is, translate from 0~1023 to -100~100. */
  if(digitalY >= deadbandUpperLimit)
  {
    vPercentage = map(digitalY, deadbandUpperLimit, 1023, 0, 100);
  }
  else if(digitalY <= deadbandLowerLimit)
  {
    vPercentage = map(digitalY, deadbandLowerLimit, 0.0, 0.0, -100.0);
  }

  Serial.print("vPercentage = "); Serial.print(vPercentage);
  Serial.print(", wPercentage = "); Serial.println(wPercentage);

  /* Distribute the signals for left and right wheels, acoording to the
   * speeds. */
  int leftPercentage = vPercentage + wPercentage;
  int rightPercentage = vPercentage - wPercentage;

  Serial.print("leftPercentage = "); Serial.print(leftPercentage);
  Serial.print(", rightPercentage = "); Serial.println(rightPercentage);

  /* Computes a scale factor. If any result exceeds 100% then adjust the scale
   * so that the result = 100% and use same scale value for other motor. */
  float maxPercentage = max(abs(leftPercentage), abs(rightPercentage));
  float scale = min(1,(100.0/maxPercentage));

  leftPercentage *= scale;
  rightPercentage *= scale;

  Serial.print(", scale = "); Serial.print(scale);
  Serial.print(", leftPercentage = "); Serial.print(leftPercentage);
  Serial.print(", rightPercentage = "); Serial.println(rightPercentage);

  /* Duty Cycle for the motors. */
  int PWMLeft = map(abs(leftPercentage), 0, 100, 0, 255);
  int PWMRight = map(abs(rightPercentage), 0, 100, 0, 255);

  Serial.print("PWMLeft = "); Serial.print(PWMLeft);
  Serial.print(leftPercentage > 0 ? " (FW)" : " (BW)");
  Serial.print(", PWMRight = "); Serial.print(PWMRight);
  Serial.println(rightPercentage > 0 ? " (FW)" : " (BW)");

  /* Finally, apply the control signals. */
  l298n.setDirection(motor1, (leftPercentage > 0) ? FW : BW);
  l298n.setDirection(motor2, (rightPercentage > 0) ? FW : BW);
  l298n.setDutyCycle(PWMLeft, PWMRight);

  Serial.println("OK.");
  delay(10);
}


/* Make something with the button, like play a buzzer or turn LEDs on. */
void applyButtonAction(int button)
{
  if(button == 1)
    tone(buzzer_pin, 260, 1000/8);
  else
    noTone(buzzer_pin);
}

void loop()
{
  /* Initialize with invalid data so that at each loop we can know if some
   * valid has been received. */
  int digitalX = -1;
  int digitalY = -1;
  int button = -1;
  
  /* Check for valid data. */
  if(waitCompleteSentence(&digitalX, &digitalY, &button) == 0)
  {
    /* Debug: */
    Serial.print("digitalX = "); Serial.print(digitalX);
    Serial.print(", digitalY = "); Serial.print(digitalY);
    Serial.print(", button = "); Serial.println(button);

    applyControlSignals(digitalX, digitalY);
    applyButtonAction(button);

    /* Register when was the last valid frame. */
    timeReceived = millis();
  }
  else
  {
    /* Compute the time interval between the last received frame
     *  and now. If higher than 1 second, turn of the motors. */
    unsigned long timeNow = millis();
    if(timeNow - timeReceived > 1000)
    {
      Serial.println("Communication timeout. Turning off motors.");
      timeReceived = millis();
      l298n.setState(STOP);
    }
  }
}

/* Just a test function, in case you want to make sure the wirings are OK. */
void testBridgeAndMotors()
{
  l298n.setState(RUN);

  l298n.setDirection(FW);
  l298n.setState(RUN);
  delay(1000);

  l298n.setState(STOP);
  delay(1000);

  l298n.setDirection(BW);
  l298n.setState(RUN);
  delay(1000);

  l298n.setState(STOP);
  delay(1000);
}
