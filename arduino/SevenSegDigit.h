#ifndef SEVENSEGDIGIT_H
#define SEVENSEGDIGIT_H

#include <arduino.h>

class SevenSegDigit {
public:
  SevenSegDigit(int startPin) : startPin(startPin) {
    for (int i = 0; i < 7; ++i) {
      pinMode(startPin + i, OUTPUT);
      digitalWrite(startPin + i, LOW);
    }
  }
  
  void display(int value);
  
private:
  static int segments[][7];
  int startPin;
};

#endif

