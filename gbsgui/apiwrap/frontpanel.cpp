//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: frontpanel.cpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#include "frontpanel.hpp"
#include "GKey.hpp"

//////////////////////////////////////////////////////////////////////////////////////
// Set Frontpanel DA pin
ST_ErrorCode_t FP_SET_DATA(BOOL OnOff)
{
   ST_ErrorCode_t ErrCode=ST_NO_ERROR;

#if defined(sat5189)
   //ErrCode|=PIO_BIT_Set_5189(2,2,OnOff);
   ErrCode|=PIO_BIT_Set(2,2,OnOff);
#endif
#if defined(ST_H205)
   ErrCode|=PIO_BIT_Set(15,4,OnOff);
#endif
   return(ErrCode);
}

//////////////////////////////////////////////////////////////////////////////////////
U8 FP_GET_DATA(void)
{
   ST_ErrorCode_t ErrCode=ST_NO_ERROR;
   BOOL    PIO_Status;

#if defined(sat5189)
   ErrCode=PIO_BIT_Get(2,2,&PIO_Status);
#endif
#if defined(ST_H205)
   ErrCode|=PIO_BIT_Get(15,4,&PIO_Status);
#endif
   return(PIO_Status);
}

//////////////////////////////////////////////////////////////////////////////////////
// Set Frontpanel CL pin
ST_ErrorCode_t FP_SET_CLK(BOOL OnOff)
{
   ST_ErrorCode_t ErrCode=ST_NO_ERROR;

#if defined(sat5189)
//   ErrCode|=PIO_BIT_Set_5189(1,3,OnOff);
   ErrCode|=PIO_BIT_Set(1,3,OnOff);
#endif
#if defined(ST_H205)
   ErrCode|=PIO_BIT_Set(14,7,OnOff);
#endif
   return(ErrCode);
}

//////////////////////////////////////////////////////////////////////////////////////
// Set Frontpanel CS pin
ST_ErrorCode_t FP_SET_STRB(BOOL OnOff)
{
   ST_ErrorCode_t ErrCode=ST_NO_ERROR;
   BOOL    PIO_Status;
#if defined(sat5189)
   ErrCode|=PIO_BIT_Set(2,6,OnOff);
#endif
#if defined(ST_H205)
   ErrCode|=PIO_BIT_Set(14,4,OnOff);
#endif
   return(ErrCode);
}

//////////////////////////////////////////////////////////////////////////////////////
U8 FP_GetStandbyKey()
{
   BOOL PIO_Status;
   PIO_BIT_Get(3, 1, &PIO_Status);
   return(PIO_Status);
}

//////////////////////////////////////////////////////////////////////////////////////
void FP_SetLockLED(BOOL OnOff)
{
#if defined(sat5189)
   //PIO_BIT_Set_5189(2,7,OnOff);
   //PIO_BIT_Set(2,7,OnOff);
#endif
#if defined(ST_H205)
   //PIO_BIT_Set(4,7,OnOff);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
void FP_SetStandbyLED(BOOL OnOff)
{
   //PIO_BIT_Set(14,4,OnOff);
   PIO_BIT_Set(4, 7, OnOff);
}

//////////////////////////////////////////////////////////////////////////////////////
void FP_SetAudioAmpOutput(BOOL OnOff)
{
   //Redmine #693: [ChangHong] Switch mute on/off by pull-down/up the PIO9_3
   PIO_BIT_Set(9, 3, OnOff);
}

//////////////////////////////////////////////////////////////////////////////////////
void FP_SetAntennaPower(BOOL OnOff)
{
   //ChangHong hardware
   PIO_BIT_Configure(3, 5, PIO_OUT);
   PIO_BIT_Set(3, 5, OnOff);
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL FP_GetAntenPowerState()
{
#if defined(CHANGHONG)
   BOOL PIO_Status;
   PIO_BIT_Configure(3, 6, PIO_IN);
   PIO_BIT_Get(3, 6, &PIO_Status);
   return (PIO_Status);
#else
   return FALSE;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
void FP_SetBuzzer(BOOL OnOff)
{
   //ChangHong hardware
   //It is used for Buzzer in ESacle application
   PIO_BIT_Configure(2, 4, PIO_OUT);
   PIO_BIT_Set(2, 4, OnOff);
}

//////////////////////////////////////////////////////////////////////////////////////
void FP_NotifyBuzzer(int numOfNotif, bool isLongNotif)
{
   FP_SetBuzzer(TRUE); //buzzer off
   for( int i = 0; i < numOfNotif; ++i )
   {
      FP_SetBuzzer(FALSE);
      if( isLongNotif )
      {
         Task_Delay(ST_GetClocksPerSecond()*3);
      }
      else
      {
         Task_Delay(ST_GetClocksPerSecond()/5);
      }
      FP_SetBuzzer(TRUE);
      if( numOfNotif > 1 )
      {
         Task_Delay(ST_GetClocksPerSecond()/3);
      }
   }
   FP_SetBuzzer(TRUE); //buzzer off
}

//////////////////////////////////////////////////////////////////////////////////////
