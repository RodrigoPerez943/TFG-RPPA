/***********************************************************************

  $Rev:: 323                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-04-20 10:09:00 +0200 #$:  Date of last commit

************************************************************************/

/*********************************************************************//**

	@file shm.c

	@brief Shared memory region

	@author 			Eduardo MENDEZ
	@author 			Marina CALVO
	@author 			RBZ Embedded Logics
	@date 2015/06/22 	Base version
	@date 2019/07/22 	Remove registers and use key - value system

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

/* Standard includes */
#include <argz.h>
#include <unistd.h>

/* Platform includes */
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include "common.h"

/* Drivers includes */

/* Library includes */

/* Application includes */

/* Specific includes */
#include "shm.h"

/************************************************************************
	DECLARATIONS
************************************************************************/


/************************************************************************
	FUNCTIONS
************************************************************************/

/* Local functions */

/*********************************************************************//**
 * 	@brief Initialize the shared memory region
 *
 * 	This function initializes the shared memory region used to communicate
 * 	the daemon to the user application
 *
 *	@param shm_handler Pointer used to return the SHM buffer handler
 *	@param shm_name String with name of SHM memory
 *	@param len Length of shared memory region
 *
 * 	@return Result of the operation
 */
error_t SHM_Init(shm_buf_t ** shm_handler, char * shm_name){
	error_t res = ERROR_OK;
	int fd;
	shm_buf_t * shmbuf = NULL;

	/* Delete previous instance of SHM object, if it exists */
	shm_unlink(shm_name);

	/* Create an open SHM object */
	mode_t old_umask = umask(0);
	fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH | S_IWOTH);
	if (fd < ERROR_OK){
		res = SHM_ERROR_CREATION;
	}
	umask(old_umask);

	/* Adjust size of object to match structure */
	if (res >= ERROR_OK){
		if (ftruncate(fd, sizeof(shm_buf_t)) < ERROR_OK){
			res = SHM_ERROR_CREATION;
		}
	}

	/* Map object */
	if (res >= ERROR_OK){
		shmbuf = mmap(NULL, sizeof(shm_buf_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (shmbuf == MAP_FAILED){
			res = SHM_ERROR_CREATION;
		}
	}

	/* Initialize semaphore */
	if (res >= ERROR_OK){
		res = sem_init(&shmbuf->sem, 1, 1);
		if (res < ERROR_OK){
			res = SHM_ERROR_SEM;
		}
	}

	/* In case of error, return NULL */
	if (res >= ERROR_OK){
		*shm_handler = shmbuf;
	}
	else{
		*shm_handler = NULL;
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Connect to an already initialized shared memory region
 *
 * 	This function is intended to stablish a connection to a shared memory
 * 	region created since other thread by using @ref SHM_Init
 *
 *	@param shm_handler Pointer used to return the SHM buffer handler
 *	@param shm_name String with name of SHM memory
 *
 * 	@return Result of the operation
 */
error_t SHM_Connect( shm_buf_t ** shm_handler, char * shm_name ){
	error_t res = ERROR_OK;
	int fd;
	shm_buf_t * shmbuf = NULL;

	/* Open SHM object */
	fd = shm_open(shm_name, O_RDWR, 0);
	if (fd < ERROR_OK){
		res = SHM_ERROR_CREATION;
	}

	/* Map object */
	if (res >= ERROR_OK){
		shmbuf = mmap(NULL, sizeof(shm_buf_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (shmbuf == MAP_FAILED){
			res = SHM_ERROR_CREATION;
		}
	}

	/* In case of error, return NULL */
	if (res >= ERROR_OK){
		*shm_handler = shmbuf;
	}
	else{
		*shm_handler = NULL;
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Unlink and un-map the shared memory region
 *
 *  @param shmbuf Pointer to SHM buffer handler
 */
void SHM_Remove( error_t SHM_Connect, shm_buf_t * shmbuf ){

	munmap(shmbuf, sizeof(shm_buf_t));
	shm_unlink(SHM_NAME);
}

/*********************************************************************//**
 * 	@brief Lock memory region
 *
 * 	Reserve a semaphore in order to prevent other processes to access
 * 	the memory region while this application is working with
 *
 * 	@param shmbuf Pointer to SHM buffer handler
 *
 *	@return Result of the operation
 */
error_t SHM_Lock( shm_buf_t * shmbuf ){
	error_t res = ERROR_OK;
	struct timespec timeout;
	int res_sem;

	/* Define timeout */
	timeout.tv_sec = 0;
	timeout.tv_nsec = ((SHM_TIMEOUT) * (MS_TO_NS));

	/* Wait semaphore */
	res_sem = sem_timedwait(&shmbuf->sem, &timeout);
	if (res_sem < 0){
		/* Release semaphore */
		sem_post(&shmbuf->sem);

		res = SHM_ERROR_LOCKING;
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Unlock memory region
 *
 * 	Release semaphore in order to unlock the memory region
 *
 * 	@param shmbuf Pointer to SHM buffer handler
 */
void SHM_Unlock( shm_buf_t * shmbuf ){

	/* Release semaphore */
	sem_post(&shmbuf->sem);

}

/*********************************************************************//**
 * 	@brief Read a data buffer from the shared memory region (unprotected
 * 	access)
 *
 * 	This function copies into destination buffer a data section from the
 * 	shared memory region. Access is protected by semaphores.
 *
 *	@warning SHM resource should be locked before any attempt to access it.
 *	This is done by calling @ref SHM_Lock and @ref SHM_Unlock
 *
 * 	@param shmbuf Pointer to SHM buffer handler
 *	@param base Relative base address in memory region to read from
 *	@param len Length of section to be read
 *	@param buf Pointer used to return the data
 *
 *	@return Result of the operation
 */
error_t SHM_ReadUnlock( shm_buf_t * shmbuf, uint8_t *key, uint16_t len,
		uint8_t * buf, float *max_value, float *min_value){
	int res;
	uint16_t len_effective = len;
	uint8_t * ptr_shmbuf;
    uint32_t base = 0;
    uint16_t i = 0;
    uint8_t done = 0;
    size_t key_len = strlen((char *)key) + 1;

	/* Read from shared memory region */
	ptr_shmbuf = (uint8_t *)shmbuf->buffer;
    for(i = 0; i < REGS_IPC_NUM_REGS; i++)
    {
      if (memcmp(key, (void *)&ptr_shmbuf[base], key_len) == 0)
      {
        done = 1;
        break;
      }
      base += SHM_POSITION_SIZE;
    }

    if(done == 1)
    {
      memcpy((void *)buf, (void *)&ptr_shmbuf[base + REGS_IPC_LEN_REG], len_effective);
      memcpy((void *)max_value, (void *)&ptr_shmbuf[base + REGS_IPC_LEN_REG + REGS_IPC_LEN_REG], sizeof(float));
      memcpy((void *)min_value, (void *)&ptr_shmbuf[base + REGS_IPC_LEN_REG + REGS_IPC_LEN_REG + sizeof(float)], sizeof(float));
      res = len_effective;
    }
    else
    {
      memset((void *)buf, 0, len_effective);
      memset((void *)max_value, 0, sizeof(max_value));
      memset((void *)min_value, 0, sizeof(min_value));
      res = len_effective;
      
    }

	return res;
}

/*********************************************************************//**
 * 	@brief Read a data buffer from the shared memory region
 *
 * 	This function copies into destination buffer a data section from the
 * 	shared memory region. Access is protected by semaphores.
 *
 * 	@param shmbuf Pointer to SHM buffer handler
 *	@param base Relative base address in memory region to read from
 *	@param len Length of section to be read
 *	@param buf Pointer used to return the data
 *
 *	@return Result of the operation
 */
error_t SHM_Read( shm_buf_t * shmbuf, uint8_t *key, uint16_t len,
		uint8_t * buf, float *max_val, float *min_val){
	error_t res;

	/* Get resource */
	res = SHM_Lock(shmbuf);
	if (res >= ERROR_OK){
		res = SHM_ReadUnlock(shmbuf, key, len, buf, max_val, min_val);

		/* Release resource */
		SHM_Unlock(shmbuf);
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Write a data buffer into the shared memory region (unprotected
 * 	access)
 *
 * 	This function copies into shared memory region the data content of
 * 	@ref buf
 *
 *	@warning SHM resource should be locked before any attempt to access it.
 *	This is done by calling @ref SHM_Lock and @ref SHM_Unlock
 *
 * 	@param shmbuf Pointer to SHM buffer handler
 *	@param base Relative base address in memory region to write into
 *	@param len Length of section to be read
 *	@param buf Pointer to buffer to be copied into shared memory region
 *
 *	@return Result of the operation
 */
error_t SHM_WriteUnlock( shm_buf_t * shmbuf, uint8_t *key, uint16_t len,
		uint8_t * buf, float max_val, float min_val){
	uint16_t res = -1;
	uint16_t len_effective = len;
	uint8_t * ptr_shmbuf;
    uint32_t base = 0;
    uint16_t i = 0;
    uint8_t zero_str[REGS_IPC_LEN_REG] = {0};
    uint8_t done = 0;
    size_t key_len = strlen((char *)key) + 1;

	/* Write into shared memory region */
	ptr_shmbuf = (uint8_t *)shmbuf->buffer;

    for(i = 0; i < REGS_IPC_NUM_REGS; i++)
    {
      if (memcmp(key, (void *)&ptr_shmbuf[base], key_len) == 0)
      {
        done = 1;
        break;
      }
      else if(memcmp(zero_str, (void *)&ptr_shmbuf[base], REGS_IPC_LEN_REG) == 0)
      {
        memcpy((void *)&ptr_shmbuf[base], (void *)key, key_len);
        memcpy((void *)&ptr_shmbuf[base + REGS_IPC_LEN_REG + REGS_IPC_LEN_REG], (void *)&max_val, sizeof(float));
        memcpy((void *)&ptr_shmbuf[base + REGS_IPC_LEN_REG + REGS_IPC_LEN_REG + sizeof(float)], (void *)&min_val, sizeof(float));
        done = 1;
        break;
      }
      base += SHM_POSITION_SIZE;
    }

    if (done == 1)
    {
      memcpy((void *)&ptr_shmbuf[base + REGS_IPC_LEN_REG], (void *)buf, len_effective);
      res = len_effective;
    }

	return res;
}

/*********************************************************************//**
 * 	@brief Write a data buffer into the shared memory region
 *
 * 	This function copies into shared memory region the data content of
 * 	@ref buf
 *
 *	@warning SHM resource should be locked before any attempt to access it.
 *	This is done by calling @ref SHM_Lock and @ref SHM_Unlock
 *
 * 	@param shmbuf Pointer to SHM buffer handler
 *	@param base Relative base address in memory region to write into
 *	@param len Length of section to be read
 *	@param buf Pointer to buffer to be copied into shared memory region
 *
 *	@return Result of the operation
 */
error_t SHM_Write( shm_buf_t * shmbuf, uint8_t *key, uint16_t len,
		uint8_t * buf ){
	uint16_t res;

	/* Get resource */
	res = SHM_Lock(shmbuf);
	if (res >= ERROR_OK){
		res = SHM_WriteUnlock(shmbuf, key, len, buf, 0, 0);

		/* Release resource */
		SHM_Unlock(shmbuf);
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Write a data buffer and the default values into the 
 * 	shared memory region
 *
 * 	This function copies into shared memory region the data content of
 * 	@ref buf. It also copies the maximum and minimum values of @ref key
 *
 *	@warning SHM resource should be locked before any attempt to access it.
 *	This is done by calling @ref SHM_Lock and @ref SHM_Unlock
 *
 * 	@param shmbuf Pointer to SHM buffer handler
 *	@param key Key to search on the memory region to write into
 *	@param len Length of section to be read
 *	@param buf Pointer to buffer to be copied into shared memory region
 *	@param max_value Maximum value of the key
 *	@param min_value Minimum value of the key
 *
 *	@return Result of the operation
 */
error_t SHM_Write_With_Limits( shm_buf_t * shmbuf, uint8_t *key, uint16_t len,
		uint8_t * buf , float max_value, float min_value){
	uint16_t res;

	/* Get resource */
	res = SHM_Lock(shmbuf);
	if (res >= ERROR_OK){
		res = SHM_WriteUnlock(shmbuf, key, len, buf, max_value, min_value);

		/* Release resource */
		SHM_Unlock(shmbuf);
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Reset buffer of the shared memory 
 *
 * 	This function erase all the content of the buffer in the shared memory 
 * 	@ref buf
 *
 *	@warning SHM resource should be locked before any attempt to access it.
 *	This is done by calling @ref SHM_Lock and @ref SHM_Unlock
 *
 * 	@param shmbuf Pointer to SHM buffer handler
 *	@return Result of the operation
 */
error_t SHM_Reset( shm_buf_t * shmbuf ){
	uint16_t res;

	/* Get resource */
	res = SHM_Lock(shmbuf);
	if (res >= ERROR_OK){
        memset(shmbuf->buffer, 0, SHM_LEN);

		/* Release resource */
		SHM_Unlock(shmbuf);
	}

	return res;
}
