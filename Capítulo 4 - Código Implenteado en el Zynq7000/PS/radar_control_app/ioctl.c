#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <signal.h>

#include "include/u-dma-buf.h"
#include "include/ioctl.h"

int check_irq = 0;

void sig_event_handler_irq(int n, siginfo_t *info, void *unused)
{
  if (n == SIGETX) {
    check_irq = info->si_int;
    /*ioctl(fd_ioctl, IOCTL_END_CAPTURE, NULL);*/
    /*my_copy((unsigned char *)buffer, virt_addr, 20480);*/
  }
}

int open_ioctl_driver() {
  int fd_ioctl;
  struct sigaction act;

  fd_ioctl = open(IOCTL_DRIVER_NAME, O_RDWR);
  if (fd_ioctl == -1) {
    printf("ERROR: could not open\n");
    return -1;
  }

  /* install custom signal handler */
  sigemptyset(&act.sa_mask);
  act.sa_flags = (SA_SIGINFO | SA_RESTART);
  act.sa_sigaction = sig_event_handler_irq;
  sigaction(SIGETX, &act, NULL);

  return fd_ioctl;
}

void close_ioctl_driver(int fd_ioctl) {
  int result = close(fd_ioctl);

  if (result == -1) {
    printf("ERROR: could not close\n");
  }
}

void read_ioctl_driver(int fd_ioctl, char ioctl_number, int32_t *value) {
  if (ioctl(fd_ioctl, ioctl_number, (int32_t*) value) < 0) {
    printf("ERROR: could not read ioctl\n");
  }
}

void write_ioctl_driver(int fd_ioctl, char ioctl_number, int32_t value) {
  if (ioctl(fd_ioctl, ioctl_number, &value) < 0) {
    printf("ERROR: could not read ioctl\n");
  }
}
