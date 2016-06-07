//File Name   ��SSD1306.c
//Description ��SSD1306 driver and basic GUI functions

#include "SSD1306.h"
#include "stdlib.h"
#include "SSD1306Font.h"  
#include "stm32f10x_tim.h"
#include "stm32f10x.h"
#include "stm32f10x_spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"  

#include "Taiwanese.h"
#include "JapanChinese.h"
#include "Japanese.h"

#include <string.h>
#include <stdio.h>

#define OLED_REFRESH_PRIORITY tskIDLE_PRIORITY+2

#define English 0


xSemaphoreHandle OLEDRelatedMutex=NULL;

u8 Language=Taiwanese;

//#include "delay.h"

//GRAM of OLED
//Save with the format as.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   
unsigned char  OLED_GRAM[128][8];

//unsigned char  OLED_GRAM_Backup[128][8];

volatile bool GRAM_Changing = false;

volatile bool GRAM_Changed = false;

volatile bool GRAM_No_Change_Timeout = false;

volatile bool UpdateOLEDJustNow = false;

volatile bool UpdateOLEDJustNow_Backup= false;

/**
  * @brief  Read/Write single byte to/from SPI2

	  @retval Received data
  */
unsigned char SPI2_ReadWriteByte(unsigned char TxData)
{
	unsigned int retry = 0;
	while ((SPI2->SR & 1 << 1) == 0)//�ȴ���������	
	{
		retry++;
		if (retry > 2000)return 0;
	}
	SPI2->DR = TxData;	 	  //����һ��byte 
	retry = 0;
	while ((SPI2->SR & 1 << 0) == 0) //�ȴ�������һ��byte  
	{
		retry++;
		if (retry > 2000)return 0;
	}
	return SPI2->DR;          //�����յ�������				    
}


/**
  * @brief  Upload GRAM to OLED immediately

	  @retval None
  */
void OLED_Refresh_Gram(void)
{
	unsigned char  i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD);
#if OLED_MODEL
		OLED_WR_Byte(0x02, OLED_CMD);
#else
		OLED_WR_Byte(0x00, OLED_CMD);
#endif
		OLED_WR_Byte(0x10, OLED_CMD);
		for (n = 0; n < 128; n++)OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
	}
}

/**
  * @brief  Write a byte to OLED

	@params dat: 8-bit data being transferred

			cmd: Command or data select

	  @retval None
  */
void OLED_WR_Byte(unsigned char  dat, unsigned char  cmd)
{
	if (cmd)
		OLED_RS_Set();
	else
		OLED_RS_Clr();
	OLED_CS_Clr();

	SPI2_ReadWriteByte(dat);

	OLED_CS_Set();
	OLED_RS_Set();
}

/**
  * @brief  Turn on display

	  @retval None
  */
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //Set DCDC Command
	OLED_WR_Byte(0X14, OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF, OLED_CMD);  //DISPLAY ON
}

/**
  * @brief  Turn off display

	  @retval None
  */
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10, OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE, OLED_CMD);  //DISPLAY OFF
}

/**
  * @brief  Clear whole screen

	  @retval None
  */
void OLED_Clear(void)
{
	unsigned char  i, n;
	for (i = 0; i < 8; i++)for (n = 0; n < 128; n++)OLED_GRAM[n][i] = 0X00;
#if OLED_REFRESH_OPTIMIZE_EN
	if (UpdateOLEDJustNow)
		OLED_Refresh_Gram();//Upload GRAM to screen
#else
	OLED_Refresh_Gram();
#endif
}

/**
  * @brief  Set position

	@param  x(from 0 to 127): X position

			y(from 0 to 63) : Y position

	  @retval None
  */
void LCD_Set_Pos(unsigned char  x, unsigned char  y)
{
#if OLED_MODEL
	x = x + 2;
#endif
	OLED_WR_Byte(0xb0 + (7 - y / 8), OLED_CMD);
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte((x & 0x0f), OLED_CMD);
}

/**
  * @brief  Draw/unDraw a single point

	@param  x(from 0 to 127): X position

			y(from 0 to 63) : Y position

			t:Draw(1) or unDraw(0)

	  @retval None
  */
