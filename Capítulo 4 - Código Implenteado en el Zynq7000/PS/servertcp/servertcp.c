/***********************************************************************
    $Revision: 1576 $: Revision of last commit
    $Author: mcalvo $: Author of last commit
    $Date: 2019-10-26 10:33:19 +0000 (sáb 26 de oct de 2019) $: Date of last commit
*************************************************************************/
/*********************************************************************//**

	@file servertcp.c

	@brief Application that manages tcp communication

	@author 		Carlos AMOR
	@author 		RBZ Embedded Logics
	@date 15 jun. 2017 	Base version

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

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <pthread.h>

/* Platform includes */
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>

/* Specific includes */
#include "servertcp.h"

/* Library includes */
#include <ipc.h>
#include <decoder_reg.h>
#include <color_supported_fps.h>

/************************************************************************
	DECLARATIONS
************************************************************************/

//#define DEBUG 

volatile int socketfd;
volatile int socket_childfd;
static int parentfd;
static int portno;
pthread_t ipc_client_rx_thread;

/************************************************************************
	FUNCTIONS
************************************************************************/
/* Local functions */
#if 0
/*********************************************************************//**
 * 	@brief function that check if a command is executed OK 
 *
 *	@return 0 if the command is executed correctly
 */
int checkError()
{
	uint32_t lectura = 0;
	uint32_t timeout;
	
	for(timeout=0; timeout<ERROR_TIMEOUT; timeout++)
	{
		IPC_Read(REG_ERROR_EXEC, &lectura);
		usleep(ERROR_SLEEP);
		if(lectura == RESULT_OK || lectura == RESULT_ERROR)
			break;
	}

	IPC_Write(REG_ERROR_EXEC, 0);

	if(lectura == RESULT_OK)
	{
		return 0;
	}
	else
	{
		return ERROR;
	}
}
#endif

static void remove_new_line(char *s) {
  while (*s) {
    if (*s == '\n') {
      *s='\0';
    }
    s++;
  }
}

/*********************************************************************//**
 * 	@brief Manage input commands 
 *
 *	@param entrada String with input commands
 *	       socket socket file descriptor
 */
