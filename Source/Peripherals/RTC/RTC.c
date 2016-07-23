#include "stm32f10x_rtc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"
#include "misc.h"
#include "stdio.h"
#include "stdbool.h"
#include "SSD1306.h"
#include "string.h"
#include "rtc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define RTC_UPDATE_PRIORITY tskIDLE_PRIORITY+2

volatile struct Data_Time RTCTime;  //����һ��ʱ��ṹ�����

void(*RTCUpdateFunctions[3])(void);

bool RTCUpdateExecute[3] = { false,false };

/**
  * @brief  Init hardware of RTC

  * @param  none

  * @retval None
  */
u8 RTC_Hardware_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);						

	//BKP_DeInit();
	
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)						//��ָ���ĺ󱸼Ĵ����ж������ݣ��ж��Ƿ�Ϊ��һ������
	{
		//printf("ʱ�����á�����\r\n");																
		BKP_DeInit();												//������BKP��ȫ���Ĵ�������Ϊȱʡֵ 	
		RCC_LSEConfig(RCC_LSE_ON);									//ʹ���ⲿ����ʱ�� 32.768KHz
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)			//���ָ����RCC��־λ�������,�ȴ����پ������
		{
		}
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);						//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);										//ʹ��RTCʱ��  
		RTC_WaitForSynchro();										//�ȴ�RTC�Ĵ���(RTC_CNT,RTC_ALR��RTC_PRL)��RTC APBʱ��ͬ��
		RTC_WaitForLastTask();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_ITConfig(RTC_IT_SEC, ENABLE);							//ʹ��RTC���ж�
		RTC_WaitForLastTask();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_SetPrescaler(32767); 									//����RTCԤ��Ƶ��ֵ  RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
		RTC_WaitForLastTask();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		Time_Set();													//ʱ������	
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);					//��ָ���ĺ󱸼Ĵ�����д���û���������0X5555���жϱ�־										
	}
	else															//���ǵ�һ������ ������ʱ
	{
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)			//���ָ����RCC��־λ�������:POR/PDR��λ
		{
			;
		}
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)		//���ָ����RCC��־λ�������:�ܽŸ�λ
		{
			;
		}
		RCC_ClearFlag();

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);		//��ռ���ȼ�1λ,�����ȼ�3λ

		NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTCȫ���ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�1λ,�����ȼ�3λ
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	//��ռ���ȼ�0λ,�����ȼ�4λ
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//ʹ�ܸ�ͨ���ж�
		NVIC_Init(&NVIC_InitStructure);		//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

		RTC_WaitForSynchro();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������

		RTC_ITConfig(RTC_IT_SEC, ENABLE);							//ʹ��RTC���ж�

		RTC_WaitForLastTask();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	}
	Time_Get();														//����ʱ�� 

	RCC_ClearFlag();												//���RCC�ĸ�λ��־λ

	return 0; //ok		
}


u8 const table_week[12] = { 0,3,3,6,1,4,6,2,5,0,3,5 }; 			

const u8 mon_table[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };	


/**
  * @brief  Update time

  * @param  none

  * @retval None
  */
u8 Time_Update(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec)
{
	u16 t;
	u32 seccount = 0;
	if (syear < 1970 || syear>2099)return 1;
	for (t = 1970; t < syear; t++)											//��������ݵ��������
	{
		if (Is_Leap_Year(t))seccount += 31622400;						//�����������
		else seccount += 31536000;			  						//ƽ���������
	}
	smon -= 1;
	for (t = 0; t < smon; t++)	   											//��ǰ���·ݵ����������
	{
		seccount += (u32)mon_table[t] * 86400;						//�·����������
		if (Is_Leap_Year(syear) && t == 1)seccount += 86400;				//����2�·�����һ���������	   
	}
	seccount += (u32)(sday - 1) * 86400;								//��ǰ�����ڵ���������� 
	seccount += (u32)hour * 3600;									//Сʱ������
	seccount += (u32)min * 60;	 									//����������
	seccount += sec;													//�������Ӽ���ȥ

	RTC_WaitForLastTask();											//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	RTC_SetCounter(seccount);										//����RTC��������ֵ
	RTC_WaitForLastTask();											//�ȴ����һ�ζ�RTC�Ĵ�����д�������  	
	return 0;
}
//�õ���ǰ��ʱ��
//����ֵ:0,�ɹ�;����:�������.
u8 Time_Get(void)
{
	static u16 daycnt = 0;
	u32 timecount = 0;
	u32 temp = 0;
	u16 temp1 = 0;

	timecount = RTC_GetCounter();  //��� RTC ������ֵ(������)											 
	temp = timecount / 86400;   //�õ�����(��������Ӧ��)
	if (daycnt != temp)//����һ����
	{
		daycnt = temp;
		temp1 = 1970;	//��1970�꿪ʼ
		while (temp >= 365)
		{
			if (Is_Leap_Year(temp1))//������
			{
				if (temp >= 366)temp -= 366;//�����������
				else { temp1++; break; }
			}
			else temp -= 365;	  //ƽ�� 
			temp1++;
		}
		RTCTime.w_year = temp1;//�õ����
		temp1 = 0;
		while (temp >= 28)//������һ����
		{
			if (Is_Leap_Year(RTCTime.w_year) && temp1 == 1)//�����ǲ�������/2�·�
			{
				if (temp >= 29)temp -= 29;//�����������
				else break;
			}
			else
			{
				if (temp >= mon_table[temp1])temp -= mon_table[temp1];//ƽ��
				else break;
			}
			temp1++;
		}
		RTCTime.w_month = temp1 + 1;//�õ��·�
		RTCTime.w_date = temp + 1;  //�õ����� 
	}
	temp = timecount % 86400;     //�õ�������   	   
	RTCTime.hour = temp / 3600;     //Сʱ
	RTCTime.min = (temp % 3600) / 60; //����	
	RTCTime.sec = (temp % 3600) % 60; //����
	RTCTime.week = RTC_Get_Week(RTCTime.w_year, RTCTime.w_month, RTCTime.w_date);//��ȡ����   
	return 0;
}