void OLED_DrawPoint(u8  x, u8  y, u8  t)
{
	u8  pos, bx, temp = 0;
#if OLED_REFRESH_OPTIMIZE_EN
	if (UpdateOLEDJustNow == true)
		LCD_Set_Pos(x, y);
#else
	LCD_Set_Pos(x, y);
#endif
	if (x > 127 || y > 63)return;//Out of range
	pos = 7 - y / 8;
	bx = y % 8;
	temp = 1 << (7 - bx);
	if (t)OLED_GRAM[x][pos] |= temp;
	else OLED_GRAM[x][pos] &= ~temp;
#if OLED_REFRESH_OPTIMIZE_EN	
	if (UpdateOLEDJustNow == true)
		OLED_WR_Byte(OLED_GRAM[x][pos], OLED_DATA);
	else
	{
		GRAM_Changing = true;
		GRAM_Changed = true;
	}
#else
	OLED_WR_Byte(OLED_GRAM[x][pos], OLED_DATA);
#endif
}

/**
  * @brief  Invert a pixel

	@param  x(from 0 to 127): X position

			y(from 0 to 63) : Y position

	  @retval None
  */
void OLED_InvertPoint(u8 x, u8 y)
{
	unsigned char  pos, bx, temp = 0;
#if OLED_REFRESH_OPTIMIZE_EN
	if (UpdateOLEDJustNow == true)
		LCD_Set_Pos(x, y);
#else
	LCD_Set_Pos(x, y);
#endif
	if (x > 127 || y > 64)return;//Out of range
	pos = 7 - y / 8;
	bx = y % 8;
	temp = 1 << (7 - bx);
	if ((OLED_GRAM[x][pos] & temp) > 0)
	{
		OLED_GRAM[x][pos] &= ~temp;
	}
	else OLED_GRAM[x][pos] |= temp;
#if OLED_REFRESH_OPTIMIZE_EN	
	if (UpdateOLEDJustNow == true)
		OLED_WR_Byte(OLED_GRAM[x][pos], OLED_DATA);
	else
	{
		GRAM_Changing = true;
		GRAM_Changed = true;
	}
#else
	OLED_WR_Byte(OLED_GRAM[x][pos], OLED_DATA);
#endif
}

/**
  * @brief  Draw a vertical line

	@param  x(from 0 to 127): Public X position

			y1(from 0 to 63): Start of line

			y2(from 0 to 63): End of line

			t:Draw(1) or unDraw(0)

			y1,y2 satisfies y1<y2

	  @retval None
  */
void OLED_DrawVerticalLine(unsigned char  x, unsigned char  y1, unsigned char y2, unsigned char  t)
{
	for (; y1 < y2 + 1; y1++)
	{
		OLED_DrawPoint(x, y1, t);
	}
}

/**
  * @brief  Draw a horizonal line

	@param  y(from 0 to 127): Public Y position

			x1(from 0 to 63): Start of line

			x2(from 0 to 63): End of line

			t:Draw(1) or unDraw(0)

			x1,x2 satisfies x1<x2

	  @retval None
  */
void OLED_DrawHorizonalLine(unsigned char  y, unsigned char  x1, unsigned char x2, unsigned char  t)
{
	for (; x1 < x2 + 1; x1++)
	{
		OLED_DrawPoint(x1, y, t);
	}
}

/**
  * @brief  Draw a rectangle

	@param  x1,y1:Position1

			x2,y2:Position2

			drawOrUnDraw:Draw(1) or unDraw(1)

			x1,y1,x2,y2 satisfies x1<x2 y1<y2

	  @retval None
  */
void OLED_DrawRect(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, u8 drawOrUnDraw)
{
	OLED_DrawHorizonalLine(y1, x1, x2, drawOrUnDraw);
	OLED_DrawHorizonalLine(y2, x1, x2, drawOrUnDraw);
	OLED_DrawVerticalLine(x1, y1, y2, drawOrUnDraw);
	OLED_DrawVerticalLine(x2, y1, y2, drawOrUnDraw);
}

/**
  * @brief  Show a char withgiven size

	@param  x,y:Position

			chr:Char

			size:Char size in y 16,12,8 are selectable

			mode:Draw(1) or unDraw(0)

	  @retval None
  */
