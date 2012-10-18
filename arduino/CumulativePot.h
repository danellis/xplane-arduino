#ifndef CUMULATIVEPOT_H
#define CUMULATIVEPOT_H

#include <arduino.h>

class CumulativePot {
public:
  CumulativePot(int pin, int startValue, unsigned long updateDelay);
  boolean hasChanged();
  int getValue();
  
private:
  int pin;
  int oldValue;
  int acc;
  unsigned long updateDelay;
  unsigned long lastUpdateTime;
};

#endif

