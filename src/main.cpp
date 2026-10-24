#include <Arduino.h>

const int keyPin[12] = {0,1 };

void setup() {
  Serial.begin(9600);
  pinMode(keyPin, INPUT_PULLUP);
}

void loop() {
  Serial.println(digitalRead(keyPin));
}
