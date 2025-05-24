/***********************************************************************
    $Revision: 1515 $: Revision of last commit
    $Author: mcalvo $: Author of last commit
    $Date: 2019-05-16 15:45:59 +0000 (jue 16 de may de 2019) $: Date of last commit
*************************************************************************/
/*********************************************************************//**

	@file servertcp.h

	@brief TCP server

	@author 		Carlos AMOR
	@author 		RBZ Embedded Logics
	@date 15 jun. 2017	Base version

    @copyright
		This software is property of RBZ Embedded Logics. Its reproduction,
		total or  partial, by any means, in any form, permanent or temporary,
		is forbidden unless explicitly authorized by RBZ Embedded Logics.
    @copyright
		Any adaptation, amendment, translation or transformation, as well as
		the decompiling or to disassemble of this software  product, shall only
		be performed with the explicit authorization of RBZ Embedded Logics.
		The user of the present software product shall be allowed  to make a
		backup copy  as long as it is necessary  for the utilization  of the
		same.
    @copyright
		The terms stated above shall be understood  affecting that stated in
		applicable Laws.

*************************************************************************/

#ifndef _SERVERTCP_H_
#define _SERVERTCP_H_

#include <stdbool.h>

/************************************************************************
	DECLARATIONS
************************************************************************/

#ifndef __error_t_defined
typedef int error_t;
#endif
#define ERROR_OK	0u

#define MAJOR_VERSION		1
#define MINOR_VERSION		0
#define REVISION		0

#define IP_ADDR "127.0.0.1"
#define BUFSIZE 4096
#define SOURCE_ADDR	2

#define READ_CONFIG     0
#define DEFAULT_CONFIG  1
#define SET1_CONFIG     2
#define SET2_CONFIG     3
#define SET3_CONFIG     4

#define ERROR_TIMEOUT   50000
#define ERROR_SLEEP     100

#define ERROR -1

#define LINE_SIZE   128
#define CMD_SIZE    60

#define PERCENT_TO_VAL  100
#define DEC_TO_VAL  10
#define SLEEP_100       100
#define EXPOSURE_TIME_ERR   14260
#define SENSOR_EXTRA_LINES 42
#define SENSOR_SHS_MIN 14
#define MICROSECS_TO_NANOSECS 1000
#define SECS_TO_NANOSECS 1000000000

#define consultConfig				\
	REG_ZOOM_POSITION,			\
	REG_FOCUS_POSITION,			\
	REG_IRIS_APERTURE,			\
	REG_CAPTURA_ENCODER_TYPE,		\
	REG_CAPTURA_FPS,			\
	REG_CAPTURA_JPEG_QUALITY,		\
	REG_CAPTURA_H264_BITRATE_CONF,		\
	REG_CAPTURA_H264_BITRATE_VAL,		\
	REG_CAPTURA_H264_QUALITY,		\
	REG_CAPTURA_H264_GOP,			\
	REG_CAPTURA_MODE,			\
	REG_CAPTURA_VIDEO_LENGTH,		\
	REG_CAPTURA_VIDEO_FILTER,		\
	REG_CAPTURA_PHOTO_INTERVAL,		\
	REG_CAPTURA_PHOTO_LENGTH,		\
	REG_CAPTURA_PHOTO_FILTER,		\
	REG_CCTV_ENCODER_TYPE,			\
	REG_CCTV_FPS,				\
	REG_CCTV_JPEG_QUALITY,			\
	REG_CCTV_H264_BITRATE_CONF,		\
	REG_CCTV_H264_BITRATE_VAL,		\
	REG_CCTV_H264_QUALITY,			\
	REG_CCTV_H264_GOP,			\
	REG_VGRABACION_ENCODER_TYPE,		\
	REG_VGRABACION_FPS,			\
	REG_VGRABACION_JPEG_QUALITY,		\
	REG_VGRABACION_H264_BITRATE_CONF,	\
	REG_VGRABACION_H264_BITRATE_VAL,	\
	REG_VGRABACION_H264_QUALITY,		\
	REG_VGRABACION_H264_GOP,		\
	REG_SENSOR_GAIN,			\
	REG_SENSOR_EXPOSURE_TIME,		\
	REG_SENSOR_FLASH_TIME,			\
	REG_DO_0_WRITE_VAL,			\
	REG_DO_0_WRITE_TIME,			\
	REG_DO_1_WRITE_VAL,			\
	REG_DO_1_WRITE_TIME,			\
	REG_FLASH_SIG_M,			\
	REG_FLASH_SIG_1,			\
	REG_FLASH_SIG_2,			\
	REG_FLASH_EN,                           \
	REG_SENSOR_FPS,				\
	REG_SENSOR_WIDTH,			\
	REG_SENSOR_HEIGHT,			\
	REG_SENSOR_H_POS,			\
	REG_SENSOR_V_POS,			\
	REG_VIDEO_SERVER_PORT,			\
	REG_CAPTURA_SERVER_PORT,		\
	REG_AUTOFOCUS_H_INIT,			\
	REG_AUTOFOCUS_H_END,			\
	REG_AUTOFOCUS_V_INIT,			\
	REG_AUTOFOCUS_V_END,			\
	REG_AUTOFOCUS_THRESHOLD,		\
	REG_AUTOFOCUS_H_INIT_1,			\
	REG_AUTOFOCUS_H_END_1,			\
	REG_AUTOFOCUS_V_INIT_1,			\
	REG_AUTOFOCUS_V_END_1,			\
	REG_AUTOFOCUS_THRESHOLD_1,		\
	REG_ZOOM_POSITION_1,			\
	REG_FOCUS_POSITION_1,			\
	REG_IRIS_APERTURE_1,			\
	REG_SENSOR_GAIN_1,			\
	REG_SENSOR_EXPOSURE_TIME_1,		\
	REG_SENSOR_FPS_1,			\
	REG_SENSOR_WIDTH_1,			\
	REG_SENSOR_HEIGHT_1,			\
	REG_SENSOR_H_POS_1,			\
	REG_SENSOR_V_POS_1,         \
    REG_SENSOR_TARGET_HIST,     \
    REG_SENSOR_MAX_GAIN,        \
    REG_SENSOR_MIN_GAIN,        \
    REG_SENSOR_MAX_GAIN_1,        \
    REG_SENSOR_MIN_GAIN_1,        \
    REG_AGC_ENABLED,             \
    REG_AGC_ENABLED_1,             \
    REG_SETUP,                   \
    REG_OCR_ENABLED,             \
    REG_NTP_ENABLED,             \
    REG_SENSOR_GAMMA_RED,        \
    REG_SENSOR_GAMMA_GREEN,        \
    REG_SENSOR_GAMMA_BLUE,        \
    REG_SENSOR_GAMMA_ALL,       \
    REG_AEC_ENABLED_1,          \
    REG_SENSOR_MAX_EXP_1,       \
    REG_SENSOR_MIN_EXP_1,       \
    REG_AIC_ENABLED_1,          \
    REG_SENSOR_IRIS_REF_1

#endif //_SERVERTCP_H_
