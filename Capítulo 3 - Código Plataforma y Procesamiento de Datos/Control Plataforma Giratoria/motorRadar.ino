/* Encoder Library - Basic Example
   http://www.pjrc.com/teensy/td_libs_Encoder.html

   Este código de ejemplo está en dominio público.
*/

#include "motor.h"
#include "controller.h"
#include "encoder.h"
#include "comm.h"

#include <DueTimer.h>

/***************************************** 
  PROBLEMAS CON LA IMPORTACIÓN DE LIBRERÍAS EN ARDUINO.
  SE COLOCAN AQUÍ POR SIMPLICIDAD 
****************************************/

/** CONTROLLER.h **/
#ifdef __cplusplus
extern "C" {
#endif
void CONTROLLER_Start      ( long l_sampling_time );
#ifdef __cplusplus
} // extern "C"
#endif
/** END CONTROLLER.h **/

/** COMMUNICATION.h **/
#ifdef __cplusplus
extern "C" {
#endif
void COMMUNICATION_Start      ( long l_sampling_time );
#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
extern "C" {
#endif
void COMMUNICATION_Stop      ( void );
#ifdef __cplusplus
} // extern "C"
#endif
/** END COMMUNICATION.h **/
/***************************************** 
  FIN PROBLEMAS CON LA IMPORTACIÓN DE LIBRERÍAS EN ARDUINO.
****************************************/


/* DEFINICIÓN DE FUNCIONES */
void COMMUNICATION_ROB ( void );
void CONTROLLER_ROB ( void );

/* DEFINICIÓN DE VARIABLES */
#define CONTROLLER_SAMPLING_PERIOD    5   // en mSec
#define COMMUNICATION_SAMPLING_PERIOD 10  // en mSec

/* VARIABLES USUARIO */
volatile float     fPos;
volatile float     fSpeed;
volatile uint32_t  uControllerCounter;
volatile uint32_t  uCommCounter;

/* VARIABLES PARA CONTROL DE BATIDO */
const float desiredMovementAngle = 900.0; // Ángulo incremental deseado

// Variable para almacenar la posición inicial (referencia)
volatile float posicionInicial = 0.0;

// Variable para el ángulo objetivo
volatile float targetAngle = 0.0;

// Variable para indicar si hay movimiento en curso
volatile bool movimientoEnCurso = false;

// Variable para alternar la dirección del movimiento
bool direccionPositiva = true;

/* DEFINICIÓN DEL PIN DEL SWITCH */
const uint8_t switchPin = 22; // Cambia este número según tu conexión

/* Variables para Debouncing */
unsigned long ultimoTiempoCambio = 0;
const unsigned long tiempoDebounce = 50; // 50 ms para debouncing

/* FIN VARIABLES USUARIO */

void setup() 
{
  /* INICIALIZAR Comunicación Serial para Depuración */
  Serial.begin(115200);
  while (!Serial) {
    ; // Espera a que se inicie la conexión serial
  }

  /* INICIALIZAR VARIABLES USUARIO */
  fPos                = 0.0;
  fSpeed              = 0.0;
  uControllerCounter  = 0;
  uCommCounter        = 0;
  movimientoEnCurso   = false;
  targetAngle         = 0.0;
  direccionPositiva   = true; // Primer movimiento será en dirección positiva

  /* INICIALIZAR CODIFICADOR */
  ENCODER_Init();
  // Guarda la posición inicial (de referencia)
  posicionInicial = (float) ENCODER_GetCount();
  ENCODER_StartSpeedCalculation(ENCODER_SPEED_CALCULATION_SAMPLING_PERIOD);

  /* INICIALIZAR MOTOR */
  MOTOR_Init();

  /* INICIALIZAR COMUNICACIÓN */
  COMMUNICATION_Init(115200);
  
  /* INICIALIZAR Pin del Switch */
  pinMode(switchPin, INPUT_PULLUP); // Usa resistencia pull-up interna

  /* INICIALIZAR Timers */
  CONTROLLER_Start(CONTROLLER_SAMPLING_PERIOD);
  COMMUNICATION_Start(COMMUNICATION_SAMPLING_PERIOD);
  
  /* INICIAR MOTOR SIN MOVER (voltage 0) */
  MOTOR_SetVoltage(0.0);
}

void CONTROLLER_ROB ( void )
{
  /* Actualizar posición y velocidad */
  fPos    = (float) ENCODER_GetCount();
  fSpeed  = (float) ENCODER_GetCountInSpeedInterval();

  if (movimientoEnCurso)
  {
    // Si el movimiento es hacia adelante (objetivo mayor que la posición inicial)
    if (targetAngle > posicionInicial)
    {
      if (fPos >= targetAngle)
      {
        MOTOR_SetVoltage(0.0); // Detener el motor
        movimientoEnCurso = false;
        Serial.println("Movimiento completado.");
      }
    }
    else // Movimiento hacia atrás (objetivo menor que la posición inicial)
    {
      if (fPos <= targetAngle)
      {
        MOTOR_SetVoltage(0.0); // Detener el motor
        movimientoEnCurso = false;
        Serial.println("Movimiento completado.");
      }
    }
  }

  uControllerCounter++;
}

void COMMUNICATION_ROB ( void )
{
  /* Imprimir información de depuración */
  Serial.print("Count: ");
  Serial.print(uCommCounter);
  Serial.print(" Pos: ");
  Serial.print(fPos, 5);
  Serial.print(" Vel: ");
  Serial.println(fSpeed, 5);

  uCommCounter++;
}

void loop()
{
  /* Leer el estado actual del switch */
  bool estadoActual = digitalRead(switchPin);

  /* Detectar cambio de estado (transición de HIGH a LOW) */
  static bool switchEstadoAnterior = HIGH; // Se asume que el switch está con pull-up
  if (estadoActual != switchEstadoAnterior)
  {
    unsigned long tiempoActual = millis();
    if ((tiempoActual - ultimoTiempoCambio) > tiempoDebounce)
    {
      if (estadoActual == LOW) // LOW indica que el botón se presionó
      {
        Serial.println("Switch presionado.");
        if (!movimientoEnCurso)
        {
          // Alterna entre movimiento positivo y negativo respecto a la posición inicial
          if (direccionPositiva)
          {
            targetAngle = posicionInicial + desiredMovementAngle;
            MOTOR_SetVoltage(4);  // Movimiento hacia adelante
          }
          else
          {
            targetAngle = posicionInicial - desiredMovementAngle;
            MOTOR_SetVoltage(-4); // Movimiento hacia atrás (usa voltaje negativo)
          }
          movimientoEnCurso = true;
          Serial.print("Nuevo targetAngle: ");
          Serial.println(targetAngle);
          // Alternar la dirección para el próximo movimiento
          direccionPositiva = !direccionPositiva;
        }
      }
      ultimoTiempoCambio = tiempoActual;
    }
  }
  switchEstadoAnterior = estadoActual;
}

/***************************************** 
  PROBLEMAS CON LA IMPORTACIÓN DE LIBRERÍAS EN ARDUINO.
  SE COLOCAN AQUÍ POR SIMPLICIDAD 
****************************************/

/** CONTROLLER.c **/
/* Entrada en mSec */
void CONTROLLER_Start ( long l_sampling_time )
{
  Timer3.attachInterrupt(CONTROLLER_ROB);
  Timer3.start(l_sampling_time * 1000); // Timer definido en uSec
}
/** END CONTROLLER.c **/

/** COMMUNICATION **/
void COMMUNICATION_Start ( long l_sampling_time )
{
  Timer2.attachInterrupt(COMMUNICATION_ROB);
  Timer2.start(l_sampling_time * 1000);
}

void COMMUNICATION_Stop ( )
{
  Timer2.stop();
}
/** END COMMUNICATION **/

/** ENCODER **/
/* Entrada en mSec */
void ENCODER_StartSpeedCalculation ( long l_sampling_time )
{
  Timer1.attachInterrupt(ENCODER_CalcSpeed);
  Timer1.start(l_sampling_time * 1000); // Timer definido en uSec
}
/** END ENCODER **/
/***************************************** 
  FIN PROBLEMAS CON LA IMPORTACIÓN DE LIBRERÍAS EN ARDUINO.
****************************************/
