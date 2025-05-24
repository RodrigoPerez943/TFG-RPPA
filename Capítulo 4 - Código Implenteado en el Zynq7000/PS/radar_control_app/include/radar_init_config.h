#ifndef RADAR_INIT_CONFIG_H
#define RADAR_INIT_CONFIG_H

#include <stdint.h>
#include <complex.h>
#include <stdbool.h>

/* Radar Default parameters */
#define N_CHANNELS         3
#define INTERMEDIATE_FREQUENCY_HZ 200000
#define NUM_CHANNELS     2
#define MAX_SAMPLES_NUMBER 40000000
#define MIN_SAMPLES_NUMBER 16

#define I_SAMPLES_OFFSET 0
#define Q_SAMPLES_OFFSET 1
#define COMPASS_SAMPLES_OFFSET 2

//General device status.
enum Status{
  PREPARING,
  READY,
  MEASURING,
  NEW_RESULTS_AVAIL};
struct sinecosine{
  float sin_vec[MAX_SAMPLES_NUMBER];
  float cos_vec[MAX_SAMPLES_NUMBER];
};

typedef struct {
  int samples_number;
  int measures_number;
  float sample_rate;
  float int_freq;
  enum Status status;
 }Radar_Config_t;

#endif //RADAR_INIT_CONFIG_H