/**
  * @brief  Get week from given date

  * @param  date

  * @retval mon(0)-sun(6)
  */
u8 RTC_Get_Week(u16 year, u8 month, u8 day)
{
	u16 temp2;
	u8 yearH, yearL;

	yearH = year / 100;	yearL = year % 100;
	if (yearH > 19)yearL += 100;

	temp2 = yearL + yearL / 4;
	temp2 = temp2 % 7;
	temp2 = temp2 + day + table_week[month - 1];
	if (yearL % 4 == 0 && month < 3)temp2--;
	return(temp2 % 7);
}

/**
  * @brief  Judge if the given year is a leap year

  * @param  year

  * @retval 1(yes) 0(no)
  */
u8 Is_Leap_Year(u16 year)
{
	if (year % 4 == 0)
	{
		if (year % 100 == 0)
		{
			if (year % 400 == 0)return 1;
			else return 0;
		}
		else return 1;
	}
	else return 0;
}

/**
  * @brief  RTC ISR

  * @param  none

  * @retval None
  */
void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_ALR))			//Alarm interrupt		
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);//Clear alarm interrupt						 				
	}
	RTC_ClearITPendingBit(RTC_IT_SEC);
	RTC_WaitForLastTask();//Wait for RTC register operation
}

/**
  * @brief  Set default time

  * @param  none

  * @retval None
  */
void Time_Set(void)
{
	Time_Update(2015, 1, 1, 0, 0, 0);
}

/**
  * @brief  Generate timestamp for date

  * @param  target string

  * @retval None
  */
void GenerateRTCDateString(char string[])
{
	sprintf(string, "%04d.%02d.%02d", RTCTime.w_year, RTCTime.w_month, RTCTime.w_date);
}

/**
  * @brief  Generate timestamp for time

  * @param  target string

  * @retval None
  */
void GenerateRTCTimeString(char string[])
{
	sprintf(string, "%02d:%02d:%02d", RTCTime.hour, RTCTime.min, RTCTime.sec);
}

/**
  * @brief  Generate timestamp for week

  * @param  target string

  * @retval None
  */
void GenerateRTCWeekString(char string[])
{
	switch (RTCTime.week)
	{
	case 1:strcpy(string, "Mon"); break;
	case 2:strcpy(string, "Tue"); break;
	case 3:strcpy(string, "Wed"); break;
	case 4:strcpy(string, "Thu"); break;
	case 5:strcpy(string, "Fri"); break;
	case 6:strcpy(string, "Sat"); break;
	case 0:strcpy(string, "Sun"); break;
	default:strcpy(string, "Err");
	}
}

//void TimeSettings(void)
//{
// struct Data_Time newTime;
// OLED_Clear();
// OLED_DrawRect(4, 20, 123, 44, DRAW);
// OLED_ShowString(8, 24, "Clock Settings"); OLED_Clear();
// newTime.w_year=GetParam("Year=",2016,2099,1,RTCTime.w_year,"year",33,10);
// newTime.w_month=GetParam("Month=",1,12,1,RTCTime.w_month,"",33,10);
// if(newTime.w_year%4==0 && newTime.w_month==2)
// {
//  newTime.w_date=GetParam("Day=",1,29,1,RTCTime.w_date,"",33,10);  
// }
// else
// newTime.w_date=GetParam("Day=",1,mon_table[newTime.w_month-1],1,RTCTime.w_date,"",33,10);  	 
// newTime.hour=GetParam("Hour=",0,23,1,RTCTime.hour,"",33,10);  	 
// newTime.min=GetParam("Min=",0,59,1,RTCTime.min,"",33,10);  	
// newTime.sec=GetParam("Sec=",0,59,1,RTCTime.sec,"",33,10);   
// Time_Update(newTime.w_year,newTime.w_month,newTime.w_date,
// newTime.hour,newTime.min,newTime.sec);
// OLED_Clear();
//}

/**
  * @brief  Check RTC time,call timeSettings() if incorrect

  * @param  None

  * @retval None
  */
void RTCCheck(void)
{
	if (RTCTime.w_year < 2016)
	{
		//TimeSettings();
	}
}

/**
  * @brief  Task which handles the update of the RTC

  * @param  None

  * @retval None
  */
void RTCUpdateHandler(void *pvParameters)
{
	while (1)
	{
		vTaskDelay(500 / portTICK_RATE_MS);
		Time_Get();
	}
}

/**
  * @brief  Init RTC

  * @param  None

  * @retval None
  */
void RTC_Init(void)
{
	RTC_Hardware_Init();
	xTaskCreate(RTCUpdateHandler, "RTC Update Handler",
		128, NULL, RTC_UPDATE_PRIORITY, NULL);
}
