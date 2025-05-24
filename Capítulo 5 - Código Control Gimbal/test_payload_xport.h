/**
 ********************************************************************
 * @file    test_xport.h
 * @brief   This is the header file for "test_xport.c", defining the structure and
 * (exported) function prototypes.
 *
 * @copyright (c) 2021 DJI. All rights reserved.
 *
 * All information contained herein is, and remains, the property of DJI.
 * The intellectual and technical concepts contained herein are proprietary
 * to DJI and may be covered by U.S. and foreign patents, patents in process,
 * and protected by trade secret or copyright law.  Dissemination of this
 * information, including but not limited to data and other proprietary
 * material(s) incorporated within the information, in any form, is strictly
 * prohibited without the express written consent of DJI.
 *
 * If you receive this source code without DJI�s authorization, you may not
 * further disseminate the information, and you must immediately remove the
 * source code and notify DJI of its removal. DJI reserves the right to pursue
 * legal actions against you for any loss(es) or damage(s) caused by your
 * failure to do so.
 *
 *********************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TEST_XPORT_H
#define TEST_XPORT_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"
#include "dji_xport.h"
#include "dji_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef struct {
    int32_t anguloMaximo;  /*!< Maximum yaw angle, range: -1800 to 1800 (0.1 degrees). */
    int32_t anguloMinimo;  /*!< Minimum yaw angle, range: -1800 to 1800 (0.1 degrees). */
    int32_t velocidad;     /*!< Desired speed, range: 0 to 90 degrees/second. */
    int32_t ciclos;        /*!< Number of back-and-forth cycles for yaw movement. */
    bool confirmarEnvio;   /*!< Confirmation flag for sending the command. */
} T_WidgetParameters;

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode DjiTest_XPortStartService(void);
T_DjiReturnCode DjiTest_XPortDeInit(void);
T_DjiReturnCode DjiTest_XPortGetSystemState(T_DjiGimbalSystemState *systemState);
T_DjiReturnCode RealizarMovimientoYaw(T_WidgetParameters parametros);

extern T_DjiMutexHandle s_movementMutex;
extern bool s_movementRequested;
extern T_WidgetParameters s_requestedParameters;

T_DjiReturnCode RequestMovement(T_WidgetParameters parametros);

#ifdef __cplusplus
}
#endif

#endif // TEST_XPORT_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
