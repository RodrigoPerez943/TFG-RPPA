#ifndef __U_DMA_BUF_DEFINE_H__
#define __U_DMA_BUF_DEFINE_H__

#define IOCTL_DMA_BUF_BASE              'W'
#define IOCTL_INIT_CAPTURE              _IO(IOCTL_DMA_BUF_BASE, 1)
#define IOCTL_END_CAPTURE               _IO(IOCTL_DMA_BUF_BASE, 2) 
#define IOCTL_CURRENT_TASK              _IOW(IOCTL_DMA_BUF_BASE, 3, int32_t*)
#define IOCTL_GET_PHYS_ADDR             _IO(IOCTL_DMA_BUF_BASE, 4)
#define IOCTL_ARM_CAPTURE               _IO(IOCTL_DMA_BUF_BASE, 5)
#define IOCTL_CONFIG_SAMPLES_NUMBER     _IOR(IOCTL_DMA_BUF_BASE, 6, int32_t*)

#define SIGETX 44

#define MAP_SIZE                                671088640UL
#define MAP_MASK                                (MAP_SIZE - 1)

#endif  // __U_DMA_BUF_DEFINE_H__
