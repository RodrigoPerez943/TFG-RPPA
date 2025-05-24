#ifndef __REMOTE_VNAHANDLER_APP_H_
#define __REMOTE_VNAHANDLER_APP_H_

/************************************************************************
	INCLUDES
************************************************************************/
#include <stdint.h>
#include <pthread.h>
#include "include/radar_init_config.h"
/**************************************************************************
 * CONSTANTS
 *************************************************************************/

extern pthread_t start_measure_thread;
/**************************************************************************
 * FUNCTIONS
 *************************************************************************/

void * start_measure(void);
int set_samples_number(uint32_t new_samples_number);

int set_ip_adresss(char* new_ip);
void get_ip_address(char* result);

#endif /* __REMOTE_VNAHANDLER_APP_H_ */
