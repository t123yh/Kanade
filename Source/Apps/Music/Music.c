//File Name     Music.c
//Description : Play Music with buzzer 

#include <stdbool.h>

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

#include "FreeRTOS_Standard_Include.h"

#include "Buzzer.h"

#include "Music.h"

#define SOUND_PLAYER_PRIORITY tskIDLE_PRIORITY+4

//Define the simple title of each pitch					
#define LL1 0
#define LL2 1												 
#define LL3 2
#define LL4 3
#define LL5 4
#define LL6 5
#define LL7 6

#define L1 7
#define L2 8
#define L3 9
#define L4 10
#define L5 11
#define L6 12
#define L7 13

#define M1 14
#define M2 15											 
#define M3 16
#define M4 17
#define M5 18
#define M6 19
#define M7 20

#define H1 21
#define H2 22												 
#define H3 23
#define H4 24
#define H5 25
#define H6 26
#define H7 27	

#define HH1 28	
#define HH2 29	

#define HALF_HIGH 0x80		
#define HALF_LOW 0x40

#define STOP 29

u16 MusicFREQTab[] = { 131,147,165,174,196,220,247,
					  262,294,330,349,392,440,494,
					523,578,659,698,784,880,988,
					1046,1175,1318,1397,1568,1760,1976,
					2092 };

xTaskHandle SoundPlayerHandle = NULL;

