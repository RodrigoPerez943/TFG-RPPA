/**
 ********************************************************************
 * @file    test_xport.c
 * @brief
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


/* Includes ------------------------------------------------------------------*/
#include <dji_gimbal.h>
#include "test_payload_xport.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "utils/util_misc.h"
#include "dji_aircraft_info.h"
#include "camera_emu/test_payload_cam_emu_base.h"
#include <math.h>
/* Private constants ---------------------------------------------------------*/
#define XPORT_TASK_FREQ             (10)
#define XPORT_TASK_STACK_SIZE       (2048)

/* Private types -------------------------------------------------------------*/


/* Private functions declaration ---------------------------------------------*/
static void *MovementTask(void *arg);
static T_DjiReturnCode ReceiveXPortSystemState(T_DjiGimbalSystemState systemState);
static T_DjiReturnCode ReceiveXPortAttitudeInformation(T_DjiGimbalAttitudeInformation attitudeInformation);

/* Private variables ---------------------------------------------------------*/
static T_DjiTaskHandle s_userXPortThread;
static T_DjiMutexHandle s_userXPortMutex;
static T_DjiGimbalSystemState s_userXPortSystemState = {0};
static bool s_isUserXPortInited = false;
static bool s_isUserXPortSystemStateVaild = false;

static T_DjiMutexHandle s_attitudeInfoMutex;

static T_DjiTaskHandle s_movementTaskHandle;
T_DjiMutexHandle s_movementMutex;
bool s_movementRequested = false;
T_WidgetParameters s_requestedParameters;



// M�O -----------------------------------------------
#define VELOCIDAD_MAXIMA 90 // Velocidad m�xima en grados por segundo
#define TIEMPO_MAX_ACCION 600 // Tiempo m�ximo de ejecuci�n en milisegundos
#define MAX_ANGULO 18000   // 180 grados en centi-grados
#define MIN_ANGULO -18000  // -180 grados en centi-grados


//Para almacenar los valores de la attitud en los callabcks y poder usarlos.
static T_DjiGimbalAttitudeInformation s_lastAttitudeInformation = {0};

// -------------------------------------------------

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_XPortStartService(void)
{
    T_DjiReturnCode djiStat;
    T_DjiXPortLimitAngle limitAngle = {0};
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo = {0};
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    djiStat = DjiXPort_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("XPort init error: 0x%08llX.", djiStat);
        return djiStat;
    }

    s_isUserXPortInited = true;

    djiStat = osalHandler->MutexCreate(&s_userXPortMutex);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user XPort mutex create error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = osalHandler->MutexCreate(&s_attitudeInfoMutex);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Attitude info mutex create error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = DjiXPort_RegReceiveSystemStateCallback(ReceiveXPortSystemState);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("register receive XPort system state callback function error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = DjiXPort_RegReceiveAttitudeInformationCallback(ReceiveXPortAttitudeInformation);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("register receive XPort attitude information callback function error: 0x%08llX.",
                       djiStat);
        return djiStat;
    }

    limitAngle.upperLimit = 300;
    limitAngle.lowerLimit = -1000;
    djiStat = DjiXPort_SetLimitAngleSync(DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_JOINT_ANGLE, limitAngle);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set pitch joint angle limit angle for XPort error: 0x%08llX.", djiStat);
        return djiStat;
    }

    limitAngle.upperLimit = 300;
    limitAngle.lowerLimit = -800;
    djiStat = DjiXPort_SetLimitAngleSync(DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE, limitAngle);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set pitch euler angle limit angle for XPort error: 0x%08llX.", djiStat);
        return djiStat;
    }

    limitAngle.upperLimit = 300;
    limitAngle.lowerLimit = -1000;
    djiStat = DjiXPort_SetLimitAngleSync(DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE_EXTENSION, limitAngle);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set pitch extension euler angle limit angle for XPort error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base information error: 0x%08llX.", djiStat);
        return djiStat;
    }

    if (aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1) {
        limitAngle.upperLimit = 1800;
        limitAngle.lowerLimit = -1800;
    } else if (aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2) {
        limitAngle.upperLimit = 1800;
        limitAngle.lowerLimit = -1800;
    } else if (aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3) {
        limitAngle.upperLimit = 1800;
        limitAngle.lowerLimit = -1800;
    } else {
        USER_LOG_WARN("payload mount position is unknown.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    djiStat = DjiXPort_SetLimitAngleSync(DJI_XPORT_LIMIT_ANGLE_CATEGORY_YAW_JOINT_ANGLE, limitAngle);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set yaw joint angle limit angle for XPort error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = DjiXPort_SetGimbalModeSync(DJI_GIMBAL_MODE_FREE);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set XPort gimbal mode error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = DjiXPort_ResetSync(DJI_GIMBAL_RESET_MODE_PITCH_AND_YAW);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("reset XPort gimbal error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = osalHandler->MutexCreate(&s_movementMutex);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Error al crear el mutex de movimiento: 0x%08llX.", djiStat);
    return djiStat;
    }

    if (osalHandler->TaskCreate("Movement_Task", MovementTask, XPORT_TASK_STACK_SIZE, NULL, &s_movementTaskHandle) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Error al crear la tarea de movimiento.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

}

static void *MovementTask(void *arg)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_WidgetParameters parametros;
    bool movementRequested = false;
    T_DjiReturnCode resultado;

    while (1) {
        osalHandler->TaskSleepMs(100); // Reducir la carga de la CPU

        // Verificar si se ha solicitado un movimiento
        resultado = osalHandler->MutexLock(s_movementMutex);
        if (resultado != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Error al bloquear mutex de movimiento: %d", resultado);
            continue;
        }
        if (s_movementRequested) {
            parametros = s_requestedParameters;
            s_movementRequested = false;
            movementRequested = true;
        }
        resultado = osalHandler->MutexUnlock(s_movementMutex);
        if (resultado != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Error al desbloquear mutex de movimiento: %d", resultado);
            continue;
        }

        if (movementRequested) {
            RealizarMovimientoYaw(parametros);
            movementRequested = false;
        }
    }

    return NULL;
}