void manejaComandos(char *entrada, int socket)
{
	char buf[BUFSIZE] = "";
	char *pch = NULL;
	char *comando = NULL;
	char *value1Str={0};
	char cmd[CMD_SIZE];
    char tmp_buf[MAX_VALUE_SIZE];
	int result = 0, i = 0;
    char value[MAX_VALUE_SIZE] = "";;
    char zero[MAX_VALUE_SIZE] = "0";;
    uint32_t reg_table_size = sizeof(registerOfIPC) / sizeof (regIPC);
	char *filename = NULL;
	FILE *file = NULL;
	char line[LINE_SIZE] = "";
    uint32_t config_set = 0;
    uint32_t timeout = 15;
    int ret = 0;
	
#ifdef DEBUG
    printf("%s\n", __func__);
    printf("%s: entrada = *%s*\n", __func__, entrada);
#endif
	pch = strchr(entrada, '=');
    if (strncmp(entrada, "GET_ACTUAL_CONFIG", 17) == 0)
    {
      for (i = 0; i < reg_table_size; i++)
      {
        bzero(buf, sizeof(buf));
        bzero(value, sizeof(value));
        IPC_Read((uint8_t *)registerOfIPC[i].reg_str, (uint8_t *)&value);
        sprintf(buf, "<=%s=%s\n", registerOfIPC[i].reg_str, value);
        ret = write(socket, buf, BUFSIZE);
      } 

      bzero(buf, sizeof(buf));
      sprintf(buf, "<=====END\r\n");
      ret = write(socket, buf, BUFSIZE);
    }
    else if (pch != NULL){
		comando = strtok(entrada, "=");
		strcpy(cmd, comando);
        IPC_Read(cmd, value);
        if (strlen(value) <= 0)
        {
          bzero(buf, BUFSIZE);
          sprintf(buf, "<=%s=UNKNOWN_COMMAND\r\n", cmd);
          ret = write(socket, buf, BUFSIZE);
          return;
        }
        else
        {
          value1Str = strtok(NULL, ":");
          if (value1Str == NULL)
          {
            sprintf(buf, "<=INVALID PARAM\r\n");
            ret = write(socket, buf, BUFSIZE);
            return;
          }
          remove_new_line(cmd);
          remove_new_line(value1Str);
          if (strncmp(cmd, "GET_SAVE_CONFIG", 15) == 0)
          {
            IPC_Write("CONFIG_DONE", zero);
            config_set = atoi(value1Str);
            switch(config_set)
            {
              case READ_CONFIG:
              case DEFAULT_CONFIG:
                filename = "/home/root/configFile.conf";
                break;

              case SET1_CONFIG:
                filename = "/home/root/configFile_set1.conf";
                break;

              case SET2_CONFIG:
                filename = "/home/root/configFile_set2.conf";
                break;

              case SET3_CONFIG:
                filename = "/home/root/configFile_set3.conf";
                break; 
            }

            result = IPC_Write(cmd, value1Str);

            if (result == -1) {
              sprintf(buf, "<=%s=WRITE_ERROR\r\n", cmd);
            }
            else if (result == -2) {
              sprintf(buf, "<=%s=OUT_OF_RANGE\r\n", cmd);
            }
            else
            {
              file = fopen(filename, "r");

              if (file != NULL){                	                                        	
                while (fgets(line, sizeof(line), file)){
                  bzero(buf, sizeof(buf));
                  sprintf(buf, "<=%s", line);
                  ret = write(socket, buf, BUFSIZE);
                  /*usleep(SLEEP_100);*/
                }
                fclose(file);
              }
              else{
                bzero(buf, BUFSIZE);
                sprintf(buf, "<=NO SAVE CONFIG IN MEMORY\r\n");
                ret = write(socket, buf, BUFSIZE);
              }
              bzero(buf, BUFSIZE);
              strcpy(buf, "<=====END\r\n");
            }
            ret = write(socket, buf, BUFSIZE);
          }
          else if (strncmp(cmd, "FACTORY_RESET", 13) == 0)
          {
            IPC_Write("CONFIG_DONE", zero);
            printf("Escribiendo comando en IPC\n");
            result = IPC_Write(cmd, value1Str);
            bzero(buf, BUFSIZE);
            printf("Imprimiendo resultado\n");
            if (result == -1) {
              sprintf(buf, "<=%s=WRITE_ERROR\r\n", cmd);
            }
            else if (result == -2) {
              sprintf(buf, "<=%s=OUT_OF_RANGE\r\n", cmd);
            }
            else
            {
              sprintf(buf, "<=FACTORY_RESET=%d\r\n", 1);
            }
            printf("Volcando resultado al socket\n");
            ret = write(socket, buf, BUFSIZE);
            printf("Saliendo\n");

          }
          else if ( strncmp(cmd,"GET_DEVICE_PARAMS",17) == 0 || strncmp(cmd, "SET_IP_ADDRESS",14 ) == 0 || strncmp(cmd, "GET_IP_ADDRESS",14) == 0 )
          {
            if(strcmp(value1Str, "") == 0)
            {
              result = -1;
              sprintf(buf, "<=%s=WRITE_ERROR\r\n", cmd);
            }
            else
            {  
              result = IPC_Write(cmd, value1Str);
              bzero(buf, BUFSIZE);
              if (result == -1) {
                sprintf(buf, "<=%s=WRITE_ERROR\r\n", cmd);
              }
              else if (result == -2) {
                sprintf(buf, "<=%s=OUT_OF_RANGE\r\n", cmd);
              }
              else {
                timeout = 200;
                usleep(10000);
                memset(value, 0, MAX_VALUE_SIZE);
                while( (value[0] == '\0' || (value[0]=='1' && value[1]=='\0') && timeout>0)){
                  IPC_Read(cmd, value);
                  usleep(10000);
                  timeout--;
                }
                if(timeout == 0)
                  sprintf(value, "NO RESPONSE");
                sprintf(buf, "<=%s=%s\r\n", cmd, value);
              }
            }
            ret = write(socket, buf, BUFSIZE);
          }
          else
          {
#ifdef DEBUG
            printf("%s: *%s* = *%s*\n", __func__, cmd, value1Str);
#endif

            if(strcmp(value1Str, "") == 0)
            {
              result = -1;
              sprintf(buf, "<=%s=WRITE_ERROR\r\n", cmd);
            }
            else
            {
              memcpy(tmp_buf, value1Str, MAX_VALUE_SIZE);
              result = IPC_Write(cmd, tmp_buf);
              bzero(buf, BUFSIZE);
              if (result == -1) {
                sprintf(buf, "<=%s=WRITE_ERROR\r\n", cmd);
              }
              else if (result == -2) {
                sprintf(buf, "<=%s=OUT_OF_RANGE\r\n", cmd);
              }
              else {
                usleep(10000);
                memset(value, 0, MAX_VALUE_SIZE);
                IPC_Read(cmd, value); 
                sprintf(buf, "<=%s=%s\r\n", cmd, value);
              }
            } 
            ret = write(socket, buf, BUFSIZE);
          }
        }
	}	
	else{	
		pch = strchr(entrada, '?');
		if(pch != NULL){
			comando = strtok(entrada, "?");
			strcpy(cmd, comando);       
            memset(value, 0, MAX_VALUE_SIZE);
            
            if(strcmp(REG_STATUS,cmd)==0)
            {           
              IPC_Write((uint8_t*)REG_STATUS, zero);
              usleep(10000);
            }
            
            IPC_Read(cmd, value);
            if(strlen(value) > 0)
            {
              bzero(buf, BUFSIZE);
              sprintf(buf, "<=%s=%s\r\n", cmd, value);
            }
            else
            {
              bzero(buf, BUFSIZE);
              sprintf(buf, "<=%s=UNKNOWN_COMMAND\r\n", cmd);
            }
            ret = write(socket, buf, BUFSIZE);
		}
	}	
}

