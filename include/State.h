#ifndef STATE_H_
#define STATE_H_

#include <MIDI.h>
#include <Connections.h>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
static int state[NUM_INPUTS];           // store current values of global analog inputs; 0-1024
static int activeNotes[POLY];           // global store of currently active notes
static float globalBend = 1.0;
IntervalTimer vibratoTimer;
IntervalTimer lfoTimer;
volatile int vibStep = 0;
volatile int lfoStep = 0;

/*************************************************************************************
 ************************************************************************************* 
 *  CHANGECONTROL & HELPERS - Called from changeControl to persist state changes
 *************************************************************************************
 ************************************************************************************/
 
/***************************************************************************
 * updateVolume
 * called from changeControl
 * scale the outMixer volume based on the volume knob
 */
void updateVolume()
{
  for( int i = 0; i < 2; i++ ) outMixer[0].gain(i, MAX_OUT_VOL*volume[state[MA_VOL_CC]]);
}

void updateRate()
{
  // TODO
}

/***************************************************************************
 * updateBoost
 * called from changeControl
 * scale the boost level to the saturation knob
 */
void updateBoost()
{
  int val;
  if(state[BO_SEL_CC]==0) val = 16 - (volume[state[BO_MIX_CC]] * 16);
  else val = 16;
  for(int i = 0; i < POLY; i++) sat[i].bits(val);
}

/***************************************************************************
 * updateWaveform
 * called from changeControl
 * change wave shape between sin, rev. triangle, triangle, square, sawtooth
 */
void updateWaveform(short tone_type)
{
  int waveformToUpdate = state[WF_SEL_CC];
  if(waveformToUpdate == 1) for(int i = 0; i < POLY; i++) waveforms1[i].begin(tone_type);
  else                      for(int i = 0; i < POLY; i++) waveforms2[i].begin(tone_type);
}

/***************************************************************************
 * readFilter, updateFilterRes, updateFilterFreq
 * Filter update functions
 * If the filter switch is inactive, these go to a default value
 * When the filter switch is activated, readFilter will call both other
 *     functions to update the values to match the state
 */
void doLFO()
{
  lfo.frequency(5000 + state[LF_DEP_CC] * DIV127 * lfoCycle[lfoStep]);
  lfoStep++;
  if(lfoStep == 128) lfoStep = 0;
}

void doVibrato()
{
  float bendF = map(float(vibCycle[vibStep]), 0, 32767, -2, 2);
  float amtToBend = pow(2, state[VI_DEP_CC] * DIV127* bendF / 12);

  for(int i = 0; i < POLY; i++)
  {
    if(activeNotes[i]!=0)
    {
      int currNote = activeNotes[i];
      waveforms1[i].frequency(frequency[currNote] * globalBend * amtToBend);
      waveforms2[i].frequency(frequency[currNote+12] * globalBend * amtToBend);
    }
  }
  vibStep++;
  if(vibStep == 32) vibStep = 0;
}

void updateRes()
{
  int val = (4.3 * (state[FI_RES_CC]*DIV127)) + 0.7;
  //lfo.resonance(val);
  for(int i = 0; i < POLY; i++) filter[i].resonance(val);
  endFilter.resonance(val);

}

void updateFilterFreq()
{
  // if(state[FI_SEL_CC] == 0) 
  // {
  //   for(int i = 0; i < POLY; i++) filter[i].frequency(2000 + 80 * state[FI_DEP_CC]);
  // }
  int val = 10000 * (state[FI_DEP_CC] * DIV127);
  endFilter.frequency(val);
}

void readLFO()
{
  if(state[LF_SEL_CC] == 1) lfoTimer.begin(doLFO, volume[state[MA_RAT_CC]]*1000000/128);
  else lfoTimer.end();
}

void readFilter()
{
  updateRes();
  updateFilterFreq();
}

void updateVibrato()
{
  if(state[VI_SEL_CC] == 1 && state[MA_RAT_CC] > 10) vibratoTimer.begin(doVibrato, volume[state[MA_RAT_CC]]*500000/32);
  else vibratoTimer.end();
}

/***************************************************************************
 * readEnvelope, updateAttack, updateDecay, updateSustain, updateRelease
 * Envelope update functions
 * Based on a max period of 3s
 * If the envelope switch is inactive, these all go to a default value
 * If the envelope switch actives, readEnvelope will update all values
 *     to match the state
 */

void updateAttack()
{
  float val;
  if(state[EN_SEL_CC] == 1) val = (MAX_ENV_MILLIS*(state[EN_ATT_CC]*DIV127)) + 10.5;
  else val = 0;
  for(int i = 0; i < POLY; i++) envelope[i].attack(val);
}

void updateDecay()
{
  float val;
  if(state[EN_SEL_CC] == 1) val = MAX_ENV_MILLIS * (state[EN_DEC_CC]*DIV127);
  else val = 0;
  for(int i = 0; i < POLY; i++) envelope[i].decay(val);
}

