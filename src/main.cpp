#include <Arduino.h>

const int threshold = 20;

const int octDeadzone = 100;

const int midiDelay = 1000;

const int keyPin[12] = {0,1,2,3,4,5,6,9,10,11,12,16};
bool previous[12];

const int susPin = 17;
bool prevSus;

const int pitchPin = A0;
int prevBend = -1;

const int baseNote = 60;
int playing[12];

void setup() {
  Serial.begin(9600);

  analogReadResolution(12);
  analogReadAveraging(8);

  for (int pins : keyPin) {
    pinMode(pins, INPUT_PULLUP);
  }

  pinMode(susPin, INPUT_PULLUP);

  delay(midiDelay);
  usbMIDI.sendControlChange(101, 0, 1);
  usbMIDI.sendControlChange(100, 0, 1);
  usbMIDI.sendControlChange(6, 12, 1);
  usbMIDI.sendControlChange(38, 0 ,1);
  usbMIDI.sendControlChange(101, 127, 1);
  usbMIDI.sendControlChange(100, 127, 1);
}

void loop() {
  int pitpot = analogRead(pitchPin);
  int bend = map(pitpot, 0, 4095, 1, 16383);
  if (abs(bend - 8192) < octDeadzone) {
    bend = 8192;
  }
  if (abs(bend - prevBend) > threshold) {
    usbMIDI.sendPitchBend(bend, 1);
    prevBend = bend;
  }


  for (int i = 0; i < 12; i++) {
    bool pressed = !digitalRead(keyPin[i]);
    if (pressed != previous[i]) {
      if (pressed) {
        playing[i] = baseNote + i;
        usbMIDI.sendNoteOn(playing[i], 100, 1);
      }
      else {
        usbMIDI.sendNoteOff(playing[i], 0, 1);
      }
      previous[i] = pressed;
      Serial.print("Key ");
      Serial.print(i);
      Serial.println(pressed ? " is pressed" : " isn't pressed");
    }
  }

  bool susyn = !digitalRead(susPin);
  if (susyn != prevSus) {
    usbMIDI.sendControlChange(64, susyn ? 127 : 0, 1);
    prevSus = susyn;
  }
}