void OLED_ShowChar(unsigned char  x, unsigned char  y, unsigned char  chr, unsigned char  size, unsigned char  mode)
{
	unsigned char  temp, t, t1, m;
	unsigned char  y0 = y;
	chr = chr - ' ';
	if (size == 8) m = 6;
	else m = size;
	for (t = 0; t < m; t++)
	{
		if (size == 12)temp = oled_asc2_1206[chr][t]; //1206
		else if (size == 16)temp = oled_asc2_1608[chr][t];//1608
		else  temp = oled_asc2_0806[chr][t];//0806
		for (t1 = 0; t1 < 8; t1++)
		{
			if (temp & 0x80)OLED_DrawPoint(x, y, mode);
			else OLED_DrawPoint(x, y, !mode);
			temp <<= 1;
			y++;
			if ((y - y0) == size)
			{
				y = y0;
				x++;
				break;
			}
		}
	}
}

/**
  * @brief  Show a Chinese char or a 16*16 icon

	@param  x,y:Position

			no:Icon No

			drawOrUnDraw:Draw(1) or unDraw(0)

	  @retval None
  */
void OLED_ShowCHinese(unsigned char  x, unsigned char  y, unsigned char  no, unsigned char drawOrUnDraw)
{
	unsigned char  temp, t, t1;
	unsigned char  y0 = y;
	for (t = 0; t < 32; t++)
	{

		temp = CHINESE[no][t];		 
		for (t1 = 0; t1 < 8; t1++)
		{
			if (temp & 0x80)OLED_DrawPoint(x, y, drawOrUnDraw);
			else OLED_DrawPoint(x, y, !drawOrUnDraw);
			temp <<= 1;
			y++;
			if ((y - y0) == 16)
			{
				y = y0;
				x++;
				break;
			}
		}
	}
}

/**
  * @brief  Power function

	  @retval Result
  */
unsigned int oled_pow(unsigned char  m, unsigned char  n)
{
	unsigned int result = 1;
	while (n--)result *= m;
	return result;
}


/**
  * @brief  Show string in 1608

	@param  x,y:Position

			*p:Target string

	  @retval None
  */
void OLED_ShowString(unsigned char  x, unsigned char  y, const char  *p)
{
#define MAX_CHAR_POSX 127
#define MAX_CHAR_POSY 63         
	while (*p != '\0')
	{
		if (x > MAX_CHAR_POSX) { x = 0; y += 16; }
		if (y > MAX_CHAR_POSY) { y = x = 0; OLED_Clear(); }
		OLED_ShowChar(x, y, *p, 16, 1);
		x += 8;
		p++;
	}
}

void OLED_ShowNotASCChar(unsigned char  x, unsigned char  y, char *chr, unsigned char  size, unsigned char  mode)
{
 u16 addr;
 unsigned char  temp, t, t1, m;
 unsigned char  y0 = y;
 switch(Language)
 {
	 case Taiwanese:addr=GetTaiwaneseAddr((s8 *)chr,size);break;
	 case Japanchinese:addr=GetJapanchineseAddr((s8 *)chr,size);break;
	 case Japanese:addr=GetJapaneseAddr((s8 *)chr,size);break;
 }
 if (size == 12) m = 24;
 else if (size==16) m=32;
 else m=0;
  temp=255;
	for (t = 0; t < m; t++)
	{
		switch(Language)
		{
		  case Taiwanese:if(size==12) temp=TaiwaneseTab12[addr].Msk[t];else temp=TaiwaneseTab16[addr].Msk[t];break;
			case Japanchinese:if(size==12) temp=JapanchineseTab12[addr].Msk[t];else temp=JapanchineseTab16[addr].Msk[t];break;
			case Japanese:if(size==12) temp=JapaneseTab12[addr].Msk[t];else temp=JapaneseTab16[addr].Msk[t];break;
		}
//		if (size == 12)temp = oled_asc2_1206[chr][t]; //1206
//		else if (size == 16)temp = oled_asc2_1608[chr][t];//1608
//		else  temp = oled_asc2_0806[chr][t];//0806
		for (t1 = 0; t1 < 8; t1++)
		{
			if (temp & 0x80)OLED_DrawPoint(x, y, mode);
			else OLED_DrawPoint(x, y, !mode);
			temp <<= 1;
			y++;
			if ((y - y0) == size)
			{
				y = y0;
				x++;
				break;
			}
		}
	}
}

/**
  * @brief  Show string with selection options

	@param  x,y:Position

			*p:Target string

			OnSelection:Invert(OnSelect) or not(NotOnSelect)

			size:Size of the string(in Y)

	  @retval None
  */
