/************************************************************************
	INCLUDES
************************************************************************/
#include "adc.h"
#include <fcntl.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>

/************************************************************************
	LOCAL DEFINES AND TYPES
************************************************************************/
/*#define DEBUG*/

/************************************************************************
	DECLARATIONS
************************************************************************/

/************************************************************************
	FUNCTIONS
************************************************************************/
/* Private Functions */

/* Public Functions */

int8_t ADC_Init(char* devPath, ADC_Dev_t *device)
{
	int8_t status = ADC_ERROR;

#ifdef DEBUG
    printf("%s\n", __func__);
#endif

	memset(device, 0, sizeof(ADC_Dev_t));

	device->FileDevice = open(devPath, O_RDWR);
	if( device->FileDevice != 0) {
		status = ADC_OK;
	}

	return status;
}

int8_t ADC_Close(ADC_Dev_t *device)
{
	return close(device->FileDevice);
}

/*********************************************************************//**
 * @brief Configure the AN41908A device for using the Zoom, Focus and Iris.
 *
 * @param device Pointer to the device to be accessed.
 *
 * @return AN41908A_OK if the operation was successful,
 * 	AN41908A_ERROR otherwise.
 */
int8_t ADC_Config(ADC_Dev_t *device)
{
	int8_t status = ADC_ERROR;

	status = ADC_WriteRegister(device, ADC_RESET_REG, 0x80);

	if(status == ADC_OK) {
		status = ADC_WriteRegister(device, ADC_FORMAT_POWERDOWN_REG, 0x00);
	}	

	if(status == ADC_OK) {
		status = ADC_WriteRegister(device, ADC_OUTPUT_MODE_REG, 0xb7);
	}

/*
    if(status == ADC_OK) {
		status = ADC_WriteRegister(device, ADC_TEST_PATTERN_MSB_REG, 0x00);
	}

	if(status == ADC_OK) {
		status = ADC_WriteRegister(device, ADC_TEST_PATTERN_LSB_REG, 0x00);
	}
*/
	return status;
}

int8_t ADC_WriteRegister(ADC_Dev_t *device, uint8_t address, uint8_t data)
{
	struct spi_ioc_transfer	xfer[XFER_LEN];
	int8_t status = ADC_ERROR;
	uint8_t tx[ADC_TRANSFER_SIZE] = {0};

	/* Create the message */
	memset(xfer, 0, sizeof xfer);
	memset(tx, 0, sizeof tx);
	tx[0] = address;
	tx[1] = (uint8_t)(data);
	xfer[0].tx_buf = (unsigned long)tx;
	xfer[0].len = ADC_TRANSFER_SIZE;

	xfer[1].rx_buf = (unsigned long)tx;
	xfer[1].len = 0;

	/* Send it */
#ifdef DEBUG
    printf("%s: 0x%x = 0x%x\n", __func__, address, data);
#endif
	status = ioctl(device->FileDevice, SPI_IOC_MESSAGE(1), xfer);

	if(status == ADC_TRANSFER_SIZE) {
		status = ADC_OK;
	}

	return status;
}

int8_t ADC_SetPattern(ADC_Dev_t *device, uint16_t pattern)
{
	int8_t status = ADC_ERROR;

	uint8_t lsb_pattern = pattern & 0xFF;
	uint8_t msb_pattern = (pattern >> 8) & 0xFF;
	msb_pattern |= 0x80;

	status = ADC_WriteRegister(device, ADC_TEST_PATTERN_MSB_REG, msb_pattern);
	

	if(status == ADC_OK) {
		status = ADC_WriteRegister(device, ADC_TEST_PATTERN_LSB_REG, lsb_pattern);
	}

	return status;
}

int8_t ADC_ResetPattern(ADC_Dev_t *device)
{
	int8_t status = ADC_ERROR;

	status = ADC_WriteRegister(device, ADC_TEST_PATTERN_MSB_REG, 0x00);
	

	if(status == ADC_OK) {
		status = ADC_WriteRegister(device, ADC_TEST_PATTERN_LSB_REG, 0x00);
	}

	return status;

}