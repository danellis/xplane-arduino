#ifndef BUTTON_H
#define BUTTON_H

#include "DigitalInput.h"

class Button : public DigitalInput {
public:
  Button(int pin) : DigitalInput(pin) {}
  boolean isPressed() { return getValue() == LOW; }
};

#endif

