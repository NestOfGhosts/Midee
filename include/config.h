#pragma once 
#include <Audio.h>

// Deadzones and delays
const int threshold = 20; // amount of distance to bend the pitch. to counteract pot instability. increase if pitch change without movement; lower if pitch doesnt change with movement (Pitch bend is out of 16384)
const int octDeadzone = 100; // centertuned deadzone. Adjust to change deadzone for middle c tuning. pitch values are out of 16383
const int midiDelay = 1000; // delay from midi power-on. adjust if the arduino is starting midi before your computer opens a serial port / midi takes too long to boot up. delay is in miliseconds 

// Pins. all the connections irl
const int keyPin[12] = {0,1,2,3,4,5,6,9,10,11,12,16}; // Pins the keys are connected to. keyPin[0] is C, keyPin[1] is Db etc
const int susPin = 17; // Pin the suspend key is connected to
const int pitchPin = A0;  // Pin the pitch bend is connected to (A0 is equivalent to digital pin 14 on the Teensy 4.0)
const int volPin = A1; // Pin the volume controls are connnected to (A1 is equivalent to digital pin 15)

// Midi settings
const int midiChannel = 1; // Channel the midi signals are sent through
const int baseNote = 60; // what middle c is tuned to. 60 is middle c, and you can change it to 47 for a lower c, and 73 for a higher c. change this if you want the instrument to play on a different range. Can also use this to change the key of the instrument

// Live playback settings
const int oscType = WAVEFORM_SAWTOOTH; // Changes the waveform type (how live playback sounds
// Adjusts these three for balancing and clipping and stuff.
const float oscAmp = 0.3; // Amplifier of each note. Adjust for volume/gain
const float mixerGain1 = .25; // Gain of the first level of mixer. Each oscillator passes through two levels of mixer
const float mixerGain2 = .33; // Gain of second level mixer