const SingleToneStruct Tori_No_Uta[] =
{
{493, 456},
{0,24},
{554, 216},
{0,24},
{587, 204},
{0,36},
{880, 228},
{0,12},
{739, 420},
{0,60},
{739, 204},
{0,36},
{659, 84},
{0,36},
{739, 1080},
{0,480},
{659, 216},
{0,24},
{739, 216},
{0,24},
{880, 216},
{0,24},
{659, 216},
{0,24},
{554, 216},
{0,24},
{587, 228},
{0,12},
{554, 432},
{0,48},
{554, 240},
{493, 84},
{0,36},
{369, 1080},
{0,720},
{493, 468},
{0,12},
{554, 228},
{0,12},
{587, 228},
{0,12},
{880, 204},
{0,36},
{739, 456},
{0,24},
{739, 240},
{659, 84},
{0,36},
{739, 1080},
{0,480},
{659, 228},
{0,12},
{739, 228},
{0,12},
{880, 228},
{0,12},
{739, 228},
{0,12},
{880, 216},
{0,24},
{1174, 228},
{0,12},
{1108, 336},
{0,24},
{987, 336},
{0,24},
{739, 876},
{0,84},
{659, 216},
{0,24},
{739, 468},
{0,12},
{880, 468},
{0,12},
{987, 468},
{0,12},
{1108, 468},
{0,12},
{739, 420},
{0,60},
{739, 204},
{0,36},
{659, 84},
{0,36},
{739, 1080},
{0,480},
{659, 216},
{0,24},
{739, 216},
{0,24},
{880, 216},
{0,24},
{659, 216},
{0,24},
{554, 216},
{0,24},
{587, 228},
{0,12},
{554, 432},
{0,48},
{554, 240},
{493, 84},
{0,36},
{369, 1080},
{0,720},
{493, 468},
{0,12},
{554, 228},
{0,12},
{587, 228},
{0,12},
{880, 204},
{0,36},
{739, 456},
{0,24},
{739, 240},
{659, 84},
{0,36},
{739, 1080},
{0,480},
{659, 228},
{0,12},
{739, 228},
{0,12},
{880, 228},
{0,12},
{739, 228},
{0,12},
{880, 216},
{0,24},
{1174, 228},
{0,12},
{1108, 1680},
{987, 216},
{0,24},
{987, 1200},
{0,240},
{987, 444},
{0,36},
{1108, 1668},
{0,12},
{987, 216},
{0,24},
{987, 1920},
{0,720},
{830, 216},
{0,24},
{830, 312},
{0,48},
{830, 348},
{0,12},
{830, 228},
{0,12},
{830, 324},
{0,36},
{739, 324},
{0,36},
{739, 348},
{0,132},
{830, 480},
{932, 216},
{0,24},
{987, 300},
{0,60},
{932, 300},
{0,60},
{830, 444},
{0,36},
{622, 204},
{0,36},
{622, 1392},
{0,48},
{554, 456},
{0,24},
{493, 480},
{0,720},
{830, 216},
{0,24},
{830, 312},
{0,48},
{830, 348},
{0,12},
{830, 228},
{0,12},
{830, 324},
{0,36},
{739, 324},
{0,36},
{739, 348},
{0,132},
{622, 444},
{0,36},
{739, 204},
{0,36},
{830, 348},
{0,12},
{932, 348},
{0,12},
{987, 2148},
{0,1692},
{830, 216},
{0,24},
{830, 312},
{0,48},
{830, 348},
{0,12},
{830, 228},
{0,12},
{830, 324},
{0,36},
{739, 324},
{0,36},
{739, 348},
{0,132},
{830, 480},
{932, 216},
{0,24},
{987, 300},
{0,60},
{932, 300},
{0,60},
{830, 444},
{0,36},
{622, 204},
{0,36},
{622, 1392},
{0,48},
{554, 456},
{0,24},
{493, 480},
{0,720},
{622, 216},
{0,24},
{622, 312},
{0,48},
{622, 336},
{0,24},
{622, 216},
{0,24},
{554, 336},
{0,24},
{622, 324},
{0,36},
{739, 468},
{0,12},
{622, 480},
{739, 204},
{0,36},
{830, 720},
{0,240},
{987, 444},
{0,36},
{932, 456},
{0,24},
{830, 1056},
{0,384},
{739, 456},
{0,24},
{698, 120},
{0,120},
{698, 120},
{0,120},
{698, 120},
{0,120},
{622, 120},
{698, 840},
{0,720},
{698, 468},
{0,12},
{739, 468},
{0,12},
{830, 468},
{0,12},
{622, 684},
{0,36},
{523, 120},
{0,120},
{523, 1200},
{0,480},
{523, 240},
{554, 192},
{0,48},
{622, 600},
{0,120},
{698, 120},
{0,120},
{698, 120},
{0,120},
{698, 120},
{0,120},
{466, 108},
{0,12},
{698, 2160},
{0,360},
{622, 216},
{0,24},
{554, 216},
{0,24},
{622, 468},
{0,12},
{622, 180},
{0,60},
{554, 204},
{0,36},
{622, 204},
{0,36},
{830, 240},
{0,240},
{830, 1680},
{0,480},
{698, 456},
{0,24},
{698, 216},
{0,24},
{622, 96},
{0,24},
{698, 840},
{0,720},
{698, 468},
{0,12},
{739, 468},
{0,12},
{830, 468},
{0,12},
{622, 684},
{0,36},
{523, 120},
{0,120},
{523, 1200},
{0,480},
{523, 240},
{554, 192},
{0,48},
{622, 600},
{0,120},
{830, 456},
{0,24},
{830, 216},
{0,24},
{739, 96},
{0,24},
{830, 2040},
{0,480},
{739, 216},
{0,24},
{659, 216},
{0,24},
{739, 456},
{0,24},
{739, 180},
{0,60},
{659, 240},
{739, 228},
{0,12},
{830, 456},
{0,24},
{830, 480},
{0,480},
{493, 456},
{0,24},
{554, 216},
{0,24},
{587, 204},
{0,36},
{880, 216},
{0,24},
{739, 420},
{0,60},
{739, 204},
{0,36},
{659, 84},
{0,36},
{739, 1080},
{0,480},
{659, 216},
{0,24},
{739, 216},
{0,24},
{880, 216},
{0,24},
{659, 216},
{0,24},
{554, 216},
{0,24},
{587, 228},
{0,12},
{554, 432},
{0,48},
{554, 240},
{493, 84},
{0,36},
{369, 1080},
{0,720},
{493, 468},
{0,12},
{554, 228},
{0,12},
{587, 228},
{0,12},
{880, 204},
{0,36},
{739, 456},
{0,24},
{739, 240},
{659, 84},
{0,36},
{739, 1080},
{0,480},
{659, 228},
{0,12},
{739, 228},
{0,12},
{880, 228},
{0,12},
{739, 228},
{0,12},
{880, 216},
{0,24},
{1174, 228},
{0,12},
{1108, 336},
{0,24},
{987, 336},
{0,24},
{739, 876},
{0,84},
{659, 216},
{0,24},
{739, 468},
{0,12},
{880, 468},
{0,12},
{987, 468},
{0,12},
{1108, 468},
{0,12},
{739, 420},
{0,60},
{739, 204},
{0,36},
{659, 84},
{0,36},
{739, 1080},
{0,480},
{659, 216},
{0,24},
{739, 216},
{0,24},
{880, 216},
{0,24},
{659, 216},
{0,24},
{554, 216},
{0,24},
{587, 228},
{0,12},
{554, 432},
{0,48},
{554, 240},
{493, 84},
{0,36},
{369, 1080},
{0,720},
{493, 468},
{0,12},
{554, 228},
{0,12},
{587, 228},
{0,12},
{880, 204},
{0,36},
{739, 456},
{0,24},
{739, 240},
{659, 84},
{0,36},
{739, 1080},
{0,480},
{659, 228},
{0,12},
{739, 228},
{0,12},
{880, 228},
{0,12},
{739, 228},
{0,12},
{880, 216},
{0,24},
{1174, 228},
{0,12},
{1108, 1680},
{987, 216},
{0,24},
{987, 1200},
{0,240},
{987, 444},
{0,36},
{1108, 1668},
{0,12},
{987, 216},
{0,24},
{987, 4800},
{0,0}
};