T_DjiReturnCode DjiTest_XPortDeInit(void)
{
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    djiStat = osalHandler->TaskDestroy(s_userXPortThread);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Destroy test xport thread error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = osalHandler->MutexDestroy(s_userXPortMutex);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Destroy test xport mutex error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = DjiXPort_DeInit();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("XPort de-init error: 0x%08llX.", djiStat);
        return djiStat;
    }

    s_isUserXPortInited = false;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_XPortGetSystemState(T_DjiGimbalSystemState *systemState)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    if (s_isUserXPortInited != true || s_isUserXPortSystemStateVaild != true) {
        USER_LOG_ERROR("user XPort has not inited.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = osalHandler->MutexLock(s_userXPortMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user XPort mutex lock error: 0x%08llX.", returnCode);
        return returnCode;
    }

    memcpy(systemState, &s_userXPortSystemState, sizeof(T_DjiGimbalSystemState));

    returnCode = osalHandler->MutexUnlock(s_userXPortMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user XPort mutex unlock error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode RealizarMovimientoYaw(T_WidgetParameters parametros) {
    USER_LOG_INFO("Iniciando función de movimiento");

    T_DjiReturnCode resultado;
    T_DjiGimbalRotationProperty propiedadRotacion = {0};
    T_DjiAttitude3d rotationValue = {0};
    uint16_t actionTime;
    double angleDiffDegrees;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    // Validar y ajustar parámetros
    int32_t anguloMaximo = parametros.anguloMaximo;
    int32_t anguloMinimo = parametros.anguloMinimo;
    int32_t velocidad = parametros.velocidad;
    int32_t ciclos = parametros.ciclos;

    // Asegurar que los ángulos y la velocidad están dentro de los límites permitidos
    if (anguloMaximo > MAX_ANGULO) anguloMaximo = MAX_ANGULO;
    if (anguloMaximo < MIN_ANGULO) anguloMaximo = MIN_ANGULO;
    if (anguloMinimo > MAX_ANGULO) anguloMinimo = MAX_ANGULO;
    if (anguloMinimo < MIN_ANGULO) anguloMinimo = MIN_ANGULO;

    if (velocidad > VELOCIDAD_MAXIMA) velocidad = VELOCIDAD_MAXIMA;
    if (velocidad <= 0) velocidad = 10; // Evitar división por cero

    if (ciclos < 1) ciclos = 1;

    // Calcular factor de reducción de velocidad
    float factorReduccionVelocidad = (float)velocidad / VELOCIDAD_MAXIMA;
    resultado = DjiXPort_SetSpeedConversionFactor(factorReduccionVelocidad);
    USER_LOG_INFO("Factor de velocidad establecido a %f", factorReduccionVelocidad);
    if (resultado != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Error al configurar factor de velocidad: %d", resultado);
        return resultado;
    }

    // Configurar propiedades de rotación
    propiedadRotacion.rotationValueInvalidFlag.pitch = true; // Ignorar eje Pitch
    propiedadRotacion.rotationValueInvalidFlag.roll = true;  // Ignorar eje Roll
    propiedadRotacion.rotationValueInvalidFlag.yaw = false;  // Controlar eje Yaw

    propiedadRotacion.absoluteAngleRotation.actionTime = 0; // Se establecerá más adelante
    propiedadRotacion.absoluteAngleRotation.jointAngleValid = false; // Usaremos rotationValue

    // Obtener el yaw actual
    int32_t yawActual = s_lastAttitudeInformation.attitude.yaw;

    // Mover a anguloMinimo inicialmente
    rotationValue.yaw = anguloMinimo;

    // Calcular diferencia de ángulo en grados
    angleDiffDegrees = fabs((double)(rotationValue.yaw - yawActual) / 100.0); // Convertir centi-grados a grados

    // Calcular tiempo de acción en centisegundos (0.01 segundos)
    actionTime = (uint16_t)((angleDiffDegrees / velocidad) * 1000.0);
    if (actionTime < 10) actionTime = 10; // Tiempo mínimo de 0.1 segundos

    propiedadRotacion.absoluteAngleRotation.actionTime = actionTime;

    // Realizar rotación a anguloMinimo
    resultado = DjiXPort_RotateSync(DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE, propiedadRotacion, rotationValue);
    if (resultado != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Error al mover a anguloMinimo: %d", resultado);
        return resultado;
    }

    // Esperar a que el movimiento se complete
    osalHandler->TaskSleepMs(actionTime * 10); // Convertir centisegundos a milisegundos

    // Ejecutar ciclos de ida y vuelta
    for (int i = 0; i < ciclos; ++i) {
        // Mover a anguloMaximo
        rotationValue.yaw = anguloMaximo;

        // Calcular diferencia de ángulo entre anguloMaximo y anguloMinimo
        angleDiffDegrees = fabs((double)(anguloMaximo - anguloMinimo) / 100.0);

        actionTime = (uint16_t)((angleDiffDegrees / velocidad) * 100.0);
        if (actionTime < 10) actionTime = 10;

        propiedadRotacion.absoluteAngleRotation.actionTime = actionTime;

        resultado = DjiXPort_RotateSync(DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE, propiedadRotacion, rotationValue);
        if (resultado != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Error al mover a anguloMaximo: %d", resultado);
            return resultado;
        }

        // Esperar a que el movimiento se complete
        osalHandler->TaskSleepMs(actionTime * 10);

        // Mover de vuelta a anguloMinimo
        rotationValue.yaw = anguloMinimo;

        // El tiempo de acción permanece igual
        resultado = DjiXPort_RotateSync(DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE, propiedadRotacion, rotationValue);
        if (resultado != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Error al regresar a anguloMinimo: %d", resultado);
            return resultado;
        }

        // Esperar a que el movimiento se complete
        osalHandler->TaskSleepMs(actionTime * 10);
    }

    // Regresar a 0 grados
    rotationValue.yaw = 635;

    angleDiffDegrees = fabs((double)(rotationValue.yaw - anguloMinimo) / 100.0); // De anguloMinimo a 0 grados

    actionTime = (uint16_t)((angleDiffDegrees / velocidad) * 100.0);
    if (actionTime < 10) actionTime = 10;

    propiedadRotacion.absoluteAngleRotation.actionTime = actionTime;

    resultado = DjiXPort_RotateSync(DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE, propiedadRotacion, rotationValue);
    if (resultado != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Error al regresar a 0 grados: %d", resultado);
        return resultado;
    }

    // Esperar a que el movimiento se complete
    osalHandler->TaskSleepMs(actionTime * 10);

    USER_LOG_INFO("Movimiento de yaw completado con éxito");

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}


T_DjiReturnCode RequestMovement(T_WidgetParameters parametros)
{
    T_DjiReturnCode resultado;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    resultado = osalHandler->MutexLock(s_movementMutex);
    if (resultado != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Error al bloquear mutex de movimiento: %d", resultado);
        return resultado;
    }

    s_requestedParameters = parametros;
    s_movementRequested = true;

    resultado = osalHandler->MutexUnlock(s_movementMutex);
    if (resultado != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Error al desbloquear mutex de movimiento: %d", resultado);
        return resultado;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif


#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

static T_DjiReturnCode ReceiveXPortSystemState(T_DjiGimbalSystemState systemState)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_userXPortMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user XPort mutex lock error: 0x%08llX.", returnCode);
        return returnCode;
    }

    s_isUserXPortSystemStateVaild = true;
    memcpy(&s_userXPortSystemState, &systemState, sizeof(T_DjiGimbalSystemState));

    returnCode = osalHandler->MutexUnlock(s_userXPortMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user XPort mutex unlock error: 0x%08llX.", returnCode);
        return returnCode;
    }

    USER_LOG_DEBUG("receive XPort system state: mounted upward flag %d, gimbal mode %d.",
                   systemState.mountedUpward, systemState.gimbalMode);

    USER_LOG_DEBUG("XPort fine tune: %d %d %d.", systemState.fineTuneAngle.pitch,
                   systemState.fineTuneAngle.roll, systemState.fineTuneAngle.yaw);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

// Modificamos este cllaback para poder almacenar los valores y usarlos ya que solo se imprimen.
static T_DjiReturnCode ReceiveXPortAttitudeInformation(T_DjiGimbalAttitudeInformation attitudeInformation)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_attitudeInfoMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Attitude info mutex lock error: 0x%08llX.", returnCode);
        return returnCode;
    }

    s_lastAttitudeInformation = attitudeInformation;

    returnCode = osalHandler->MutexUnlock(s_attitudeInfoMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Attitude info mutex unlock error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}



/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
