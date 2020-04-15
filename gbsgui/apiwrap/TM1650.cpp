/*
 * TM1650.cpp
 *
 *  Created on: Jan 21, 2015
 *      Author: trungkt
 */

#include "TM1650.hpp"
#include "GKey.hpp"
#include "frontpanel.hpp"
#include "GDebug.hpp"

#define WRITE_COMMAND     0x48
#define READ_COMMAND      0x4F
#define MODE_7SEGMENTS    0x08
#define MODE_8SEGMENTS    0x00
#define ENABLE_DISPLAY    0x01
#define BRIGHTNESS_MOD    0x01 // Brightness level 5
#define DIG1              0x68
#define DIG2              0x6A
#define DIG3              0x6C
#define DIG4              0x6E
//#define KI1               0x47

#define SW4               0xDF //KI6 0x6F
#define SW5               0xEF //KI7 0x77
#define SW6               0x9F //KI2 0x4F
#define SW7               0xAF //KI3 0x57
#define SW8               0xBF //KI4 0x5F
#define SW9               0xCF //KI5 0x67


#define timedelay     Task_Delay(ST_GetClocksPerSecond()/10000);

void TM1650_WriteByte(U8 wr_data)
{
   for(int i=0; i<8;i++)
   {
      FP_SET_CLK(FALSE);
      timedelay;
      if((wr_data & 0x80) == 0x80)
      {
         FP_SET_DATA(TRUE);
      }
      else
      {
         FP_SET_DATA(FALSE);
      }
      timedelay;
      wr_data <<= 1;
      FP_SET_CLK(TRUE);
      timedelay;
      FP_SET_CLK(FALSE);
   }
}

U8 TM1650_ReadByte()
{
   U8 rd_data = 0;
   FP_SET_DATA(FALSE);
   for(int i=0; i< 8; i++)
   {
      rd_data <<= 1;  //to move to next BIT
      FP_SET_CLK(TRUE);
      timedelay;
      FP_SET_CLK(FALSE);
      timedelay;
      if(FP_GET_DATA() == 1) rd_data |= 0x01;
      else rd_data &= 0xFE;
   }
   return rd_data;
}

void TM1650_WriteData(U8 addr, U8 data)
{
   TM1650_Start();
   TM1650_WriteByte(addr);
   TM1650_Ack();
   TM1650_WriteByte(data);
   TM1650_Ack();
   TM1650_Stop();
}

void TM1650_Test()
{
   TM1650_WriteData(WRITE_COMMAND,MODE_8SEGMENTS);
   TM1650_WriteData(WRITE_COMMAND,ENABLE_DISPLAY);
   TM1650_WriteData(DIG1,CHAR_5);
   TM1650_WriteData(DIG2,CHAR_6);
}

void TM1650_LedDisplay(U8 *Buffer_p)
{
   //TM1650_WriteData(WRITE_COMMAND,MODE_8SEGMENTS);
   TM1650_WriteData(WRITE_COMMAND,ENABLE_DISPLAY);
   TM1650_WriteData(DIG1,Buffer_p[0]);
   TM1650_WriteData(DIG2,Buffer_p[1]);
   TM1650_WriteData(DIG3,Buffer_p[2]);
   TM1650_WriteData(DIG4,Buffer_p[3]);
}

U8 TM1650_ReadData()
{
   U8 rd_data;
   TM1650_Start();
   TM1650_WriteByte(READ_COMMAND);
   TM1650_Ack();
   rd_data = TM1650_ReadByte();
   TM1650_Ack();
   TM1650_Stop();
   return rd_data;
}

void TM1650_KeyProcess(void)
{

}

int TM1650_GetKeyStatus()
{
   U8 keyCode= TM1650_ReadData();
   switch(keyCode)
   {
      case SW4:
        // _DTraceError("KEY_RC_LEFT pressed");
         return KEY_RC_LEFT;
         break;
      case SW5:
        // _DTraceError("KEY_RC_RIGHT pressed");
         return KEY_RC_RIGHT;
         break;
      case SW6:
        // _DTraceError("KEY_RC_UP pressed");
         return KEY_RC_UP;
         break;
      case SW7:
        // _DTraceError("KEY_RC_DOWN pressed");
         return KEY_RC_DOWN;
         break;
      case SW8:
        // _DTraceError("KEY_RC_OK pressed");
      return KEY_RC_OK;
         break;
      case SW9:
        // _DTraceError("KEY_RC_MENU pressed");
         return KEY_RC_MENU;
         break;
      default:
         return 0;
   }

}

void TM1650_Start()
{
   FP_SET_CLK(TRUE);
   FP_SET_DATA(TRUE);
   timedelay;
   FP_SET_DATA(FALSE);
}

void TM1650_Stop()
{
   FP_SET_CLK(TRUE);
   FP_SET_DATA(FALSE);
   timedelay;
   FP_SET_DATA(TRUE);
}

void TM1650_Ack()
{
   FP_SET_CLK(FALSE);
   FP_SET_CLK(TRUE);
   timedelay;
   FP_SET_CLK(FALSE);
   timedelay;
}
