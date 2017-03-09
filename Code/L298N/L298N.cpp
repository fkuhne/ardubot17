/*
 * L298N Library
 *
 * Felipe Kühne
 * 03/05/2017
 */

#include "L298N.h"

DCMotor::DCMotor(const int enable, const int inA, const int inB)
{
  enable_pin = enable;
  inA_pin = inA;
  inB_pin = inB;
  
  _motorState = STOP;
  _linearVelocity = _lastlinearVelocity = V0;
}

/*DCMotor::DCMotor& operator= (const DCMotor& other)
{
  if (this != &other) // protect against invalid self-assignment
  {
    enable_pin = other.enable_pin;
	inA_pin = other.inA_pin;
	inB_pin = other.inB_pin;
	
	_motorDirection = other._motorDirection;
	_motorState = other._motorState;
	_linearVelocity = other._linearVelocity;
	_lastlinearVelocity = other._lastlinearVelocity;
  }
  return *this;
}*/

L298N::L298N(DCMotor& motor)
{
  pinMode(motor.enable_pin, OUTPUT);
  digitalWrite(motor.enable_pin, LOW);

  pinMode(motor.inA_pin, OUTPUT);
  digitalWrite(motor.inA_pin, LOW);

  pinMode(motor.inB_pin, OUTPUT);
  digitalWrite(motor.inB_pin, LOW);
  
  _motor1 = motor;
  isMotor2Defined = false;
};

L298N::L298N(const DCMotor& motor1, const DCMotor& motor2)
{
  pinMode(motor1.enable_pin, OUTPUT);
  digitalWrite(motor1.enable_pin, LOW);

  pinMode(motor1.inA_pin, OUTPUT);
  digitalWrite(motor1.inA_pin, LOW);

  pinMode(motor1.inB_pin, OUTPUT);
  digitalWrite(motor1.inB_pin, LOW);

  _motor1 = motor1;
  
  pinMode(motor2.enable_pin, OUTPUT);
  digitalWrite(motor2.enable_pin, LOW);

  pinMode(motor2.inA_pin, OUTPUT);
  digitalWrite(motor2.inA_pin, LOW);

  pinMode(motor2.inB_pin, OUTPUT);
  digitalWrite(motor2.inB_pin, LOW);

  _motor2 = motor2;
  isMotor2Defined = true;
}

int L298N::setState(const DCMotor& motor, const motorState state)
{
  if(motor.enable_pin == 0)
	  return -1;
  
  if(state == STOP)
    digitalWrite(motor.enable_pin, LOW);

  else if(state == RUN)
    digitalWrite(motor.enable_pin, HIGH);

  else
    return -1;

  return 0;
}

int L298N::setState(const motorState state)
{
  int ret = setState(_motor1, state);
  if(!ret && isMotor2Defined)
    ret = setState(_motor1, state);

  return ret;
}

int L298N::setDirection(const DCMotor& motor, motorDirection direction)
{
  if(motor.inA_pin == 0 || motor.inB_pin == 0)
	  return -1;
  
  if(direction == FW)
  {
    digitalWrite(motor.inA_pin, LOW);
    digitalWrite(motor.inB_pin, HIGH);
  }
  else if(direction == BW)
  {
    digitalWrite(motor.inA_pin, HIGH);
    digitalWrite(motor.inB_pin, LOW);
  }
  else
    return -1;

  return 0;
}

int L298N::setDirection(const motorDirection direction)
{
  int ret = setDirection(_motor1, direction);
  if(!ret && isMotor2Defined)
    ret = setDirection(_motor2, direction);

  return ret;
}

/* Maps the linear velocity back to a PWM duty cycle. */
int L298N::setSpeed(const DCMotor& motor, const linearVelocity v, const angularVelocity w)
{
	if(motor.enable_pin == 0)
		return -1;
	
	if(v == V0)
		setState(motor, STOP);
	
	int dutyCycle = map(v, V0, V1, 0, 255);
	analogWrite(motor.enable_pin, dutyCycle);

	return 0;
}

int L298N::setSpeed(const linearVelocity v, const angularVelocity w)
{
	int ret = setSpeed(_motor1, v, w);
	if(ret == 0 && isMotor2Defined == true)
		ret = setSpeed(_motor2, v, w);

	return ret;
}
