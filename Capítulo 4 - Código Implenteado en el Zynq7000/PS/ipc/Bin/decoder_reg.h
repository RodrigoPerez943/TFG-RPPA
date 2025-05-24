/***********************************************************************
    $Revision: 1541 $: Revision of last commit
    $Author: mcalvo $: Author of last commit
    $Date: 2019-07-24 09:56:42 +0200 (mié 24 de jul de 2019) $: Date of last commit
*************************************************************************/
/*********************************************************************//**

	@file decoder_reg.h

	@brief

	@author 		Carlos AMOR
	@author 		RBZ Embedded Logics
	@date 15 jun. 2017 	Base version

  @copyright
		This software is property of RBZ Embedded Logics. Its reproduction,
		total or  partial, by any means, in any form, permanent or temporary,
		is forbidden unless explicitly authorized by RBZ Embedded Logics.
  @copyright
		Any adaptation, amendment, translation or transformation, as well as
		the decompiling or to disassemble of this software  product, shall only
		be performed with the explicit authorization of RBZ Embedded Logics.
		The user of the present software product shall be allowed  to make a
		backup copy  as long as it is necessary  for the utilization  of the
		same.
  @copyright
		The terms stated above shall be understood  affecting that stated in
		applicable Laws.

*************************************************************************/

#ifndef DECODER_REG_H_
#define DECODER_REG_H_

#include <stdbool.h>

/**************************************************************************
 * CONSTANTS
 *************************************************************************/

#define RESULT_OK				                    1
#define RESULT_ERROR				                0xFFFF

#define REG_SAVE_CONFIG                             "SAVE_CONFIG"
#define REG_GET_ACTUAL_CONFIG                       "GET_ACTUAL_CONFIG"
#define REG_GET_SAVE_CONFIG                         "GET_SAVE_CONFIG"
#define REG_FACTORY_RESET                           "FACTORY_RESET"
#define REG_DIAGNOSTIC                              "DIAGNOSTIC"
#define REG_ERROR_EXEC			                    "ERROR_EXEC"
#define REG_SET_DATE		                        "SET_DATE"
#define REG_GET_DATE		                        "GET_DATE"
#define REG_START_MEASURE                           "START_MEASURE"
#define REG_START_FREQ                              "START_FREQ"
#define REG_END_FREQ                                "END_FREQ"
#define REG_STEP_FREQ                               "STEP_FREQ"
#define REG_SERVERTCP_WD                            "SERVERTCP_WD"
#define REG_SERVERTCP_WD_1                          "SERVERTCP_WD_1"
#define REG_SERVERTCP_WD_2                          "SERVERTCP_WD_2"
#define REG_DESC_WD                                 "DESC_WD"
#define REG_SAMPLES_NUMBER                          "SAMPLES_NUMBER"
#define REG_CAPTURES_NUMBER                         "CAPTURES_NUMBER"
#define REG_STATUS                                  "STATUS"
#define REG_GET_DEVICE_PARAMS                       "GET_DEVICE_PARAMS"


#define REG_SAVE_CONFIG_MAX                         4           
#define REG_SAVE_CONFIG_MIN			                1
#define REG_GET_ACTUAL_CONFIG_MAX                   1
#define REG_GET_ACTUAL_CONFIG_MIN		            0
#define REG_GET_SAVE_CONFIG_MAX                     4
#define REG_GET_SAVE_CONFIG_MIN			            0
#define REG_FACTORY_RESET_MAX                       1
#define REG_FACTORY_RESET_MIN			            0
#define REG_DIAGNOSTIC_MAX                          2048
#define REG_DIAGNOSTIC_MIN			                0
#define REG_ERROR_EXEC_MAX			                1
#define REG_ERROR_EXEC_MIN			                0
#define REG_SET_DATE_MAX                            2147483647
#define REG_SET_DATE_MIN                            0
#define REG_GET_DATE_MAX                            2147483647
#define REG_GET_DATE_MIN                            0
#define REG_START_MEASURE_MAX                       1
#define REG_START_MEASURE_MIN                       0
#define REG_MEASURE_STATUS_MAX                      1
#define REG_MEASURE_STATUS_MIN                      0
#define REG_START_FREQ_MAX                          6000000 
#define REG_START_FREQ_MIN                          2.5
#define REG_END_FREQ_MAX                            6000000
#define REG_END_FREQ_MIN                            2.5
#define REG_STEP_FREQ_MAX                           6000000
#define REG_STEP_FREQ_MIN                           1
#define REG_SERVERTCP_WD_MAX                        1
#define REG_SERVERTCP_WD_MIN                        0
#define REG_SERVERTCP_WD_1_MAX                      1
#define REG_SERVERTCP_WD_1_MIN                      0
#define REG_SERVERTCP_WD_2_MAX                      1
#define REG_SERVERTCP_WD_2_MIN                      0
#define REG_DESC_WD_MAX                             1
#define REG_DESC_WD_MIN                             0
#define REG_SAMPLES_NUMBER_MIN                      1
#define REG_SAMPLES_NUMBER_MAX                      5000000
#define REG_CAPTURES_NUMBER_MIN                        1
#define REG_CAPTURES_NUMBER_MAX                        5000000
#define REG_WIRE_MAP_MAX                            1
#define REG_WIRE_MAP_MIN                            0
#define REG_STATUS_MAX                              7
#define REG_STATUS_MIN                              0
#define REG_GET_DEVICE_PARAMS_MAX                   1
#define REG_GET_DEVICE_PARAMS_MIN                   0
#define REG_REQ_SERVER_RESTART_MAX                  1
#define REG_REQ_SERVER_RESTART_MIN                  0

