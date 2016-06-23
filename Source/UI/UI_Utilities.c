//File Name     UI_Utilities.c
//Description : UI Utilities

#include <string.h>
#include <stdio.h>

#include "SSD1306.h" 

#include "UI_Utilities.h" 

/**
  * @brief  Get central position(Start Position) of the string between x1,x2

  * @param  startPos:Position of start grid

			endPos:  Position of end grid

			stringLength:Length of the string

	  @retval Start position of the target string
  */
u8 GetCentralPosition(u8 startPos, u8 endPos, u8 stringLength)
{
	u8 pos;
	pos = startPos + (endPos - startPos) / 2;
	pos = pos - stringLength * 3;
	return(pos);
}

/**
  * @brief  Get length of given string

  * @param  string[]:Given string

	  @retval Length of given string
  */
u8 GetStringLength(const char string[])
{
	u8 length = 0;
	for (; *string != 0; string++)
	{
		if (*string > 127)
		{
			string += 2;
			length++;
		}
		length++;
	}
	return(length);
}

/**
  * @brief  Draw or unDraw a vertical dashed grid with given density

* @param    x:Position

			drawOrUnDraw:Draw(1) or unDraw(1)

			gridsDensity:2 pixels per dot(HighDensity)

			gridsDensity:4 pixels per dot(LowDensity)

	  @retval None
  */
void DrawVerticalDashedGrid(u8 x, bool drawOrUnDraw, u8 gridsDensity)
{
	u8 i;
	/*If highDensity is given*/
	if (gridsDensity == HighDensity)
		for (i = 0; i < 64; i++)
		{
			if (i % 2 == 0)
			{
				OLED_DrawPoint(x, i, drawOrUnDraw);
			}
		}
	/*If lowDensity is given*/
	else
	{
		for (i = 0; i < 64; i++)
		{
			if ((i - 1) % 4 == 0)
			{
				OLED_DrawPoint(x, i, drawOrUnDraw);
			}
		}
	}
}

/**
  * @brief  Draw or unDraw a horizonal dashed grid with given density

* @param    y:Position

			drawOrUnDraw:Draw(1) or unDraw(1)

			gridsDensity:2 pixels per dot(HighDensity)

			gridsDensity:4 pixels per dot(LowDensity)

	  @retval None
  */
void DrawHorizonalDashedGrid(u8 y, bool drawOrUnDraw, u8 gridsDensity)
{
	u8 i;
	/*If highDensity is given*/
	if (gridsDensity == HighDensity)
		for (i = 0; i < 128; i++)
		{
			if (i % 2 == 0)
			{
				OLED_DrawPoint(i, y, drawOrUnDraw);
			}
		}
	/*If lowDensity is given*/
	else
		for (i = 0; i < 128; i++)
		{
			if ((i - 1) % 4 == 0)
			{
				OLED_DrawPoint(i, y, drawOrUnDraw);
			}
		}
}