const SingleToneStruct Ichiban_no_takaramono[] =
{
 {50,-7},
 // 
 // {M4,1},{L7|HALF_LOW,2},{M1,2},{M1,1},{M4,1},{M1,2},{M1,2},{M1,1},{M4,1},
 // {M4,1},{L6,2},{L1,2},{M1,1},{M4,1},{M4,1},{M4,1},{L7|HALF_LOW,1},{M1,1},
 // {M4,1},{L7|HALF_LOW,2},{M1,2},{M1,1},{M4,1},{M4,1},{M1,2},{M1,2},{M1,1},
 // {M4,1},{M4,1},{L6,2},{M1,2},{M1,1},{M4,1},{M4,1},{M4,1},{M1,1},
 // {M1,1},//pre

  {M6,2},{M7 | HALF_LOW,2},{H1,1},{M7 | HALF_LOW,2},{H1,2},{H1,2},{M6,2},{M5,2},{M4,2},
  {M4,1},{STOP,1},{M6 | HALF_LOW,2},{M7,2},{H1,1},{H1,2},{H1,2},{H1,2},{H4,2},
  {H4,2},{H4,2},{H3,1},{STOP,1},{M6 | HALF_LOW,2},{M7,2},{H1,1},{M7 | HALF_LOW,2},{H1,2},
  {H1,2},{M6,2},{M5,2},{M5,2},{M4,1},{M4,2},{M5,1},{M5,2},{M6,1},
  {M6,1},{M6,2},{M6,1},{M6,1},{STOP,2},{M6,2},{M7 | HALF_LOW,2},{M6,2},//main1

  {H1,1},{H2,2},{H1,1},{H3,1},{H4,2},{H4,2},{H1,1},{H4,1},{H5,1},
  {H6,2},{H6,2},{H7 | HALF_LOW,1},{H6,1},{H4,1},{H1,2},{H1,2},{H4,2},{H4,1},
  {H4,2},{H6,2},{H7 | HALF_LOW,1},{HH1,1},{H5,2},{H4,1},{H2,1},{H1,1},{H1,2},
  {H4,2},{H4,1},{H4,2},{H6,2},{H7 | HALF_LOW,1},{HH1,1},{H5,2},{H4,1},{H3 | HALF_LOW,1},
  {H4,2},{H4,2},{H6,2},{H7 | HALF_LOW,1},{H6,2},{H7 | HALF_LOW,1},//main2

  {HH1,1},{HH1,1},{HH1,1},{HH1,1},{STOP,1},{STOP,1},{STOP,1},{STOP,1},
  //{STOP,1},{STOP,1},{STOP,1},{STOP,1},{STOP,1},{STOP,1},{STOP,2},
  {H6,1},{H5 | HALF_HIGH,2},{H6,1},{H3,2},{H2,1},{H6,1},{H7,2},{H7,2},{HH1 | HALF_HIGH,1},
  {HH1 | HALF_HIGH,1},{H6,1},{H5 | HALF_HIGH,2},{H6,1},{H3,2},{H2,1},{H6,1},{H7,2},{H7,2},
  {HH1 | HALF_HIGH,1},{HH1 | HALF_HIGH,1},{H6,1},{H5 | HALF_HIGH,2},//main3

  {H6,1},{H3,2},{H2,1},{H6,1},{H7,2},{H7,2},{HH1 | HALF_HIGH,1},{HH1 | HALF_HIGH,2},{H6,2},
  {H7,1},{H6,2},{H6,2},{H2,1},{H1 | HALF_HIGH,1},{M7,1},{M7,2},{M7,2},{M6,2},
  {M6,1},{STOP,2},{H6,1},{H5 | HALF_HIGH,2},{H6,1},{H3,2},{H2,1},{H6,1},{H7,2},
  {H7,2},{HH1 | HALF_HIGH,1},{HH1 | HALF_HIGH,1},{H6,1},{H5 | HALF_HIGH,2},//main4

  {H6,1},{H3,2},{H2,1},{H6,1},{H7,2},{H7,2},{HH1 | HALF_HIGH,2},{H6,1},{STOP,2},
  {H6,1},{H5 | HALF_HIGH,2},{H6,1},{H3,2},{H2,1},{H6,1},{H7,2},{H7,2},{HH1 | HALF_HIGH,1},
  {HH1 | HALF_HIGH,2},{H6,2},{H7,1},{H6,2},{H6,2},{H2,1},{H1 | HALF_HIGH,1},
  {M7,1},{M7,2},{M7,2},{M6,2},{M6,1},{M6,1},{STOP,1},//main5

 // {H7,1},{H4|HALF_HIGH,2},{H3,1},{H7,1},{HH1|HALF_HIGH,2},{HH1|HALF_HIGH,2},
 // {HH2|HALF_HIGH,2},{HH2|HALF_HIGH,2},{H7,1},{H6|HALF_HIGH,2},//main6

  {0,0}
};

