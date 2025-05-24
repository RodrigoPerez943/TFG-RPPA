#ifndef IOCTL_H_
#define IOCTL_H_

/************************************************************************
	INCLUDES
************************************************************************/
#include <stdint.h>

/**************************************************************************
 * CONSTANTS
 *************************************************************************/

#define IOCTL_DRIVER_NAME "/dev/udmabuf0"

/**************************************************************************
 * FUNCTIONS
 *************************************************************************/
int open_ioctl_driver();
void close_ioctl_driver(int fd_ioctl);
void read_ioctl_driver(int fd_ioctl, char ioctl_number, int32_t *value);
void write_ioctl_driver(int fd_ioctl, char ioctl_number, int32_t value);

#endif /* IOCTL_H_ */
