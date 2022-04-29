/**************************************************************************/ /**
 * @file      GPSSensor.h
 * @brief     driver for testing gps

 * @date      2020-04-08

 ******************************************************************************/

#ifndef GPS_SENSOR_H
#define	GPS_SENSOR_H

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <asf.h>
#include <stdarg.h>

#include "string.h"

/******************************************************************************
 * Defines
 ******************************************************************************/
#define GPS_READ 0x55     ///< Command to try reading GPS

/******************************************************************************
 * Structures and Enumerations
 ******************************************************************************/

/******************************************************************************
 * Global Function Declarations
 ******************************************************************************/

/******************************************************************************
 * Functions
 ******************************************************************************/
void InitializeGPSSensor(void);
void DeinitializeGPSSerial(void);

int32_t GPSSensorRead(char *gps, const TickType_t xMaxBlockTime);
void GPSUsartWritecallback(struct usart_module *const usart_module);
void GPSUsartReadcallback(struct usart_module *const usart_module);

#endif /*GPS_SENSOR_H*/
