#include "DigitalInput.h"

DigitalInput::DigitalInput(int pin) : pin(pin) {
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
  oldValue = digitalRead(pin);
}

boolean DigitalInput::hasChanged() {
  int value = digitalRead(pin);
  boolean changed = (value != oldValue);
  oldValue = value;
  return changed;
}

int DigitalInput::getValue() {
  oldValue = digitalRead(pin);
  return oldValue;
}

