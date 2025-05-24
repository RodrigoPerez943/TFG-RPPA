#include "motor.h"

uint16_t uPWMPeriod; 

void MOTOR_Init ( void )
{
  /* Hard PWM  */
  pinMode(EN_A, OUTPUT); // Cambiar por HAL
  digitalWrite(EN_A, HIGH); // Cambiar por HAL

  uPWMPeriod = (PWM_HARD_CLK / PWM_HARD_PRESCALER ^ 2) / (PWM_HARD_FREQ );

  pmc_enable_periph_clk (PWM_INTERFACE_ID);  // turn on clocking to PWM unit

  //  ojo estas vainas hay que cambiarlas para usar el HAL de stm32
  // usamos PWMH0 (PC2-PIN34), PWMH1 (PC5-PIN37), PWMH2 (PC7-PIN39), PWMH3 (PC9-PIN41),
  PWMC_ConfigureChannel (PWM, PWM_HARD_IN1_A, PWM_HARD_PRESCALER, 0, PWM_CMR_CPOL); // PWM channel 0
  PWMC_SetPeriod (PWM, 0, uPWMPeriod);
  PWMC_EnableChannel (PWM, 0);   // enable

  PWMC_ConfigureChannel (PWM, PWM_HARD_IN2_A, PWM_HARD_PRESCALER, 0, PWM_CMR_CPOL); // PWM channel 1
  PWMC_SetPeriod (PWM, 1, uPWMPeriod);
  PWMC_EnableChannel (PWM, 1);   // enable

  PWMC_ConfigureChannel (PWM, 2, PWM_HARD_PRESCALER, 0, PWM_CMR_CPOL); // PWM channel 2
  PWMC_SetPeriod (PWM, 2, uPWMPeriod);
  PWMC_EnableChannel (PWM, 2);   // enable

  PWMC_ConfigureChannel (PWM, 3, PWM_HARD_PRESCALER, 0, PWM_CMR_CPOL); // PWM channel 3
  PWMC_SetPeriod (PWM, 3, uPWMPeriod);
  PWMC_EnableChannel (PWM, 3);   // enable

  PIOC->PIO_PDR = 0x2A4 ;  // disable PIO control
  PIOC->PIO_IDR = 0x2A4 ;   // disable PIO interrupts
  PIOC->PIO_ABSR |= 0x2A4 ;  // switch to B peripheral
}

void MOTOR_SetVoltage ( float f_voltage )
{ 
    int u_dir;  
     /* Normalize */
    f_voltage /= MOTOR_MAX_VOLTAGE;
    
    /* Check Limits */
    if ( f_voltage > 1.0 )
        f_voltage = 1.0;
    else if ( f_voltage < -1.0 ) 
        f_voltage = - 1.0;

    /* ST MOTOR SHIELD */
    /* Set dir */    
    if ( f_voltage >= 0.0 )
        u_dir = LEFT;
    else
        u_dir = RIGHT;
    
    //f_voltage = 255 * fabs(f_voltage);

      /* Set voltage */
      if ( u_dir == LEFT )
      {
        PWMC_SetDutyCycle (PWM, PWM_HARD_IN1_A,0); // Cmbiar por HAL
        PWMC_SetDutyCycle (PWM, PWM_HARD_IN2_A,(uint16_t) (((float) uPWMPeriod) * fabs(f_voltage))); // Cmbiar por HAL
      }
      else
      {
        PWMC_SetDutyCycle (PWM, PWM_HARD_IN1_A,(uint16_t) (((float) uPWMPeriod) * fabs(f_voltage))); // Cmbiar por HAL
        PWMC_SetDutyCycle (PWM, PWM_HARD_IN2_A,0); // Cmbiar por HAL
      } 
}
