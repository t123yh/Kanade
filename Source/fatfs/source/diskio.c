/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */
#include "sdcardff.h"

#include "RTC.h"

#include  <stdio.h>

/* Definitions of physical drive number for each drive */
#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define USB		2	/* Example: Map USB MSD to physical drive 2 */

vu8 SD_Card_Ready;

extern u8  SD_Type;

extern u32 Mass_Block_Count;//������
extern u32 Mass_Block_Size;//������С
extern u32 Mass_Memory_Size;//����

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	u8 state;
    if(pdrv)
    {
        return STA_NOINIT;  //��֧�ִ���0�Ĳ���
    }
		    state = MSD_Init();
	SD_Card_Ready=state;//SD����ʼ���ɹ���־
			if(SD_Card_Ready)//SD��û��ʼ���ɹ�
	{
	   return STA_NODISK;
	}
	    if(state == STA_NODISK)
    {
        return STA_NODISK;
    }
    else if(state != 0)
    {
        return STA_NOINIT;  //�������󣺳�ʼ��ʧ��
    }
    else
    {
        return 0;           //��ʼ���ɹ�
    }
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
  u8 result;
    if(pdrv)
    {
        return STA_NOINIT; 
    }
  result = MSD_Init();
    if(result == STA_NODISK)
    {
        return STA_NODISK;
    }
    else if(result != 0)
    {
        return STA_NOINIT;
    }
    else
    {
        return 0;
    }
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	u8 res;
  if (pdrv || !count)
    { 
        return RES_PARERR;
    }
	if(SD_Card_Ready)//SD��û��ʼ���ɹ�
	{
	   return RES_NOTRDY;
	}
    if(count==1) 
    { 
    	if(SD_Type!=SD_TYPE_V2HC)
    	{
       		res = MSD_ReadBlock(buff, (u32)sector*512, 512);  
    	}else{
	
	   		res = MSD_ReadBlock(buff, (u32)sector, 512);  
    } 
	}
    else 
    { 
        res = MSD_ReadBuffer(buff,sector*512,count*512);
    }		

    if(res == 0x00)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	u8 res;
    if (pdrv || !count)
    { 
        return RES_PARERR;
    }
	if(SD_Card_Ready)
	{
	   return RES_NOTRDY;
	}
    if(count==1) 
    {
    	if(SD_Type!=SD_TYPE_V2HC)
    	{	      	   
			res = MSD_WriteBlock((u8*)buff, (u32)sector*512, 512);
		}else{
	    	res = MSD_WriteBlock((u8*)buff, (u32)sector, 512);
		}

    }
    else 
    { 
    	if(SD_Type!=SD_TYPE_V2HC)
    	{	   
			res = MSD_WriteBuffer((u8*)buff, (u32)sector*512, (u32)count*512);
		}else{
			res = MSD_WriteBuffer((u8*)buff, (u32)sector, (u32)count*512);
    }
	}		
    if(res == 0)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }		
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;

		if(SD_Card_Ready)//SD��û��ʼ���ɹ�
	{
	   return RES_NOTRDY;
	}
    if (pdrv)
    { 
        return RES_PARERR; 
    }
   switch(cmd)
    {
    case CTRL_SYNC:
        SD_CS_ENABLE();
        if(MSD_GetResponse(0xff)==0)
        {
            res = RES_OK;
        }
        else
        {
            res = RES_ERROR;
        }
        SD_CS_DISABLE();
        break;
        
    case GET_BLOCK_SIZE:
        *(WORD*)buff = Mass_Block_Size;
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT:
        *(DWORD*)buff = Mass_Block_Count;
        res = RES_OK;
        break;
    default:
        res = RES_PARERR;
        break;
    }
 return res;
}

/*-----------------------------------------------------------------------*/
/* User defined function to give a current time to fatfs module          */
/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{
#if	 GET_FATTIME_EN
    DWORD temp;
	temp=(RTCTime.w_year-1980)<<25|RTCTime.w_month<<21|RTCTime.w_date<<16|
	RTCTime.hour<<11|RTCTime.min<<5|RTCTime.sec;
    return temp;
#else
     return 0;
#endif
}


