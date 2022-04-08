/**************************************************************************/ /**
* @file      main.c
* @brief     Main application entry point
* @author    Eduardo Garcia
* @date      2020-02-15
* @copyright Copyright Bresslergroup\n
*            This file is proprietary to Bresslergroup.
*            All rights reserved. Reproduction or distribution, in whole
*            or in part, is forbidden except by express written permission
*            of Bresslergroup.
******************************************************************************/


/******************************************************************************
* Includes
******************************************************************************/
#include <errno.h>
#include "asf.h"
#include "main.h"
#include "stdio_serial.h"
#include "SerialConsole.h"
#include "FreeRTOS.h"
#include "driver/include/m2m_wifi.h"
#include "CliThread/CliThread.h"
#include "WifiHandlerThread/WifiHandler.h"


/******************************************************************************
* Defines and Types
******************************************************************************/
#define APP_TASK_ID 0 /**< @brief ID for the application task */
#define CLI_TASK_ID 1 /**< @brief ID for the command line interface task */
#define BOOT_TEST	1 //Uncomment me to compile boot test.

/******************************************************************************
* Local Function Declaration
******************************************************************************/
void vApplicationIdleHook(void);
//!< Initial task used to initialize HW before other tasks are initialized
static void StartTasks(void);
void vApplicationDaemonTaskStartupHook(void);
/******************************************************************************
* Variables
******************************************************************************/
static TaskHandle_t cliTaskHandle    = NULL; //!< CLI task handle
static TaskHandle_t daemonTaskHandle    = NULL; //!< Daemon task handle
static TaskHandle_t wifiTaskHandle    = NULL; //!< CLI task handle

char bufferPrint[64]; //Buffer for daemon task
bool taskAflag;
FIL file_object;
/**
 * \brief Main application function.
 *
 * Application entry point.
 *
 * \return program return value.
 */
int main(void)
{


	/* Initialize the board. */
	system_init();

	/* Initialize the UART console. */
	InitializeSerialConsole();
	SerialConsoleWriteString("we are now in MAIN\r\n");
    // Start FreeRTOS scheduler
    vTaskStartScheduler();


	return 0;
}

#ifdef BOOT_TEST

static void TestA(void)
{
	init_storage();
	SerialConsoleWriteString("Test Program A - LED Toggles every 500ms\r\n");

	FIL file_object; //FILE OBJECT used on main for the SD Card Test
	char test_file_name[] = "0:FlagB.txt";
	test_file_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
	FRESULT res = f_open(&file_object,
	(char const *)test_file_name,
	FA_CREATE_ALWAYS | FA_WRITE);

	if (res != FR_OK)
	{
		LogMessage(LOG_INFO_LVL ,"[FAIL] res %d\r\n", res);
	}
	else
	{
	SerialConsoleWriteString("FlagB.txt added! Hold button pressed to reset device!\r\n");
	}
	f_close(&file_object); //Close file

	while(1)
	{
		port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE);
		delay_s(1);
		port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
		delay_s(1);
		if(port_pin_get_input_level(BUTTON_0_PIN) == false)
		{
			delay_ms(1000);
			if(port_pin_get_input_level(BUTTON_0_PIN) == false)
			{	
				taskAflag = false;
				char delete_name[] = "0:FlagA.txt";
				//delete_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
				FRESULT resdel = f_unlink((char const *)delete_name);
				if (resdel != FR_OK) {
					SerialConsoleWriteString("Failed at deleting flagA\r\n");
				}
				else {
					SerialConsoleWriteString("deleted flagA\r\n");
				}
				delay_ms(1000);
				system_reset();
			}
		}
	}
}


static void TestB(void)
{
	init_storage();
	SerialConsoleWriteString("Test Program B - LED Toggles every 100 ms second\r\n");

	FIL file_object; //FILE OBJECT used on main for the SD Card Test
	char test_file_name[] = "0:FlagA.txt";
	test_file_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
	FRESULT res = f_open(&file_object,
	(char const *)test_file_name,
	FA_CREATE_ALWAYS | FA_WRITE);
	
	if (res != FR_OK)
	{
		LogMessage(LOG_INFO_LVL ,"[FAIL] res %d\r\n", res);
	}
	else
	{
		SerialConsoleWriteString("FlagA.txt added! Hold button pressed to reset device!\r\n");
	}
	f_close(&file_object); //Close file
	
	while(1)
	{
		port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE);
		delay_ms(200);
		port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
		delay_ms(200);
		if(port_pin_get_input_level(BUTTON_0_PIN) == false)
		{
			delay_ms(1000);
			if(port_pin_get_input_level(BUTTON_0_PIN) == false)
			{	
				taskAflag = true;
				char delete_name[] = "0:FlagB.txt";
				//delete_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
				FRESULT resdel = f_unlink((char const *)delete_name);
				if (resdel != FR_OK) {
					SerialConsoleWriteString("Failed at deleting flagB\r\n");
				}
				else {
					SerialConsoleWriteString("deleted flagB\r\n");
				}

				delay_ms(1000);
				system_reset();
			}
			
		}
	}
}

