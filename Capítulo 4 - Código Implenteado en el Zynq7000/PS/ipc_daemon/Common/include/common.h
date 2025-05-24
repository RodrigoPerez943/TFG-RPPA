/***********************************************************************

  $Rev:: 268                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-03-29 15:19:02 +0200 #$:  Date of last commit
d
************************************************************************/

/*********************************************************************//**

	@file common.h

	@brief Common and useful declarations for this application

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
    @copyrightSHM_ERROR_CREATION
		The terms stated above shall be understood  affecting that stated in
		applicable Laws.

*************************************************************************/

#ifndef COMMON_H_
#define COMMON_H_

/************************************************************************
	GENERAL INCLUDES
************************************************************************/

#include <stdint.h>
#include <argz.h>		/* Used for error_t types */
#include <stdbool.h>

/************************************************************************
	ERROR CODES
************************************************************************/

/** @defgroup g_common_errors Common error codes
 *
 * 	@brief Common and general error codes
 *  @{
 */
#define ERROR_OK		0		/**< @brief No error (all is going OK) */
/** @}*/

/************************************************************************
	TIME DEFINITIONS
************************************************************************/

/** @brief Conversion factor from ms to us */
#define MS_TO_US		1000u

/** @brief Conversion factor from ms to ns */
#define MS_TO_NS		1000000u

/************************************************************************
	STANDARD VALUES
************************************************************************/

#define TRUE		(true)
#define FALSE		(false)

/************************************************************************
	TRACE
************************************************************************/

#if TRACE_MODE == 1
#define TRACE 	printf
#else
#define TRACE
#endif

#endif /* COMMON_H_ */
