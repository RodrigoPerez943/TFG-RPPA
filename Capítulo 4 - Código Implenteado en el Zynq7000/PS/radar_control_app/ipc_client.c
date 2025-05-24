/**
@file ipc_client.c

@brief Functions that handles IPC connection 

@author Marina CALVO
@author RBZ Embedded Logics
@date 2018/01/16 Base version

@copyright
	This software is property of RBZ Embedded Logics. Its reproduction,
	total or partial, by any means, in any form, permanent or temporary,
	is forbidden unless explicitly authorized by RBZ Embedded Logics.
@copyright
	Any adaptation, amendment, translation or transformation, as well as
	the decompiling or to disassemble of this software product, shall only
	be performed with the explicit authorization of RBZ Embedded Logics.
	The user of the present software product shall be allowed to make a
	backup copy as long as it is necessary for the utilization of the
	same.
@copyright
	The terms stated above shall be understood affecting that stated in
	applicable Laws.
*/

/************************************************************************/
/*  INCLUDES                                                            */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <poll.h>

#include "include/radar_control_app.h"
#include "include/string_aux.h"
#include <ipc.h>
#include <decoder_reg.h>
#include "include/switchs.h"
#include "include/ipc_client.h"
#include "include/radar_init_config.h"


/************************************************************************/
/* DECLARATIONS                                                         */
/************************************************************************/

static Radar_Config_t *radar_config;
extern pthread_t ipc_client_thread;
extern pthread_t start_calib_thread;

pthread_t ipc_client_rx_thread; /**< @brief Thread to receive messages from the IPC */
//#define DEBUG

/*********************************************************************//**
 *  @brief Writes a value to an IPC register 
 *
 *  This function writes a new value to an IPC register passed as an argument. 
 *
 *  @param reg Register to be written
 *  @param value New value of the register
 *
 *  @return Returns write status
 *
 */
int ipc_client_write_register(uint8_t *reg, uint8_t *value)
{
    int ret;
    ret = IPC_Write(reg, value);
    return ret;
}

/*********************************************************************//**
 *  @brief Reads a value from an IPC register 
 *
 *  This function writes a new value to an IPC register passed as an argument. 
 *
 *  @param reg Register to be written
 *  @param value New value of the register
 *
 *  @return Returns write status
 *
 */
int ipc_client_read_register(uint8_t *reg, uint8_t *value)
{
    int ret;
    ret = IPC_Read(reg, value);
    return ret;
}

static void ipc_client_read_default_config()
{
  char ipc_reg_val[MAX_VALUE_SIZE] = {0};

  ipc_client_read_register(REG_SAMPLES_NUMBER, ipc_reg_val);
  radar_config->samples_number = atoi((char *)ipc_reg_val);
  radar_config->measures_number = 1;
}

/* 
 * Clarifying note. You will see variables declared inside switchs cases, but that has a reason.
 * This way I limit the scope variable, therefore I will have more control over it.
 */
void * ipc_client_rx(IPC_NOTIF_conn_t *ptr_notif)
{
  int res = 0;
  IPC_NOTIF_conn_t notif = *(ptr_notif);
  IPC_NOTIF_msg_t msg;
  char temp[20];

#ifdef DEBUG
  printf("%s\n", __func__);
#endif
  
  ipc_client_read_default_config();

  IPC_AddFilter(&notif, (uint8_t *)REG_START_MEASURE);
  IPC_AddFilter(&notif, (uint8_t *)REG_START_FREQ);
  IPC_AddFilter(&notif, (uint8_t *)REG_END_FREQ);
  IPC_AddFilter(&notif, (uint8_t *)REG_STEP_FREQ);
  IPC_AddFilter(&notif, (uint8_t *)REG_SAMPLES_NUMBER);
  IPC_AddFilter(&notif, (uint8_t *)REG_STATUS);


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
      /*ipc_client_refresh_watchdog();*/
    }
    else
    {
#ifdef DEBUG
      printf("%s: recibido mensaje %s\n", __func__, msg.key);
#endif
      //value = atoi((char *)msg.value);
      switchs (msg.key)
      {
        cases (REG_START_MEASURE)
          if (radar_config->status == READY || radar_config->status == NEW_RESULTS_AVAIL)
            pthread_create(&start_measure_thread, NULL, (void* )&start_measure, NULL);
          break;
        cases (REG_SAMPLES_NUMBER);
          if (radar_config->status == READY || radar_config->status == NEW_RESULTS_AVAIL){
            if ( set_samples_number(atoi((char *)msg.value)) >= 0){
              radar_config->samples_number = atoi((char *)msg.value);
            }
          }else
          {
            sprintf(temp, "%d", radar_config->samples_number);
            ipc_client_write_register(REG_SAMPLES_NUMBER, temp);
          }
          printf("NEW SAMPLES NUMBER ARRIVED! : %d \n",radar_config->samples_number);
          break;
          
          break;
        cases (REG_STATUS);
          sprintf(temp,"%d",radar_config->status);
          ipc_client_write_register(REG_STATUS, temp);
          break;
        defaults
#ifdef DEBUG
          printf("%s  = %s\n", msg.key, msg.value);
#endif
        break;
      } switchs_end;
      /*ipc_client_refresh_watchdog();*/
    }
  }
   
  return 0;
}

/**
 *  @brief Starts the conection to the IPC 
 *
 *  Configure the connetion to the IPC and create a thread for reading 
 *  changes in some registers of the IPC
 *
 *  @return Returns status of thread creation
 *
 */
int ipc_client_start(Radar_Config_t* __radar_config)
{
    uint8_t res;
    IPC_NOTIF_conn_t notif;

    radar_config = __radar_config;
#ifdef DEBUG
    printf("%s\n", __func__);
#endif

    res = IPC_Init((char *)IP_ADDR, APP_CAPTURA_ID);
    if(res < 0)
    {
        printf("ERROR: IPC_Init fail\n");
        return res;
    }

    res = IPC_Create(&notif, IP_ADDR);
    if(res < 0)
    {
      printf("ERROR: IPC_Create fail\n");
      return res;
    }
    pthread_create(&ipc_client_rx_thread, NULL, (void *)&ipc_client_rx, &notif);
    pthread_join(ipc_client_rx_thread, NULL);
    return res;
}
