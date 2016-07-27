//File Name   Cooling_Fan.c
//Description Cooling_Fan App 

#include "stm32f10x_gpio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "TempSensors.h"

#include "Cooling_Fan.h"

#define COOLING_FAN_SERVICE_PRIORITY tskIDLE_PRIORITY+6

#define Cooling_fan_turn_on() GPIO_SetBits(GPIOA, GPIO_Pin_15)
#define Cooling_fan_turn_off() GPIO_ResetBits(GPIOA, GPIO_Pin_15)

void Cooling_Fan_GPIO_Init()
{
 GPIO_InitTypeDef GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Cooling_Fan_Service(void *pvParameters)
{
 for(;;)
	{
	 if(InternalTemperature>50)Cooling_fan_turn_on();
	 else Cooling_fan_turn_off();
	 vTaskDelay(500/portTICK_RATE_MS);
	}
}

void Cooling_Fan_Service_Init(void)
{
	Cooling_Fan_GPIO_Init();
	CreateTaskWithExceptionControl(Cooling_Fan_Service, "Cooling_Fan_Service",
		128, NULL, COOLING_FAN_SERVICE_PRIORITY, NULL);
}