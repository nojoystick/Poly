/*********
 * Dallin Williams
 * 2.26.2019 - 10.12.2019
 * This is an 8 voice, dual oscillator polyphonic synthesizer
 * It takes 5 pin MIDI input to RX1
 * Teensy 4.0 and Teensy Audio Board
 */
#define USB

#include <Audio.h>
#include "NoteEngine.h"
#include "Io.h"

void setup()
{
  pinMode(A8, INPUT);
  delay(1000);
  AudioMemory(100);
  initializeAudio();
  startMidi();
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.7);
  Serial.begin(9600);
  Serial.println("Hello!");
}

void loop()
{
  #ifdef USB
//    usbMIDI.read();
  #endif
    MIDI.read(); // calls all the daughter functions as needed (onNote(), offNote(), changeControl())
  int read = analogRead(A8)*DIV8;
  if(state[BO_MIX_CC] != read) changeControl(0, BO_MIX_CC, read);
}