/***********************************************************************

  $Rev:: 268                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-03-29 15:19:02 +0200 #$:  Date of last commit

************************************************************************/

/*********************************************************************//**

	@file project_config.h

	@brief General configuration of board

	@author 			Eduardo MENDEZ
	@author 			RBZ Embedded Logics
	@date 2015/06/19 	Base version

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

#ifndef PROJECT_CONFIG_H_
#define PROJECT_CONFIG_H_

/*********************************************************************//**
  	@defgroup g_project_config Project configuration

  	@brief Low level configuration

  	@{
 *//***********************************************************************/

/************************************************************************
	PROJECT VERSION
************************************************************************/

/** @name Project version definitions
 *  @{
 */
#define PROJECT_HEADER				"IPC"
#define PROJECT_MODEL				"EdAa"
#define PROJECT_MANUFACTURER		"EMDEP"
#define PROJECT_TIME_AND_DATE		19,7,26,0,0,0	/* 2017/01/01; 00:00:00 */

#define PROJECT_VERSION				0x0001			/* v0.01 */
/** @}*/

/************************************************************************
	DEBUG OPTIONS
************************************************************************/

/** @name Options for debug purpose
 *  @{
 */
#ifdef DEBUG_MODE
	#define TRACE_MODE			1		/**< Enable traces */
#else
	#define TRACE_MODE			0		/**< Disable traces */
#endif
/** @}*/

/************************************************************************
	SHARED MEMORY CONFIGURATION
************************************************************************/

/** @defgroup g_project_config_shm Configuration of shared memory region
 *
 * 	@brief Configuration of shared memory region
 *  @{
 */

/** @brief Name of shared memory region for GPIOs */
#define SHM_NAME_IPC			"/ipc_shm_regs"

/** @brief Name of shared memory region for WD */
#define SHM_NAME_WD				"/ipc_shm_wd"

/** @brief Indicates if SHM initializing functions are going to be compiled */
#define SHM_MASTER				(TRUE)

/** @brief Number of registers to implement in shared memory region */
#define REGS_IPC_NUM_REGS		512u

/** @brief Length of registers in shared memory region (bytes) */
#define REGS_IPC_LEN_REG		256u		/* (bytes) */

/** @brief Reserved size for shared memory region */
#define SHM_LEN					96000u

/** @}*/

/*********************************************************************//**
 @}	(close g_project_config group)
*//***********************************************************************/

#endif /* PROJECT_CONFIG_H_ */