/*********************************************************************//**
 * 	@brief Manage error messages 
 *
 *	@param msg String with error message
 */
void error(const char *msg)
{	
	perror(msg);
	exit(1);
}

/**
 *  @brief Function that refresh watchdog 
 *
 *  This function refresh watchdog register 
 *
 */
void ipc_client_refresh_watchdog()
{
  uint8_t swatchdog_reg[MAX_VALUE_SIZE] = {0};
  int32_t watchdog_val = 0;

  if(portno == 4997)
  {
    IPC_Read((uint8_t *)REG_SERVERTCP_WD, swatchdog_reg);
  }
  else if(portno == 4998)
  {
    IPC_Read((uint8_t *)REG_SERVERTCP_WD_1, swatchdog_reg);
  }
  else if(portno == 4999)
  {
    IPC_Read((uint8_t *)REG_SERVERTCP_WD_2, swatchdog_reg);
  }
  else
  {
    return;
  }

  watchdog_val = atoi((char *)swatchdog_reg);

  if(watchdog_val == 0)
  {
    memset(swatchdog_reg, 0, MAX_VALUE_SIZE);
    sprintf((char *)swatchdog_reg, "1");
    if(portno == 4997)
    {
      IPC_Write((uint8_t *)REG_SERVERTCP_WD, swatchdog_reg);
    }
    else if(portno == 4998)
    {
      IPC_Write((uint8_t *)REG_SERVERTCP_WD_1, swatchdog_reg);
    }
    else if(portno == 4999)
    {
      IPC_Write((uint8_t *)REG_SERVERTCP_WD_2, swatchdog_reg);
    }
    else
    {
      return;
    }
  }

#ifdef DEBUG
  printf("Watchdog servertcp %d\n", portno);
#endif
}

void * ipc_client_rx(IPC_NOTIF_conn_t *ptr_notif)
{
  int res = 0;
  IPC_NOTIF_conn_t notif = *(ptr_notif);
  IPC_NOTIF_msg_t msg;

#ifdef DEBUG
  printf("%s\n", __func__);
#endif
  IPC_AddFilter(&notif, (uint8_t *)REG_STATUS);
  IPC_AddFilter(&notif, (uint8_t *)REG_REQ_SERVER_RESTART);
  while(1)
  {
    res = IPC_ReadNotif(&notif, &msg);
    if(res < 0)
    {
      printf("ERROR: IPC_ReadNotif fail\n");
      return 0;
    }
    else if(res == 0)
    {
      ipc_client_refresh_watchdog();
    }
    else
    {
      ipc_client_refresh_watchdog();
      if (strncmp(msg.key, "REQ_SERVER_RESTART",18)==0)
      {
        printf("%s\n",__func__);
        close(socket_childfd);
        close(socketfd);
        parentfd = OpenSocket(portno);
        socketfd = parentfd;
      }
    }
  } 
  return 0;
}

