//File Name   Startup.c
//Description System startup  

#include "FreeRTOS_Standard_Include.h"
#include "FreeRTOS_Cli.h"

#include "rtc.h"
#include "LED_Animate.h"
#include "Keys.h"
#include "PWM_Ref.h"
#include "ADC.h"

#include "Temperature_Sensors.h"
#include "DataPins_Voltage_Sensors.h"
#include "SSD1306.h"
#include "Music.h"
#include "sdcard.h"
#include "USBMeter.h"
#include "MCP3421.h"
#include "BadApplePlayer.h"
#include "CPU_Usage.h"
#include "MultiLanguageStrings.h"
#include "FastCharge_Trigger_Circuit.h"
#include "W25Q64.h"
#include "UI_Utilities.h"
#include "UI_Print.h"
#include "Cooling_Fan.h"
#include "Digital_Load.h"
#include "SDCardff.h"
#include "DataPin2Protocol.h"

#include "system_stm32f10x.h"

#include "mass_mal.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"

#include "UI_Print.h"

#include "Settings.h"
#include "Flash_Test.h"

#include "Startup.h"

#define SYSTEM_STARTUP_PRIORITY tskIDLE_PRIORITY+4
#define SYSTEM_STARTUP_STATUS_UPDATE_PRIORITY tskIDLE_PRIORITY+3

xQueueHandle InitAnimatePosHandle;

xQueueHandle InitStatusMsg;

xTaskHandle LogoAnimateHandle = NULL;

xTaskHandle InitStatusHandle = NULL;

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
	unsigned char LoadingAddr=20;
	unsigned char m, n;
	xLastWakeTime = xTaskGetTickCount();
	while (1)
	{
		xQueueReceive(InitAnimatePosHandle, &LoadingAddr, 0);
		
		if(LoadingAddr==127)
		{
	    vQueueDelete(InitAnimatePosHandle);
			LogoAnimateHandle=NULL;
			vTaskDelete(NULL);
		}
		
		if (xSemaphoreTake(OLEDRelatedMutex, 0) != pdPASS)
		{
			goto Wait;
		}
		IsOLEDMutexTokenByLogoAnimateHandler = true;

		SetUpdateOLEDJustNow();
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
		ResetUpdateOLEDJustNow();
		xSemaphoreGive(OLEDRelatedMutex);
		IsOLEDMutexTokenByLogoAnimateHandler = false;
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
		
		if(*initStatus==0)
		{
	    vQueueDelete(InitStatusMsg);
			InitStatusHandle=NULL;
			vTaskDelete(NULL);
		}
		
		/*Get the length of the string to calculate the central position*/
		stringLength = GetStringGraphicalLength(initStatus);
		/*Calculate central addr*/
		startAddr = GetCentralPosition(0, 127, stringLength);
		/*Make room for "blocks animation"*/
		startAddr = startAddr + 9;

		xSemaphoreTake(OLEDRelatedMutex, portMAX_DELAY);
		IsOLEDMutexTokenByInitStatusHandler = true;

		/*Clear the area that last initString occupies*/
		OLED_FillRect(0, 39, 127, 55, 0);
		/*Show the new initString*/
		OLED_ShowAnyString(startAddr, 39, initStatus, false, 12);
		SetUpdateOLEDJustNow();
		OLED_Refresh_Gram();
		ResetUpdateOLEDJustNow();

		IsOLEDMutexTokenByInitStatusHandler = false;
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
	if (LogoAnimateHandle == NULL)
		CreateTaskWithExceptionControl(LogoHandler, "Logo handler",
			128, NULL, SYSTEM_STARTUP_STATUS_UPDATE_PRIORITY, &LogoAnimateHandle);
	InitAnimatePosHandle = xQueueCreate(1, sizeof(u8));
}


/**
  * @brief  Init Logo

	  @retval None
  */
void InitStatusHandler_Init(void)
{
	if (InitStatusHandle == NULL)
		CreateTaskWithExceptionControl(InitStatusUpdateHandler, "Init Status Handler",
			128, NULL, SYSTEM_STARTUP_STATUS_UPDATE_PRIORITY, &InitStatusHandle);
	InitStatusMsg = xQueueCreate(1, 30);
}

/**
  * @brief  DeInit  logo

	  @retval None
  */
void Logo_DeInit()
{
	u8 exitCode=127;
	xQueueSend(InitAnimatePosHandle, &exitCode, portMAX_DELAY);
  while(LogoAnimateHandle!=NULL)
	{
	 vTaskDelay(2/portTICK_RATE_MS);
	}
}

/**
  * @brief  DeInit InitStatus handler

	  @retval None
  */
void InitStatus_DeInit()
{
	xQueueSend(InitStatusMsg,"", portMAX_DELAY);
  while(InitStatusHandle!=NULL)
	{
	 vTaskDelay(2/portTICK_RATE_MS);
	}
}

/**
  * @brief  Init logo and InitStatusHandler

	  @retval None
  */
void LogoWithInitStatus_Init()
{
	InitStatusHandler_Init();
	Logo_Init();
}

/**
  * @brief  DeInit logo and InitStatusHandler

	  @retval None
  */
void LogoWithInitStatus_DeInit()
{
	InitStatus_DeInit();
	Logo_DeInit();
}



/**
  * @brief  System startup task

	  @retval None
  */
void SystemStartup(void *pvParameters)
{
	Key_Init();
	Cooling_Fan_Service_Init();
	OLED_Init();	
	PWMRef_Init();
	Digital_Load_Init();
	MCP3421_Init();
	RTC_Init();
	LED_Animate_Init(LEDAnimation_Startup);
	Settings_Init();
	W25Q64_Init();
	LogoWithInitStatus_Init();
	xQueueSend(InitStatusMsg, SystemInit_Str[CurrentSettings->Language], 0);
  ADC_Hardware_Init();
	TemperatureSensors_Init();
  DataPins_Voltage_Sensor_Init();
  DataPin2Protocol_Service_Init();
	SDCard_Init(true);
	SDCardPlugAndPlay_Service_Init(); 
	vTaskDelay(500 / portTICK_RATE_MS);
	CheckEBDDirectories(true);
	USB_Interrupts_Config();
	Set_USBClock();
	FastCharge_Trigger_Service_Init();
	LED_Animate_DeInit();
	LogoWithInitStatus_DeInit();
	UpdateOLEDJustNow = false;
	xSemaphoreTake(OLEDRelatedMutex, portMAX_DELAY);
	OLED_Clear();
	xSemaphoreGive(OLEDRelatedMutex);
  //Flash_Test_Run();
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