const SingleToneStruct Alarm[] =
{
 {75,0},
 {M7,8},
 {STOP,8},
 {M7,8},
 {STOP,8},
 {M7,8},
 {STOP,8}, {STOP,1}, {STOP,1}, {STOP,1}, {STOP,1}, {STOP,1}, {STOP,1},
 {0,0}
};

/**
  * @brief  Play sound
  * @retval : None
  */

static void SoundPlayer(void *pvParameters)
{
	//TIM_TimeBaseInitTypeDef  TIM4_TimeBaseStructure;
	//TIM_OCInitTypeDef TimOCInitStructure;
	portTickType xLastWakeTime;
	int nextFREQ;
	int nextTime;
	int addr;
	SingleToneStruct * sound = pvParameters;
	addr = 1;
	xLastWakeTime = xTaskGetTickCount();
	while (1)
	{
		if (sound[addr].LastingTime != 0) //If it is not the end of the score
		{
		  nextFREQ = sound[addr].NoteFreq;//Get the FREQ of the tone in the next pitch
			nextTime = sound[addr].LastingTime;//Calculate the time of the next tone 
		}
		else
		{
			nextFREQ = sound[0].NoteFreq;//Go to the start of the score
			nextTime = sound[0].LastingTime;//The first pitch of the score
			addr = 1;
		}
		TIM_Cmd(TIM4, DISABLE);//Turn off the timer for safety
//		TIM4_TimeBaseStructure.TIM_Prescaler = 10;
//		TIM4_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//		TIM4_TimeBaseStructure.TIM_Period = (100000 * 72) / nextFREQ - 1;//Get the countValue according to FREQ
//		TIM4_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//		TIM4_TimeBaseStructure.TIM_RepetitionCounter = 0x0;
//		TIM_TimeBaseInit(TIM4, &TIM4_TimeBaseStructure);
//		TIM_OCStructInit(&TimOCInitStructure);
//		TimOCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//		TimOCInitStructure.TIM_Pulse = (50000 * 72) / nextFREQ;
//		TimOCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//		TimOCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//		TIM_OC1Init(TIM4, &TimOCInitStructure);
		TIM4->CCR1 = (50000 * 72) / nextFREQ;
		TIM4->ARR = (100000 * 72) / nextFREQ - 1;
		TIM4->PSC = 10;
		//TIM_CtrlPWMOutputs(TIM4, ENABLE);
		if (sound[addr].NoteFreq != 0)
			TIM_Cmd(TIM4, ENABLE);
		addr++;
		vTaskDelayUntil(&xLastWakeTime, nextTime / portTICK_RATE_MS);
	}
}


/**
  * @brief  Start playing music

  * @retval : TimerNo for kanade
  */
void SoundStart(const SingleToneStruct sound[])
{
	BuzzerConfig();
	if (SoundPlayerHandle != NULL)
	{
		vTaskDelete(SoundPlayerHandle);
	}
	xTaskCreate(SoundPlayer, "Sound Player",
		128, (SingleToneStruct*)sound, SOUND_PLAYER_PRIORITY, &SoundPlayerHandle);
}

/**
  * @brief   Stop playing music

  * @Param : None

  * @retval: None
  */
void SoundStop(void)
{
	TIM_DeInit(TIM4);
	if (SoundPlayerHandle != NULL)
		vTaskDelete(SoundPlayerHandle);
	SoundPlayerHandle = NULL;
}
