# EncodeRay.ino

Drive motors with encoder library for Arduino.

```cpp
#include <EncodeRay.h>

// EN1, EN2, IN1, IN2, PWM, STBY, GearRatio
EncodeRay motor1(20, 27, 26, 28, 13, 12, 211);

void setup() {
  Serial.begin(115200);
}

void loop() {
  motor1.update();
}

void serialEvent() {
  if (!Serial.available())
    return;

  char c = Serial.read();

  if (c == 'm') {
    motor1.rotateTo(Serial.parseFloat());
  }
  else if (c == 'r') {
    motor1.rotate(Serial.parseFloat());
  }
}
```
