#include <Encoder.h>
#include "EncodeRay.h"

#define ACCEPTABLE_COUNT_ERROR 0
#define ACTIVATION_COUNT_ERROR 1
#define MAXIMAL_VOLTAGE 255

// TODO: each motor should have different parameters
const float Kp = 2;
const float Ki = 0.00001; //0.00001;
const float Kd = 0;       // negative here to compensate for different speeds when approaching target

const int KiActivationErrorRange = 100;
const int KiRange2 = 10;
const float KiRange2Factor = 2;
const int KdActivationErrorRange = 100;

EncodeRay::EncodeRay(uint8_t pinEN1, uint8_t pinEN2, uint8_t pinIN1, uint8_t pinIN2, uint8_t pinPWM, uint8_t pinSTBY, int _gearRatio) {
  _pinIN1 = pinIN1;
  _pinIN2 = pinIN2;
  _pinPWM = pinPWM;
  _pinSTBY = pinSTBY;
  gearRatio = _gearRatio;
  _prevTime = _currTime = micros();
  _encoder = new Encoder(pinEN1, pinEN2);
  pinMode(pinIN1, OUTPUT);
  pinMode(pinIN2, OUTPUT);
  pinMode(pinPWM, OUTPUT);
  pinMode(pinSTBY, OUTPUT);
}

void EncodeRay::read() {
  _pos = _encoder->read();
  _prevPos = _pos;

  _prevTime = _currTime;
  _currTime = micros();
  _deltaTime = _currTime - _prevTime;

  if (_deltaTime > 1000) {
    float newSpeed = (float)(_pos - _prevPos) / _deltaTime;
    _prevPos = _pos;
    _speed = _speed * 0.8 + newSpeed * 0.2;
  }
};

void EncodeRay::stop() {
  _active = false;
  digitalWrite(_pinIN1, LOW);
  digitalWrite(_pinIN2, LOW);
}

void EncodeRay::standby() {
  digitalWrite(_pinSTBY, LOW);
}

void EncodeRay::brake() {
  digitalWrite(_pinPWM, LOW);
}

void EncodeRay::rotate(float r) {
  rotateTo(getPosition() + r);
}

void EncodeRay::rotateTo(float r) {
  _targetPos = r * cpr * gearRatio;
}

float EncodeRay::getPosition() {
  return (float)_pos / cpr / gearRatio;
}

bool EncodeRay::isActive() {
  return _active;
}

void EncodeRay::drive() {
  float voltage = getVoltage();

  if (voltage == 0) {
    stop();
    return;
  }

  int direction = voltage < 0 && !_reversed ? LOW : HIGH;

  //Select the motor to turn, and set the direction and the Voltage
  digitalWrite(_pinIN1, direction);
  digitalWrite(_pinIN2, !direction);
  analogWrite(_pinPWM, abs(voltage));

  //Finally , make sure STBY is disabled - pull it HIGH
  digitalWrite(_pinSTBY, HIGH);
}

void EncodeRay::update() {
  read();
  drive();
} 

float EncodeRay::getVoltage(){
  long error;
  float I, D;
  int voltage;

  error = _targetPos - _pos;
  long errorAbs = abs(error);

  // Integral
  if (errorAbs < KiActivationErrorRange) {
    _integral += error * _deltaTime;
    if (errorAbs < KiRange2) {
      I = Ki * KiRange2Factor * _integral;
    } else {
      I = Ki * _integral;
    }
  } else {
    _integral = 0;
    I = 0;
  }
  
  //Derivative
  if (errorAbs < KdActivationErrorRange) {
    D = Kd * _speed;
  } else {
    D = 0;
  }

  //Derive driving voltage
  if (errorAbs > ACTIVATION_COUNT_ERROR) {
     _active = true;
  }
  if (errorAbs > ACCEPTABLE_COUNT_ERROR && _active) {
    // after motor has reached ACCEPTABLE_COUNT_ERROR, the error needs to excceed ACTIVATION_COUNT_ERROR to start the motor again.
    voltage = Kp * error + I + D;

    if (voltage > 0) {
      if (voltage > MAXIMAL_VOLTAGE) {
        voltage = MAXIMAL_VOLTAGE;
      }
    } else {
      if (voltage < -MAXIMAL_VOLTAGE) {
        voltage = -MAXIMAL_VOLTAGE;
      }
    }
  } else {
    _integral = 0;
    voltage = 0;
  }

  return voltage;
}

void EncodeRay::reverse() {
  _reversed = !_reversed;
}


void EncodeRay::setOrigin() {
  _targetPos = 0;
  _pos = 0;
  _prevPos = 0;
  stop();
}
