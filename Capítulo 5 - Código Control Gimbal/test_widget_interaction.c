/**
 ********************************************************************
 * @file    test_widget_interaction.c
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
#include "test_widget_interaction.h"
#include <dji_widget.h>
#include <dji_logger.h>
#include "../utils/util_misc.h"
#include <dji_platform.h>
#include <stdio.h>
#include <waypoint_v2/test_waypoint_v2.h>
#include <hms/test_hms.h>
#include <gimbal_manager/test_gimbal_manager.h>
#include <camera_manager/test_camera_manager.h>
#include <flight_control/test_flight_control.h>
#include <fc_subscription/test_fc_subscription.h>
#include <dji_perception.h>
#include "liveview/test_liveview.h"
#include "perception/test_perception.h"
#include "file_binary_array_list_en.h"
#include <stdarg.h>
#include "dji_aircraft_info.h"
#include "dji_core.h"
#include <payload_collaboration/test_payload_collaboration.h>
#include <waypoint_v3/test_waypoint_v3.h>
#include "dji_sdk_config.h"
#include "hms/hms_text_c/en/hms_text_config_json.h"
#include "dji_hms.h"
#include <xport/test_payload_xport.h>

/* Private constants ---------------------------------------------------------*/
#define WIDGET_DIR_PATH_LEN_MAX         (256)
#define WIDGET_TASK_STACK_SIZE          (2048)

#define WIDGET_LOG_STRING_MAX_SIZE      (40)
#define WIDGET_LOG_LINE_MAX_NUM         (5)

/* Private types -------------------------------------------------------------*/

typedef struct {
    bool valid;
    char content[WIDGET_LOG_STRING_MAX_SIZE];
} T_DjiTestWidgetLog;

/* Private functions declaration ---------------------------------------------*/

static void *DjiTest_WidgetTask(void *arg);
static void *DjiTest_WidgetInteractionTask(void *arg);
static T_DjiReturnCode DjiTestWidget_SetWidgetValue(E_DjiWidgetType widgetType, uint32_t index, int32_t value,
                                                    void *userData);
static T_DjiReturnCode DjiTestWidget_GetWidgetValue(E_DjiWidgetType widgetType, uint32_t index, int32_t *value,
                                                    void *userData);

// Mi funci�n -------------------------------------

static T_DjiReturnCode EjecutarAccionConValores(void);

/* Private values ------------------------------------------------------------*/

static T_DjiTaskHandle s_widgetTestThread;
static T_DjiTaskHandle s_widgetInteractionTestThread;
static bool s_isInjectErrcode = false;
static bool s_isEliminateErrcode = false;
static bool s_isallowRunFlightControlSample = false;
static bool s_isSampleStart = false;
static E_DjiMountPosition s_mountPosition = DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1;
static T_DjiAircraftInfoBaseInfo s_aircraftInfoBaseInfo = {0};
static T_DjiTestWidgetLog s_djiTestWidgetLog[WIDGET_LOG_LINE_MAX_NUM] = {0};

// Valores privados mios ----------------------

static const T_DjiWidgetHandlerListItem s_widgetHandlerList[] = {
    {0, DJI_WIDGET_TYPE_BUTTON,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},   // Confirmar Env�o
    {1, DJI_WIDGET_TYPE_INT_INPUT_BOX, DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},  // �ngulo Horizontal
    {2, DJI_WIDGET_TYPE_INT_INPUT_BOX, DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},  // �ngulo Vertical
    {3, DJI_WIDGET_TYPE_INT_INPUT_BOX, DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},  // Velocidad
    {4, DJI_WIDGET_TYPE_INT_INPUT_BOX, DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL}  // N�mero de Ciclos

};

static const char *s_widgetTypeNameArray[] = {
    "Unknown",
    "Button",
    "Switch",
    "Scale",
    "List",
    "Int input box"
};

