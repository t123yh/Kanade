//File Name   USBMeterUI.c
//Description Main UI

#include <stdio.h>
#include <string.h>

#include "stm32f10x.h"

#include "FreeRTOS_Standard_Include.h"

#include "Digital_Load.h"
#include "Temperature_Sensors.h"
#include "SSD1306.h"
#include "RTC.h"
#include "MCP3421.h"
#include "VirtualRTC.h"
#include "UI_Confirmation.h"
#include "MultiLanguageStrings.h"
#include "Keys.h"
#include "sdcard.h"
#include "sdcardff.h"

#include "mass_mal.h"

#include "Settings.h"
#include "StepUpTest.h"
#include "FastCharge_Trigger.h"
#include "LegacyTest.h"
#include "MassStorage.h"
#include "ExceptionHandle.h"

#include "UI_Dialogue.h"
#include "UI_Utilities.h"

#include "USBMeter.h"

#define USB_METER_PRIORITY tskIDLE_PRIORITY+3

#define EFFECT_DATA_NUM_PER_SCREEN 127

float CurrentCurveBuff[EFFECT_DATA_NUM_PER_SCREEN];
float VoltageCurveBuff[EFFECT_DATA_NUM_PER_SCREEN];

u16 Data_p = 0;

bool firstCycle = true;

static void DisplayBasicData(char tempString[], u8 currentStatus, u8 firstEnter);

static void DisplayRecordData(char tempString[]);

static void ScrollDialgram_Routine(void);

/**
  * @brief  USBMeter

  * @retval None
  */
static void USBMeter(void *pvParameters)
{
	char tempString[20];
	bool fastChargeStatus = false;
	u8 firstEnter = 1;
	Key_Message_Struct keyMessage;
	u8 status = USBMETER_ONLY;
	u8 reSendLoadCommandCnt = 0;
	Legacy_Test_Param_Struct legacy_Test_Params;
	ClearKeyEvent();
	while (1)
	{
	Refresh:
		xSemaphoreTake(OLEDRelatedMutex, portMAX_DELAY);
		xSemaphoreTake(USBMeterState_Mutex, portMAX_DELAY);
		DisplayBasicData(tempString, status, firstEnter);
		xSemaphoreGive(USBMeterState_Mutex);
		if (status == USBMETER_RECORD || status == LEGACY_TEST)
		{
			DisplayRecordData(tempString);
		}
		xSemaphoreGive(OLEDRelatedMutex);
		if ((InternalTemperature > CurrentSettings->InternalTemperature_Max ||
			ExternalTemperature > CurrentSettings->ExternalTemperature_Max) &&
			firstEnter == 0)
		{
			System_OverHeat_Exception_Handler(status, &legacy_Test_Params);
		}
		if (status == LEGACY_TEST)
		{
			if (CurrentMeterData.Voltage * 1000 < legacy_Test_Params.ProtectVolt ||
				CurrentMeterData.Voltage < 0.5)
			{
				StopRecord(&status, 1);
				goto Refresh;
			}
			if (legacy_Test_Params.TestMode == ConstantPower)
			{
				reSendLoadCommandCnt++;
				if (reSendLoadCommandCnt == 5)
				{
					if (CurrentMeterData.Current < 0.1)
						Send_Digital_Load_Command((float)legacy_Test_Params.Power / CurrentMeterData.Voltage, Load_Start);
					else
						Send_Digital_Load_Command((float)legacy_Test_Params.Power / CurrentMeterData.Voltage, Load_Keep);
					reSendLoadCommandCnt = 0;
				}
			}
		}
		if (firstEnter) firstEnter--;
		if (xQueueReceive(Key_Message, &keyMessage, 500 / portTICK_RATE_MS) == pdPASS)
		{
			if (status == USBMETER_ONLY)
			{
				switch (keyMessage.KeyEvent)
				{
				case MidClick: ScrollDialgram_Routine(); break;
				case MidDouble:if (GetConfirmation(RecordConfirm_Str[CurrentSettings->Language], ""))
					StartRecord(&status); break;
				case MidLong:Settings(); break;
				case LeftClick:if (GetConfirmation(StepUpConfirm_Str[CurrentSettings->Language], ""))
					RunAStepUpTest(); break;
				case RightClick:if (GetConfirmation(LegacyTestConfirm_Str[CurrentSettings->Language], ""))
					RunLegacyTest(&status, &legacy_Test_Params); break;
				}
				switch (keyMessage.AdvancedKeyEvent)
				{
				case LeftContinous:
					if (!fastChargeStatus)
					{
						if (GetConfirmation(QCMTKConfirm_Str[CurrentSettings->Language], ""))
							FastChargeTriggerUI(&fastChargeStatus);
					}
					else
					{
						if (GetConfirmation(ReleaseFastCharge_Str[CurrentSettings->Language], ""))
							ReleaseFastCharge(&fastChargeStatus);
					} break;
				case RightContinous:if
					(GetConfirmation(MountUSBMassStorageConfirm_Str[CurrentSettings->Language], ""))
					MassStorage_App(); break;
				}
				goto Refresh;
			}
			else
			{
				xSemaphoreTake(OLEDRelatedMutex, portMAX_DELAY);
				OLED_Clear();
				xSemaphoreGive(OLEDRelatedMutex);
				if (keyMessage.KeyEvent == MidDouble)
				{
					if (GetConfirmation(RecordStopConfirm_Str[CurrentSettings->Language], ""))
					{
						StopRecord(&status, 0);
						goto Refresh;
					}
				}
        else if (keyMessage.KeyEvent == MidClick) 
        {
         ScrollDialgram_Routine();
         goto Refresh;
        }
				else
				{
					ShowDialogue(Hint_Str[CurrentSettings->Language],
						RecordIsRunningHint1_Str[CurrentSettings->Language],
						RecordIsRunningHint2_Str[CurrentSettings->Language], false, false);
					vTaskDelay(1000 / portTICK_RATE_MS);
					xSemaphoreTake(OLEDRelatedMutex, portMAX_DELAY);
					OLED_Clear();
					xSemaphoreGive(OLEDRelatedMutex);
				}
				goto Refresh;
			}
		}
	}
}


