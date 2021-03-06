//File Name     UI_Menu.c
//Description : UI Menu

#include <string.h>
#include <stdio.h>

#include "SSD1306.h"
#include "Keys.h" 

#include "UI_Utilities.h" 

#include "UI_Menu.h" 

#include "ExceptionHandle.h" 

#define UI_MENU_HANDLER_PRIORITY tskIDLE_PRIORITY+2

xQueueHandle UI_MenuMsg; 

xTaskHandle UI_MenuHandle=NULL;
/**
  * @brief  Show menu on screen

  * @param  ItemString:A string which contains the strings
	  of the items,use'%'to seperate two strings
		e.g."string1%string2&string3"

						ItemNum:The quantity of the items that listed

						DefaultValue:The default selection of the buttons

	@retval The handler will send the result to UI_MenuMsg when confirmation got
  */
void UI_Menu_Handler(void *pvParameters)
{
	int currentPos, i, p, q, currentRelativePos, lastRelativePos;
	UI_Menu_Param_Struct * menuParams=pvParameters;
	bool allContentChanged;
	bool relativePosChanged;
  Key_Message_Struct keyMessage;
	u8 selection;
	u8 displayedItemNum=(menuParams->ItemNum>=4?4:menuParams->ItemNum);
	currentPos = menuParams->DefaultPos;
	currentRelativePos = 0;
	allContentChanged = true;
	relativePosChanged = false;
	lastRelativePos = 0;
	SetKeyBeatRate(menuParams->FastSpeed);
  for(;;)
	{
		ReDraw:
		xSemaphoreTake( OLEDRelatedMutex, portMAX_DELAY );
		if (allContentChanged == true)
		{
			for (i = 0; i < displayedItemNum; i++)
			{
				p = GetStringGraphicalLength(menuParams->ItemStrings[currentPos + i]);
				q = GetCentralPosition(0, 127, p);
				OLED_FillRect(2, i * 15 + 2, 125, i * 15 + 14, 0);
				OLED_ShowAnyString(q, i * 15 + 2, menuParams->ItemStrings[currentPos + i], NotOnSelect, 12);
			}
			OLED_DrawRect(0, 0, 127, displayedItemNum*15, DRAW);
			for(i = 1; i < displayedItemNum; i++)
			{
			 OLED_DrawHorizonalLine(15*i, 0, 127, DRAW);
			}
			OLED_InvertRect(2, currentRelativePos * 15 + 2, 125, currentRelativePos * 15 + 14);
			allContentChanged = false;
		}
		if (relativePosChanged == true)
		{
			OLED_InvertRect(2, lastRelativePos * 15 + 2, 125, lastRelativePos * 15 + 14);
			OLED_InvertRect(2, currentRelativePos * 15 + 2, 125, currentRelativePos * 15 + 14);
			lastRelativePos = currentRelativePos;
			relativePosChanged = false;
		}	 
	  xSemaphoreGive(OLEDRelatedMutex);
		xQueueReceive(Key_Message, & keyMessage, portMAX_DELAY );	
		if(keyMessage.KeyEvent==LeftClick||keyMessage.AdvancedKeyEvent==LeftContinous)
		{
			if (currentRelativePos == 0)
			{
				currentPos--;
				if (currentPos == -1)
				{
					currentPos = 0;
					goto ReDraw;
				}
				allContentChanged = true;
				goto ReDraw;
			}
			else
			{
				currentRelativePos--;
				relativePosChanged = true;
				goto ReDraw;
			}
		}
		else if(keyMessage.KeyEvent==RightClick||keyMessage.AdvancedKeyEvent==RightContinous)
		{
			if (currentRelativePos == displayedItemNum-1)
			{
				currentPos++;
				if (currentPos == menuParams->ItemNum - (displayedItemNum-1))
				{
					currentPos = menuParams->ItemNum - displayedItemNum;
					goto ReDraw;
				}
				allContentChanged = true;
				goto ReDraw;
			}
			else
			{
				currentRelativePos++;
				relativePosChanged = true;
				goto ReDraw;
			}		 
		}
		else if(keyMessage.KeyEvent == MidDouble)
		{
		 selection=255;
		 xQueueSend(UI_MenuMsg,&selection, 100/portTICK_RATE_MS);
		 for(;;) vTaskDelay(portMAX_DELAY);
		}
		else if(keyMessage.KeyEvent == MidClick)
		{
		 selection=currentPos + currentRelativePos;
		 xQueueSend(UI_MenuMsg,&selection, 100/portTICK_RATE_MS);
		  for(;;) vTaskDelay(portMAX_DELAY);
		}
	}
}


/**
  * @brief  See UI_Menu_Handler for details

  * @param  Menu UI param struct

  */
void UI_Menu_Init(UI_Menu_Param_Struct * menuParams)
{
	UI_MenuMsg=xQueueCreate(1, sizeof(u8));
	CreateTaskWithExceptionControl(UI_Menu_Handler, "UI_Menu Handler",
	256, menuParams, UI_MENU_HANDLER_PRIORITY, &UI_MenuHandle);
}		

/**
  * @brief  DeInit UI_Menu
  */
void UI_Menu_DeInit()
{
 if(UI_MenuHandle!=NULL)
	 vTaskDelete(UI_MenuHandle);
 vQueueDelete(UI_MenuMsg);
 UI_MenuHandle=NULL;
}
