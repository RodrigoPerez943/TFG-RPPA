/***********************************************************************

  $Rev:: 323                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-04-20 10:09:00 +0200 #$:  Date of last commit
d
************************************************************************/

/*********************************************************************//**

	@file shm.h

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

#ifndef SHM_H_
#define SHM_H_

/*********************************************************************//**
  	@defgroup g_shm Shared Memory Region

  	@brief Shared memory region management

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
#include <semaphore.h>

/* Drivers includes */

/* Library includes */

/* Application includes */

/************************************************************************
	CONFIGURATION
************************************************************************/

#ifndef SHM_LEN
/** @brief Size of SHM buffer in bytes
 *
 * 	This parameter defines the size of container reserver to application
 * 	data
 */
#define SHM_LEN			96000u 		/* (bytes) */
#endif

#ifndef SHM_NAME
/** @brief Name of shared memory region */
#define SHM_NAME		"/shm_mem"
#endif

#ifndef SHM_TIMEOUT
/** @brief Maximum amount of time to wait the shared memory resource (ms)
 *
 * 	@warning Maximum value is 999ms */
#define SHM_TIMEOUT		999u		/* (ms) */
#endif

#ifndef SHM_MASTER
/** @brief Define if initializing functions are going to be compiled */
#define SHM_MASTER		(TRUE)
#endif

/************************************************************************
	DECLARATIONS
************************************************************************/

/** @brief Structure used as shared memory buffer */
typedef struct{
	sem_t sem;					/**< Semaphore to protect access */
	uint8_t buffer[SHM_LEN];	/**< Buffer reserved for higher application level */
}shm_buf_t;

/** @name Error codes
 * 	@{
 */
#define SHM_ERROR_CREATION		(-1)	/**< An error has occurred while opening the memory region */
#define SHM_ERROR_SEM			(-2)	/**< An error occurred with the semaphore */
#define SHM_ERROR_LOCKING		(-3)	/**< Error while waiting to lock resource */
#define SHM_POSITION_SIZE       ((REGS_IPC_LEN_REG*2) + (2*sizeof(float)))   /**< Size of one position on the table of the shared memory */
/** @}*/

/************************************************************************
	FUNCTIONS
************************************************************************/

/*********************************************************************//**
  See shm.c for details about how to use this function.
 */
error_t SHM_Init(shm_buf_t ** shm_handler, char * shm_name);

/*********************************************************************//**
  See shm.c for details about how to use this function.
 */
error_t SHM_Connect( shm_buf_t ** shm_handler, char * shm_name );

/*********************************************************************//**
  See shm.c for details about how to use this function.
 */
void SHM_Remove( error_t SHM_Connect, shm_buf_t * shmbuf );

/*********************************************************************//**
  See shm.c for details about how to use this function.
 */
error_t SHM_Lock( shm_buf_t * shmbuf );

/*********************************************************************//**
  See shm.c for details about how to use this function.
 */
void SHM_Unlock( shm_buf_t * shmbuf );

/*********************************************************************//**
  See shm.c for details about how to use this function.
 */
error_t SHM_ReadUnlock( shm_buf_t * shmbuf, uint8_t *key, uint16_t len,
		uint8_t * buf, float *max_value, float *min_value );

/*********************************************************************//**
  See shm.c for details about how to use this function.
 */
error_t SHM_Read( shm_buf_t * shmbuf, uint8_t *key, uint16_t len,
		uint8_t * buf, float *max_val, float *min_val);

/*********************************************************************//**
  See shm.c for details about how to use this function.
 */
error_t SHM_WriteUnlock( shm_buf_t * shmbuf, uint8_t *key, uint16_t len,
		uint8_t * buf, float max_val, float min_val );

/*********************************************************************//**
  See shm.c for details about how to use this function.
 */
error_t SHM_Write( shm_buf_t * shmbuf, uint8_t *key, uint16_t len,
		uint8_t * buf );

/*********************************************************************//**
  See shm.c for details about how to use this function.
 */
error_t SHM_Write_With_Limits( shm_buf_t * shmbuf, uint8_t *key, uint16_t len,
		uint8_t * buf , float max_value, float min_value);

/*********************************************************************//**
  See shm.c for details about how to use this function.
 */
error_t SHM_Reset( shm_buf_t * shmbuf );

/*********************************************************************//**
 @}	(close g_shm group)
*//***********************************************************************/

#endif /* SHM_H_ */
