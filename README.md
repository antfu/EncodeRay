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

## Install

1. Download the zip file on the [release page](https://github.com/antfu/EncodeRay/releases/latest)
2. Go to Arduino IDE, navigate to `Sketch > Include Library > Add .ZIP Library...`
3. Select the downloaded zip
4. Add `#include <EncodeRay.h>` to your code and enjoy!

## License

MIT
