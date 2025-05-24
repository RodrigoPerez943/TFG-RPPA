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
#include <pthread.h>
#include <dirent.h>
#include "include/radar_control_app.h"
#include "include/u-dma-buf.h"
#include "include/adc.h"
#include "include/ipc_client.h"
#include "include/radar_init_config.h"
#include "include/string_aux.h"
#include <math.h> //rem
#include <sys/time.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <decoder_reg.h>



#define IOCTL_DRIVER_NAME "/dev/udmabuf0"

#define SN_LEN 20

pthread_t start_measure_thread;

static Radar_Config_t *radar_config;

static uint32_t *map_base = NULL;
static uint32_t *virt_addr = NULL;
static uint32_t target_mem;

static int check = 0;

static int fd_ioctl;
static int fd_mem;

static void open_driver(const char *driver_name);
static void close_driver(const char *driver_name);

static int init_mem()
{
  off_t target;

  fd_mem = open("/dev/mem", O_RDWR | O_SYNC);
  target = (off_t)target_mem;

  /* Map one page */
  map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mem, target);
  virt_addr = map_base;

  return 0;
}

static int close_mem()
{
  munmap(map_base, MAP_SIZE);
  close(fd_mem);
  return 0;
};

/* Function for opening resources */ 
static void open_driver(const char *driver_name)
{
  fd_ioctl = open(driver_name, O_RDWR);
  if (fd_ioctl == -1)
  {
    printf("ERROR: could not open \"%s\".\n", driver_name);
    printf("    errno = %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return;
}

/* Function for safely unitiliaze resoueces */ 
static void close_driver(const char *driver_name)
{
  int result = close(fd_ioctl);

  if (result == -1)
  {
    printf("ERROR: could not close \"%s\".\n", driver_name);
    printf("    errno = %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

/* Signal event handler for knowing when the samples are ready to be copied. */
static void sig_event_handler(int n, siginfo_t *info, void *unused)
{
  if (n == SIGETX)
  {
    check = info->si_int;
    printf("Termino la captura\n");
    ioctl(fd_ioctl, IOCTL_END_CAPTURE, NULL);
  }
}


/*
 * Set samples number per channel
 */
int set_samples_number(uint32_t new_samples_number)
{
  open_driver(IOCTL_DRIVER_NAME);
  int ret = 0;

  if (new_samples_number > MAX_SAMPLES_NUMBER || new_samples_number < 1)
  {
    printf("[WARN]: Value out of range. Setting min value (%d)\n", MIN_SAMPLES_NUMBER);
    new_samples_number = MIN_SAMPLES_NUMBER;
  }
  if ( (new_samples_number % 16) != 0) 
  {
    new_samples_number = (new_samples_number + 15) & ~15;
    printf("[WARN]: Not 16 multiple value. Forcing value to %d\n",new_samples_number);
  }
  ret = ioctl(fd_ioctl, IOCTL_CONFIG_SAMPLES_NUMBER, &new_samples_number);
  

  if (ret < 0)
  {
    perror("Error ioctl IOCTL_CONFIG_SAMPLES_NUMBER");
    exit(EXIT_FAILURE);
  }
  close_driver(IOCTL_DRIVER_NAME);
  return ret;
}

/*
 * Setup DMA IRQ and registers a Linux Signal for it. 
 */

static int setup_devices(struct sigaction *act, uint32_t samples_num, int32_t *task_number)
{
  /* install custom signal handler */
  sigemptyset(&act->sa_mask);  
  act->sa_flags = (SA_SIGINFO | SA_RESTART);
  act->sa_sigaction = sig_event_handler;
  sigaction(SIGETX, act, NULL);

#ifdef DEBUG
  printf("%s: samples_number: %d\n", __func__, samples_num);
#endif

  open_driver(IOCTL_DRIVER_NAME);

  if (ioctl(fd_ioctl, IOCTL_CURRENT_TASK, (int32_t *)&task_number) < 0)
  {
    perror("Error ioctl IOCTL_CURRENT_TASK");
    exit(EXIT_FAILURE);
  }

  if (ioctl(fd_ioctl, IOCTL_CONFIG_SAMPLES_NUMBER, &samples_num) < 0)
  {
    perror("Error ioctl IOCTL_CONFIG_SAMPLES_NUMBER");
    exit(EXIT_FAILURE);
  }

  if (ioctl(fd_ioctl, IOCTL_GET_PHYS_ADDR, &target_mem) < 0)
  {
    perror("Error ioctl IOCTL_GET_PHYS_ADDR");
    exit(EXIT_FAILURE);
  }

  printf("target_mem: 0x%x\n", target_mem);
  fd_mem = init_mem();


  return 0;
}


/*
 * Function for starting to store samples in FPGA
 */
static void init_capture()
{
  ioctl(fd_ioctl, IOCTL_INIT_CAPTURE, NULL);

  while (check == 0)
  {
    usleep(10); // 10
  }
  check = 0;

}

/* Thread function for capture samples */
void *start_measure(void)
{
  FILE *fp_raw_data;
  struct sigaction act;
  int32_t number;
  char* filename = "/home/root/samples_raw.txt";
  uint32_t samples_number = radar_config->samples_number;
  int measures_number = 1;

  setup_devices(&act, samples_number, &number);

  usleep(1000);

  fp_raw_data = fopen(filename, "w+");
  if (fp_raw_data == NULL)
  {
    perror("Could not store samples");
  }
  radar_config->status = MEASURING;

  printf("Starting %d measures\n", measures_number);

  usleep(1000U);

  for (int n = 0; n < measures_number; n++)
  {
    //Samples capture
    init_capture();
   
    //Samples storage. samples_raw.txt
    for (unsigned int i = 0; i < samples_number*2; i++)
    {
      fprintf(fp_raw_data, "0x%08x \n", virt_addr[i]);
    }

  }

    //Samples processing functions call.

  printf("Samples ready to process\n");
 
  fclose(fp_raw_data);
  radar_config->status = NEW_RESULTS_AVAIL;

  close_driver(IOCTL_DRIVER_NAME);
  close_mem();

  return EXIT_SUCCESS;
}

void get_device_params(char *params)
{
  char device_sn[20] = "RADAR_CONTROL_DEV";

  snprintf(params, 256, "|SN=%s;|", device_sn);

  return;
}

void sigterm_handler(int signum)
{
  if (signum == SIGTERM || signum == SIGINT || signum == SIGKILL)
  {
    printf("Closing radar_control_app safely.\n");

    if (fcntl(fd_ioctl, F_GETFD) != -1)
      close_driver(IOCTL_DRIVER_NAME);

    if (virt_addr != NULL)
      close_mem();

    exit(EXIT_SUCCESS);
  }
}


int main()
{
  static Radar_Config_t radar_conf;
  
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = sigterm_handler;
  sigaction(SIGTERM, &action, NULL);
  sigaction(SIGKILL, &action, NULL);
  sigaction(SIGINT, &action, NULL);

  radar_config = &radar_conf;
  // Radar Init Config
  radar_config->int_freq = INTERMEDIATE_FREQUENCY_HZ;

  radar_config->status = READY;
  ipc_client_start(radar_config);

  return 0;
}
