/***********************************************************************

  $Rev:: 449                         $:  Revision of last commit
  $Author:: emendez                  $:  Author of last commit
  $Date:: 2017-07-19 12:01:04 +0200 #$:  Date of last commit

************************************************************************/

/*********************************************************************//**

	@file ipc_notif.c

	@brief IPC notification module

	The IPC notification module allows applications to receive notifications
	when some other application changes values in the IPC registers

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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"

/* Drivers includes */

/* Library includes */
#include "regs_ipc.h"

/* Application includes */
#include "trace.h"

/* Specific includes */
#include "ipc_notif.h"

/************************************************************************
	DECLARATIONS
************************************************************************/
/*#define DEBUG*/

/* Socket handlers */

/** @brief Local interface for outbound multicast datagrams */
static struct in_addr localInterface;

/** @brief Multicast transmission socket */
static struct sockaddr_in groupSock;

/** @brief Multicast reception socket */
static struct sockaddr_in localSock;

static int sd_tx;			/**< @brief Transmission socket */

/** @brief Source address of calling application
 *
 * 	Source address is used to identify which is the application who is sending
 * 	notifications, and used also to avoid reception of own notifications
 */
static uint32_t IPC_NOTIF_source_addr = 0;

/************************************************************************
	FUNCTIONS
************************************************************************/

/* Local functions */
static error_t IPC_NOTIF_InitTx( char * ip );
static error_t IPC_NOTIF_InitRx( int * sd_rx, char * ip );
static void IPC_NOTIF_CloseTx( void );
static error_t IPC_NOTIF_RcvMsg(int sd_rx, uint8_t * data, uint16_t len);
static error_t IPC_NOTIF_SendMsg(uint8_t * data, uint16_t len);
static error_t IPC_NOTIF_Filter(IPC_NOTIF_conn_t * ptr_conn, IPC_NOTIF_msg_t * ptr_msg);

/*********************************************************************//**
 * 	@brief Initialize the IPC notification module
 *
 *	@param ip IP address. NULL to use default value
 *	@param source_addr Source address of calling application
 *
 * 	@return Result of the operation
 */
