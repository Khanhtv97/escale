/*
 * TM1668.cpp
 *
 *  Created on: Jan 21, 2015
 *      Author: trungkt
 */

#include "TM1668.hpp"
#include "GKey.hpp"
#include "frontpanel.hpp"
#include "GDebug.hpp"

#define SET_DISPLAYMODE            0x03 //set display mode ,7 GRID 11 SEG
#define SET_WRITEDATA_AUTOMODE     0x40 //set auto address mode
#define WRITELEDMODE_Z             0x41
#define READ_KEYVALUE              0x42 // read KEY scan data
#define SET_WRITEDATA_FIXMODE      0x44 //set fix address mode
#define SET_DISPLAY_STARTADDRESS   0xc0
#define SET_DISPLAYCONTROL         0x8c //command for display control,to set pulse width 11/16

#define SEGMENT       10
#define GRID          7
#define DATACOUNT     2*GRID    //number of data transferred in auto addr mode

#define timedelay     Task_Delay(ST_GetClocksPerSecond()/10000);


/* ========================================================================
   Name:        TM1668_WriteByte
   Description:  to send 8bits  data to TM1668, LSB first
 ======================================================================== */
void TM1668_WriteByte(U8 wr_data)
{
   U32 i=8;

   Task_Delay(ST_GetClocksPerSecond()/5000);
   //Task_Delay(ST_GetClocksPerSecond()/1000);
   FP_SET_STRB(FALSE);   //  to set ��STB�� LOW
   Task_Delay(ST_GetClocksPerSecond()/5000);
   //Task_Delay(ST_GetClocksPerSecond()/1000);
   while(i--)        //  start sending 8 bit data once per cycle
   {
      FP_SET_CLK(FALSE);  //to clear ��CLK��
      timedelay;

      if((wr_data & 0x01)!=0)
      {
         FP_SET_DATA(TRUE);
      }
      else
      {
         FP_SET_DATA(FALSE);
      }

      timedelay;
      wr_data = wr_data>>1;  //to move to next BIT
      FP_SET_CLK(TRUE);         //to SET  ��CLK�� HIGH
      timedelay;
   }
}

/* ========================================================================
   Name:        TM1668_Send_Data
   Description:  Write to TM1668 to light LED , addr is 8bits, data is 8 bits
   ======================================================================== */
void TM1668_Send_Data(U8 addr_led, U8 data_led)
{
   FP_SET_STRB(TRUE);
   Task_Delay(ST_GetClocksPerSecond()/1000);
   TM1668_WriteByte( addr_led);
   TM1668_WriteByte( data_led);
   FP_SET_STRB(TRUE);
   Task_Delay(ST_GetClocksPerSecond()/1000);
}

/* ========================================================================
   Name:        TM1668_LedDisplay
   Description:  to send TM1668 control commands ,and Drive LED display screen
   ======================================================================== */
void TM1668_LedDisplay(U8 *Buffer_p)
{
   U32 ilength;

   FP_SET_STRB(TRUE);
   FP_SET_CLK(TRUE);
   FP_SET_DATA(TRUE);
   timedelay;
   TM1668_WriteByte(SET_DISPLAYMODE); /*to set display mode*/
   FP_SET_STRB(TRUE);
   TM1668_WriteByte(SET_WRITEDATA_AUTOMODE); /*to write data using the address auto-plus-one*/
   FP_SET_STRB(TRUE);
   TM1668_WriteByte(SET_DISPLAY_STARTADDRESS);/* the beginning of register 00H unit*/
   for(ilength=0;ilength<14;ilength++)
   {
      //TM1668_WriteByte(0xff);
      TM1668_WriteByte(Buffer_p[ilength]);
   }

   FP_SET_STRB(TRUE);
   TM1668_WriteByte(SET_DISPLAYCONTROL);//Display control parameter
}

void TM1668_Show_2012()
{
   FP_SET_STRB(TRUE);
   TM1668_WriteByte( SET_DISPLAYMODE);  /*to set display mode*/
   FP_SET_STRB(TRUE);
   TM1668_WriteByte( SET_WRITEDATA_FIXMODE);  /*WRITE LED in fix address MODE*/

   FP_SET_STRB(TRUE);

   /*to display "2012" in LED */
   TM1668_Send_Data(SET_DISPLAY_STARTADDRESS,CHAR_2);
   TM1668_Send_Data(0xc2,CHAR_0);
   TM1668_Send_Data(0xc4,CHAR_1);
   TM1668_Send_Data(0xc6,CHAR_2);

   FP_SET_STRB(TRUE);
   TM1668_WriteByte( SET_DISPLAYCONTROL); /*display control pulse width*/
   Task_Delay(ST_GetClocksPerSecond()*3);
}

