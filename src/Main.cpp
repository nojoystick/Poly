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
  AudioMemory(100);
  initializeAudio();
  startMidi();
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.6);
  Serial.begin(9600);
}

void loop()
{
  static int vel = 47;
  static int note = 69;
  #ifdef USB
//    usbMIDI.read();
  #endif
  MIDI.read(); // calls all the daughter functions as needed (onNote(), offNote(), changeControl())
  //int read = analogRead(A8)*DIV8;
  //if(state[BO_MIX_CC] != read) changeControl(0, BO_MIX_CC, read);


  if(MIDI.read() && MIDI.getType() == midi::PitchBend)
  {
    int bend = MIDI.getData2() * 256 + MIDI.getData1();
    Serial.println(bend);
    pitchBend(0, bend);
  }

  while(state[LF_DEP_CC] < 127)
  {
    vel = 37;
    while(vel <= 120)
      {
        onNote(0, note, vel);
        onNote(0, note-24, vel);
        onNote(0, note+4, vel);
        onNote(0, note+7, vel);
        onNote(0, note+11, vel);
        int i = 18000;
        while(i < 36000)
        {
          i+=1000;
          pitchBend(0, i);
          delay(25);
        }
        offNote(0, note, vel);
        offNote(0, note-24, vel);
        offNote(0, note+4, vel);
        offNote(0, note+7, vel);
        offNote(0, note+11, vel);
        delay(100);
        vel+=25;
      }
      vel = 37;
      while(vel <= 120)
      {
        onNote(0, note-7, vel);
        onNote(0, note-24-7, vel);
        onNote(0, note+4-7, vel);
        onNote(0, note+7-7, vel);
        onNote(0, note+11-7, vel);
        delay(450);
        offNote(0, note-7, vel);
        offNote(0, note-24-7, vel);
        offNote(0, note+4-7, vel);
        offNote(0, note+7-7, vel);
        offNote(0, note+11-7, vel);
        delay(100);
        vel+=25;
      }
  }
  
}