/***********************************************************************

  $Rev:: 449                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-07-19 12:01:04 +0200 #$:  Date of last commit

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
#include "shm.h"
#include "regs_ipc.h"
#include "ipc_notif.h"

/* Application includes */
#include "trace.h"

/* Specific includes */
#include "ipc.h"

/************************************************************************
	DECLARATIONS
************************************************************************/


/************************************************************************
	FUNCTIONS
************************************************************************/

/* Local functions */

/*********************************************************************//**
 * 	@brief Initialize the IPC interface
 *
 * 	This function initializes the shared memory region used to communicate
 * 	the daemon to the user application
 *
 *	@param ip IP address. In case "0" IP used is default
 *	@param source_addr Source address for IPC notifications of calling application
 *
 * 	@return Result of the operation
 */
error_t IPC_Init( char * ip, uint32_t source_addr ){
	error_t res = ERROR_OK;

	if (strcmp(ip, "0") == 0){
		TRACE_Info("Setting default IP address for notifications");
		ip = 0;
	}

	/* Connect to IPC interface */
	if (res >= ERROR_OK){
		res = REGS_IPC_Connect();
	}

	/* Initialize notification module */
	if (res >= ERROR_OK){
		res = IPC_NOTIF_Init(ip, source_addr);
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Init function for master
 *
 * 	This function is intended to be called by the IPC master (app which
 * 	creates the SHM region, daemon, etc.)
 *
 * 	@return Result of the operation
 */
error_t IPC_InitMaster( void ){
	error_t res = ERROR_OK;

	res = REGS_IPC_Init();
    
    if(res == ERROR_OK)
    {
      res = REGS_IPC_Default();
    }

	return res;
}

/*********************************************************************//**
 * 	@brief Close notification module
 *
 *	@warning This function should be called for every reception socket
 *
 *	@param ptr_conn Pointer to connection to be closed
 */
void IPC_Close( IPC_NOTIF_conn_t * ptr_conn ){

	IPC_NOTIF_Close();
	IPC_NOTIF_CloseRx(ptr_conn);
}

/*********************************************************************//**
 * 	@brief Write into IPC interface
 *
 * 	@param key Key of the message to be written
 * 	@param value Value to be written into key
 *
 * 	@return Result of the operation
 */
error_t IPC_Write( uint8_t *key, uint8_t *value){
	error_t res = ERROR_OK;

	/* Write value into IPC */
    res = REGS_IPC_Write(key, value);
	if (res >= ERROR_OK){
		/* Send notification to other applications */
		res = IPC_NOTIF_Tx(key, value);
        res = REGS_IPC_Read(key, value);
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Read from IPC interface
 *
 * 	@param reg Register address to be read
 * 	@param value Pointer used to return value
 *
 * 	@return Result of the operation
 */
error_t IPC_Read( uint8_t *key, uint8_t *value ){
	error_t res = ERROR_OK;

	/* Read value from IPC */
	res = REGS_IPC_Read(key, value);

	return res;
}

/*********************************************************************//**
 * 	@brief Create a connection handler for reception of notifications
 *
 * 	@param ptr_conn Pointer used to return connection handler
 *	@param ip Ip address. Null to use default value
 *
 * 	@return Result of the operation
 */
error_t IPC_Create(IPC_NOTIF_conn_t * ptr_conn, char * ip){
	error_t res = IPC_OK;

	if (strcmp(ip, "0") == 0){
		TRACE_Info("Setting default IP address for notifications");
		ip = 0;
	}

	res = IPC_NOTIF_CreateConnection(ptr_conn, ip);

	return res;
}

/*********************************************************************//**
 * 	@brief Add filter to current connection
 *
 * 	@param ptr_conn Pointer used to return connection handler
 *
 * 	@return Result of the operation
 */
error_t IPC_AddFilter(IPC_NOTIF_conn_t * ptr_conn, uint8_t *filter_reg){
	error_t res = IPC_OK;

	res = IPC_NOTIF_AddFilter(ptr_conn, filter_reg);

	return res;
}

/*********************************************************************//**
 * 	@brief Read notification
 *
 *	@warning This is a blocking function
 *
 * 	@param ptr_conn Pointer used to return connection handler
 * 	@param ptr_msg Pointer used to return message
 *
 * 	@return Result of the operation
 */
error_t IPC_ReadNotif(IPC_NOTIF_conn_t * ptr_conn, IPC_NOTIF_msg_t * ptr_msg){
	error_t res = IPC_OK;
	res = IPC_NOTIF_Read(ptr_conn, ptr_msg);

	return res;
}