void updateSustain()
{
  float val;
  if(state[EN_SEL_CC] == 1) val = state[EN_SUS_CC]*DIV127;
  else val = 1;
  for(int i = 0; i < POLY; i++) envelope[i].sustain(val);
}

void updateRelease()
{
  float val;
  if(state[EN_SEL_CC] == 1) val = MAX_ENV_MILLIS * (state[EN_REL_CC]*DIV127);
  else val = 0;
  for(int i = 0; i < POLY; i++) envelope[i].release(val);
}

void readEnvelope()
{
  updateAttack();
  updateDecay();
  updateSustain();
  updateRelease();
}

/***************************************************************************
 * changeControl
 * Handled by MIDI.read()
 * Also called when hardware values change
 * Facilitator for all non-note state changes
 * Stores values from pins into global state[] array
 * Calls various helper functions to parse that new information
 */
void changeControl(byte channel, byte control, byte value)
{
  Serial.print(control);
  Serial.print("    ");
  Serial.println(value);

  if(control <= NUM_INPUTS) state[control] = value;  // update global state with the new reading
  switch(control)
  {
    // master and pre-amp stages
    case MA_VOL_CC:
      updateVolume();
    break;
    case MA_RAT_CC:
      updateRate();
    break;
    case WF_MI1_CC:
    break;
    case WF_MI2_CC:
    break;


    case NO_SEL_CC:
    break;
    case NO_MIX_CC:
    break;
    case BO_SEL_CC:
    break;
    case BO_MIX_CC:
      updateBoost();
    break;

    // waveform configurations
    case WF_SEL_CC:
    break;
    case WF_SIN_CC:
      updateWaveform(WAVEFORM_SINE);
    break;
    case WF_REV_CC:
      updateWaveform(WAVEFORM_TRIANGLE_VARIABLE);
    break;
    case WF_TRI_CC:
      updateWaveform(WAVEFORM_TRIANGLE);
    break;
    case WF_SAW_CC:
      updateWaveform(WAVEFORM_SAWTOOTH);
    break;
    case WF_SQU_CC:
      updateWaveform(WAVEFORM_SQUARE);
    break;


    // filter and lfo
    case LF_SEL_CC:
      readLFO();
    break;
    case LF_DEP_CC:
    break;
    case FI_RES_CC:
      updateRes();
    break;
    case FI_SEL_CC:
      readFilter();
    break;
    case FI_DEP_CC:
      updateFilterFreq();
    break;

    // vibrato
    case VI_DEP_CC:
      updateVibrato();
    break;

    // envelope 
    case EN_SEL_CC:
      readEnvelope();
    break;
    case EN_ATT_CC:
      updateAttack();
    break;
    case EN_DEC_CC:
      updateDecay();
    break;
    case EN_SUS_CC:
      updateSustain();
    break;
    case EN_REL_CC:
      updateRelease();
    break;
  }
}

/***************************************************************************
 * initializeAudio
 * Configure default values
 * Delete it once the hardware is set;
 */
void initializeAudio()
{ 
  //range 0 - 127
  state[MA_VOL_CC] = 120;
  state[MA_RAT_CC] = 50;
  state[WF_MI1_CC] = 120;
  state[WF_MI2_CC] = 120;
  state[NO_MIX_CC] = 30;
  state[BO_MIX_CC] = 60;
  state[FI_RES_CC] = 50;
  state[FI_DEP_CC] = 120;
  state[LF_DEP_CC] = 60;
  state[VI_DEP_CC] = 10;
  state[EN_ATT_CC] = 5;
  state[EN_DEC_CC] = 127;
  state[EN_SUS_CC] = 127;
  state[EN_REL_CC] = 40;

  // range 0 - 1
  state[WF_SEL_CC] = 0;
  state[NO_SEL_CC] = 0;
  state[BO_SEL_CC] = 1;
  state[FI_SEL_CC] = 1;
  state[LF_SEL_CC] = 1;
  state[VI_SEL_CC] = 1;
  state[EN_SEL_CC] = 1;

  for( int i = 0; i < 4; i++)
    for(int j = 0; j < NUM_INPUT_MIXER; j++)
      inMixer[j].gain(i, 0.1);

  // default waveform inits
  // this is temporary hardcoding, later these will be updated based on hardware
  for( int i = 0; i < POLY; i++)
  {
    activeNotes[i] = 0;
    waveforms1[i].amplitude(0);
    waveforms2[i].amplitude(0);
    waveforms1[i].begin(WAVEFORM_SAWTOOTH);
    waveforms2[i].begin(WAVEFORM_SQUARE);
    pink[i].amplitude(0);
    white[i].amplitude(0);
  }

  readEnvelope();
  readLFO();
  readFilter();
  updateVolume();
  updateBoost();
  updateVibrato();
}

#endif