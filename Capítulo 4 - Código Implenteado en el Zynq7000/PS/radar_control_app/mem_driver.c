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

#include "include/mem_driver.h"

int fd;

void copy_mem_driver(volatile unsigned char *dst, volatile unsigned char *src, int sz)
{
  if (sz & 63) {
    sz = (sz & -64) + 64;
  }
  asm volatile (
      "NEONCopyPLD:                          \n"
      "    VLDM %[src]!,{d0-d7}                 \n"
      "    VSTM %[dst]!,{d0-d7}                 \n"
      "    SUBS %[sz],%[sz],#0x40                 \n"
      "    BGT NEONCopyPLD                  \n"
      : [dst]"+r"(dst), [src]"+r"(src), [sz]"+r"(sz) : : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "cc", "memory");
}

int init_mem_driver(uint32_t target_mem, void *map_base, void *virt_addr)
{
  off_t target;

  fd = open("/dev/mem", O_RDWR | O_SYNC);
  target = (off_t)target_mem;

  /* Map one page */
  map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target);
  virt_addr = map_base + (target & MAP_MASK);

  return 0;
}

void close_mem_driver(void *map_base)
{
  munmap(map_base, MAP_SIZE);
  close(fd);
}
