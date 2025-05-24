#ifndef ADC_H_
#define ADC_H_

/************************************************************************
	INCLUDES
************************************************************************/
#include <stdint.h>

/**************************************************************************
 * CONSTANTS
 *************************************************************************/

/* SPI Configuration constants */
#define ADC_TRANSFER_SIZE			2
#define XFER_LEN                    2

/* Status constants */
#define ADC_OK						0
#define ADC_ERROR					(-1)

/* ADC Register */
#define ADC_RESET_REG              0x00
#define ADC_FORMAT_POWERDOWN_REG   0x01
#define ADC_OUTPUT_MODE_REG        0x02
#define ADC_TEST_PATTERN_MSB_REG   0x03
#define ADC_TEST_PATTERN_LSB_REG   0x04


#define ADC_PRIM_LINUX_DEVICE_ADDRESS_1   "/dev/spidev3.0"
#define ADC_PRIM_LIUNX_DEVICE_ADDRESS_2   "/dev/spidev3.1" 
#define ADC_TEST_LINUX_DEVICE_ADDRESS   "/dev/spidev4.0"


/************************************************************************
	GLOBAL DEFINES AND TYPES
************************************************************************/
typedef struct ADC_Dev{
	int FileDevice;		/**<File device identifier */
}ADC_Dev_t;

/**************************************************************************
 * FUNCTIONS
 *************************************************************************/
int8_t ADC_Init(char* devPath, ADC_Dev_t *device);
int8_t ADC_Close(ADC_Dev_t *device);
int8_t ADC_Config(ADC_Dev_t *device);
int8_t ADC_WriteRegister(ADC_Dev_t *device, uint8_t address, uint8_t data);
int8_t ADC_SetPattern(ADC_Dev_t *device, uint16_t pattern);
int8_t ADC_ResetPattern(ADC_Dev_t *device);

#endif /* ADC_H_ */
