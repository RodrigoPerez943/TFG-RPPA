#include "comm.h"

volatile long COMMUNICATION_lCommTimeCounter;

void COMMUNICATION_Init ( int speed )
{
  COMMUNICATION_lCommTimeCounter = 0;
  Serial.begin(speed);
}
