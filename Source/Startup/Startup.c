//File Name   ��Startup.c
//Description ��System startup  

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeRTOS_Cli.h"

#include "rtc.h"
#include "LED.h"
#include "Keys.h"
#include "TempSensors.h"
#include "SSD1306.h"
#include "Music.h"
#include "sdcard.h"
#include "EBProtocol.h"
#include "USBMeter.h"
#include "BadApplePlayer.h"
#include "CPU_Usage.h"
#include "MultiLanguageStrings.h"
#include "USBCDC.h"
#include "UI_Confirmation.h" 

#include "Startup.h"

#define SYSTEM_STARTUP_PRIORITY tskIDLE_PRIORITY+4
#define SYSTEM_STARTUP_STATUS_UPDATE_PRIORITY tskIDLE_PRIORITY+3

xQueueHandle InitAnimatePosHandle;

xQueueHandle InitStatusMsg;

const unsigned char logo[128][4] =
{
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0xF8,0x00,
0x00,0x03,0xE0,0x00,0x00,0x0F,0x80,0x00,0x00,0x3E,0x00,0x00,0x00,0xF8,0x00,0x00,
0x03,0xE0,0x00,0x00,0x0F,0x80,0x00,0x00,0x3E,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,
0x3E,0x00,0x00,0x00,0x0F,0x80,0x00,0x00,0x03,0xE0,0x00,0x00,0x00,0xF8,0x00,0x00,
0x00,0x3E,0x00,0x00,0x00,0x0F,0x80,0x00,0x00,0x03,0xE0,0x00,0x00,0x00,0xF8,0x00,
0x00,0x00,0x3E,0x00,0x00,0x00,0x0F,0x80,0x00,0x00,0x03,0xE0,0x00,0x00,0x00,0xF8,
0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0xF8,0x00,0x00,0x03,0xE0,0x00,0x00,0x0F,0x80,
0x00,0x00,0x3E,0x00,0x00,0x00,0xF8,0x00,0x00,0x03,0xE0,0x00,0x00,0x0F,0x80,0x00,
0x00,0x3E,0x00,0x00,0x00,0x0F,0x80,0x00,0x00,0x03,0xE0,0x00,0x00,0x00,0xF8,0x00,
0x00,0x00,0x3E,0x00,0x00,0x00,0x0F,0x80,0x00,0x00,0x03,0xE0,0x00,0x00,0x0F,0x80,
0x00,0x00,0x1E,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x00,
};

/**
  * @brief  Task which displays the logo animation

  * @param  lesex:Useless rubbish

  * @retval None
  */
void LogoHandler(void *pvParameters)
{
	portTickType xLastWakeTime;
	unsigned char VerticalAddr = 0;
	unsigned char DrawOrUnDraw = 1;
	unsigned char LoadingAddr;
	unsigned char m, n;
	xLastWakeTime = xTaskGetTickCount();
	while (1)
	{
		xQueueReceive(InitAnimatePosHandle, &LoadingAddr, 0);
		if (xSemaphoreTake(OLEDRelatedMutex, 0) != pdPASS)
		{
			goto Wait;
		}
		UpdateOLEDJustNow = true;
		/*If it's drawing's turn,draw the respective line at respective verticalAddr*/
		if (DrawOrUnDraw)
			for (m = 0; m < 4; m++)
			{
				for (n = 0; n < 8; n++)
				{
					OLED_DrawPoint(VerticalAddr, m * 8 + n, (bool)(logo[VerticalAddr][m] & ((u8)(0x01) << (7 - n))));
				}
			}
		/*Else clear the respective line*/
		else
		{
			OLED_DrawVerticalLine(VerticalAddr, 0, 31, 0);
		}
		VerticalAddr++;
		/*If verticalAddr reached the end of the screen,change the mode to draw/unDraw*/
		if (VerticalAddr == 128)
		{
			VerticalAddr = 0;
			DrawOrUnDraw = !DrawOrUnDraw;
		}
		/*Set n to determine the position of the blank block*/
		n = VerticalAddr / 32;
		switch (n)
		{
		case 0: n = 1; break;
		case 1: n = 2; break;
		case 2: n = 4; break;
		case 3: n = 8;
		}
		/*Draw the blocks of the animation*/
		OLED_FillRect(LoadingAddr, 39, LoadingAddr + 5, 44, !(n & 1));
		OLED_FillRect(LoadingAddr + 7, 39, LoadingAddr + 11, 44, !(n & 2));
		OLED_FillRect(LoadingAddr + 7, 45, LoadingAddr + 11, 50, !(n & 4));
		OLED_FillRect(LoadingAddr, 45, LoadingAddr + 5, 50, !(n & 8));
		UpdateOLEDJustNow = false;
		xSemaphoreGive(OLEDRelatedMutex);
	Wait:
		vTaskDelayUntil(&xLastWakeTime, 8 / portTICK_RATE_MS);
	}
}