// Estructura de almacenamiento de valores de widgets
static T_WidgetParameters s_widgetParams = {0, 0, 0, 0, false};

// -----------------------------------------------------

static const uint32_t s_widgetHandlerListCount = sizeof(s_widgetHandlerList) / sizeof(T_DjiWidgetHandlerListItem);
static int32_t s_widgetValueList[sizeof(s_widgetHandlerList) / sizeof(T_DjiWidgetHandlerListItem)] = {0};
static bool s_isWidgetFileDirPathConfigured = false;
static char s_widgetFileDirPath[DJI_FILE_PATH_SIZE_MAX] = {0};

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_WidgetInteractionStartService(void)
{
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    //Step 1 : Init DJI Widget
    djiStat = DjiWidget_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Dji widget init error, stat = 0x%08llX", djiStat);
        return djiStat;
    }



#ifdef SYSTEM_ARCH_LINUX_DISABLEED
    //Step 2 : Set UI Config (Linux environment)
    char curFileDirPath[WIDGET_DIR_PATH_LEN_MAX];
    char tempPath[WIDGET_DIR_PATH_LEN_MAX];
    djiStat = DjiUserUtil_GetCurrentFileDirPath(__FILE__, WIDGET_DIR_PATH_LEN_MAX, curFileDirPath);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get file current path error, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    if (s_isWidgetFileDirPathConfigured == true) {
        snprintf(tempPath, WIDGET_DIR_PATH_LEN_MAX, "%swidget_file/en_big_screen", s_widgetFileDirPath);
    } else {
        snprintf(tempPath, WIDGET_DIR_PATH_LEN_MAX, "%swidget_file/en_big_screen", curFileDirPath);
    }

    //set default ui config path
    djiStat = DjiWidget_RegDefaultUiConfigByDirPath(tempPath);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Add default widget ui config error, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    //set ui config for English language
    djiStat = DjiWidget_RegUiConfigByDirPath(DJI_MOBILE_APP_LANGUAGE_ENGLISH,
                                             DJI_MOBILE_APP_SCREEN_TYPE_BIG_SCREEN,
                                             tempPath);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Add widget ui config error, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    //set ui config for Chinese language
    if (s_isWidgetFileDirPathConfigured == true) {
        snprintf(tempPath, WIDGET_DIR_PATH_LEN_MAX, "%swidget_file/cn_big_screen", s_widgetFileDirPath);
    } else {
        snprintf(tempPath, WIDGET_DIR_PATH_LEN_MAX, "%swidget_file/cn_big_screen", curFileDirPath);
    }

    djiStat = DjiWidget_RegUiConfigByDirPath(DJI_MOBILE_APP_LANGUAGE_CHINESE,
                                             DJI_MOBILE_APP_SCREEN_TYPE_BIG_SCREEN,
                                             tempPath);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Add widget ui config error, stat = 0x%08llX", djiStat);
        return djiStat;
    }
