//File Name   Startup.c
//Description System startup  

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
#include "UI_Utilities.h"

#include "mass_mal.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"

#include "UI_Print.h"

#include "Settings.h"

#include "Startup.h"

#define SYSTEM_STARTUP_PRIORITY tskIDLE_PRIORITY+4
#define SYSTEM_STARTUP_STATUS_UPDATE_PRIORITY tskIDLE_PRIORITY+3

xQueueHandle InitAnimatePosHandle;

xQueueHandle InitStatusMsg;

xTaskHandle LogoAnimateHandle=NULL;

xTaskHandle InitStatusHandle=NULL;

bool IsOLEDMutexTokenByLogoAnimateHandler;

bool IsOLEDMutexTokenByInitStatusHandler;

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
		IsOLEDMutexTokenByLogoAnimateHandler=true;
		
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
	  IsOLEDMutexTokenByLogoAnimateHandler=false;
	Wait:
		vTaskDelayUntil(&xLastWakeTime, 8 / portTICK_RATE_MS);
	}
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
		stringLength = GetStringGraphicalLength(initStatus);
		/*Calculate central addr*/
		startAddr = GetCentralPosition(0, 127, stringLength);
		/*Make room for "blocks animation"*/
		startAddr = startAddr + 9;
		
		xSemaphoreTake(OLEDRelatedMutex, portMAX_DELAY);
		IsOLEDMutexTokenByInitStatusHandler=true;
		
		/*Clear the area that last initString occupies*/
		OLED_FillRect(0, 39, 127, 55, 0);
		/*Show the new initString*/
		OLED_ShowAnyString(startAddr, 39, initStatus, false, 12);
		UpdateOLEDJustNow = true;
		OLED_Refresh_Gram();
		UpdateOLEDJustNow = false;
		
		IsOLEDMutexTokenByInitStatusHandler=false;
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
void Logo_Init()
{
	if(LogoAnimateHandle==NULL)
	xTaskCreate(LogoHandler, "Logo handler",
		128, NULL, SYSTEM_STARTUP_STATUS_UPDATE_PRIORITY, &LogoAnimateHandle);
	InitAnimatePosHandle = xQueueCreate(1, sizeof(u8));
}


/**
  * @brief  Init Logo

	  @retval None
  */
void InitStatusHandler_Init(void)
{
	if(InitStatusHandle==NULL)
	xTaskCreate(InitStatusUpdateHandler, "Init Status Handler",
		128, NULL, SYSTEM_STARTUP_STATUS_UPDATE_PRIORITY, &InitStatusHandle);
	InitStatusMsg = xQueueCreate(1, 30);
}

/**
  * @brief  DeInit  logo

	  @retval None
  */
void Logo_DeInit()
{
 if(LogoAnimateHandle!=NULL)
 {
  vTaskDelete(LogoAnimateHandle);
 }
 if(IsOLEDMutexTokenByLogoAnimateHandler)
 {
  xSemaphoreGive(OLEDRelatedMutex);
 }
 LogoAnimateHandle=NULL;
 vQueueDelete(InitAnimatePosHandle);
}

/**
  * @brief  DeInit InitStatus handler

	  @retval None
  */
void InitStatus_DeInit()
{
 if(InitStatusHandle!=NULL)
 {
  vTaskDelete(InitStatusHandle);
 }
 if(IsOLEDMutexTokenByInitStatusHandler)
 {
  xSemaphoreGive(OLEDRelatedMutex);
 }
 InitStatusHandle=NULL;
 vQueueDelete(InitStatusMsg);
}

/**
  * @brief  Init logo and InitStatusHandler

	  @retval None
  */
void LogoWithInitStatus_Init()
{
 Logo_Init();
 InitStatusHandler_Init();
}

/**
  * @brief  DeInit logo and InitStatusHandler

	  @retval None
  */
void LogoWithInitStatus_DeInit()
{
 Logo_DeInit();
 InitStatus_DeInit();
}

/**
  * @brief  System startup task

	  @retval None
  */
void SystemStartup(void *pvParameters)
{
	Key_Init();
	OLED_Init();
	if(RIGHT_KEY==KEY_ON)
	{
	 OSStatInit();	
	}
	//USBCDC_Init();
	//CommandLine_Init();
	Settings_Init();
	LED_Animate_Init(LEDAnimation_Startup);
	
  //W25X_CS_Init();
	
	RTC_Init();
	
	LogoWithInitStatus_Init();
	xQueueSend(InitStatusMsg, SystemInit_Str[CurrentSettings->Language], 0);
	
	vTaskDelay(100 / portTICK_RATE_MS);
	TemperatureSensors_Init();
	
	EBD_Init();
	vTaskDelay(50 / portTICK_RATE_MS);
	
	sdcard_Init(true);
	vTaskDelay(500 / portTICK_RATE_MS);
	
	ShowCurrentTempSensor();
	CheckEBDDirectories(true);

	USB_Interrupts_Config();    
	Set_USBClock();   	  	
	
	LED_Animate_DeInit();
	LogoWithInitStatus_DeInit();
	
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