/* ========================================================================
   Name:        TM1668_ReadByte
   Description:  to read 8bits key data from TM1668, LSB first
 ======================================================================== */
U8 TM1668_ReadByte()
{
   U32 i;
   U8 rec_data=0;
   FP_SET_STRB(FALSE);  //  To an effective chip select signals (chip select signal is active-low)
   Task_Delay(ST_GetClocksPerSecond()/10000);
   FP_SET_DATA(TRUE);     //SET DIO "high"��the default key value is 00H when no key pressed
   Task_Delay(ST_GetClocksPerSecond()/10000);
   for(i=0;i<8;i++) //DIO  will be high, because there is no button pressed, the key defaults to 00H, this time with different keys
   {
      FP_SET_CLK(FALSE);
      Task_Delay(ST_GetClocksPerSecond()/10000);
      rec_data=rec_data>>1;
      if( FP_GET_DATA()==1 )
      {
         rec_data=rec_data | 0x80; // bat bit 7
      }
      else
      {
         rec_data=rec_data & 0x7f; // tat bit 7
      }
      FP_SET_CLK(TRUE);
      Task_Delay(ST_GetClocksPerSecond()/10000);
   }
   return(rec_data);
}

int TM1668_GetKeyStatus()
{
   U8 count,key[5];
   U8 KeyCode = 0;
   FP_SET_STRB(TRUE);
   FP_SET_DATA(TRUE);
   FP_SET_CLK(TRUE);
   memset(key,0,5);
   //Task_Delay(ST_GetClocksPerSecond()/1000);
   //FP_SET_STRB(TRUE);
   //Task_Delay(ST_GetClocksPerSecond()/1000);

   TM1668_WriteByte(READ_KEYVALUE);//   Reading button to send the command (42H)
   Task_Delay(ST_GetClocksPerSecond()/500);

   for( count=0; count<5; count++ )
   {
      key[count]=TM1668_ReadByte(); //  Reading 5 byte key data
      //if (key[count] !=0 && key[count] != 0xff)
      //_DTraceError("FP key[%d] = 0x%x \n",count,key[count]);
   }
#if defined(sat5189)
   if(key[0] == 0x02) KeyCode |= KEY_MENU;
   else if(key[0] == 0x10) KeyCode |= KEY_OK;

   if(key[1] == 0x02) KeyCode |= KEY_CHDOWN;
   else if(key[1] == 0x10) KeyCode |= KEY_CHUP;

   if(key[2] == 0x02) KeyCode |= KEY_VOLDOWN;
   else if(key[2] == 0x10) KeyCode |= KEY_VOLUP;

   if(key[3] == 0x02) KeyCode |= KEY_POWER;
#endif

#if defined(ST_H205)
   if( key[0] == 0x02 )
   {
      KeyCode |= KEY_OK;
   }
   else if( key[2] == 0x10 )
   {
      KeyCode |= KEY_MENU;
   }

   if( key[2] == 0x02 )
   {
      KeyCode |= KEY_CHDOWN;
   }
   else if( key[1] == 0x10 )
   {
      KeyCode |= KEY_CHUP;
   }

   if( key[1] == 0x02 )
   {
      KeyCode |= KEY_VOLDOWN;
   }
   else if( key[0] == 0x10 )
   {
      KeyCode |= KEY_VOLUP;
   }

   if( key[3] == 0x02 )
   {
      KeyCode |= KEY_POWER;
   }
#endif

#if defined(CHANGHONG)
   switch(KeyCode)
   {
      case KEY_OK:
         return KEY_FP_OK;
      case KEY_MENU:
         return KEY_FP_MENU;
      case KEY_CHUP:
         return KEY_FP_DOWN;
      case KEY_CHDOWN:
         return KEY_FP_UP;
      case KEY_VOLDOWN:
         return KEY_FP_LEFT;
      case KEY_VOLUP:
         return KEY_FP_RIGHT;
      default:
         return 0;
   }
#else
   switch(KeyCode)
   {
      case KEY_OK:
         return KEY_FP_OK;
      case KEY_MENU:
         return KEY_FP_MENU;
      case KEY_CHUP:
         return KEY_FP_UP;
      case KEY_CHDOWN:
         return KEY_FP_DOWN;
      case KEY_VOLDOWN:
         return KEY_FP_LEFT;
      case KEY_VOLUP:
         return KEY_FP_RIGHT;
      default:
         return 0;
   }
#endif

}

