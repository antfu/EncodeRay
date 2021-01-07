#ifndef ENCODE_RAY_H
#define ENCODE_RAY_H

#include <Arduino.h>
#include <Encoder.h>

class EncodeRay {
  public:
    EncodeRay(uint8_t pinENA, uint8_t pinENB, uint8_t pinIN1, uint8_t pinIN2, uint8_t pinPWM, uint8_t pinSTBY, int gearRatio);
    void read();
    void update();
    void drive();
    void stop();
    void standby();
    void brake();
    void rotate(float r);
    void rotateTo(float r);
    float getPosition();
    bool isActive();
    void reverse();

    int cpr = 12;
    int gearRatio;
  private:
    Encoder* _encoder;
    int _pos = 0, _prevPos = 0, _targetPos = 0, _speed = 0;
    int _pinIN1, _pinIN2, _pinPWM, _pinSTBY;
    int _currTime, _prevTime, _deltaTime;
    long _integral = 0;
    bool _active = false;
    bool _reversed = false;

    float getVoltage();
};

#endif
