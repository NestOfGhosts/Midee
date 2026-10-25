#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "config.h"

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform4; //xy=327,450
AudioSynthWaveform       waveform1;      //xy=333,243
AudioSynthWaveform       waveform2; //xy=340,307
AudioSynthWaveform       waveform12; //xy=340,1056
AudioSynthWaveform       waveform9; //xy=346,849
AudioSynthWaveform       waveform3; //xy=350,361
AudioSynthWaveform       waveform8; //xy=351,755
AudioSynthWaveform       waveform10; //xy=353,913
AudioSynthWaveform       waveform5; //xy=357,548
AudioSynthWaveform       waveform6; //xy=364,612
AudioSynthWaveform       waveform11; //xy=363,967
AudioSynthWaveform       waveform7; //xy=374,666
AudioMixer4              mixer1;         //xy=670,303
AudioMixer4              mixer3; //xy=683,909
AudioMixer4              mixer2; //xy=694,608
AudioMixer4              mixer4;         //xy=982,527
AudioOutputI2S           i2s1;           //xy=1314,544
AudioConnection          patchCord1(waveform4, 0, mixer1, 3);
AudioConnection          patchCord2(waveform1, 0, mixer1, 0);
AudioConnection          patchCord3(waveform2, 0, mixer1, 1);
AudioConnection          patchCord4(waveform12, 0, mixer3, 3);
AudioConnection          patchCord5(waveform9, 0, mixer3, 0);
AudioConnection          patchCord6(waveform3, 0, mixer1, 2);
AudioConnection          patchCord7(waveform8, 0, mixer2, 3);
AudioConnection          patchCord8(waveform10, 0, mixer3, 1);
AudioConnection          patchCord9(waveform5, 0, mixer2, 0);
AudioConnection          patchCord10(waveform6, 0, mixer2, 1);
AudioConnection          patchCord11(waveform11, 0, mixer3, 2);
AudioConnection          patchCord12(waveform7, 0, mixer2, 2);
AudioConnection          patchCord13(mixer1, 0, mixer4, 0);
AudioConnection          patchCord14(mixer3, 0, mixer4, 2);
AudioConnection          patchCord15(mixer2, 0, mixer4, 1);
AudioConnection          patchCord16(mixer4, 0, i2s1, 0);
AudioConnection          patchCord17(mixer4, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=116,96
// GUItool: end automatically generated code

AudioSynthWaveform* waves[12] = {
  &waveform1, &waveform2, &waveform3, &waveform4, 
  &waveform5, &waveform6, &waveform7, &waveform8, 
  &waveform9, &waveform10, &waveform11, &waveform12, 
};

bool previous[12];

bool prevSus;

int prevBend = -1;

float prevVol = -1;

int playing[12];

int semis = (int)bendRange;

int cents = (int)((bendRange - semis) * 100);

float bendSemis = 0;

float noteToHz(float note) {
  return 440.0 * pow(2.0, (note + bendSemis - 69) / 12.0);
}

void setup() {
  Serial.begin(9600);

  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0);

  for (int i = 0; i < 12; i++) {
    waves[i]->begin(oscType);
    waves[i]->amplitude(0);
  }

  for (int c = 0; c < 4; c++) {
    mixer1.gain(c, mixerGain1);
    mixer2.gain(c, mixerGain1);
    mixer3.gain(c, mixerGain1);
  }

  for (int c = 0; c < 3; c++) {
    mixer4.gain(c, mixerGain2);
  }
    
  analogReadResolution(12);
  analogReadAveraging(8);

  for (int pins : keyPin) {
    pinMode(pins, INPUT_PULLUP);
  }

  pinMode(susPin, INPUT_PULLUP);

  delay(midiDelay);
  usbMIDI.sendControlChange(101, 0, midiChannel);
  usbMIDI.sendControlChange(100, 0, midiChannel);
  usbMIDI.sendControlChange(6, semis, midiChannel);
  usbMIDI.sendControlChange(38, cents, midiChannel);
  usbMIDI.sendControlChange(101, 127, midiChannel);
  usbMIDI.sendControlChange(100, 127, midiChannel);
}

void loop() {
  float vol = analogRead(volPin) / 4095.0;
  if (fabs(vol - prevVol) > 0.01) {
    sgtl5000_1.volume(vol);
    prevVol = vol;
  }
  
  int pitpot = analogRead(pitchPin);
  int bend = map(pitpot, 0, 4095, 1, 16383);
  if (abs(bend - 8192) < octDeadzone) {
    bend = 8192;
  }
  if (abs(bend - prevBend) > threshold) {
    usbMIDI.sendPitchBend(bend, midiChannel);
    prevBend = bend;
    bendSemis = (bend - 8192) / 8192.0 * bendRange;
    for (int i = 0; i < 12; i++) {
      if (playing[i] > 0) {
        waves[i]->frequency(noteToHz(playing[i]));
      }
    }
  }


  for (int i = 0; i < 12; i++) {
    bool pressed = !digitalRead(keyPin[i]);
    if (pressed != previous[i]) {
      if (pressed) {
        playing[i] = baseNote + i;
        usbMIDI.sendNoteOn(playing[i], 100, midiChannel);
        waves[i]->frequency(noteToHz(playing[i]));
        waves[i]->amplitude(oscAmp);
      }
      else {
        usbMIDI.sendNoteOff(playing[i], 0, midiChannel);
        waves[i]->amplitude(0);
      }
      previous[i] = pressed;
      Serial.print("Key ");
      Serial.print(i);
      Serial.println(pressed ? " is pressed" : " isn't pressed");
    }
  }

  bool susyn = !digitalRead(susPin);
  if (susyn != prevSus) {
    usbMIDI.sendControlChange(64, susyn ? 127 : 0, midiChannel);
    prevSus = susyn;
  }
}