/**
  * @brief  Write a new dataset to curve buffer

  * @retval None
  */
void WriteCurrentMeterData2CurveBuff()
{
	/*Increase data pos*/
	Data_p++;

	/*Reset data pos to 0 and clear first cycle flag*/
	if (Data_p == EFFECT_DATA_NUM_PER_SCREEN)
	{
		firstCycle = false;
		Data_p = 0;
	}

	/*Especially,the first data of the forst cycle should also apply to pos 0*/
	if (Data_p == 1 && firstCycle)
	{
		VoltageCurveBuff[0] = FilteredMeterData.Voltage;
		CurrentCurveBuff[0] = FilteredMeterData.Current;
	}

	VoltageCurveBuff[Data_p] = FilteredMeterData.Voltage;
	CurrentCurveBuff[Data_p] = FilteredMeterData.Current;
}

/**
  * @brief  Find the num th data in dataset according to the pos of Data_p

  * @retval None
  */
static float FindxthData(float* data, u16 num)
{
	u16 p;
	p = num + Data_p + 1;
	if (p >= EFFECT_DATA_NUM_PER_SCREEN)
		p = p - EFFECT_DATA_NUM_PER_SCREEN;
	if (firstCycle) p = num;
	return data[p];
}

/**
  * @brief  Draw a single curve in curve buffer

  * @retval None
  */
static void DrawSingleCurveInCurveBuff(float* data, float min, float max)
{
	u8 pixelsPerData = 128 / EFFECT_DATA_NUM_PER_SCREEN;
	u8 currentAddr = 0;
	float currentPosData, nextPosData;
	u8 y1, y2;
	u16 i;

	/*Especially for the first cycle,the effective data length in curve buffers
  should be assigned to Data_p*/
	int numMax = (firstCycle == true) ? (Data_p) : EFFECT_DATA_NUM_PER_SCREEN - 1;

	for (i = 0; i < numMax; i++)
	{
		currentPosData = FindxthData(data, i);
		nextPosData = FindxthData(data, i + 1);
		y1 = 3 + (u8)
			((float)55 * ((max - currentPosData) /
			(float)(max - min)));
		y2 = 3 + (u8)
			((float)55 * ((max - nextPosData) /
			(float)(max - min)));
		OLED_DrawAnyLine(currentAddr, y1, currentAddr + pixelsPerData, y2, DRAW);
		currentAddr = currentAddr + pixelsPerData;
	}

}

