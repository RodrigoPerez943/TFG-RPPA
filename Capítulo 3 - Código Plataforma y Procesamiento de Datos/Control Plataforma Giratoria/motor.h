#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <Arduino.h>

// For Motor Shield Arduino, CAMBIAR PARA EL STM32
#define PWMA    3
#define DIRA    12
#define BREAKA  9

#define PWMB    11
#define DIRB    13
#define BREAKB  8

/* For Motor shield ST */
#define EN_A  2
#define IN1_A 4
#define IN2_A 5

#define LEFT 0
#define RIGHT 1

#define MOTOR_MAX_VOLTAGE 12.0
#define MOTOR_GEARBOX     9.68
#define ENCODER_CPR       48

/* Hard PWM */
#define PWM_HARD_FREQ       40000
#define PWM_HARD_PRESCALER  1
#define PWM_HARD_CLK        42000000

#define PWM_HARD_IN1_A 0
#define PWM_HARD_IN2_A 1



#ifdef __cplusplus
extern "C"{
#endif
void MOTOR_Init      ( void );
#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
extern "C"{
#endif
void MOTOR_SetVoltage ( float f_voltage );
#ifdef __cplusplus
} // extern "C"
#endif

#endif
