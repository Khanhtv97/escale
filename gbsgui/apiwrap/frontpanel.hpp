//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: frontpanel.hpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////
#ifndef FRONTPANEL_HPP_
#define FRONTPANEL_HPP_

#include "stapp_main.h"
#include "GDebug.hpp"

#define LEDFRONTPANEL

#ifndef LED_COMMONANODE
//#define LED_COMMONANODE //Un comment for LED common Anode
#endif  //LED_COMMONANODE
#if defined(LEDFRONTPANEL)

// 7 segment:
//      a
//   -------
//  |f      | b
//  |   g   |       ##
//   -------           h
//  |e      | c     ##
//  |   d   |
//   -------

#define SEG7_A      0x01
#define SEG7_B      0x02
#define SEG7_C      0x04
#define SEG7_D      0x08
#define SEG7_E      0x10
#define SEG7_F      0x20
#define SEG7_G      0x40

#define SEG_DOT     0x80
#define SEG_COLON   0x80 //???

#define CHAR_       0x00
#define CHAR_SPACE  0x00

#define CHAR_A  (SEG7_F | SEG7_E | SEG7_A | SEG7_B | SEG7_C | SEG7_G)
#define CHAR_b  (SEG7_F | SEG7_E | SEG7_G | SEG7_C | SEG7_D)
#define CHAR_C  (SEG7_A | SEG7_F | SEG7_E | SEG7_D)
#define CHAR_c  (SEG7_G | SEG7_E | SEG7_D)
#define CHAR_d  (SEG7_G | SEG7_E | SEG7_D | SEG7_B | SEG7_C)
#define CHAR_E  (SEG7_F | SEG7_E | SEG7_A | SEG7_G | SEG7_D)
#define CHAR_F  (SEG7_F | SEG7_E | SEG7_A | SEG7_G)
#define CHAR_g  (SEG7_A | SEG7_F | SEG7_G | SEG7_B | SEG7_C | SEG7_D)
#define CHAR_H  (SEG7_F | SEG7_E | SEG7_G | SEG7_B | SEG7_C)
#define CHAR_h  (SEG7_F | SEG7_E | SEG7_G | SEG7_C)
#define CHAR_I  (SEG7_F | SEG7_E)
#define CHAR_i  (SEG7_E)
#define CHAR_J  (SEG7_B | SEG7_C | SEG7_D | SEG7_E)
#define CHAR_K  (SEG7_F | SEG7_E | SEG7_G)
#define CHAR_L  (SEG7_F | SEG7_E | SEG7_D)
#define CHAR_l  (SEG7_F | SEG7_E)
#define CHAR_m  (SEG7_E | SEG7_A | SEG7_G | SEG7_C)
#define CHAR_N  (SEG7_F | SEG7_E | SEG7_A | SEG7_B | SEG7_C)
#define CHAR_n  (SEG7_E | SEG7_G | SEG7_C)
#define CHAR_O  (SEG7_A | SEG7_B | SEG7_C | SEG7_D | SEG7_E | SEG7_F)
#define CHAR_o  (SEG7_C | SEG7_D | SEG7_E | SEG7_G)
#define CHAR_P  (SEG7_F | SEG7_E | SEG7_A | SEG7_B | SEG7_G)
#define CHAR_q  (SEG7_A | SEG7_F | SEG7_G | SEG7_B | SEG7_C)
#define CHAR_r  (SEG7_E | SEG7_G)
#define CHAR_S  (SEG7_A | SEG7_F | SEG7_G | SEG7_C | SEG7_D)
#define CHAR_t  (SEG7_F | SEG7_E | SEG7_D | SEG7_G)
#define CHAR_U  (SEG7_F | SEG7_E | SEG7_D | SEG7_C | SEG7_B)
#define CHAR_u  (SEG7_E | SEG7_D | SEG7_C)
#define CHAR_V  (SEG7_F | SEG7_E | SEG7_D | SEG7_C | SEG7_B)
#define CHAR_v  (SEG7_E | SEG7_D | SEG7_C)
#define CHAR_w  (SEG7_A | SEG7_E | SEG7_D | SEG7_C)
#define CHAR_X  (SEG7_F | SEG7_E | SEG7_G | SEG7_B | SEG7_C)
#define CHAR_y  (SEG7_F | SEG7_G | SEG7_B | SEG7_C)
#define CHAR_z  (SEG7_A | SEG7_B | SEG7_G | SEG7_E | SEG7_D)
#define CHAR__  (SEG7_D)
#define CHAR_HYPHEN  (SEG7_G)

#define CHAR_0  CHAR_O
#define CHAR_1  (SEG7_B | SEG7_C)
#define CHAR_2  (SEG7_A | SEG7_B | SEG7_G | SEG7_E | SEG7_D)
#define CHAR_3  (SEG7_A | SEG7_B | SEG7_G | SEG7_C | SEG7_D)
#define CHAR_4  (SEG7_F | SEG7_G | SEG7_B | SEG7_C)
#define CHAR_5  CHAR_S
#define CHAR_6  (SEG7_A | SEG7_F | SEG7_E | SEG7_G | SEG7_C | SEG7_D)
#define CHAR_7  (SEG7_A | SEG7_B | SEG7_C)
#define CHAR_8  (SEG7_A | SEG7_B | SEG7_C | SEG7_D | SEG7_E | SEG7_F | SEG7_G)
#define CHAR_9  CHAR_g

#define KEY_POWER   0x01
#define KEY_MENU    0x02
#define KEY_CHDOWN  0x04
#define KEY_CHUP    0x10
#define KEY_VOLDOWN 0x20
#define KEY_VOLUP   0x40
#define KEY_OK      0x80

extern "C" {

   //ST_ErrorCode_t PIO_BIT_Set_5189(U32 PIO_Index,U32 PIO_Bit,BOOL OnOff);
   //ST_ErrorCode_t PIO_BIT_Get_5189(U32 PIO_Index,U32 PIO_Bit,BOOL *Status);

   ST_ErrorCode_t FP_SET_DATA(BOOL OnOff);
   ST_ErrorCode_t FP_SET_CLK(BOOL OnOff);
   ST_ErrorCode_t FP_SET_STRB(BOOL OnOff);

   U8 FP_GET_DATA(void);
   U8 FP_GetStandbyKey();
   BOOL FP_GetAntenPowerState();

   void FP_SetLockLED(BOOL OnOff); // Off: Blue LED, On: Red Led
   void FP_SetStandbyLED(BOOL OnOff);
   void FP_SetAudioAmpOutput(BOOL OnOff);
   void FP_SetAntennaPower(BOOL OnOff);
   void FP_SetBuzzer(BOOL OnOff);
   void FP_NotifyBuzzer(int numOfNotif, bool isLongNotif=false);
}

#endif //defined(LEDFRONTPANEL)

#endif //FRONTPANEL_HPP_
