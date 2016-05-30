/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

//#include "stm32f10x_
#include "misc.h"
#include "stm32f10x_spi.h"

/* ISR Priority 0(highest)-15(lowest)*/
//����ԽС���ȼ�Խ��
#define DMA_CHANEL_Priority 6
#define USART1_IRQn_Priority 7
#define USART2_IRQn_Priority 8
#define USB_IRQn_Priority 9
#define EXTI15_10_IRQn_Priority 10
#define TIME3_IRQn_Priority 11
#define TIME4_IRQn_Priority 12
#define RTC_IRQn_Priority 13
#define EXIT9_5_IRQn_Priority 14

//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 


//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 


//���� GPIO PORT ��������� �ڳ�ʼ���� ��ȫ���������
typedef enum 
{
	LED_RUN_PIN = 0,		//PB14
	LED_TX_PIN,				//PB15
	LED_RX_PIN,				//PA8
	BEEP_PIN,				//PC13
	USB_DISCONNECT_PIN,		//PA15
	S485EN_PIN				//PA1
}GPIO_PIN_DEF;

typedef enum
{ 					
	ExKey1=0,		 
	ExKey2,	
}ExKeY;

static GPIO_TypeDef* GPIO_OUT_PORT[6]={GPIOB, GPIOB,GPIOA,GPIOC,GPIOA,GPIOA};
static const u16 GPIO_OUT_PIN[6]={GPIO_Pin_14, GPIO_Pin_15,GPIO_Pin_8,GPIO_Pin_13,GPIO_Pin_15,GPIO_Pin_1};

#define GPIO_OH(pin)	GPIO_SetBits(GPIO_OUT_PORT[pin], GPIO_OUT_PIN[pin]);	//�����
#define GPIO_OL(pin)	GPIO_ResetBits(GPIO_OUT_PORT[pin], GPIO_OUT_PIN[pin]); //�����

#define RdKey1()		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
#define RdKey2()		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)

#define ReadBit(reg,bit) ((reg>>bit)&1)

#endif /* __PLATFORM_CONFIG_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
