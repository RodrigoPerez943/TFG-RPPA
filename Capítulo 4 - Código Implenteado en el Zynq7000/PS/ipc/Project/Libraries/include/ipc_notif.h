/***********************************************************************

  $Rev:: 449                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-07-19 12:01:04 +0200 #$:  Date of last commit
d
************************************************************************/

/*********************************************************************//**

	@file ipc_notif.h

	@brief IPC notification module

	@author 			Eduardo MENDEZ
	@author 			RBZ Embedded Logics
	@date 2017/03/10 	Base version

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

#ifndef IPC_NOTIF_H_
#define IPC_NOTIF_H_

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

/* Application includes */

/************************************************************************
	CONFIGURATION
************************************************************************/

#ifndef IPC_NOTIF_MULTICAST_GROUP
/** @brief Multicast group address
 *
 * 	This address is used to send multicast datagrams
 */
#define IPC_NOTIF_MULTICAST_GROUP			"226.1.1.1"
#endif

#ifndef IPC_NOTIF_MULTICAST_PORT
/** @brief Multicast port
 *
 *  Port used to send multicast datagrams
 */
#define IPC_NOTIF_MULTICAST_PORT			4321u
#endif

#ifndef IPC_NOTIF_MULTICAST_IP
/** @brief Multicast IP
 *
 *  IP used to receive multicast datagrams. By default, this
 *  address should be localhost address (127.0.0.1)
 */
#define IPC_NOTIF_MULTICAST_IP				"127.0.0.1"
#endif

#ifndef IPC_NOTIF_MSG_MAX_LEN
/** @brief Maximum length of multicast messages */
#define IPC_NOTIF_MSG_MAX_LEN				512u
#endif

#ifndef IPC_NOTIF_MSG_HEADER
/** @brief Header value to be added to message */
#define IPC_NOTIF_MSG_HEADER				0xAC562814
#endif

#ifndef IPC_NOTIF_MAX_NUM_FILTERS
/** @brief Maximum number of filters per connection */
#define IPC_NOTIF_MAX_NUM_FILTERS			512u
#endif

#ifndef IPC_NOTIF_SOCK_TIMEOUT_
/** @brief Socket reception timeout in seconds */
#define IPC_NOTIF_SOCK_TIMEOUT			    2u
#endif

/************************************************************************
	DECLARATIONS
************************************************************************/

/** @name Error messages
 *  @{
 */
#define IPC_NOTIF_OK						 1		/**< @brief Everything is ok */
#define IPC_NOTIF_ERROR_SOCKET				-1		/**< @brief Error opening socket */
#define IPC_NOTIF_ERROR_SOCKET_CFG			-2		/**< @brief Error configuring socket */
#define IPC_NOTIF_ERROR_SOCKET_TX			-3		/**< @brief Error in transmission */
#define IPC_NOTIF_ERROR_SOCKET_RX			-4		/**< @brief Error in reception */
#define IPC_NOTIF_ERROR_MEM_FAIL			-5		/**< @brief Memory allocation error */
#define IPC_NOTIF_ERROR_MAX_NUM_FILTERS		-6		/**< @brief Reached maximum number of filters */
#define IPC_NOTIF_ERROR_NOTIF				-7		/**< @brief Error receiving notification */
#define IPC_NOTIF_ERROR_NOT_IN_FILTER		-8		/**< @brief Notification is not in filter list */
/** @}*/

/** @brief Message structure for IPC notification messages */
typedef struct{
	uint32_t header;		/**< @brief Header indicator */
	uint32_t source;		/**< @brief Source (origin) of message */
	uint8_t key[MAX_KEY_SIZE];			    /**< @brief	Register key */
	uint8_t value[MAX_KEY_SIZE];			/**< @brief Register value */
}IPC_NOTIF_msg_t;

typedef uint8_t filter_t[MAX_KEY_SIZE];

/** @brief Connection handler for receptions */
typedef struct{
	int socket;				/**< @brief Socket identifier */
	filter_t *filter;		/**< @brief Pointer to filter structure */
	uint16_t num_filters;	/**< @brief Current number of filter */
}IPC_NOTIF_conn_t;

/************************************************************************
	FUNCTIONS
************************************************************************/

/*********************************************************************//**
  See ipc_notif.c for details about how to use this function.
 */
error_t IPC_NOTIF_Init( char * ip, uint32_t source_addr );

/*********************************************************************//**
  See ipc_notif.c for details about how to use this function.
 */
void IPC_NOTIF_Close( void );

/*********************************************************************//**
  See ipc_notif.c for details about how to use this function.
 */
error_t IPC_NOTIF_CreateConnection(IPC_NOTIF_conn_t * ptr_conn, char * ip);

/*********************************************************************//**
  See ipc_notif.c for details about how to use this function.
 */
error_t IPC_NOTIF_AddFilter(IPC_NOTIF_conn_t * ptr_conn, uint8_t *filter_reg);

/*********************************************************************//**
  See ipc_notif.c for details about how to use this function.
 */
error_t IPC_NOTIF_Read(IPC_NOTIF_conn_t * ptr_conn, IPC_NOTIF_msg_t * ptr_msg);

/*********************************************************************//**
  See ipc_notif.c for details about how to use this function.
 */
error_t IPC_NOTIF_Tx(uint8_t *key, uint8_t *value);

/*********************************************************************//**
  See ipc_notif.c for details about how to use this function.
 */
void IPC_NOTIF_CloseRx( IPC_NOTIF_conn_t * ptr_conn );

/*********************************************************************//**
 @}	(close g_ipc_notif group)
*//***********************************************************************/

#endif /* IPC_NOTIF_H_ */
