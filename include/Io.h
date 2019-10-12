#ifndef IO_H_
#define IO_H_

// pins for IO
const int io[] = {  
  MA_VOL_IN, MA_RAT_IN, WF_MI1_IN, WF_MI2_IN, NO_MIX_IN, BO_MIX_IN, FI_RES_IN, FI_FRQ_IN, EN_ATT_IN, EN_DEC_IN, EN_SUS_IN, EN_REL_IN,
  WF_SEL_SW, NO_SEL_SW, BO_SEL_SW, FI_SEL_SW, EN_SEL_SW, BO_SEL_LD, FI_SEL_LD, EN_SEL_LD, MA_RAT_LD, WF_SW1_SW, WF_SW2_SW, WF_SW3_SW
};

static int waveformSum;                 // sum of waveform bits i.e. 101 = 5

/*************************************************************************************
 ************************************************************************************* 
 * I/O HANDLERS
 *************************************************************************************
 ************************************************************************************/

/***************************************************************************
 * readAnalog
 * Check all analog inputs
 * Call changeControl if something has changed from the previous state
 */
void readAnalog()
{
  for(int i = 0; i < NUM_ANA_INPUTS; i++)
  {
    int curr = analogRead(io[i]);
    if(curr != state[i]) changeControl(0, i, curr);
  }
}

/***************************************************************************
 * readDigital
 * Check all digital inputs
 * Call changeControl if something has changed from the previous state
 */
void readDigital()
{
  for(int i = DIG_START_IDX; i < NUM_DIG_INPUTS; i++)
  {
    int curr = digitalRead(io[i]);
    if(curr != state[i]) changeControl(0, i, curr);
  }
}

/***************************************************************************
 * parseSwitch
 * parse from 3-way switch value (1, 3, 2, 6, 4)
 * to midi value (17, 18, 19, 20, 21)
 * and call changeControl() to persist the change
 */
void parseSwitch()
{ 
  for(int i = WF_SIN_CC; i <= WF_SQU_CC; i++) 
    if(waveformSum == wfKey[i]) changeControl(0, wfCCs[i], 0);
}

/***************************************************************************
 * updateSwitch
 * Check each pin to see if it matches the old val
 * If something has changed set the flag that the switch should be parsed
 *     again
 */
int updateSwitch()
{
  static int wfVal[NUM_INPUTS]; // for persisting the pin values across function calls
  int updateFlag = 0;
  for(int i = 0; i < NUM_WFS_INPUTS; i++)
  {
    int curr = digitalRead(io[i+WFS_START_IDX]);
    if(curr != wfVal[i])
    {
      bitWrite(waveformSum, i, curr);
      wfVal[i] = curr;
      updateFlag = 1;
    }
  }
  return updateFlag;
}

/***************************************************************************
 * readSwitch
 * Call updateSwitch() to check if the 5 way switch values have changed
 * If so, call parseSwitch to find the switch position and trigger
 *     the controlChange() call
 */
void readSwitch()
{
  int updateStatus = updateSwitch();
  if(updateStatus == 1) parseSwitch();
}

/***************************************************************************
 * inputRead
 * Master handler for reading IO pins
 */
void inputRead()
{
  readAnalog();
  readDigital();
  readSwitch();
}

#endif