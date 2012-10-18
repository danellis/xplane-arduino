#include "SevenSegDigit.h"

int SevenSegDigit::segments[][7] = {
  {0, 1, 1, 1, 1, 1, 1}, // 0
  {0, 0, 0, 1, 0, 0, 1}, // 1
  {1, 0, 1, 1, 1, 1, 0}, // 2
  {1, 0, 1, 1, 0, 1, 1}, // 3
  {1, 1, 0, 1, 0, 0, 1}, // 4
  {1, 1, 1, 0, 0, 1, 1}, // 5
  {1, 1, 1, 0, 1, 1, 1}, // 6
  {0, 0, 1, 1, 0, 0, 1}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 8, 1, 1}, // 9
};

void SevenSegDigit::display(int value) {
  for (int b = 0; b < 7; ++b) {
    digitalWrite(b + startPin, segments[value][b] ? HIGH : LOW);
  }
}

