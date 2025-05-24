/***********************************************************************
  $Revision: 1062 $: Revision of last commit
  $Author: mcalvo $: Author of last commit
  $Date: 2018-03-20 10:01:15 +0100 (mar 20 de mar de 2018) $: Date of last commit
 *************************************************************************/

/*********************************************************************//**
@file ipc_client.h 

@brief Header of ipc_client.c file

@author Marina CALVO
@author RBZ Embedded Logics
@date 2017/03/10 Base version

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
*************************************************************************/

#ifndef __IPC_CLIENT_H__
#define __IPC_CLIENT_H__

/************************************************************************
  DECLARATIONS	
 ************************************************************************/
#define SOURCE_ADDR     19
#define IP_ADDR         "127.0.0.1"


/************************************************************************
  FUNCTIONS	
 ************************************************************************/
int ipc_client_start();
int ipc_client_write_register(uint8_t *reg, uint8_t *value);
int ipc_client_read_register(uint8_t *reg, uint8_t *value);

#endif
