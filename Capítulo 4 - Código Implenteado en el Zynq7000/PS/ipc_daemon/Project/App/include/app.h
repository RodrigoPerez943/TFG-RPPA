/***********************************************************************

  $Rev:: 268                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-03-29 15:19:02 +0200 #$:  Date of last commit
d
************************************************************************/

/*********************************************************************//**

	@file app.h

	@brief Application main file

	@author 			Eduardo MENDEZ
	@author 			RBZ Embedded Logics
	@date 2017/03/08 	Base version

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

#ifndef APP_H_
#define APP_H_

/*********************************************************************//**
  	@defgroup g_app Application structure

  	@brief High level structure application

	This module implements the IPC daemon application. This daemon
	implement an inter-proccess communication method used to manage
	communications between several applications.

	In addition, this IPC handles the system WD, according to a WD
	supervisor which runs as virtual WD handler.

  	@{
 *//***********************************************************************/

/************************************************************************
	INCLUDES
************************************************************************/

/************************************************************************
	CONFIGURATION
************************************************************************/

#ifndef APP_WD_TIMEOUT
/** @brief Period of watchdog execution
 *
 * The function which process the watchdog is called every this time, and
 * not at every loop cycle (ms)
 * */
#define APP_WD_TIMEOUT		500		/* (ms) */
#endif

/************************************************************************
	DECLARATIONS
************************************************************************/

/************************************************************************
	FUNCTIONS
************************************************************************/

/*********************************************************************//**
  See app.c for details about how to use this function.
 */
error_t APP_Start( void );

/*********************************************************************//**
 @}	(close g_app group)
*//***********************************************************************/

#endif /* SHM_H_ */
