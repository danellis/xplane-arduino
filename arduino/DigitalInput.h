#ifndef DIGITALINPUT_H
#define DIGITALINPUT_H

#include <arduino.h>

class DigitalInput {
public:
  DigitalInput(int pin);
  boolean hasChanged();
  int getValue();

protected:
  int pin;
  int oldValue;
};

#endif