error_t IPC_NOTIF_Init( char * ip, uint32_t source_addr ){
	error_t res = IPC_NOTIF_OK;

	/* Set source address */
	IPC_NOTIF_source_addr = source_addr;

	if (res >= ERROR_OK){
		res = IPC_NOTIF_InitTx(ip);
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Close notification module
 *
 * 	This function should be called when application exit
 */
void IPC_NOTIF_Close( void ){

	IPC_NOTIF_CloseTx();

}

/*********************************************************************//**
 * 	@brief Initialize the IPC notification transmission socket
 *
 *	@param ip IP address. NULL to use default value
 *
 * 	@return Result of the operation
 */
static error_t IPC_NOTIF_InitTx( char * ip ){
	error_t res = IPC_NOTIF_OK;

	/* Create a datagram socket on which to send */
	sd_tx = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd_tx < 0)
	{
		TRACE_Err("Opening datagram socket error");
		res = IPC_NOTIF_ERROR_SOCKET;
	}
	else{
		TRACE_Info("Datagram TX socket opened");
	}

	if (res >= ERROR_OK){
		/* Initialize the group sockaddr structure */
		memset((char *) &groupSock, 0, sizeof(groupSock));
		groupSock.sin_family = AF_INET;
		groupSock.sin_addr.s_addr = inet_addr(IPC_NOTIF_MULTICAST_GROUP);
		groupSock.sin_port = htons(IPC_NOTIF_MULTICAST_PORT);
	}

	if (res >= ERROR_OK){
		/* Set local interface for outbound multicast datagrams.
		   The IP address specified must be associated with a local,
		   multicast capable interface. */
		if (ip == NULL){
			localInterface.s_addr = inet_addr(IPC_NOTIF_MULTICAST_IP);
		}
		else{
			localInterface.s_addr = inet_addr(ip);
		}

		if(setsockopt(sd_tx, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
		{
			TRACE_Err("Setting local interface error");
			res = IPC_NOTIF_ERROR_SOCKET_CFG;
		}
		else{
			TRACE_Info("Datagram socket configured correctly");
		}
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Initialize the IPC notification reception socket
 *
 *	@param ip IP address. NULL to use default value
 *
 * 	@return Result of the operation
 */
static error_t IPC_NOTIF_InitRx( int * sd_rx, char * ip ){
	error_t res = IPC_NOTIF_OK;
	int reuse = 1;
	struct ip_mreq group;
	int rx_socket;

	/* Create a datagram socket on which to receive */
	rx_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (rx_socket < 0){
		TRACE_Err("Opening datagram socket error");
		res = IPC_NOTIF_ERROR_SOCKET;
	}
	else{
		TRACE_Info("Datagram RX socket opened");
	}

	if (res >= ERROR_OK){
		/* Enable SO_REUSEADDR to allow multiple instances of this socket application
		 * to receive copies of the multicast datagrams */
		if (setsockopt(rx_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0){
			TRACE_Err("Setting SO_REUSEADDR error");
			close(rx_socket);
			res = IPC_NOTIF_ERROR_SOCKET_CFG;
		}
	}

	if (res >= ERROR_OK){
		/* Bind to the proper port number with the IP address specified as INADDR_ANY. */
		memset((char *) &localSock, 0, sizeof(localSock));
		localSock.sin_family = AF_INET;
		localSock.sin_port = htons(IPC_NOTIF_MULTICAST_PORT);
		localSock.sin_addr.s_addr = INADDR_ANY;
		if(bind(rx_socket, (struct sockaddr*)&localSock, sizeof(localSock)))
		{
			TRACE_Err("Binding datagram socket error");
			close(rx_socket);
			res = IPC_NOTIF_ERROR_SOCKET_CFG;
		}
	}

	if (res >= ERROR_OK){
		/* Join multicast group on the local interface
		 *
		 * Note that this IP-ADD_MEMBERSHIP option must be called for each local interface
		 * which the multicast datagrams are to be received
		 */
		group.imr_multiaddr.s_addr = inet_addr(IPC_NOTIF_MULTICAST_GROUP);
		if (ip == NULL){
			group.imr_interface.s_addr = inet_addr(IPC_NOTIF_MULTICAST_IP);
		}
		else{
			group.imr_interface.s_addr = inet_addr(ip);
		}

		if(setsockopt(rx_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
		{
			TRACE_Err("Adding multicast group error");
			close(rx_socket);
			res = IPC_NOTIF_ERROR_SOCKET_CFG;
		}
	}

	/* Return socket handler */
	*sd_rx = rx_socket;

	return res;
}

/*********************************************************************//**
 * 	@brief Close transmission socket
 *
 */
static void IPC_NOTIF_CloseTx( void ){

	close(sd_tx);

}

/*********************************************************************//**
 * 	@brief Transmit multicast data
 *
 * 	@param data Buffer to be transmitted
 * 	@param len Length of data to be transmitted
 *
 * 	@return Result of the operation
 */
static error_t IPC_NOTIF_SendMsg(uint8_t * data, uint16_t len){
	error_t res = IPC_NOTIF_OK;

	if (res >= ERROR_OK){
		/* Send a message to the multicast group specified by the groupSock sockaddr structure. */
		if (sendto(sd_tx, data, len, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0){
			TRACE_Err("Error in socket transmission");
			res = IPC_NOTIF_ERROR_SOCKET_TX;
		}
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Create a connection handler for reception
 *
 * 	@param ptr_conn Pointer used to return connection handler
 *	@param ip Ip pointer. NULL to use default value
 *
 * 	@return Result of the operation
 */
error_t IPC_NOTIF_CreateConnection(IPC_NOTIF_conn_t * ptr_conn, char * ip){
	error_t res = IPC_NOTIF_OK;
	int socket;
	uint8_t **filter;

	if (res >= ERROR_OK){
		/* Open reception socket */
		res = IPC_NOTIF_InitRx(&socket, ip);
	}

    if (res >= ERROR_OK){
      /* Allocate memory for filters */
      filter = calloc(IPC_NOTIF_MAX_NUM_FILTERS, MAX_KEY_SIZE);

      if (filter == NULL){
        res = IPC_NOTIF_ERROR_MEM_FAIL;
      }
    }

	if (res >= ERROR_OK){
		/* Fill data */
		ptr_conn->socket = socket;
        ptr_conn->filter = (uint8_t (*)[MAX_KEY_SIZE])filter;
		ptr_conn->num_filters = 0;
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Add filter to current connection
 *
 * 	@param ptr_conn Pointer used to return connection handler
  *
 * 	@return Result of the operation
 */
error_t IPC_NOTIF_AddFilter(IPC_NOTIF_conn_t * ptr_conn, uint8_t *filter_reg){
	error_t res = IPC_NOTIF_OK;
	uint16_t num_filters = ptr_conn->num_filters;
#ifdef DEBUG
    printf("%s\n", __func__);
#endif

	if (num_filters < IPC_NOTIF_MAX_NUM_FILTERS){
#ifdef DEBUG
        printf("%s: Add filter for %s register\n", __func__, filter_reg);
#endif
        /*memcpy(ptr_conn->filter[num_filters], filter_reg, MAX_KEY_SIZE);*/
		strcpy((char *)ptr_conn->filter[num_filters], (char *)filter_reg);
		ptr_conn->num_filters = (num_filters + 1);
	}
	else{
		res = IPC_NOTIF_ERROR_MAX_NUM_FILTERS;
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Read notification
 *
 *	When a message is received, it if verified if it is in the filter list.
 *	In case filter list is empty, all notifications are received
 *
 *	Messages received from source IPC_NOTIF_source_addr are ignored
 *
 * 	@param ptr_conn Pointer used to return connection handler
 * 	@param ptr_msg Pointer used to return message
 *
 * 	@return Result of the operation
 */
error_t IPC_NOTIF_Read(IPC_NOTIF_conn_t * ptr_conn, IPC_NOTIF_msg_t * ptr_msg){
	error_t res = IPC_NOTIF_OK;
    time_t t_start, t_end, t_diff;

    t_start = time(NULL);
	for(;;){
		res = IPC_NOTIF_RcvMsg(ptr_conn->socket, (uint8_t *)ptr_msg, sizeof(IPC_NOTIF_msg_t));
        t_end = time(NULL);
		if (res > ERROR_OK){
			/* Check header */
			if (ptr_msg->header != IPC_NOTIF_MSG_HEADER){
				res = IPC_NOTIF_ERROR_NOTIF;
			}
		}
        else if(res == 0)
        {
          break;
        }


		if (res >= ERROR_OK){
          /* Verifies source is not own address */
          if (ptr_msg->source == IPC_NOTIF_source_addr){
#ifdef DEBUG
            printf("%s: message from source address\n", __func__);
#endif
            res = IPC_NOTIF_ERROR_NOTIF;
          }
          else{
				/* Filter list */
				res = IPC_NOTIF_Filter(ptr_conn, ptr_msg);
				if (res >= (error_t)ERROR_OK){
#ifdef DEBUG
                    printf("%s: message is in filter list\n", __func__);
#endif
					/* Message is in filter list: return message */
					break;
				}
                else
                {
#ifdef DEBUG
                    printf("%s: message is NOT in filter list\n", __func__);
#endif
                }
          }

		}

        t_diff = t_end - t_start;

        if(t_diff >= IPC_NOTIF_SOCK_TIMEOUT)
        {
          t_start = time(NULL);
          res = 0;
          break;
        }

	}

	return res;
}

/*********************************************************************//**
 * 	@brief Verifies if received message is in filter list
 *
 *	If message is not in filter list, it will be rejected
 *
 * 	@param ptr_conn Pointer used to return connection handler
 * 	@param ptr_msg Pointer used to return message
 *
 * 	@return Result of the operation
 */
static error_t IPC_NOTIF_Filter(IPC_NOTIF_conn_t * ptr_conn, IPC_NOTIF_msg_t * ptr_msg){
	error_t res = IPC_NOTIF_ERROR_NOT_IN_FILTER;
	uint16_t i = 0;
	uint16_t num_filters = ptr_conn->num_filters;

#ifdef DEBUG
    printf("%s: check if %s is in filter list\n", __func__, ptr_msg->key);
#endif

	if (num_filters > 0){
		for(i=0; i<num_filters; i++){
#ifdef DEBUG
            printf("%s: filter %d = %s\n", __func__, i, ptr_conn->filter[i]);
#endif
            /*if (memcmp(ptr_conn->filter[i], ptr_msg->key, strlen(ptr_msg->key)) == 0){*/
			if (strcmp((char *)ptr_conn->filter[i], (char *)ptr_msg->key) == 0){
				res = IPC_NOTIF_OK;
			}
		}
	}
	else{
#ifdef DEBUG
    printf("%s: There's no filters\n", __func__);
#endif
		res = IPC_NOTIF_OK;
	}

	return res;
}

/*********************************************************************//**
 * 	@brief Receive multicast data
 *
 *	@param sd_rx Socket identifier to read
 * 	@param data Buffer to be received
 * 	@param len Length of data to be received
 *
 * 	@return Result of the operation; When positive, number of read bytes
 */
static error_t IPC_NOTIF_RcvMsg(int sd_rx, uint8_t * data, uint16_t len){
	error_t res = IPC_NOTIF_OK;
	ssize_t len_rx = 0;
    struct timeval tv = {(int) IPC_NOTIF_SOCK_TIMEOUT, 0};
    fd_set readset;

    FD_ZERO(&readset);
    FD_SET(sd_rx, &readset);
    res = select(sd_rx + 1, &readset, NULL, NULL, &tv);

    if(res > 0)
    {
      len_rx = read(sd_rx, data, len);
      if (len_rx < 0){
        TRACE_Err("Error reading datagram message");
        close(sd_rx);
        res = IPC_NOTIF_ERROR_SOCKET_RX;
      }
      else{
        res = len_rx;
      }
    }

    FD_CLR(sd_rx, &readset);

	return res;
}

/*********************************************************************//**
 * 	@brief Transmit multicast data
 *
 * 	@param reg Register address to be written
 * 	@param value Value to be written into reg
 *
 * 	@return Result of the operation
 */
error_t IPC_NOTIF_Tx(uint8_t *key, uint8_t *value){
	error_t res = IPC_NOTIF_OK;
	IPC_NOTIF_msg_t msg;

	/* Fill message structure */
	msg.source = (uint32_t)IPC_NOTIF_source_addr;
	msg.header = (uint32_t)IPC_NOTIF_MSG_HEADER;
	memcpy(msg.key, key, MAX_KEY_SIZE);
	memcpy(msg.value, value, MAX_KEY_SIZE);

	/* Send notification */
	res = IPC_NOTIF_SendMsg((uint8_t *)&msg, sizeof(msg));

	return res;
}

/*********************************************************************//**
 * 	@brief Close reception socket
 *
 *	Close connection and free memory
 *	@param ptr_conn Pointer to connection to be closed
 */
void IPC_NOTIF_CloseRx( IPC_NOTIF_conn_t * ptr_conn ){

	close(ptr_conn->socket);
	free(ptr_conn->filter);

}
