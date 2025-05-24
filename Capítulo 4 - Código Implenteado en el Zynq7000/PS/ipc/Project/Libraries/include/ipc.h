/***********************************************************************

  $Rev:: 449                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-07-19 12:01:04 +0200 #$:  Date of last commit
d
************************************************************************/

/*********************************************************************//**

	@file ipc.h

	@brief Shared memory region

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

#ifndef IPC_H_
#define IPC_H_

/*********************************************************************//**
  	@defgroup g_ipc IPC

  	@brief IPC (Inter-Process Communication) management

  	@{
 *//***********************************************************************/

/************************************************************************
	INCLUDES
************************************************************************/

/* Board Configuration Files */

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>

/* Platform includes */

/* Drivers includes */

/* Library includes */
#include "regs_ipc.h"
#include "ipc_notif.h"

/* Application includes */

/************************************************************************
	CONFIGURATION
************************************************************************/



/************************************************************************
	DECLARATIONS
************************************************************************/

/** @name Error messages
 *  @{
 */
#define IPC_OK						 1		/**< @brief Everything is ok */
#define IPC_ERROR_INCORRECT_REG		-1		/**< @brief Register is not correct */
/** @}*/

#ifndef __error_t_defined
typedef int error_t;
#endif

/************************************************************************
	FUNCTIONS
************************************************************************/

/*********************************************************************//**
  See ipc.c for details about how to use this function.
 */
error_t IPC_Init( char * ip, uint32_t source_addr );

/*********************************************************************//**
  See ipc.c for details about how to use this function.
 */
error_t IPC_InitMaster( void );

/*********************************************************************//**
  See ipc.c for details about how to use this function.
 */
void IPC_Close( IPC_NOTIF_conn_t * ptr_conn );

/*********************************************************************//**
  See ipc.c for details about how to use this function.
 */
error_t IPC_Write( uint8_t *key,uint8_t *value);

/*********************************************************************//**
  See ipc.c for details about how to use this function.
 */
error_t IPC_Read( uint8_t *key, uint8_t *value );

/*********************************************************************//**
  See ipc.c for details about how to use this function.
 */
error_t IPC_ReadNotif(IPC_NOTIF_conn_t * ptr_conn, IPC_NOTIF_msg_t * ptr_msg);

/*********************************************************************//**
  See ipc.c for details about how to use this function.
 */
error_t IPC_AddFilter(IPC_NOTIF_conn_t * ptr_conn, uint8_t *filter_reg);

/*********************************************************************//**
  See ipc.c for details about how to use this function.
 */
error_t IPC_Create(IPC_NOTIF_conn_t * ptr_conn, char * ip);

/*********************************************************************//**
 @}	(close g_ipc group)
*//***********************************************************************/

#endif /* IPC_H_ */
