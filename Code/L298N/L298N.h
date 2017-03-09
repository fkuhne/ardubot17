/*
 * L298N Library
 *
 * Felipe Kühne
 * 03/05/2017
 */

#ifndef __L298N_H__
#define __L298N_H__

#include <Arduino.h>

enum motorDirection {FW = 0, BW};
enum angularVelocityDirection {CW = 0, CCW};
enum motorState {STOP = 0, RUN};
/* These two enums maps analog values for velocities to 6 defined levels. It
 * must then be mapped back to PWM values (0 to 255). */
//enum linearVelocity {V0 = 0, V1, V2, V3, V4, V5};
//enum angularVelocity {W0 = 0, W1, W2, W3, W4, W5};
enum linearVelocity {V0 = 0, V1};
enum angularVelocity {W0 = 0, W1};

class DCMotor
{
public:	
  DCMotor() {};
  DCMotor(const int enable, const int inA, const int inB);
  
  //DCMotor& operator= (const DCMotor& other);

  int enable_pin;
  int inA_pin;
  int inB_pin;

  motorDirection _motorDirection;
  motorState _motorState;
  int _linearVelocity, _lastlinearVelocity;
};

class L298N
{
public:
  L298N(DCMotor& motor);
  L298N(const DCMotor& motor1, const DCMotor& motor2);

  int setState(const DCMotor& motor, const motorState state);
  int setState(const motorState state);
  int setDirection(const DCMotor& motor, motorDirection direction);
  int setDirection(const motorDirection direction);
  int setSpeed(const DCMotor& motor, const linearVelocity v, const angularVelocity w);
  int setSpeed(const linearVelocity v, const angularVelocity w);

private:
  bool isMotor2Defined;
  DCMotor _motor1, _motor2;
};

#endif /* __L298N_H__ */
