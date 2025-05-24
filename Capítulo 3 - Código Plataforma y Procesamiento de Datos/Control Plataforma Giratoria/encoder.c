#include "encoder.h"

volatile signed long ENCODER_lCounterEncoder;
volatile signed long ENCODER_lCounterEncoderOld;
volatile signed long ENCODER_lSpeed;

volatile char ENCODER_cStateEncoder;
volatile char ENCODER_cStateEncoderOld;

const int ENCODER_CHANNEL_A = 3;
const int ENCODER_CHANNEL_B = 7;

const int LED     = 6;
bool ledOn          = false;

void  LED_Toggle         ( void )
{
  ledOn = !ledOn;
  digitalWrite(LED, ledOn); // Led on, off, on, off...
}

void ENCODER_Init ( void )
{
  pinMode(ENCODER_CHANNEL_A, INPUT_PULLUP);
  pinMode(ENCODER_CHANNEL_B, INPUT_PULLUP);

  ENCODER_lCounterEncoder = 0;
  ENCODER_lCounterEncoderOld = 0;
  ENCODER_lSpeed = 0;
    
  //Workout what the current state is.
  char chanA = digitalRead(ENCODER_CHANNEL_A);
  char chanB = digitalRead(ENCODER_CHANNEL_B);
    
  //2-bit state.
  ENCODER_cStateEncoder = (chanA << 1) | (chanB);
  ENCODER_cStateEncoder = ENCODER_cStateEncoderOld;  

  attachInterrupt(digitalPinToInterrupt(ENCODER_CHANNEL_A), ENCODER_Count, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CHANNEL_B), ENCODER_Count, CHANGE);

  pinMode ( LED,     OUTPUT );

}

void ENCODER_Count ( void )
{
  int change = 0;
    
  //Workout what the current state is.
  char chanA = digitalRead(ENCODER_CHANNEL_A);
  char chanB = digitalRead(ENCODER_CHANNEL_B);
    
  //2-bit state.
  ENCODER_cStateEncoder = (chanA << 1) | (chanB);
    
  //Entered a new valid state.
  if (((ENCODER_cStateEncoder ^ ENCODER_cStateEncoderOld) != INVALID) && 
      ( ENCODER_cStateEncoder != ENCODER_cStateEncoderOld )) {
        
      //2 bit state. Right hand bit of prev XOR left hand bit of current
      //gives 0 if clockwise rotation and 1 if counter clockwise rotation.
      change = (ENCODER_cStateEncoderOld & PREV_MASK) ^ ((ENCODER_cStateEncoder & CURR_MASK) >> 1);

      if (change == 0) 
          change = -1;

      ENCODER_lCounterEncoder -= change;      
    }
    else
    {
     LED_Toggle();
    }
    ENCODER_cStateEncoderOld = ENCODER_cStateEncoder;
}

void ENCODER_CalcSpeed ( void )
{
  ENCODER_lSpeed = ( ENCODER_lCounterEncoder - ENCODER_lCounterEncoderOld ) * 1000 / ENCODER_SPEED_CALCULATION_SAMPLING_PERIOD; //Secs in a mSec / num of mSecs of int
  
  ENCODER_lCounterEncoderOld = ENCODER_lCounterEncoder;
}


long ENCODER_GetCount ( void )
{
    return ENCODER_lCounterEncoder;
}


long  ENCODER_GetCountInSpeedInterval    ( void )
{
  return ENCODER_lSpeed;
}

void ENCODER_Reset ( void )
{
  ENCODER_lCounterEncoder = 0;
  ENCODER_lCounterEncoderOld = 0;
  ENCODER_lSpeed = 0.0;
}