#endif

    // Registrar configuración de UI por defecto
    djiStat = DjiWidget_RegDefaultUiConfigByDirPath("widget_file/en_big_screen");
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Error registrando configuración de UI, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    // Registrar configuración de UI para idioma español y pantalla grande
    djiStat = DjiWidget_RegUiConfigByDirPath(DJI_MOBILE_APP_LANGUAGE_ENGLISH,
                                            DJI_MOBILE_APP_SCREEN_TYPE_BIG_SCREEN,
                                            "widget_file/en_big_screen");
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Error registrando configuración de UI para ingles, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    //Step 3 : Registra handlers de los widgets 
    djiStat = DjiWidget_RegHandlerList(s_widgetHandlerList, s_widgetHandlerListCount);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Error al registrar lista de manejadores, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    //Step 4 : Run widget tasks

    // Crear tarea para mostrar mensajes en la ventana flotante
    if (osalHandler->TaskCreate("user_widget_task", DjiTest_WidgetTask, WIDGET_TASK_STACK_SIZE, NULL,
                                &s_widgetTestThread) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Dji widget test task create error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    // Crear tarea para la interacción con widgets
    if (osalHandler->TaskCreate("user_widget_task", DjiTest_WidgetInteractionTask, WIDGET_TASK_STACK_SIZE, NULL,
                                &s_widgetInteractionTestThread) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Dji widget test task create error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    
}

//Funciones para poder mostar mensajes en el mando del Dron OJO PUEDO HABER BORRADO ALGO

static void *DjiTest_WidgetTask(void *arg)
{
    char message[DJI_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN];
    uint32_t sysTimeMs = 0;
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_UTIL_UNUSED(arg);

    while (1) {
        djiStat = osalHandler->GetTimeMs(&sysTimeMs);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Get system time ms error, stat = 0x%08llX", djiStat);
        }
#ifndef USER_FIRMWARE_MAJOR_VERSION
        snprintf(message, DJI_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN,
                 "System time : %u ms \r\n%s \r\n%s \r\n%s \r\n%s \r\n%s \r\n",
                 sysTimeMs,
                 s_djiTestWidgetLog[0].content,
                 s_djiTestWidgetLog[1].content,
                 s_djiTestWidgetLog[2].content,
                 s_djiTestWidgetLog[3].content,
                 s_djiTestWidgetLog[4].content);
#else
        snprintf(message, DJI_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN,
                 "System time : %u ms\r\nVersion: v%02d.%02d.%02d.%02d\r\nBuild time: %s %s", sysTimeMs,
                 USER_FIRMWARE_MAJOR_VERSION, USER_FIRMWARE_MINOR_VERSION,
                 USER_FIRMWARE_MODIFY_VERSION, USER_FIRMWARE_DEBUG_VERSION,
                 __DATE__, __TIME__);
#endif

        djiStat = DjiWidgetFloatingWindow_ShowMessage(message);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Floating window show message error, stat = 0x%08llX", djiStat);
        }

        osalHandler->TaskSleepMs(200);
    }
}



void DjiTest_WidgetLogAppend(const char *fmt, ...)
{
    va_list args;
    char string[WIDGET_LOG_STRING_MAX_SIZE];
    int32_t i;

    va_start(args, fmt);
    vsnprintf(string, WIDGET_LOG_STRING_MAX_SIZE, fmt, args);
    va_end(args);

    for (i = 0; i < WIDGET_LOG_LINE_MAX_NUM; ++i) {
        if (s_djiTestWidgetLog[i].valid == false) {
            s_djiTestWidgetLog[i].valid = true;
            strcpy(s_djiTestWidgetLog[i].content, string);
            break;
        }
    }

    if (i == WIDGET_LOG_LINE_MAX_NUM) {
        for (i = 0; i < WIDGET_LOG_LINE_MAX_NUM - 1; i++) {
            strcpy(s_djiTestWidgetLog[i].content, s_djiTestWidgetLog[i + 1].content);
        }
        strcpy(s_djiTestWidgetLog[WIDGET_LOG_LINE_MAX_NUM - 1].content, string);
    }
}


//---------------------------------

