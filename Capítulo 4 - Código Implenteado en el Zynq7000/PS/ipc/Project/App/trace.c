/***********************************************************************

  $Rev:: 268                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-03-29 15:19:02 +0200 #$:  Date of last commit

************************************************************************/

/*********************************************************************//**

	@file trace.c

	@brief Trace functions

	@author 			Eduardo MENDEZ
	@author 			RBZ Embedded Logics
	@date 2017/03/13 	Base version

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

/* Platform includes */
#include "common.h"
#include <syslog.h>

/* Drivers includes */

/* Library includes */

/* Application includes */

/* Specific includes */
#include "trace.h"

/************************************************************************
	DECLARATIONS
************************************************************************/

/************************************************************************
	FUNCTIONS
************************************************************************/

/* Local functions */

/*********************************************************************//**
 * 	@brief Initialize trace module
 *
 *	@return Result of the operation
 */
error_t TRACE_Init( void ){
	error_t res = ERROR_OK;

	/* Nothing to do here */

	return res;
}

/*********************************************************************//**
 * 	@brief Print a error trace
 *
 * 	Error message is saved into syslog
 */
void TRACE_Err( char * str){
	char str_log[TRACE_STR_LEN];

	/* Compose final message */
	snprintf(str_log, TRACE_STR_LEN, "%s %s\n", TRACE_HEADER_ERR, str);

#if TRACE_PRINT_ERROR == TRUE

#if PROJECT_DAEMON_MODE == TRUE
	/* Print error on syslog */
	openlog(TRACE_SYSLOG_FILE, LOG_PID, LOG_USER);
	syslog(LOG_ERR, "%s", str);
	closelog();
#else
	/* Print error over terminal */
	perror(str_log);

#endif

#endif
}

/*********************************************************************//**
 * 	@brief Print an information message
 *
 * 	Information message is saved into syslog
 */
void TRACE_Info( char * str ){
	char str_log[TRACE_STR_LEN];

	/* Compose final message */
	snprintf(str_log, TRACE_STR_LEN, "%s %s\n", TRACE_HEADER_INFO, str);

#if TRACE_PRINT_INFO == TRUE

#if PROJECT_DAEMON_MODE == TRUE
	/* Print error on syslog */
	openlog(TRACE_SYSLOG_FILE, LOG_PID, LOG_USER);
	syslog(LOG_INFO, "%s", str);
	closelog();
#else
	/* Print error over terminal */
	printf("%s", str_log);
#endif

#endif
}
