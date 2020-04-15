//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GKey.hpp                                                                   //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GKEY_HPP_
#define GKEY_HPP_

#define GKEY_CODE_BASE 0xFF

typedef enum GKey_Code_e
{
   KEY_KB_ESC = GKEY_CODE_BASE, //255 phim nay la phim escape cho cac ky tu dac biet, nen khi dung can press 2 lan moi co tac dung 1 lan
   KEY_KB_BACKSPACE,            //256
   KEY_KB_ENTER,                //257
   KEY_KB_END,                  //258
   KEY_KB_HOME,                 //259
   KEY_KB_UP,                   //260
   KEY_KB_DOWN,                 //261
   KEY_KB_LEFT,                 //262
   KEY_KB_RIGHT,                //263
   KEY_KB_INSERT,               //264
   KEY_KB_DELETE,               //265
   KEY_KB_TAB,                  //266
   KEY_KB_CLEAR_LINE,           //267
   KEY_KB_CLEAR_EOL,            //268
   KEY_KB_CTRL_LEFT,            //269
   KEY_KB_CTRL_RIGHT,           //270
   KEY_KB_CTRL_UP,              //271
   KEY_KB_CTRL_DOWN,            //272
   KEY_KB_PGUP,                 //273
   KEY_KB_PGDN,                 //274
   KEY_KB_F1,                   //275
   KEY_KB_F2,
   KEY_KB_F3,
   KEY_KB_F4,
   KEY_KB_F5,
   KEY_KB_F6,          // 280
   KEY_KB_F7,
   KEY_KB_F8,
   KEY_KB_F9,
   KEY_KB_F10,
   KEY_KB_F11,          // 285
   KEY_KB_F12,          // 286

   KEY_RC_MUTE,         // 287
   KEY_RC_POWER,
   KEY_RC_EXIT,
   KEY_RC_CH_PLUS,      // 290
   KEY_RC_CH_MINUS,
   KEY_RC_VOL_PLUS,
   KEY_RC_VOL_MINUS,
   KEY_RC_OK,
   KEY_RC_FAV,          // 295
   KEY_RC_MENU,
   KEY_RC_TEXT,
   KEY_RC_0,
   KEY_RC_1,
   KEY_RC_2,            // 300
   KEY_RC_3,
   KEY_RC_4,
   KEY_RC_5,
   KEY_RC_6,
   KEY_RC_7,            // 305
   KEY_RC_8,
   KEY_RC_9,
   KEY_RC_INFO,
   KEY_RC_UP,
   KEY_RC_DOWN,         // 310
   KEY_RC_LEFT,
   KEY_RC_RIGHT,
   KEY_RC_PGUP,
   KEY_RC_PGDOWN,
   KEY_RC_PLAY,         // 315
   KEY_RC_STOP,
   KEY_RC_PAUSE,
   KEY_RC_RECORD,
   KEY_RC_BACKWARD,
   KEY_RC_FORWARD,      // 320
   KEY_RC_A,
   KEY_RC_B,
   KEY_RC_C,
   KEY_RC_EPG,
   KEY_RC_BACK,         // 325
   KEY_RC_RED,
   KEY_RC_GREEN,
   KEY_RC_YELLOW,
   KEY_RC_BLUE,
   KEY_RC_AUDIO,        // 330
   KEY_RC_HDD,
   KEY_RC_DVD,
   KEY_RC_HELP,
   KEY_RC_LAST,
   KEY_RC_INPUT,        // 335
   KEY_RC_ENTER,
   KEY_RC_GUIDE,
   KEY_RC_PIP_ON_OFF,
   KEY_RC_PIP_SWAP,
   KEY_RC_PIP_MOVE,     // 340
   KEY_RC_PIP_PLUS,
   KEY_RC_PIP_MINUS,
   KEY_RC_MY_DVR,
   KEY_RC_LIVE,
   KEY_RC_POWER_B,      // 345
   KEY_RC_POWER_ON,
   KEY_RC_POWER_OFF,
   KEY_RC_PREV_CH,
   KEY_RC_REPLAY,
   KEY_RC_LIST,         // 350
   KEY_RC_SELECT,
   KEY_RC_ADVANCE,
   KEY_RC_FORMAT,
   KEY_RC_ACTIVE,
   KEY_RC_WIDE,         // 355
   KEY_RC_TIMESHIFT,
   KEY_RC_MOSAIC,
   KEY_RC_SEEK_START,
   KEY_RC_SEEK_END,
   KEY_RC_SUBTITLES,    // 360
   KEY_RC_TV_RADIO,     // 361

   KEY_FP_UP,           // 362
   KEY_FP_DOWN,
   KEY_FP_LEFT,
   KEY_FP_RIGHT,
   KEY_FP_OK,
   KEY_FP_MENU,         // 367
   KEY_FP_POWER,

   KEY_NUMBERS                      // tong so luong keys = KEY_NUMBERS - GKEY_CODE_BASE
}
GKey_Code_t;

