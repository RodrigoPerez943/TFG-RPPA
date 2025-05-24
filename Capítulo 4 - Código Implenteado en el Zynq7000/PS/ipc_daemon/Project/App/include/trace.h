/***********************************************************************

  $Rev:: 268                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-03-29 15:19:02 +0200 #$:  Date of last commit
d
************************************************************************/

/*********************************************************************//**

	@file trace.h

	@brief Trace functions

	@author 			Eduardo MENDEZ
	@author 			RBZ Embedded Logics
	@date 2015/06/29 	Base version

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

#ifndef TRACE_H_
#define TRACE_H_

/*********************************************************************//**
  @ingroup g_app
  @{
 *//***********************************************************************/

/*********************************************************************//**
  	@defgroup g_trace Trace functions

  	@brief Functions to handler log messages

  	This group of functions define how the errors and informations messages
  	are reported to system

  	@{
 *//***********************************************************************/

/************************************************************************
	INCLUDES
************************************************************************/

/* Board Configuration Files */
#include "project_config.h"

/* Standard includes */

/* Platform includes */

/* Drivers includes */

/* Library includes */

/* Application includes */


/************************************************************************
	CONFIGURATION
************************************************************************/

#ifndef TRACE_HEADER_INFO
/** @brief Header to concatenate at first of an information message */
#define TRACE_HEADER_INFO			"INFO:"
#endif

#ifndef TRACE_HEADER_ERR
/** @brief Header to concatenate at first of an error message */
#define TRACE_HEADER_ERR			"ERROR:"
#endif

#ifndef TRACE_SYSLOG_FILE
/** @brief Name of syslog file */
#define TRACE_SYSLOG_FILE			(PROJECT_HEADER)
#endif

#ifndef TRACE_PRINT_INFO
/** @brief Print or not traces for messages in standard output */
#define TRACE_PRINT_INFO			(TRUE)
#endif

#ifndef TRACE_PRINT_ERROR
/** @brief Print or not traces for errors in standard output */
#define TRACE_PRINT_ERROR			(TRUE)
#endif

/************************************************************************
	DECLARATIONS
************************************************************************/

/** @brief Maximum length for traces including header */
#define TRACE_STR_LEN				1024u

/************************************************************************
	FUNCTIONS
************************************************************************/

/*********************************************************************//**
  See trace.c for details about how to use this function.
 */
error_t TRACE_Init( void );

/*********************************************************************//**
  See trace.c for details about how to use this function.
 */
void TRACE_Err( char * str);

/*********************************************************************//**
  See trace.c for details about how to use this function.
 */
void TRACE_Info( char * str );

/*********************************************************************//**
 @}	(close g_trace group)
*//***********************************************************************/

/*********************************************************************//**
 @}	(close g_app group)
*//***********************************************************************/

#endif /* TRACE_H_ */
