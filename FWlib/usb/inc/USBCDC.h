//USBCDC for STM32
#ifndef _USBCDC_H__
#define _USBCDC_H__

#include "platform.h"
#include <stdbool.h>

void USBCDC_Init(void); //��ʼ��USBCDC���⴮�� 

bool USBCDC_ready(void);//��ȡ����״̬,TRUE��ʾ����ͨѶ 

void USBCDC_connect(void);//��USB����
void USBCDC_disconnect(void);//�Ͽ�USB����

void USBCDC_senddata(uint8 *pdata,uint16 length);//������
uint16 USBCDC_recvdata(uint8 *pdata, uint16 limit);//������,����ʵ�ʽ��յ������ݳ���
//limit��pdata��������
void USBCDC_sync(void);//�ȴ�������� 

void __USBCDC_ISR(void);//USB�жϷ���ʱ����
//��usb_istr.c��ʵ��

void USBCDC_SendString(char string[]);

#endif
