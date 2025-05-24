/***********************************************************************

  $Rev:: 323                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-04-20 10:09:00 +0200 #$:  Date of last commit
d
************************************************************************/

/*********************************************************************//**

	@file regs_ipc.h

	@brief Register interface for IPC

	This file implements an interface between the SHM driver and the higher
	layers of application used for IPC purpose

	@author 			Eduardo MENDEZ
	@author 			RBZ Embedded Logics
	@date 2017/03/09 	Base version

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

#ifndef REGS_IPC_H_
#define REGS_IPC_H_

/*********************************************************************//**
  @ingroup g_app
  @{
 *//***********************************************************************/

/*********************************************************************//**
  	@defgroup g_regs_gpios Registers

  	@brief Registers interface for GPIOs through a shared memory region

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

/* Application includes */

/************************************************************************
	CONFIGURATION
************************************************************************/

#ifndef REGS_IPC_NUM_REGS
/** @brief Number of considered outputs in SHM interface */
#define REGS_IPC_NUM_REGS					512u
#endif

#ifndef REGS_IPC_LEN_REG
/** @brief Length of registers in bytes */
#define REGS_IPC_LEN_REG					256u		/* (bytes) */
#endif

#ifndef REGS_IPC_MODE_MASTER
/** @brief Define if initializing functions are going to be compiled */
#define REGS_IPC_MODE_MASTER				(SHM_MASTER)
#endif

#ifndef REGS_IPC_DEF_VALUE
/** @brief Default value for registers */
#define REGS_IPC_DEF_VALUE					0x00
#endif

#ifndef MAX_KEY_SIZE 
/** @brief Maximum key size of a message */
#define MAX_KEY_SIZE					256u
#endif

#ifndef MAX_VALUE_SIZE 
/** @brief Maximum value size of a message */
#define MAX_VALUE_SIZE					256u
#endif

/************************************************************************
	DECLARATIONS
************************************************************************/

/** @name Error messages
 *  @{
 */
#define IPC_ERROR_OUT_OF_RANGE		-2		/**< @brief Out of range value */
/** @}*/

typedef char *REGS_IPC_reg_t[MAX_VALUE_SIZE];
#ifndef __error_t_defined
typedef int error_t;
#endif

/************************************************************************
	FUNCTIONS
************************************************************************/

/*********************************************************************//**
  See regs_gpios.c for details about how to use this function.
 */
error_t REGS_IPC_Init( void );

/*********************************************************************//**
  See regs_gpios.c for details about how to use this function.
 */
error_t REGS_IPC_Connect( void );

/*********************************************************************//**
  See regs_gpios.c for details about how to use this function.
 */
error_t REGS_IPC_Reset( void );

/*********************************************************************//**
  See regs_gpios.c for details about how to use this function.
 */
void REGS_IPC_Unlock( void );

/*********************************************************************//**
  See regs_gpios.c for details about how to use this function.
 */
error_t REGS_IPC_Lock( void );

/*********************************************************************//**
  See regs_gpios.c for details about how to use this function.
 */
error_t REGS_IPC_Write(uint8_t *key, uint8_t *value);

/*********************************************************************//**
  See regs_gpios.c for details about how to use this function.
 */
error_t REGS_IPC_Read(uint8_t *key, uint8_t *value);

/*********************************************************************//**
  See regs_gpios.c for details about how to use this function.
 */
error_t REGS_IPC_Default( void );

/*********************************************************************//**
 @}	(close g_regs_ipc group)
*//***********************************************************************/

/*********************************************************************//**
 @}	(close g_app group)
*//***********************************************************************/

#endif /* REGS_IPC_H_ */