/**
  * @brief  Get central position(Start Position) of the string between x1,x2

  * @param  startPos:Position of start grid

			endPos:  Position of end grid

			stringLength:Length of the string

	  @retval Start position of the target string
  */
u8 GetCentralPosition(u8 startPos, u8 endPos, u8 stringLength)
{
	u8 pos;
	pos = startPos + (endPos - startPos) / 2;
	pos = pos - stringLength * 3;
	return(pos);
}

/**
  * @brief  Get length of given string

  * @param  string[]:Given string

	  @retval Length of given string
  */
u8 GetStringLength(char string[])
{
	u8 length;
	for (length = 0; string[length] != 0; length++);
	return(length);
}

/**
  * @brief  Task which update the initStatus

	@param  None

  * @retval None
  */
void InitStatusUpdateHandler(void *pvParameters)
{
	u8 startAddr;
	u8 stringLength;
	u8 loadingAddr;
	u8 outOfDateLoadingAddr;
	char initStatus[30];
	while (1)
	{
		while (xQueueReceive(InitStatusMsg, &initStatus, portMAX_DELAY) != pdPASS);
		/*Get the length of the string to calculate the central position*/
		stringLength = GetStringLength(initStatus);
		/*Calculate central addr*/
		startAddr = GetCentralPosition(0, 127, stringLength);
		/*Make room for "blocks animation"*/
		startAddr = startAddr + 9;
		xSemaphoreTake(OLEDRelatedMutex, portMAX_DELAY);
		/*Clear the area that last initString occupies*/
		OLED_FillRect(0, 39, 127, 55, 0);
		/*Show the new initString*/
		OLED_ShowAnyString(startAddr, 39, initStatus, false, 12);
		UpdateOLEDJustNow = true;
		OLED_Refresh_Gram();
		UpdateOLEDJustNow = false;
		xSemaphoreGive(OLEDRelatedMutex);
		/*Adjust the position of "blocks animation"(loadingAnimation)*/
		loadingAddr = startAddr - 16;
		if (xQueueSend(InitAnimatePosHandle, &loadingAddr, 100 / portTICK_RATE_MS) != pdPASS)
		{
			/*Clear the queue and resend the message*/
			xQueueReceive(InitAnimatePosHandle, &outOfDateLoadingAddr, 10);
			xQueueSend(InitAnimatePosHandle, &loadingAddr, 100 / portTICK_RATE_MS);
		}
	}
}

/**
  * @brief  Init Logo

	  @retval None
  */
xTaskHandle Logo_Init()
{
	xTaskHandle logoHandle;
	xTaskCreate(LogoHandler, "Logo handler",
		64, NULL, SYSTEM_STARTUP_STATUS_UPDATE_PRIORITY, &logoHandle);
	InitAnimatePosHandle = xQueueCreate(1, sizeof(u8));
	return(logoHandle);
}


/**
  * @brief  Init Logo

	  @retval None
  */
xTaskHandle InitStatusHandler_Init(void)
{
	xTaskHandle initStatusHandle;
	xTaskCreate(InitStatusUpdateHandler, "Init Status Handler",
		64, NULL, SYSTEM_STARTUP_STATUS_UPDATE_PRIORITY, &initStatusHandle);
	InitStatusMsg = xQueueCreate(1, 30);
	return(initStatusHandle);
}

/**
  * @brief  System startup task

	  @retval None
  */
void SystemStartup(void *pvParameters)
{
	xTaskHandle logoHandle;
	xTaskHandle initStatusUpdateHandle;
	Key_Init();
	OLED_Init();
	if(RIGHT_KEY==KEY_ON)
	{
	 OSStatInit();	
	}
	USBCDC_Init();
	CommandLine_Init();
	LED_Animate_Init(LEDAnimation_Startup);
	RTC_Init();
	logoHandle = Logo_Init();
	initStatusUpdateHandle = InitStatusHandler_Init();
	xQueueSend(InitStatusMsg, SystemInit_Str[Language], 0);
	vTaskDelay(100 / portTICK_RATE_MS);
	TemperatureSensors_Init();
	EBD_Init();
	vTaskDelay(50 / portTICK_RATE_MS);
	sdcard_Init();
	vTaskDelay(500 / portTICK_RATE_MS);
	ShowCurrentTempSensor();
	CheckEBDDirectories();
	LED_Animate_DeInit();
	vTaskDelete(initStatusUpdateHandle);
	vTaskDelete(logoHandle);
	xSemaphoreGive(OLEDRelatedMutex);
	UpdateOLEDJustNow=false;
	OLED_Clear();
	if(LEFT_KEY==KEY_ON)
	{
	 BadApplePlayer_Init();	
	}
	else
	USBMeter_Init(USBMETER_ONLY);
	vTaskDelete(NULL);
}

/**
  * @brief  System start entrance

	  @retval None
  */
void SystemStart()
{
	xTaskCreate(SystemStartup, "SystemStartup",
		256, NULL, SYSTEM_STARTUP_PRIORITY, NULL);
	vTaskStartScheduler();
}