#endif

/**************************************************************************/ /**
* function          vApplicationDaemonTaskStartupHook
* @brief            Initialization code for all subsystems that require FreeRToS
* @details			This function is called from the FreeRToS timer task. Any code
*					here will be called before other tasks are initialized.
* @param[in]        None
* @return           None
*****************************************************************************/

void vApplicationDaemonTaskStartupHook(void)
{
#ifdef BOOT_TEST	
	init_storage();
	FIL file_object; //FILE OBJECT used on main for the SD Card Test
	char testA_file_name[] = "0:FlagA.txt";
	testA_file_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
	FRESULT restxt = f_open(&file_object, (char const *)testA_file_name, FA_READ);
	snprintf(bufferPrint, 64, "restxt is %i\r\n", restxt);
	SerialConsoleWriteString(bufferPrint);
	if (restxt == FR_OK) {
		taskAflag = true;
		SerialConsoleWriteString("flagA.txt is in SD card, do testA \r\n");
		//SerialConsoleWriteString("run testA\r\n");
		TestA(); //run taskA if taskAflag is true
	}
	else {
		taskAflag = false;
		SerialConsoleWriteString("flagA.txt is not in SD card, do testB \r\n");
		//SerialConsoleWriteString("run testB\r\n");
		TestB(); //run taskB if taskAflag is false
	}

#endif
	StartTasks();

	vTaskSuspend(daemonTaskHandle);
}

/**************************************************************************//**
* function          StartTasks
* @brief            Initialize application tasks
* @details
* @param[in]        None
* @return           None
*****************************************************************************/
static void StartTasks(void)
{


snprintf(bufferPrint, 64, "Heap before starting tasks: %d\r\n", xPortGetFreeHeapSize());
SerialConsoleWriteString(bufferPrint);

//Initialize Tasks here

if (xTaskCreate(vCommandConsoleTask, "CLI_TASK", CLI_TASK_SIZE, NULL, CLI_PRIORITY, &cliTaskHandle) != pdPASS) {
	SerialConsoleWriteString("ERR: CLI task could not be initialized!\r\n");
}

snprintf(bufferPrint, 64, "Heap after starting CLI: %d\r\n", xPortGetFreeHeapSize());
SerialConsoleWriteString(bufferPrint);


if (xTaskCreate(vWifiTask, "WIFI_TASK", WIFI_TASK_SIZE, NULL, WIFI_PRIORITY, &wifiTaskHandle) != pdPASS) {
	SerialConsoleWriteString("ERR: CLI task could not be initialized!\r\n");
}

snprintf(bufferPrint, 64, "Heap after starting WIFI: %d\r\n", xPortGetFreeHeapSize());
SerialConsoleWriteString(bufferPrint);
}













static void configure_console(void)
{

	stdio_base = (void *)GetUsartModule();
	ptr_put = (int (*)(void volatile*,char))&usart_serial_putchar;
	ptr_get = (void (*)(void volatile*,char*))&usart_serial_getchar;


	# if defined(__GNUC__)
	// Specify that stdout and stdin should not be buffered.
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	// Note: Already the case in IAR's Normal DLIB default configuration
	// and AVR GCC library:
	// - printf() emits one character at a time.
	// - getchar() requests only 1 byte to exit.
	#  endif
	//stdio_serial_init(GetUsartModule(), EDBG_CDC_MODULE, &usart_conf);
	//usart_enable(&cdc_uart_module);
}




void vApplicationMallocFailedHook(void)
{
SerialConsoleWriteString("Error on memory allocation on FREERTOS!\r\n");
while(1);
}

void vApplicationStackOverflowHook(void)
{
SerialConsoleWriteString("Error on stack overflow on FREERTOS!\r\n");
while(1);
}

#include "MCHP_ATWx.h"
void vApplicationTickHook (void)
{
SysTick_Handler_MQTT();
}




