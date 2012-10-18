#ifndef SWITCH_H
#define SWITCH_H

#include "DigitalInput.h"

class Switch : public DigitalInput {
public:
  Switch(int pin) : DigitalInput(pin) {}
  boolean isOn() { return getValue() == LOW; }
};

#endif