#define DIAGNOSTIC_DEFAULT                          "0"
#define ZERO_DEFAULT                                "0"
#define NO_LIMIT                                    0
#define GET_DATE_DEFAULT                            "0"
#define END_FREQ_DEFAULT                            "6000000"
#define START_FREQ_DEFAULT                          "100000"
#define STEP_FREQ_DEFAULT                           "100000" 
#define SAMPLES_NUMBER_DEFAULT                      "16384"
#define CAPTURES_NUMBER_DEFAULT                         "59"
#define STATUS_DEFAULT                              "0"
#define REG_GET_DEVICE_PARAMS_DEFAULT               "0"
#define REG_REQ_SERVER_RESTART_DEFAULT              "0"

/** @brief Structure of IPC register
 *
 *      Data structure to store the register ID, name and value max and min.
 *      This structure contains the following fields:
 *
 *      - Number of IPC register
 *      - Name of IPC register
 *      - Max Value of register
 *      - Min Value of register
 */
typedef struct regsIPC{
	unsigned char reg_str[256];       /**< Name of IPC register */
	unsigned char value_str[256];       /**< Name of IPC register */
	float maxValue;         /**< Max value of IPC register */
	float minValue;         /**< Min value of IPC register */
}regIPC;

#define registerOfIPC (const struct regsIPC[]) { \
{   REG_SAVE_CONFIG,    ZERO_DEFAULT,   REG_SAVE_CONFIG_MAX,      REG_SAVE_CONFIG_MIN },                                            \
{   REG_GET_ACTUAL_CONFIG,  ZERO_DEFAULT,   REG_GET_ACTUAL_CONFIG_MAX,    REG_GET_ACTUAL_CONFIG_MIN },                              \
{   REG_GET_SAVE_CONFIG,    ZERO_DEFAULT,   REG_GET_SAVE_CONFIG_MAX,      REG_GET_SAVE_CONFIG_MIN },                                \
{   REG_FACTORY_RESET,  ZERO_DEFAULT,   REG_FACTORY_RESET_MAX,    REG_FACTORY_RESET_MIN },                                          \
{   REG_ERROR_EXEC, ZERO_DEFAULT,   REG_ERROR_EXEC_MAX,   REG_ERROR_EXEC_MIN },                                                     \
{   REG_SET_DATE,   ZERO_DEFAULT,   REG_SET_DATE_MAX,     REG_SET_DATE_MIN },                                                       \
{   REG_GET_DATE,   ZERO_DEFAULT,   REG_GET_DATE_MAX,     REG_GET_DATE_MIN },                                                       \
{   REG_START_MEASURE,   ZERO_DEFAULT,   REG_START_MEASURE_MAX,     REG_START_MEASURE_MIN },                                        \
{   REG_START_FREQ, START_FREQ_DEFAULT, REG_START_FREQ_MAX,   REG_START_FREQ_MIN },                                                 \
{   REG_END_FREQ, END_FREQ_DEFAULT, REG_END_FREQ_MAX,   REG_END_FREQ_MIN },                                                         \
{   REG_STEP_FREQ, STEP_FREQ_DEFAULT, REG_STEP_FREQ_MAX,   REG_STEP_FREQ_MIN },                                                     \
{   REG_SERVERTCP_WD,   ZERO_DEFAULT,   REG_SERVERTCP_WD_MAX,     REG_SERVERTCP_WD_MIN },                                           \
{   REG_SERVERTCP_WD_1,   ZERO_DEFAULT,   REG_SERVERTCP_WD_1_MAX,     REG_SERVERTCP_WD_1_MIN },                                     \
{   REG_SERVERTCP_WD_2,   ZERO_DEFAULT,   REG_SERVERTCP_WD_2_MAX,     REG_SERVERTCP_WD_2_MIN },                                     \
{   REG_DESC_WD,   ZERO_DEFAULT,   REG_DESC_WD_MAX,     REG_DESC_WD_MIN },                                                          \
{   REG_SAMPLES_NUMBER, SAMPLES_NUMBER_DEFAULT, REG_SAMPLES_NUMBER_MAX, REG_SAMPLES_NUMBER_MIN },                                   \
{   REG_CAPTURES_NUMBER, CAPTURES_NUMBER_DEFAULT, REG_CAPTURES_NUMBER_MAX, REG_CAPTURES_NUMBER_MIN},                                \
{   REG_DIAGNOSTIC, DIAGNOSTIC_DEFAULT, REG_DIAGNOSTIC_MAX,   REG_DIAGNOSTIC_MIN },                                                 \
{   REG_STATUS, STATUS_DEFAULT, REG_STATUS_MAX,   REG_STATUS_MIN },                                                                 \
{   REG_GET_DEVICE_PARAMS, REG_GET_DEVICE_PARAMS_DEFAULT, REG_GET_DEVICE_PARAMS_MAX, REG_GET_DEVICE_PARAMS_MIN},                    \
}