T_DjiReturnCode DjiTest_WidgetInteractionSetConfigFilePath(const char *path)
{
    memset(s_widgetFileDirPath, 0, sizeof(s_widgetFileDirPath));
    memcpy(s_widgetFileDirPath, path, USER_UTIL_MIN(strlen(path), sizeof(s_widgetFileDirPath) - 1));
    s_isWidgetFileDirPathConfigured = true;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wformat"
#endif


// Se ejecuta en bucle, se revisa todo el rato
static void *DjiTest_WidgetInteractionTask(void *arg)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode returnCode;
    uint32_t errorCode;
    E_DjiHmsErrorLevel errorLevel;

    // Los user log errors se muestran en terminal
    returnCode = DjiAircraftInfo_GetBaseInfo(&s_aircraftInfoBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
        return NULL;
    }

    while (1) {
        //Frecuencia de 10 Hz
        osalHandler->TaskSleepMs(100);
        // ------------------------------

        s_isSampleStart = false;
        //Salto de linea en la terminal
        // printf("\r\n");
        // USER_LOG_INFO("--------------------------------------------------------------------------------------------->");
        // USER_LOG_INFO("Revisando el valor de Boton de Env�o");

        if (s_widgetParams.confirmarEnvio) {
            printf("\r\n");
            USER_LOG_INFO("--------------------------------------------------------------------------------------------->");
            USER_LOG_INFO("Valor de Boton de Env�o Obtenido:%d", s_widgetParams.confirmarEnvio );
            // Llama a la funci�n para ejecutar la acci�n con los valores configurados
            EjecutarAccionConValores();

            // Resetea la bandera para evitar ejecuciones repetidas
            s_widgetParams.confirmarEnvio = false;
        }
}

}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

static T_DjiReturnCode DjiTestWidget_SetWidgetValue(E_DjiWidgetType widgetType, uint32_t index, int32_t value,
                                                    void *userData)
{
    USER_UTIL_UNUSED(userData);
    USER_LOG_INFO("Set widget value, widgetType = %s, widgetIndex = %d ,widgetValue = %d", s_widgetTypeNameArray[widgetType], index, value);

    s_widgetValueList[index] = value;

    switch (index) {
        case 1: // �ngulo Horizontal
            s_widgetParams.anguloMaximo = value;
            USER_LOG_INFO("�ngulo Horizontal configurado a: %d grados", s_widgetParams.anguloMaximo);
            break;

        case 2: // �ngulo Vertical
            s_widgetParams.anguloMinimo = value;
            USER_LOG_INFO("�ngulo Vertical configurado a: %d grados", s_widgetParams.anguloMinimo);
            break;

        case 3: // Velocidad
            s_widgetParams.velocidad = value;
            USER_LOG_INFO("Velocidad configurada a: %d m/s", s_widgetParams.velocidad);
            break;

        case 4: // N�mero de Ciclos
            s_widgetParams.ciclos = value;
            USER_LOG_INFO("N�mero de Ciclos configurado a: %d", s_widgetParams.ciclos);
            break;

        case 0: // Confirmar Env�o
            if (value == 1) { // Solo si el bot�n est� presionado
                s_widgetParams.confirmarEnvio = true;
                USER_LOG_INFO("Bot�n de Confirmar Env�o activado");
            }
            break;

        default:
            USER_LOG_WARN("�ndice de widget desconocido: %d", index);
            break;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTestWidget_GetWidgetValue(E_DjiWidgetType widgetType, uint32_t index, int32_t *value,
                                                    void *userData)
{
    USER_UTIL_UNUSED(userData);
    USER_UTIL_UNUSED(widgetType);

    *value = s_widgetValueList[index];

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief   Ejecuta una acci�n en el dron utilizando los valores configurados.
 * @note    Esta funci�n se activar� cuando el usuario confirme el env�o.
 */
T_DjiReturnCode EjecutarAccionConValores(void)
{
    T_DjiReturnCode resultado;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    // Ejemplo de uso de los valores configurados para el dron
    USER_LOG_INFO("Ejecutando acción con los valores:");
    USER_LOG_INFO("Ángulo Horizontal: %d grados", s_widgetParams.anguloMaximo);
    USER_LOG_INFO("Ángulo Vertical: %d grados", s_widgetParams.anguloMinimo);
    USER_LOG_INFO("Velocidad: %d m/s", s_widgetParams.velocidad);
    USER_LOG_INFO("Número de Ciclos: %d", s_widgetParams.ciclos);

    // Solicitar el movimiento
    resultado = RequestMovement(s_widgetParams);
    if (resultado != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Error al solicitar el movimiento: %d", resultado);
        return resultado;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}



/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