/*********************************************************************//**
 * 	@brief Initialize the IPC client
 *
 * 	@return Result of the operation
 */
int ipc_client_start()
{
  uint8_t res;
  IPC_NOTIF_conn_t notif;

  if (portno == 4997)
  {
    res = IPC_Init((char *)IP_ADDR, APP_SERVERTCP_ID);
    if(res < 0)
    {
      printf("ERROR: IPC_Init fail\n");
      return res;
    }
  }
  else if (portno == 4998)
  {
    res = IPC_Init((char *)IP_ADDR, APP_SERVERTCP_1_ID);
    if(res < 0)
    {
      printf("ERROR: IPC_Init fail\n");
      return res;
    }
  }
  else if (portno == 4999)
  {
    res = IPC_Init((char *)IP_ADDR, APP_SERVERTCP_2_ID);
    if(res < 0)
    {
      printf("ERROR: IPC_Init fail\n");
      return res;
    }
  }
  else
  {
    printf("%s: unknown port number\n", __func__);
    return -1;
  }

  res = IPC_Create(&notif, IP_ADDR);
  if(res < 0)
  {
    printf("ERROR: IPC_Create fail\n");
    return res;
  }

  pthread_create(&ipc_client_rx_thread, NULL, (void *)&ipc_client_rx, &notif);
  sleep(1);

  return res;
}

/*********************************************************************//**
 * 	@brief Function that enable and config keepalive option 
 *
 *	@param sock socket file descriptor
 */
void enable_keepalive(int sock){
	int yes = 1;
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));

	int idle = 1;
	setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int));

	int interval = 1;
	setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int));

	int maxpkt = 10;
	setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int));
}

/*********************************************************************//**
 * 	@brief Create and open server socket
 *
 *	@param port to open
 *
 * 	@return socket file descriptor
 */
int OpenSocket(int port){
	int sd;
	struct sockaddr_in addr;

	sd = socket(PF_INET, SOCK_STREAM, 0);
	if (sd < 0){
		error("ERROR opening socket");
	}
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
          error("setsockopt(SO_REUSEADDR) failed");
	enable_keepalive(sd);

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0){
		error("ERROR bind");
	}

	if (listen(sd, 1) != 0){
		error("ERROR listen");
	}
	return sd;
}

void sig_term_handler(int signum, siginfo_t *info, void *ptr)
{
  printf("Received term signal. Closing socket\n");
  close(socketfd);
  close(socket_childfd);
  exit(-1);
} 

void catch_sigterm()
{
  static struct sigaction _sigact;

  memset(&_sigact, 0, sizeof(_sigact));
  _sigact.sa_sigaction = sig_term_handler;
  _sigact.sa_flags = SA_SIGINFO;

  sigaction(SIGTERM, &_sigact, NULL);
}


/*********************************************************************//**
 * 	@brief APP main function. Open socket tcp and wait for input
 *		connections and messages 
 *
 * 	@return Result of the operation
 */
int main(int argc, char *argv[])
{
	int childfd;
    struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	char buf[BUFSIZE];
    char sipc_value[MAX_VALUE_SIZE] = {0};
	int n=1;

	if (argc != 2){
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}
	portno = atoi(argv[1]);

	if(ipc_client_start() < 0){
		error("ERROR IPC Init");
	}

    catch_sigterm();


    parentfd = OpenSocket(portno);
    socketfd = parentfd;


    memset(sipc_value, 0, MAX_VALUE_SIZE);
    sprintf(sipc_value, "%d", 0);
    IPC_Write(REG_ERROR_EXEC, sipc_value);
	while(1){
#ifdef DEBUG
		printf("Esperando conexion ...\n");
#endif
		bzero(&clientaddr, sizeof(clientaddr));
		len = sizeof(clientaddr);
		childfd = accept(parentfd, (struct sockaddr*)&clientaddr, &len);
        socket_childfd = childfd;
#ifdef DEBUG
		printf("connection to: %s\n", inet_ntoa(clientaddr.sin_addr));
#endif

		while(n != 0){
			bzero(buf, BUFSIZE);
			n = read(childfd, buf, BUFSIZE);
			if(n > 0){
#ifdef DEBUG
                printf("%s\n", buf);
#endif
                manejaComandos(buf, childfd);
			}else{
              break;
            }
		}
		close(childfd);
		n = 1;
	}
}
