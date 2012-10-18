#include "CumulativePot.h"

CumulativePot::CumulativePot(int pin, int startValue, unsigned long updateDelay) : pin(pin), acc(startValue), updateDelay(updateDelay) {
  oldValue = analogRead(pin);
  lastUpdateTime = millis();
}

boolean CumulativePot::hasChanged() {
  unsigned long time = millis();
  if (time < lastUpdateTime + updateDelay) {
    return false;
  } else {
    lastUpdateTime = time;
  }
  
  int value = analogRead(pin) - 512;
  int newAcc = acc + value / 128;
  if (newAcc > 127) newAcc = 127;
  if (newAcc < -128) newAcc = -128;
  boolean changed = (acc != newAcc);
  acc = newAcc;
  return changed;
}

int CumulativePot::getValue() {
  int value = analogRead(pin) - 512;
  acc += value / 128;
  if (acc > 127) acc = 127;
  if (acc < -128) acc = -128;
  return acc;
}