typedef enum GKeyboardCode_e
{
   //A-Z
   GPS2_KEY_A = 0x1C,
   GPS2_KEY_B = 0x32,
   GPS2_KEY_C = 0x21,
   GPS2_KEY_D = 0x23,
   GPS2_KEY_E = 0x24,
   GPS2_KEY_F = 0x2B,
   GPS2_KEY_G = 0x34,
   GPS2_KEY_H = 0x33,
   GPS2_KEY_I = 0x43,
   GPS2_KEY_J = 0x3B,
   GPS2_KEY_K = 0x42,
   GPS2_KEY_L = 0x4B,
   GPS2_KEY_M = 0x3A,
   GPS2_KEY_N = 0x31,
   GPS2_KEY_O = 0x44,
   GPS2_KEY_P = 0x4D,
   GPS2_KEY_Q = 0x15,
   GPS2_KEY_R = 0x2D,
   GPS2_KEY_S = 0x1B,
   GPS2_KEY_T = 0x2C,
   GPS2_KEY_U = 0x3C,
   GPS2_KEY_V = 0x2A,
   GPS2_KEY_W = 0x1D,
   GPS2_KEY_X = 0x22,
   GPS2_KEY_Y = 0x35,
   GPS2_KEY_Z = 0x1A,
   //0-9
   GPS2_KEY_0 = 0x45,
   GPS2_KEY_1 = 0x16,
   GPS2_KEY_2 = 0x1E,
   GPS2_KEY_3 = 0x26,
   GPS2_KEY_4 = 0x25,
   GPS2_KEY_5 = 0x2E,
   GPS2_KEY_6 = 0x36,
   GPS2_KEY_7 = 0x3D,
   GPS2_KEY_8 = 0x3E,
   GPS2_KEY_9 = 0x46,
   //F1-F12
   GPS2_KEY_F1 = 0x05,
   GPS2_KEY_F2 = 0x06,
   GPS2_KEY_F3 = 0x04,
   GPS2_KEY_F4 = 0x0C,
   GPS2_KEY_F5 = 0x03,
   GPS2_KEY_F6 = 0x0B,
   GPS2_KEY_F7 = 0x8A,
   GPS2_KEY_F8 = 0x03,
   GPS2_KEY_F9 = 0x01,
   GPS2_KEY_F10 = 0x09,
   GPS2_KEY_F11 = 0x78,
   GPS2_KEY_F12 = 0x07,

   GPS2_KEY_ESC = 0x76,

   GPS2_KEY_ENTER = 0x5A,
   GPS2_KEY_ENTER2 = 0xE05A,

   GPS2_KEY_SHIFT_L = 0x12,
   GPS2_KEY_SHIFT_R = 0x59,

   GPS2_KEY_CTRL_L = 0x14,
   GPS2_KEY_CTRL_R = 0xE014,

   GPS2_KEY_ALT_L = 0x11,

   GPS2_KEY_ALT_R = 0xE011,

   GPS2_KEY_SPACE = 0x29,

   GPS2_KEY_INSERT = 0xE070,

   GPS2_KEY_PG_UP = 0xE07D,
   GPS2_KEY_PG_DOWN = 0xE07A,

   GPS2_KEY_DELETE = 0xE071,
   GPS2_KEY_BACK = 0x66,

   GPS2_KEY_HOME = 0xE06C,
   GPS2_KEY_END = 0xE069,
   GPS2_KEY_TAB = 0x0D,

   GPS2_KEY_ARROW_U = 0xE075,
   GPS2_KEY_ARROW_D = 0xE072,
   GPS2_KEY_ARROW_L = 0xE06B,
   GPS2_KEY_ARROW_R = 0xE074
}
GKeyboard_Code;

//namespace GBS {
//namespace STM {
//namespace Framework {
//////////////////////////////////////////////////////////////////////////////////////
// ! Dummy
//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif //GKEY_HPP_
