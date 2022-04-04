 /**************************************************************************//**
* @file      UiHandlerThread.c
* @brief     File that contains the task code and supporting code for the UI Thread for ESE516 Spring (Online) Edition
* @author    You! :)
* @date      2020-04-09 

******************************************************************************/


/******************************************************************************
* Includes
******************************************************************************/
#include <errno.h>
#include "asf.h"
#include "UiHandlerThread/UiHandlerThread.h"
#include "SeesawDriver/Seesaw.h"
#include "SerialConsole.h"
#include "main.h"

/******************************************************************************
* Defines
******************************************************************************/

/******************************************************************************
* Variables
******************************************************************************/
uiStateMachine_state uiState;
/******************************************************************************
* Forward Declarations
******************************************************************************/

/******************************************************************************
* Callback Functions
******************************************************************************/


/******************************************************************************
* Task Function
******************************************************************************/

/**************************************************************************//**
* @fn		void vUiHandlerTask( void *pvParameters )
* @brief	STUDENT TO FILL THIS
* @details 	student to fill this
                				
* @param[in]	Parameters passed when task is initialized. In this case we can ignore them!
* @return		Should not return! This is a task defining function.
* @note         
*****************************************************************************/

char UiPrintBuf[64];
uint8_t SeesawEventBuf[64];

void vUiHandlerTask( void *pvParameters )
{
	//Do initialization code here
	SerialConsoleWriteString("UI Task Started!");
	uiState = UI_STATE_HANDLE_BUTTONS;

	TickType_t startTime = xTaskGetTickCount();

	//Here we start the loop for the UI State Machine
	while(1)
	{
		static uint8_t curr_cnt = 0;
		static uint8_t prev_cnt = 0;
	
		switch(uiState)
		{
			case(UI_STATE_HANDLE_BUTTONS):
			{
				//Do the handle buttons code here!
			
				curr_cnt = SeesawGetKeypadCount();
				//snprintf(UiPrintBuf, 64, "curr count is: %d\r\n", curr_cnt);
				//SerialConsoleWriteString(UiPrintBuf);
			
				if(xTaskGetTickCount() - startTime > 200)
				{
					startTime = xTaskGetTickCount(); //Save tick time for next round
				
					//snprintf(UiPrintBuf, 64, "delta count is: %d\r\n", curr_cnt);
					//SerialConsoleWriteString(UiPrintBuf);
				
					if (curr_cnt != 0)
					{
						int readKeypadError = SeesawReadKeypad(SeesawEventBuf, curr_cnt);
						//snprintf(UiPrintBuf, 64, "SeesawReadKeypad error is: %d\r\n", readKeypadError);
						//SerialConsoleWriteString(UiPrintBuf);

						//SerialConsoleWriteString(SeesawEventBuf);
						//SerialConsoleWriteString("\r\n");
						
						int i = 0;
						while(SeesawEventBuf[i] != NULL)
						{
							uint8_t byte = SeesawEventBuf[i];
							
							uint8_t num = (byte & 0b11111100) >> 2; //get first 6 bits
							uint8_t press = byte & 0b00000011; //get last 2 bits
							
							int real_num = NEO_TRELLIS_SEESAW_KEY(num);
							snprintf(UiPrintBuf, 64, "real key number is: %i\r\n", real_num);
							SerialConsoleWriteString(UiPrintBuf);
							
							if(press == 0b11)
							{
								SerialConsoleWriteString("pressed!!!\r\n");
								SeesawSetLed(real_num, 255, 0, 0);
								SeesawOrderLedUpdate();
							}
							else if(press == 0b10)
							{
								SerialConsoleWriteString("unpressed!!!\r\n");
								SeesawSetLed(real_num, 0, 0, 0);
								SeesawOrderLedUpdate();
							}
							else
							{
								SerialConsoleWriteString("What the deuce?\r\n");
							}
							
							i++;
						}
					
						curr_cnt = 0;
					}
				}
				
				break;
			}

			case(UI_STATE_IGNORE_PRESSES):
			{
			//Ignore me for now
				break;
			}

			case(UI_STATE_SHOW_MOVES):
			{
			//Ignore me as well
				break;
			}

		default: //In case of unforseen error, it is always good to sent state machine to an initial state.
			uiState = UI_STATE_HANDLE_BUTTONS;
		break;
	}

	//After execution, you can put a thread to sleep for some time.
	vTaskDelay(50);
}



}




/******************************************************************************
* Functions
******************************************************************************/