/**
  * @brief  Draw max and min indicator and grids

  * @retval None
  */
static void DrawDialgramMinAndMaxAndGrids(float voltMin, float voltMax, float curtMin, float curtMax, bool drawMinAndMax)
{
	char tempString[10];
	u8 length;
	u8 i;

	/*Draw horizonal grids*/
	for (i = 0; i < 6; i++)
	{
		DrawHorizonalDashedGrid(3 + i * 11, DRAW, LowDensity);
	}

	if (drawMinAndMax)
	{
		sprintf(tempString, "%.1fV", voltMax);
		OLED_ShowAnyString(0, 0, tempString, NotOnSelect, 8);
		sprintf(tempString, "%.1fV", voltMin);
		OLED_ShowAnyString(0, 56, tempString, NotOnSelect, 8);

		sprintf(tempString, "%.1fA", curtMax);
		length = GetStringGraphicalLength(tempString);
		OLED_ShowAnyString(127 - 6 * length, 0, tempString, NotOnSelect, 8);

		sprintf(tempString, "%.1fA", curtMin);
		length = GetStringGraphicalLength(tempString);
		OLED_ShowAnyString(127 - 6 * length, 56, tempString, NotOnSelect, 8);
	}
}

/**
  * @brief  Refresh dynamic diagram

  * @retval None
  */
static void RefreshDialgram()
{
	u16 temp;
	float currentMin, currentMax, voltageMin, voltageMax;
	u16 effectiveDataNum;
	if (firstCycle == true) effectiveDataNum = Data_p;
	else effectiveDataNum = EFFECT_DATA_NUM_PER_SCREEN;

	/*Find max and min in current and voltage buffer*/
	currentMin = FindMin(CurrentCurveBuff, effectiveDataNum);
	voltageMin = FindMin(VoltageCurveBuff, effectiveDataNum);
	currentMax = FindMax(CurrentCurveBuff, effectiveDataNum);
	voltageMax = FindMax(VoltageCurveBuff, effectiveDataNum);

	/*Make max and min into n*0.1 for a better display*/
	temp = (currentMax - currentMin) * 10;
	temp = temp + 2;
	currentMin = (float)((u16)(currentMin * 10)) / 10;
	currentMax = currentMin + (float)temp*0.1;
	temp = (voltageMax - voltageMin) * 10;
	temp = temp + 2;
	voltageMin = (float)((u16)(voltageMin * 10)) / 10;
	voltageMax = voltageMin + (float)temp*0.1;

	/*Fixed max and min code,remove for better dynamic display*/
	voltageMin = 0;
	currentMin = 0;
	voltageMax = voltageMax > 12 ? voltageMin : 12;
	currentMax = currentMax > 2 ? currentMax : 2;

	xSemaphoreTake(OLEDRelatedMutex, portMAX_DELAY);
	OLED_Clear();
	DrawSingleCurveInCurveBuff(VoltageCurveBuff, voltageMin, voltageMax);
	DrawSingleCurveInCurveBuff(CurrentCurveBuff, currentMin, currentMax);
	DrawDialgramMinAndMaxAndGrids(voltageMin, voltageMax, currentMin, currentMax, true);
	OLED_Refresh_Gram();
	xSemaphoreGive(OLEDRelatedMutex);
}

/**
  * @brief  Scroll dialgram mode

  * @retval None
  */
static void ScrollDialgram_Routine()
{
	Key_Message_Struct keyMessage;

	/*Initialize Data_p and make firstcycle flag*/
	Data_p = 0;
	firstCycle = true;

	for (;;)
	{
		WriteCurrentMeterData2CurveBuff();
		RefreshDialgram();
		if (xQueueReceive(Key_Message, &keyMessage, 200 / portTICK_RATE_MS) == pdPASS)
		{
			if (keyMessage.KeyEvent == MidClick)
			{
				xSemaphoreTake(OLEDRelatedMutex, portMAX_DELAY);
				OLED_Clear();
				xSemaphoreGive(OLEDRelatedMutex);
				return;
			}
		}
	}

}


/**
  * @brief  Display Volt,Cureent,Power and Temperature
	on the screen
  * @param  a tempString which includes at lease 7 elements
	By sharing the tempString,more sources can be saved
	@retval None
  */