void OLED_ShowAnyString(unsigned char  x, unsigned char  y, const char  *p, bool OnSelection, unsigned char size)
{
#define MAX_CHAR_POSX 127
#define MAX_CHAR_POSY 63         
	while (*p != '\0')
	{
		if (x > MAX_CHAR_POSX) { x = 0; y += 16; }
		if (y > MAX_CHAR_POSY) { y = x = 0; OLED_Clear(); }
		if (*p>128) 
		{
			OLED_ShowNotASCChar(x, y,(char *) p, size, (!OnSelection));
			if (size == 16)
				x += 16;
			else
			x += 12;
			p++;p++;
		}
		else
		{
		OLED_ShowChar(x, y, *p, size, (!OnSelection));
		if (size == 16)
			x += 8;
		else
			x += 6;
		p++;
	}
	}
}

/**
  * @brief  Invert a rectangular range

	@param  x1,y1:Position1

			x2,y2:Position2

			x1,y1,x2,y2 satisfies x1<x2,y1<y2

	  @retval None
  */
void OLED_InvertRect(u8 x1, u8 y1, u8 x2, u8 y2)
{
	u8 x, y;
	for (x = x1; x < x2 + 1; x++)
	{
		for (y = y1; y < y2; y++)
		{
			OLED_InvertPoint(x, y);
		}
	}
}

/**
  * @brief  Fill a rectangular range

	@param  x1,y1:Position1

			x2,y2:Position2

			x1,y1,x2,y2 satisfies x1<x2,y1<y2

	  @retval None
  */
void OLED_FillRect(u8 x1, u8 y1, u8 x2, u8 y2, u8 t)
{
	u8 x, y;
	for (x = x1; x < x2 + 1; x++)
	{
		for (y = y1; y < y2; y++)
		{
			OLED_DrawPoint(x, y, t);
		}
	}
}

/**
  * @brief  Draw a line with any position set

	@param  x1,y1:Position1

			x2,y2:Position2

			x1,y1,x2,y2 satisfies x1<x2,y1<y2

	  @retval None
  */
void OLED_DrawAnyLine(u8 x1, u8 y1, u8 x2, u8 y2, bool drawOrUndraw)
{
	u8 x, y;

	if (x1 == x2)
	{
		if (y1 > y2)
			OLED_DrawVerticalLine(x1, y2, y1, drawOrUndraw);
		else
			OLED_DrawVerticalLine(x1, y1, y2, drawOrUndraw);
	}

	else if (y1 == y2)
	{
		OLED_DrawHorizonalLine(y1, x1, x2, drawOrUndraw);
	}

	else if (y2 >= y1)
	{
		if (x2 - x1 <= y2 - y1)
			for (y = y1; y < y2 + 1; y++)
			{
				x = (x2 - x1)*((float)(y - y1) / (float)(y2 - y1)) + x1;
				OLED_DrawPoint(x, y, drawOrUndraw);
			}
		else
		{
			for (x = x1; x < x2 + 1; x++)
			{
				y = (y2 - y1)*((float)(x - x1) / (float)(x2 - x1)) + y1;
				OLED_DrawPoint(x, y, drawOrUndraw);
			}
		}
	}
	else
	{
		if (x2 - x1 <= y1 - y2)
			for (y = y2; y < y1 + 1; y++)
			{
				x = x1 + (x2 - x1)*((float)(y1 - y) / (float)(y1 - y2));
				OLED_DrawPoint(x, y, drawOrUndraw);
			}
		else
		{
			for (x = x1; x < x2 + 1; x++)
			{
				y = y1 - (y1 - y2)*((float)(x - x1) / (float)(x2 - x1));
				OLED_DrawPoint(x, y, drawOrUndraw);
			}
		}
	}
}

/**
  * @brief  Init SPI of OLED

	  @retval None
  */
void SPI2_Init(void)

{
	SPI_InitTypeDef  SPI_InitStructure;

	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable SPI2 and GPIOB clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//ʹ��SPI2��ʱ��

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��IO��ʱ��

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Configure SPI2 pins: SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure PB.12 as Output push-pull, used as Flash Chip select */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB, GPIO_Pin_12);//Ƭѡ�ź�Ϊ��

	/* SPI1 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 //����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	//SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; //ʱ�����ո�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//���ݲ����ڵڶ���ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;	//�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//������Ԥ��ƵֵΪ2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7; //SPI_CRCPolynomial����������CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);

	/* Enable SPI2  */
	SPI_Cmd(SPI2, ENABLE);

}

