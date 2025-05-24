/***********************************************************************

  $Rev:: 268                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-03-29 15:19:02 +0200 #$:  Date of last commit
d
************************************************************************/

/*********************************************************************//**

	@file app_config.h

	@brief General configuration of application

	@author 			Eduardo MENDEZ
	@author 			RBZ Embedded Logics
	@date 2015/06/19 	Base version

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

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

/* Board Configuration Files */
#include "project_config.h"

/*********************************************************************//**
  	@defgroup g_app_config Application configuration

  	@brief High level configuration

  	@{
 *//***********************************************************************/

/************************************************************************
	STRINGS CONFIGURATION
************************************************************************/

/** @defgroup g_app_config_strings Application string messages
 *
 * 	@brief Definition of all string messages used in the application
 *  @{
 */
#define APP_STR_ERROR_NO_ROOT		"User must be run as root"
#define APP_STR_ERROR_INIT_HW		"Error initializing HW"
#define APP_STR_ERROR_INIT			"Error initializing App"
#define APP_STR_ERROR_IN_SUPERIO	"Error accessing SuperIO"
#define APP_STR_ERROR_IN_SHM		"Error accessing SHM"
#define APP_STR_ERROR_CLOSED		"Daemon has been closed"
/** @}*/

/************************************************************************
	APPLICATION CONFIGURATION
************************************************************************/

/** @defgroup g_app_config_main Application configuration
 *
 * 	@brief Configuration of main loop
 *  @{
 */

/************************************************************************
	START MODE
************************************************************************/

/** @brief Set application in daemon mode or not
 *
 * 	Normal mode is intended for debug mode
 * */
#define PROJECT_DAEMON_MODE				(FALSE)

/************************************************************************
	MAIN LOOP CONFIGURATION
************************************************************************/

/** @brief Tick of main loop */
#define PROJECT_MAIN_LOOP_DELAY			10u	/* (ms) */

/************************************************************************
	SYSLOG
************************************************************************/

/** @brief Syslog name file */
#define ERR_SYSLOG_FILE					"rbz_ipc"

/************************************************************************
	WATCHDOG CONFIGURATION
************************************************************************/

/** @defgroup g_app_config_wd Watchdog configuration
 *
 * 	@brief High level watchdog configuration
 *  @{
 */

/** @brief Period of watchdog execution
 *
 * The function which process the watchdog is called every this time, and
 * not at every loop cycle
 * */
#define APP_WD_TIMEOUT			1000		/* (ms) */

/** @}*/

/*********************************************************************//**
 @}	(close g_app_config group)
*//***********************************************************************/

#endif /* APP_CONFIG_H_ */
