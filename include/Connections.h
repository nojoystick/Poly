#ifndef CONNECTIONS_H_
#define CONNECTIONS_H_

#include <Constants.h>

static AudioSynthWaveform       waveforms1[POLY]; 
static AudioSynthWaveform       waveforms2[POLY];       
static AudioSynthNoisePink      pink[POLY];
static AudioSynthNoiseWhite     white[POLY];
static AudioEffectEnvelope      envelope[POLY];
static AudioFilterStateVariable velFilter[POLY];
static AudioEffectBitcrusher    sat[POLY];
static AudioFilterStateVariable filter; 
static IntervalTimer            releaseTimer[POLY];
static AudioMixer4              inMixer[NUM_INPUT_MIXER];
static AudioMixer4              outMixer[NUM_OUTPUT_MIXER];
AudioOutputI2S                  lineout;   
AudioControlSGTL5000            sgtl5000_1;                // controller object           

/****************************************************************************
* Huge array of patchCords to link mixer and waveform objects together
* Final signal chain:
* Waveform1 -\
*             \   | condense | |-----FX-----|  |--filtering--| 
* Waveform2 ---\  |    to    |       ** TODO **
*               ->|    one   |->[sat]->[trem]->[env]->[filter]->[out]
* Pink --------/  | outMixer |
*             /
* White -----/
*
*/
static AudioConnection     patchCord[]
{ 
  // each voice has 2 oscillators + white and pink noise
  // those four components feed into each mixer
  { waveforms1[0], 0, inMixer[0], 0 } , { waveforms2[0], 0, inMixer[0], 1 } , 
  { pink[0],       0, inMixer[0], 2 } , { white[0],      0, inMixer[0], 3 } , 
  { waveforms1[1], 0, inMixer[1], 0 } , { waveforms2[1], 0, inMixer[1], 1 } ,
  { pink[1],       0, inMixer[1], 2 } , { white[1],      0, inMixer[1], 3 } ,
  { waveforms1[2], 0, inMixer[2], 0 } , { waveforms2[2], 0, inMixer[2], 1 } , 
  { pink[2],       0, inMixer[2], 2 } , { white[2],      0, inMixer[2], 3 } , 
  { waveforms1[3], 0, inMixer[3], 0 } , { waveforms2[3], 0, inMixer[3], 1 } ,
  { pink[3],       0, inMixer[3], 2 } , { white[3],      0, inMixer[3], 3 } ,
  { waveforms1[4], 0, inMixer[4], 0 } , { waveforms2[4], 0, inMixer[4], 1 } , 
  { pink[4],       0, inMixer[4], 2 } , { white[4],      0, inMixer[4], 3 } , 
  { waveforms1[5], 0, inMixer[5], 0 } , { waveforms2[5], 0, inMixer[5], 1 } ,
  { pink[5],       0, inMixer[5], 2 } , { white[5],      0, inMixer[5], 3 } ,
  { waveforms1[6], 0, inMixer[6], 0 } , { waveforms2[6], 0, inMixer[6], 1 } , 
  { pink[6],       0, inMixer[6], 2 } , { white[6],      0, inMixer[6], 3 } , 
  { waveforms1[7], 0, inMixer[7], 0 } , { waveforms2[7], 0, inMixer[7], 1 } ,
  { pink[7],       0, inMixer[7], 2 } , { white[7],      0, inMixer[7], 3 } ,
  // feed each voice into its own bitcrusher
  { inMixer[0],    0, sat[0],     0 } , { inMixer[1],    0, sat[1],     0 } ,
  { inMixer[2],    0, sat[2],     0 } , { inMixer[3],    0, sat[3],     0 } ,
  { inMixer[4],    0, sat[4],     0 } , { inMixer[5],    0, sat[5],     0 } ,
  { inMixer[6],    0, sat[6],     0 } , { inMixer[7],    0, sat[7],     0 } ,
  // feed each bitcrusher into its own envelope
  { sat[0],        0, envelope[0],0 } , { sat[1],        0, envelope[1],0 } ,
  { sat[2],        0, envelope[2],0 } , { sat[3],        0, envelope[3],0 } ,
  { sat[4],        0, envelope[4],0 } , { sat[5],        0, envelope[5],0 } ,
  { sat[6],        0, envelope[6],0 } , { sat[7],        0, envelope[7],0 } ,
  // feed each envelope into its own velocity filter
  { envelope[0],   0, velFilter[0],  0 } , { envelope[1],    0, velFilter[1],   0 } ,
  { envelope[2],   0, velFilter[2],  0 } , { envelope[3],    0, velFilter[3],   0 } ,
  { envelope[4],   0, velFilter[4],  0 } , { envelope[5],    0, velFilter[5],   0 } ,
  { envelope[6],   0, velFilter[6],  0 } , { envelope[7],    0, velFilter[7],   0 } ,
  // condense all inputs down to a single mixer, outMixer[0]
  { velFilter[0],  0, outMixer[1],   0 } , { velFilter[1],   0, outMixer[1],   1 } ,
  { velFilter[2],  0, outMixer[1],   2 } , { velFilter[3],   0, outMixer[1],   3 } ,
  { velFilter[4],  0, outMixer[2],   0 } , { velFilter[5],   0, outMixer[2],   1 } ,
  { velFilter[6],  0, outMixer[2],   2 } , { velFilter[7],   0, outMixer[2],   3 } ,
  { outMixer[2],   0, outMixer[0],   0 } , { outMixer[1],    0, outMixer[0],   1 } , 
  // effects chain leading to lineout
  { outMixer[0],   0, filter,            0 } ,
  { filter,        0, lineout,        0 } , { filter,         0, lineout,       1 }
};


#endif