#if OLED_REFRESH_OPTIMIZE_EN
/**
  * @brief  This function handles the update of OLED

	@hint   When OLED_GRAM keeps not edited for 50ms
			This function will upload GRAM to OLED

	  @retval None
  */
void OLED_Refresh_Handler(void *pvParameters)
{
 while(1)
 {
  if(UpdateOLEDJustNow==false)
	{
		if (GRAM_Changed == true)
		{
			if (GRAM_Changing == false)
			{
				GRAM_Changed = false;
				xSemaphoreTake( OLEDRelatedMutex, portMAX_DELAY );
				OLED_Refresh_Gram();
				xSemaphoreGive(OLEDRelatedMutex);
			}
			else
			{
				GRAM_Changing = false;
			}
		}	 
	}
	vTaskDelay(30/portTICK_RATE_MS);
 }
}
#endif

void SetUpdateOLEDJustNow()
{
 UpdateOLEDJustNow_Backup=UpdateOLEDJustNow;
 UpdateOLEDJustNow=true;
}

void ResetUpdateOLEDJustNow()
{
 UpdateOLEDJustNow=UpdateOLEDJustNow_Backup;
}

/**
  * @brief  Init OLED

	  @retval None
  */
void OLED_Init(void)
{


	GPIO_InitTypeDef  GPIO_InitStructure;

	//SendLogString("SSD1306 Initializing...\n");
	SPI2_Init();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	OLED_WR_Byte(0xAE, OLED_CMD); //�ر���ʾ
	OLED_WR_Byte(0xD5, OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_Byte(0x80, OLED_CMD);   //[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED_WR_Byte(0xA8, OLED_CMD); //��������·��
	OLED_WR_Byte(0X3f, OLED_CMD); //Ĭ��0X3F(1/64) 
	OLED_WR_Byte(0xD3, OLED_CMD); //������ʾƫ��
	OLED_WR_Byte(0X00, OLED_CMD); //Ĭ��Ϊ0

	OLED_WR_Byte(0x40, OLED_CMD); //������ʾ��ʼ�� [5:0],����.

	OLED_WR_Byte(0x8D, OLED_CMD); //��ɱ�����
	OLED_WR_Byte(0x14, OLED_CMD); //bit2������/�ر�
	OLED_WR_Byte(0x20, OLED_CMD); //�����ڴ��ַģʽ
	OLED_WR_Byte(0x02, OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_Byte(0xA1, OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0, OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WR_Byte(0xDA, OLED_CMD); //����COMӲ����������
	OLED_WR_Byte(0x12, OLED_CMD); //[5:4]����

	OLED_WR_Byte(0x81, OLED_CMD); //�Աȶ�����
	OLED_WR_Byte(0xFF, OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_Byte(0xD9, OLED_CMD); //����Ԥ�������
	OLED_WR_Byte(0xf1, OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB, OLED_CMD); //����VCOMH ��ѹ����
	OLED_WR_Byte(0x30, OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4, OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6, OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_Byte(0xAF, OLED_CMD); //������ʾ	 
	UpdateOLEDJustNow = true;
	OLED_Clear();
	UpdateOLEDJustNow = false;
  xTaskCreate(OLED_Refresh_Handler,"OLED Refresh Handler",
	configMINIMAL_STACK_SIZE,NULL,OLED_REFRESH_PRIORITY,NULL);
	OLEDRelatedMutex=xSemaphoreCreateMutex();
}

void Draw_BMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char bmp[])
{ 	
  unsigned int ii=0;
  unsigned char x,y;
  unsigned char c;
  if(y1%8==0) 
  y=y1/8;      
  else 
  y=y1/8+1;
	for(y=y0;y<=y1;y++) 
	{
		LCD_Set_Pos(x0,y*8);	
    for(x=x0;x<x1;x++)
	    {      
				c=bmp[ii++];
        c = ( c & 0x55 ) << 1 | ( c & 0xAA ) >> 1;
        c = ( c & 0x33 ) << 2 | ( c & 0xCC ) >> 2;
        c = ( c & 0x0F ) << 4 | ( c & 0xF0 ) >> 4;
	    	OLED_WR_Byte(c,OLED_DATA);	    	
	    }
	}
}

























