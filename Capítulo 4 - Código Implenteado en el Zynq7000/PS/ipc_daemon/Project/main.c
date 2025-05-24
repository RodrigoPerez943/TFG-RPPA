/***********************************************************************

  $Rev:: 321                         $:  Revision of last commit
  $Author:: mcalvo                   $:  Author of last commit
  $Date:: 2017-04-19 17:19:32 +0200 #$:  Date of last commit

************************************************************************/

/*********************************************************************//**

	@file main.c

	@brief Startup application file

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

/************************************************************************
	INCLUDES
************************************************************************/

/* Board Configuration Files */
#include "project_config.h"
#include "app_config.h"

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* Platform includes */
#include "common.h"
#include <sys/uio.h>
#include <signal.h>

/* Drivers includes */

/* Library includes */

/* Application includes */
#include "app.h"
#include "trace.h"

/* Specific includes */

/************************************************************************
	DECLARATIONS
************************************************************************/

/************************************************************************
	FUNCTIONS
************************************************************************/

/* Local functions */
static error_t InitDevice( void );

#if PROJECT_DAEMON_MODE == TRUE
static void CreateDaemon( void );
#endif

#if PROJECT_DAEMON_MODE == TRUE
/*********************************************************************//**
 * 	@brief Create daemon
 */
static void CreateDaemon( void ){
	pid_t pid;

	/* Ford off the parent process */
	pid = fork();

	/* An error occurred */
	if (pid < 0){
		exit(EXIT_FAILURE);
	}

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");
}
#endif

/*********************************************************************//**
 * 	@brief Start point of application at system level
 */
int main(void) {
	error_t res = ERROR_OK;

	/* This application must be executed as root */
#if 0
    if (iopl(3) < 0) {
      TRACE_Err(APP_STR_ERROR_NO_ROOT);

      return 0;
    }
#endif

#if PROJECT_DAEMON_MODE == TRUE
	/* Create daemon */
	CreateDaemon();
#endif

	/* Initialize all low level layers */
	res = InitDevice();

	if (res >= ERROR_OK){
		/* Call application main loop */
		APP_Start();
	}
	else{
		/* Return error */
		TRACE_Err(APP_STR_ERROR_INIT_HW);
	}

	/* End application */
	return 0;
}

/*********************************************************************//**
 * 	@brief Initialize all low level layers of application
 *
 * 	@return Result of operation
 */
static error_t InitDevice( void ){
	error_t res = ERROR_OK;

	/* Nothing to do here */

	return res;
}