static void DisplayBasicData(char tempString[], u8 currentStatus, u8 firstEnter)
{
	if (FilteredMeterData.Voltage >= 10.0f)
	{
		sprintf(tempString, "%.2fV", FilteredMeterData.Voltage);
	}
	else
	{
		sprintf(tempString, "%.3fV", FilteredMeterData.Voltage);
	}
	OLED_ShowString(0, 0, tempString);
	if (FilteredMeterData.Current > 0.1)
		sprintf(tempString, "%.3fA", FilteredMeterData.Current);
	else
		sprintf(tempString, "%04.1fmA", FilteredMeterData.Current * 1000);
	OLED_ShowString(80, 0, tempString);
	if (FilteredMeterData.Power >= 10.0f)
	{
		sprintf(tempString, "%.2fW ", FilteredMeterData.Power);
	}
	else
	{
		sprintf(tempString, "%.3fW", FilteredMeterData.Power);
	}
	OLED_ShowString(0, 16, tempString);
	if (CurrentTemperatureSensor == Internal)
	{
		GenerateTempString(tempString, Internal);
		if (firstEnter)
			OLED_ShowString(80, 16, "-----C");
		else
		{
			if (tempString[0] != '1')tempString[0] = '0';
			OLED_ShowString(80, 16, tempString);
		}
	}
	else
	{
		GenerateTempString(tempString, External);
		OLED_ShowAnyString(80, 16, "Ex", NotOnSelect, 8);
		if (firstEnter)
			OLED_ShowAnyString(92, 16, "-----C", NotOnSelect, 8);
		else
			OLED_ShowAnyString(92, 16, tempString, NotOnSelect, 8);
		GenerateTempString(tempString, Internal);
		OLED_ShowAnyString(80, 24, "In", NotOnSelect, 8);
		if (firstEnter)
			OLED_ShowAnyString(92, 24, "-----C", NotOnSelect, 8);
		else
			OLED_ShowAnyString(92, 24, tempString, NotOnSelect, 8);
	}
	if (currentStatus == USBMETER_ONLY)
	{
		GenerateRTCDateString(tempString);
		OLED_ShowString(0, 32, tempString);
		GenerateRTCTimeString(tempString);
		OLED_ShowString(0, 48, tempString);
		GenerateRTCWeekString(tempString);
		OLED_ShowString(104, 32, tempString);
		sprintf(tempString, "%5.2fV", CurrentMeterData.VoltageDP);
		OLED_ShowAnyString(92, 48, tempString, NotOnSelect, 8);
		sprintf(tempString, "%5.2fV", CurrentMeterData.VoltageDM);
		OLED_ShowAnyString(92, 56, tempString, NotOnSelect, 8);
		OLED_ShowAnyString(80, 48, "D+", NotOnSelect, 8);
		OLED_ShowAnyString(80, 56, "D-", NotOnSelect, 8);
	}
}

/**
  * @brief  Display Capacity,Work,LastingTime
  * @param  a tempString which includes at lease 9 elements
	By sharing the tempString,more sources can be saved
	@retval None
  */
static void DisplayRecordData(char tempString[])
{
	if (CurrentSumUpData.Capacity >= 10000)
		sprintf(tempString, "%05.2fAh", CurrentSumUpData.Capacity);
	else
		sprintf(tempString, "%05.0fmAh", CurrentSumUpData.Capacity * 1000);
	OLED_ShowString(0, 32, tempString);
	if (CurrentSumUpData.Work >= 10000)
		sprintf(tempString, "%05.2fWh", CurrentSumUpData.Work);
	else
		sprintf(tempString, "%05.0fmWh", CurrentSumUpData.Work * 1000);
	OLED_ShowString(0, 48, tempString);
	GenerateVirtualRTCString(tempString);
	OLED_ShowAnyString(82, 51, tempString, NotOnSelect, 12);
	sprintf(tempString, "%d Day(s)", RTCCurrent.Day);
	OLED_ShowAnyString(82, 35, tempString, NotOnSelect, 12);
}

void USBMeter_Init(u8 status)
{
	CreateTaskWithExceptionControl(USBMeter, "USBMeter",
		384, &status, USB_METER_PRIORITY, NULL);
}