typedef struct configReg{
  unsigned char reg_str[256];       /**< Name of register */
  unsigned char value_str[256];     /**< Value of register */
}configReg;

#define configTable  \
{ REG_START_FREQ, START_FREQ_DEFAULT}, \
{ REG_END_FREQ, END_FREQ_DEFAULT }, \
{ REG_STEP_FREQ, STEP_FREQ_DEFAULT }, \
{ REG_SAMPLES_NUMBER, SAMPLES_NUMBER_DEFAULT },\
{ REG_CAPTURES_NUMBER, CAPTURES_NUMBER_DEFAULT },\
{ REG_OUTPUT_FILENAME, OUTPUT_FILENAME_DEFAULT},\
{ REG_REFERENCE_AMPLITUDE, REFERENCE_AMPLITUDE_DEFAULT },\
{ REG_THRESHOLD_AMPLITUDE, THRESHOLD_AMPLITUDE_DEFAULT },\
{ REG_LOAD_CALIB_FILE, REG_LOAD_CALIB_FILE_DEFAULT},\
{ REG_MEASUREMENT_MODE, REG_MEASUREMENT_MODE_DEFAULT},\

#define APP_CONFIG_ID			    1
#define APP_DESC_ID			        2
#define APP_SERVERTCP_ID			3
#define APP_SERVERTCP_1_ID			4
#define APP_SERVERTCP_2_ID			5
#define APP_CAPTURA_ID              6

#define APP_CONFIG_STR			    "config"
#define APP_DESC_STR			    "desc"
#define APP_SERVERTCP_STR           "servertcp"
#define APP_SERVERTCP_1_STR         "servertcp"
#define APP_SERVERTCP_2_STR         "servertcp"
#define APP_CAPTURA_STR             "captura"

/** @brief Struct apps identifications
 *
 *      Data structure to store the name of applications and his id. This structure
 *      contains the following fields:
 *
 *      - App identification
 *      - App name
 *
 */
typedef struct apps{
        uint8_t app_id;     /**< Aplication identificator */
        char app_str[60];   /**< Aplication name */
}arrayApp;

#define appsID 								            \
{	APP_CONFIG_ID,			    APP_CONFIG_STR },		\
{   APP_DESC_ID,                APP_DESC_STR },         \
{   APP_SERVERTCP_ID,           APP_SERVERTCP_STR },    \
{   APP_SERVERTCP_1_ID,         APP_SERVERTCP_1_STR },  \
{   APP_SERVERTCP_2_ID,         APP_SERVERTCP_2_STR },  \
{   APP_CAPTURA_ID,             APP_CAPTURA_STR },

#endif // DECODER_REG_H_
