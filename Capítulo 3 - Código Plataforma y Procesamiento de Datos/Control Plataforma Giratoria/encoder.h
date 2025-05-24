#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <Arduino.h>

#define PREV_MASK 0x1 //Mask for the previous state in determining direction
//of rotation.
#define CURR_MASK 0x2 //Mask for the current state in determining direction
//of rotation.
#define INVALID   0x3 //XORing two states where both bits have changed.

#define ENCODER_SPEED_CALCULATION_SAMPLING_PERIOD 50 // in mSec

#ifdef __cplusplus
extern "C"{
#endif
void ENCODER_Init      ( void );
#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
extern "C"{
#endif
void ENCODER_Count      ( void );
#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
extern "C"{
#endif
long  ENCODER_GetCount    ( void );
#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
extern "C"{
#endif
void  ENCODER_CalcSpeed    ( void );
#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
extern "C"{
#endif
long  ENCODER_GetCountInSpeedInterval    ( void );
#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
extern "C"{
#endif
void  ENCODER_Reset    ( void );
#ifdef __cplusplus
} // extern "C"
#endif


#endif
