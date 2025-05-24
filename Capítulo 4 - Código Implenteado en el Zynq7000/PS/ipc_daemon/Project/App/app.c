/***********************************************************************

  $Rev:: 268                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-03-29 15:19:02 +0200 #$:  Date of last commit

************************************************************************/

/*********************************************************************//**

	@file app.c

	@brief Application main file

	@author 			Eduardo MENDEZ
	@author 			RBZ Embedded Logics
	@date 2015/06/22 	Base version

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

/************************************************************************
	INCLUDES
************************************************************************/

/* Board Configuration Files */
#include "project_config.h"
#include "app_config.h"

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Platform includes */
#include "common.h"

/* Drivers includes */
#include "shm.h"

/* Library includes */
#include "ipc.h"

/* Application includes */
#include "trace.h"

/* Test includes */

/* Specific includes */
#include "app.h"

/************************************************************************
	DECLARATIONS
************************************************************************/


/************************************************************************
	FUNCTIONS
************************************************************************/

/* Local functions */
static error_t APP_Loop( void );
static error_t APP_Init( void );
static void APP_Wd( uint32_t elapsed_time );

/*********************************************************************//**
 * 	@brief Initialize higher layers of application
 *
 *	@return Result of the operation
 */
static error_t APP_Init( void ){
	error_t res = ERROR_OK;

	/* Initialize shared memory region GPIOs */
	if (res >= ERROR_OK){
		res = IPC_InitMaster();
	}

	// TODO: Registros para el WD

	// TODO: Inicialización del WD

	return res;
}

/*********************************************************************//**
 * 	@brief Start point of application
 *
 * 	Start the main loop of application
 *
 * 	@return Result of the operation
 */
error_t APP_Start( void ){
	error_t res = ERROR_OK;

	/* Initialize higher layers of application */
	res = APP_Init();
	if (res < ERROR_OK){
		TRACE_Err(APP_STR_ERROR_INIT);
	}

	if (res >= ERROR_OK){
		APP_Loop();
	}

	TRACE_Err(APP_STR_ERROR_CLOSED);

	return res;
}

/*********************************************************************//**
 * 	@brief Main loop of application
 *
 * 	Implement the main loop of application
 *
 * 	@return Result of the operation
 */
static error_t APP_Loop( void ){
	error_t res = ERROR_OK;

	while(res >= ERROR_OK){

		/* Process wd */
		APP_Wd(PROJECT_MAIN_LOOP_DELAY);

		usleep(PROJECT_MAIN_LOOP_DELAY * MS_TO_US);
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Call WD function
 *
 * 	Watchdog function is only executed every APP_WD_TIMEOUT ms
 *
 * 	@param elapsed_time Elapsed time since last call
 */
static void APP_Wd( uint32_t elapsed_time ){
	static uint32_t timeout = 0;

	timeout += elapsed_time;
	if (timeout >= APP_WD_TIMEOUT){

		timeout = 0;
	}
}
