/**************************************************************************/ /**
 * @file      DistanceSensor.h
 * @brief     Driver for the US-100 Distance Sensor, in SERIAL MODE. This means the jumper must remain on!
 In this mode, it will work with an UART driver. The MCU can send 0x55 and should receive two bytes indicating the distance in mm (high byte, low byte)
 If you send 0x50, it will return the temperature in Degrees C.

 This criver will be written compatible to be run from RTOS thread, with non-blocking commands in mind.
 See https://www.bananarobotics.com/shop/US-100-Ultrasonic-Distance-Sensor-Module for more information
 * @author    Eduardo Garcia
 * @date      2020-04-08

 ******************************************************************************/

#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <asf.h>
#include <stdarg.h>

#include "string.h"

/******************************************************************************
 * Defines
 ******************************************************************************/
#define DISTANCE_US_100_CMD_READ_DISTANCE 0x55     ///< Command to send to the US-100 to order a distance command
#define DISTANCE_US_100_CMD_READ_TEMPERATURE 0x50  ///< Command to send to the US-100 to order a temperature command read

/******************************************************************************
 * Structures and Enumerations
 ******************************************************************************/

/******************************************************************************
 * Global Function Declarations
 ******************************************************************************/

/******************************************************************************
 * Functions
 ******************************************************************************/
void InitializeDistanceSensor(void);
void DeinitializeDistanceSerial(void);
double parseGPRMC_lat(char stringtocheck[]);
double parseGPRMC_long(char stringtocheck[]);
int32_t DistanceSensorGetDistance(float *distance, const TickType_t xMaxBlockTime); //Derek-@628
void distUsartWritecallback(struct usart_module *const usart_module);
void distUsartReadcallback(struct usart_module *const usart_module);

#endif /*DISTANCE_SENSOR_H*/
