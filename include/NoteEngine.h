#ifndef NOTEENGINE_H_
#define NOTEENGINE_H_

#include <State.h>

/*************************************************************************************
 ************************************************************************************* 
 * NOTE HANDLERS - Note on/off controllers
 *************************************************************************************
 ************************************************************************************/

/***************************************************************************
 * searchArr
 * Used to find the index of a note in the activeNotes array
 * Runs O(n) which is find because the n = 8
 * Returns -1 if the note isn't found
 */
int searchArr(int note)
{
  int i = 0;
  while(activeNotes[i] != note)
  {
    i++;
    if(i >= POLY) return -1;
  }
  return i;
}

/***************************************************************************
 * startNote
 * Helper for onNote
 * activate the two oscillators, pink or white noise, and envelope
 * calculate amplitude based on some function of velocity, volume knob,
 *   and exponential volume curve
 */
void startNote(int onIndex, int note, int velocity)
{
  int osc1Vol = (state[WF_MI1_CC] * velocity)/PEAK_VOL;
  int osc2Vol = (state[WF_MI2_CC] * velocity)/PEAK_VOL;
  int noisVol = (state[NO_MIX_CC]*velocity)/PEAK_VOL;
  
  waveforms1[onIndex].frequency(frequency[note] * globalBend);
  waveforms2[onIndex].frequency(frequency[note+12] * globalBend);
  waveforms1[onIndex].amplitude(volume[osc1Vol]);
  waveforms2[onIndex].amplitude(volume[osc2Vol]);

  if(state[NO_SEL_CC] == 1) pink[onIndex].amplitude(volume[noisVol]);
  else white[onIndex].amplitude(volume[noisVol]); 
  if(state[FI_SEL_CC] == 1) filter[onIndex].frequency(2000 + 80 * state[FI_DEP_CC] * hardVolume[velocity]);
  if(state[BO_SEL_CC] == 1) sat[onIndex].bits(16 - (volume[(int)(velocity*state[BO_MIX_CC]/127)] * 16));
  envelope[onIndex].noteOn(); 
}

/***************************************************************************
 * stopNote
 * Helper for offNote
 * mute the oscilattors and noise
 * note that this isn't called if the envelope is active
 * only release a note when the vibrato isn't modulating (start of cycle)
 */
void stopNote(int offIndex)
{
  while(vibStep != 0){}
  waveforms1[offIndex].amplitude(0);
  waveforms2[offIndex].amplitude(0);
  pink[offIndex].amplitude(0);
  white[offIndex].amplitude(0);
}

/***************************************************************************
 * onNote
 * Handled by MIDI.read()
 * finds the first empty slot in the activeNotes array
 * if there are no empty slots it overwrites at index 0
 * calls startNote() to begin audio
 * stores note value globally in activeNotes
 */
void onNote(byte channel, byte note, byte velocity)
{
  // A0 - C7
  if( note > 21 && note < 109 )
  {
    int onIndex = searchArr(0);
    if(onIndex == -1) onIndex = 0;
    startNote(onIndex, note, velocity);
    activeNotes[onIndex] = note;
  }
}

/***************************************************************************
 * offNote
 * Handled by MIDI.read()
 * search activeNotes to find the index of the note to remove
 * if the envelope is active, start the release process
 * otherwise, just mute the waveforms
 * sthen remove it from the global array
 */
void offNote(byte channel, byte note, byte velocity)
{
  if(note > 21 && note < 109)
  {
    int offIndex = searchArr((int)note);
    if(offIndex != -1)
    {
      if(state[EN_SEL_CC] == 0) stopNote(offIndex);
      else 
      {
        while(vibStep!=0){}
        envelope[offIndex].noteOff();
      }
      activeNotes[offIndex] = 0;
    }
  }
}

/***************************************************************************
 * pitchBend
 * The only MIDI function not automatically built in to MIDI.read()
 * A MIDI bend value is 16 bits instead of 8
 * Use library function map() to map that to a whole tone of pitch
 * Update the globalBend variable to catch any existing notes
 */
void pitchBend(byte channel, int bend)
{
  float bendF = map(float(bend), 0, 32767, -2, 2);
  globalBend = pow(2, bendF / 12);

  for(int i = 0; i < POLY; i++)
  {
    if(activeNotes[i] != 0)
    {
      int currNote = activeNotes[i];
      waveforms1[i].frequency(frequency[currNote] * globalBend);
      waveforms2[i].frequency(frequency[currNote+12] * globalBend);
    }
  }
}

void startMidi()
{
  #ifdef USB
//    usbMIDI.setHandleControlChange(changeControl);
  #endif
  MIDI.begin();
  MIDI.setHandleNoteOn(onNote);
  MIDI.setHandleNoteOff(offNote);
  MIDI.setHandleControlChange(changeControl);
  //MIDI.setHandlePitchChange(pitchBend);
}

#endif