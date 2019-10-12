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
  
  waveforms1[onIndex].frequency(frequency[note]);
  waveforms2[onIndex].frequency(frequency[note+12]);
  waveforms1[onIndex].amplitude(volume[osc1Vol]);
  waveforms2[onIndex].amplitude(volume[osc2Vol]);
  if(state[NO_SEL_CC] == 1) pink[onIndex].amplitude(volume[noisVol]);
  else white[onIndex].amplitude(volume[noisVol]);
  envelope[onIndex].noteOn();  
  if(state[FI_SEL_CC] == 1) velFilter[onIndex].frequency(80 * state[FI_FRQ_CC] * hardVolume[velocity]);
  if(state[BO_SEL_CC] == 1) sat[onIndex].bits(16 - ((int)(state[BO_MIX_CC]*DIV8)*volume[velocity]));
}

/***************************************************************************
 * stopNote
 * Helper for offNote
 * mute the oscilattors and noise
 * note that this isn't called if the envelope is active
 */
void stopNote(int offIndex)
{
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
      else envelope[offIndex].noteOff();
      activeNotes[offIndex] = 0;
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
}

#endif