#include <Servo.h>
#include "Switch.h"
#include "Button.H"
#include "SevenSegDigit.h"
#include "CumulativePot.h"

#define BATTERY_SWITCH_PIN 2
#define ENGINE_START_PIN 3
#define SPEED_DIGIT_PIN 4
#define ALTIMETER_PIN 11
#define BRAKE_WARN_PIN 12
#define GEN_WARN_PIN 11
#define OIL_WARN_PIN 13

Switch switches[] = {
  Switch(BATTERY_SWITCH_PIN)
};

Button buttons[] = {
  Button(ENGINE_START_PIN)
};

SevenSegDigit digit(SPEED_DIGIT_PIN);
// Servo altimeterServo;
CumulativePot throttle(A5, -128, 10);
CumulativePot mixture(A4, -128, 10);

void setup() {
  Serial.begin(115200);
//  altimeterServo.attach(ALTIMETER_PIN);
  pinMode(BRAKE_WARN_PIN, OUTPUT);
  digitalWrite(BRAKE_WARN_PIN, LOW);
  pinMode(OIL_WARN_PIN, OUTPUT);
  digitalWrite(OIL_WARN_PIN, LOW);
  pinMode(GEN_WARN_PIN, OUTPUT);
  digitalWrite(GEN_WARN_PIN, LOW);
}

void loop() {
  for (int i = 0; i < sizeof(switches) / sizeof(Switch); ++i) {
    if (switches[i].hasChanged()) {
      Serial.write(switches[i].isOn() ? 1 : 0);
      Serial.write(i);
    }
  }

  for (int i = 0; i < sizeof(buttons) / sizeof(Button); ++i) {
    if (buttons[i].hasChanged()) {
      Serial.write(buttons[i].isPressed() ? 2 : 3);
      Serial.write(i);
    }
  }
  
  if (throttle.hasChanged()) {
    Serial.write(4);
    Serial.write(throttle.getValue() + 128);
  }

  if (mixture.hasChanged()) {
    Serial.write(5);
    Serial.write(mixture.getValue() + 128);
  }

  if (Serial.available() >= 2) {
    int type = Serial.read();
    int value = Serial.read();
    switch (type) {
      case 0:
        digit.display(value % 10);
        break;
  
/*      case 1:
        altimeterServo.write(value);
        break;*/
      
      case 2:
        digitalWrite(BRAKE_WARN_PIN, value ? HIGH : LOW);
        break;

      case 3:
        digitalWrite(OIL_WARN_PIN, value ? HIGH : LOW);
        break;

      case 4:
        digitalWrite(GEN_WARN_PIN, value ? HIGH : LOW);
        break;
    }     
  }
}

