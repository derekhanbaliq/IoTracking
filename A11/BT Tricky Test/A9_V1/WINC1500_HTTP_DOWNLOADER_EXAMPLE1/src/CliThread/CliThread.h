/**********************************************************************//**
* @file      CliThread.h
* @brief     File for the CLI Thread handler. Uses FREERTOS + CLI
* @author    Eduardo Garcia
* @date      2020-02-15

******************************************************************************/

#pragma once

#include "asf.h"
#include "SerialConsole.h"
#include "FreeRTOS_CLI.h"


#define CLI_TASK_SIZE	256		///<STUDENT FILL
#define CLI_PRIORITY (configMAX_PRIORITIES - 1) ///<STUDENT FILL
#define CLI_TASK_DELAY 150	///STUDENT FILL

#define MAX_INPUT_LENGTH_CLI    50	//STUDENT FILL
#define MAX_OUTPUT_LENGTH_CLI   100	//STUDENT FILL



#define ASCII_BACKSPACE					0x08
#define ASCII_DELETE                    0x7F
#define ASCII_WHITESPACE				0x20
#define ASCII_ESC						27


void vCommandConsoleTask( void *pvParameters );