/***********************************************************************

  $Rev:: 323                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-04-20 10:09:00 +0200 #$:  Date of last commit

************************************************************************/

/*********************************************************************//**

	@file regs_ipc.c

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

/************************************************************************
	INCLUDES
************************************************************************/

/* Board Configuration Files */
#include "project_config.h"
#include "app_config.h"

/* Standard includes */
#include <argz.h>
#include <unistd.h>

/* Platform includes */
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "common.h"

/* Drivers includes */
#include "shm.h"

/* Library includes */

/* Application includes */
#include "err.h"

/* Specific includes */
#include "regs_ipc.h"
#include "decoder_reg.h"

/************************************************************************
	DECLARATIONS
************************************************************************/

/** @brief SHM handler for IPC */
static shm_buf_t * shm_ipc_handler = NULL;

/************************************************************************
	FUNCTIONS
************************************************************************/

/* Local functions */

#if REGS_IPC_MODE_MASTER == TRUE

/*********************************************************************//**
 * 	@brief Initialize the registers interface
 *
 * 	Shared memory interface is used for communication between the user
 * 	application and this daemon.
 *
 * 	@return Result of the operation
 */
error_t REGS_IPC_Init( void ){
	error_t res = ERROR_OK;

	/* Initialize SHM driver */
	res = SHM_Init(&shm_ipc_handler, SHM_NAME_IPC);

	if (res >= ERROR_OK){
		/* Reset to default values SHM interface */
		REGS_IPC_Reset();
	}

	return res;
}

#endif

/*********************************************************************//**
 * 	@brief Sets de default config of all keys on the IPC
 *
 *  This function writes all the default keys table (stored on registerOfIPC) 
 *  to the shared memory
 * 	@return Result of the operation
 */
error_t REGS_IPC_Default( void ){
  error_t res = ERROR_OK;
  uint16_t i = 0;
  for(i = 0; i < sizeof(registerOfIPC)/SHM_POSITION_SIZE; i++)
  {
    res = SHM_Write_With_Limits(shm_ipc_handler, (uint8_t *)registerOfIPC[i].reg_str, REGS_IPC_LEN_REG, (uint8_t *)registerOfIPC[i].value_str, registerOfIPC[i].maxValue, registerOfIPC[i].minValue); 

    if(res < ERROR_OK)
    {
      break;
    }
  }

  return res;
}

/*********************************************************************//**
 * 	@brief Connect to register resource
 *
 * 	Connect to an already initialized register resource
 *
 * 	@return Result of the operation
 */
error_t REGS_IPC_Connect( void ){
	error_t res = ERROR_OK;

	/* Connect to SHM driver */
	res = SHM_Connect(&shm_ipc_handler, SHM_NAME_IPC);

	return res;
}

/*********************************************************************//**
 * 	@brief Reset input registers to default values
 *
 *	@return Result of operation
 */
error_t REGS_IPC_Reset( void ){
	error_t res = ERROR_OK;

	/* Reset inputs */
    SHM_Reset(shm_ipc_handler);

	return res;
}

/*********************************************************************//**
 * 	@brief Write a register value into SHM region
 *
 *	@return Result of operation
 */
error_t REGS_IPC_Write(uint8_t *key, uint8_t *value){
	error_t res = ERROR_OK;
    uint8_t old_value[REGS_IPC_LEN_REG] = {0};
    float new_value = 0;
    float max_val, min_val;

    res = SHM_Read(shm_ipc_handler, key, REGS_IPC_LEN_REG, old_value, &max_val, &min_val);
    if (res >= ERROR_OK)
    {
      new_value = atof((char *)value);

      if(((max_val != min_val) && (new_value >= min_val) && (new_value <= max_val)) || (max_val == min_val))
      {
        res = SHM_Write(shm_ipc_handler, key, REGS_IPC_LEN_REG, value);
      }
      else
      {
        res = IPC_ERROR_OUT_OF_RANGE;
      }
    }

	return res;
}

/*********************************************************************//**
 * 	@brief Read a register value from SHM region
 *
 *	@return Result of operation
 */
error_t REGS_IPC_Read(uint8_t *key, uint8_t *value){
	error_t res = ERROR_OK;
    float max_val, min_val;

    res = SHM_Read(shm_ipc_handler, key, REGS_IPC_LEN_REG, value, &max_val, &min_val);

	return res;
}

