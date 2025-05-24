#ifndef MEM_DRIVER_H_
#define MEM_DRIVER_H_

/************************************************************************
	INCLUDES
************************************************************************/
#include <stdint.h>

/**************************************************************************
 * CONSTANTS
 *************************************************************************/

#define MAP_SIZE        671088640UL
#define MAP_MASK        (MAP_SIZE - 1)

/**************************************************************************
 * FUNCTIONS
 *************************************************************************/
int init_mem_driver(uint32_t target_mem, void *map_base, void *virt_addr);
void close_mem_driver(void *map_base);
void copy_mem_driver(volatile unsigned char *dst, volatile unsigned char *src, int sz);

#endif /* MEM_DRIVER_H_ */
