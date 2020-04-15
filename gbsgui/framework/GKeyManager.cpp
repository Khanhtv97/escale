//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: KeyManager.cpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#include "GKeyManager.hpp"
#include "GDebug.hpp"
#include "GStringUtils.hpp"
#include "frontpanel.hpp"
#include "GSystem.hpp"
#include <cctype> // for isprint()

//typedef struct GKey_Context_s
//{
//   Task_t TaskId;
//   BOOL  TaskRunning;
//   STEVT_Handle_t GKey_EVT_Handle;
//   Semaphore_t Sem_EventOccured;
//   Semaphore_t Sem_NewAction;
//   STEVT_EventConstant_t GKey_Event;
//   ST_ErrorCode_t GKey_EventError;
//   //Please do not change the size of the array
//   U32 KeyQueue[256];
//   U8 KeyProducer;
//   U8 KeyConsumer;
//}
//GKey_Context_t;

//static GKey_Context_t _keyContext;
Task_t KeyListenerTaskId;
BOOL   KeyListenerTaskRunning;

typedef struct Key_Bind_s
{
   GKey_Code_t key_code;
   char* key_string;
}
Key_Bind_t;

static const Key_Bind_t key_binds[] = {
   //{ KEY_KB_ESC,        "\x1B"      }, // cai nay la bat dau cua nhieu key khac nen phai xu lu ly rieng ben duoi
   { KEY_KB_BACKSPACE,    "\x08"      },
   { KEY_KB_BACKSPACE,    "\x7F"      },
   { KEY_KB_BACKSPACE,    "\b"        },
   { KEY_KB_ENTER,        "\n"        },
   { KEY_KB_ENTER,        "\r"        },
   { KEY_KB_END,          "\x04"      },
   { KEY_KB_END,          "\x1BOF"    },
   { KEY_KB_END,          "\x1B[8~"   },
   { KEY_KB_END,          "\x1B[F"    },
   { KEY_KB_END,          "\x05"      },
   { KEY_KB_HOME,         "\x1B[7~"   },
   { KEY_KB_HOME,         "\x1BOH"    },
   { KEY_KB_HOME,         "\x1B[H"    },
   { KEY_KB_HOME,         "\x1B[1~"   },
   { KEY_KB_HOME,         "\x01"      },
   { KEY_KB_UP,           "\x1B[A"    },
   { KEY_KB_UP,           "\x10"      },
   { KEY_KB_DOWN,         "\x1B[B"    },
   { KEY_KB_DOWN,         "\x0E"      },
   { KEY_KB_LEFT,         "\x1B[D"    },
   { KEY_KB_LEFT,         "\x02"      },
   { KEY_KB_RIGHT,        "\x1B[C"    },
   { KEY_KB_RIGHT,        "\x06"      },
   { KEY_KB_INSERT,       "\x1B[2~"   },
   { KEY_KB_DELETE,       "\x1B[3~"   },
   { KEY_KB_END,          "\x1B[4~"   },
   { KEY_KB_TAB,          "\t"        },
   { KEY_KB_CLEAR_LINE,   "\x15"      },
   { KEY_KB_CLEAR_EOL,    "\x0B"      },
   { KEY_KB_CTRL_LEFT,    "\x1B[1;5D" },
   { KEY_KB_CTRL_RIGHT,   "\x1B[1;5C" },
   { KEY_KB_CTRL_UP,      "\x1B[1;5A" },
   { KEY_KB_CTRL_DOWN,    "\x1B[1;5B" },
   { KEY_KB_PGUP,         "\x1B[5~"   },
   { KEY_KB_PGDN,         "\x1B[6~"   },
   { KEY_KB_F1,           "\x1BOP"    },
   { KEY_KB_F1,           "\x1B[[A"   },
   { KEY_KB_F2,           "\x1BOQ"    },
   { KEY_KB_F2,           "\x1B[[B"   },
   { KEY_KB_F3,           "\x1BOR"    },
   { KEY_KB_F3,           "\x1B[[C"   },
   { KEY_KB_F4,           "\x1BOS"    },
   { KEY_KB_F4,           "\x1B[[D"   },
   { KEY_KB_F5,           "\x1B[15~"  },
   { KEY_KB_F5,           "\x1B[[E"   },
   { KEY_KB_F6,           "\x1B[17~"  },
   { KEY_KB_F7,           "\x1B[18~"  },
   { KEY_KB_F8,           "\x1B[19~"  },
   { KEY_KB_F9,           "\x1B[20~"  },
   { KEY_KB_F10,          "\x1B[21~"  },
   { KEY_KB_F11,          "\x1B[23~"  },
   { KEY_KB_F12,          "\x1B[24~"  },
};

#define DEF_KEY_BINDS_COUNT (sizeof(key_binds)/sizeof(Key_Bind_t))

#define MAX_KEY_STRLEN  6

#define GET_BIND_MATCH(inc, buf, res)                             \
do                                                                \
{                                                                 \
   res=-1;                                                        \
   for( inc = 0; inc < DEF_KEY_BINDS_COUNT; inc++ )               \
   {                                                              \
      if( !strcmp((char*)buf, (char*)key_binds[inc].key_string) ) \
      {                                                           \
         res=inc;                                                 \
         break;                                                   \
      }                                                           \
   }                                                              \
}                                                                 \
while (0);

//////////////////////////////////////////////////////////////////////////////////////
// return KEY_NUMBERS if no supported key pressed
U32 pollkey_kb(int uart)
{
   U8 uart_msg[3], c;
   U32 uart_read = 0;
   U32 uart_timeout = 1; // 0 --> INFINITE
   U32 toread = 1;
   U8 key_str[MAX_KEY_STRLEN+1];
   GKey_Code_t key_code = KEY_NUMBERS;
   int key_len = 0;

   if( uart )
   {
      STUART_Read(UART_Handle[0], uart_msg, toread, &uart_read, uart_timeout);
      if( uart_read <= 0 )
      {
         return KEY_NUMBERS;
      }
      c = uart_msg[0];
   }
   else
   {
      U32 key;
      SYS_PollKey(&key);
      c = (char)key;
   }

   //_DTraceDebug("char detected: 0x%02X (%d), isprint:%d\n", c, c, isprint(c));
   if( !isprint(c) )
   { // special key
      do
      {
         //_DTraceDebug("special key: %c (%d, 0x%02X)\n", c, c, c);
         key_str[key_len] = c;
         key_len++;
         key_str[key_len] = '\0';
         int i, found;
         GET_BIND_MATCH(i, key_str, found);
         // If we found a match, get the action and break

         if (found >= 0)
         {
            key_code = key_binds[found].key_code;
            key_len = 0;
            c = ' ';
            break;
         }
         // At least, check if an action key begins with the chars we have
         // If not, no need to continue and process the printable chars we have readen
         else
         {
            for( i = 0; i < DEF_KEY_BINDS_COUNT; i++ )
            {
               if( !strncmp((char*)key_str, (char*)key_binds[i].key_string, key_len) )
               {
                  break;
               }
            }
            /* Nothing found */
            if( i >= DEF_KEY_BINDS_COUNT )
            {
               break;
            }
         }

         // If no action was found AND the string we have so far COULD match but is not complete
         // AND we have reached the max char number of a action string : GET OUT
         if( key_len >= MAX_KEY_STRLEN )
         {
            break;
         }

         // Else: Read another character
         if( uart )
         {
            STUART_Read(UART_Handle[0], uart_msg, toread, &uart_read, uart_timeout);
            c = uart_msg[0];
         }
         else
         {
            U32 key;
            SYS_PollKey(&key);
            c = (char)key;
         }
      }

      while(1);

      if( key_code != KEY_NUMBERS )
      {
         return key_code;
      }
      else if( key_str[0] == '\x1B' )
      {
         return KEY_KB_ESC;
      }
   }
   else
   {
      if( uart )
      {
         if( uart_read > 0 )
         {
            return c;
         }
      }
      else
      {
         return c;
      }
   }

   return KEY_NUMBERS;
}

//////////////////////////////////////////////////////////////////////////////////////
U32 mapBlastKey(U32 blastkey)
{
   U32 gkey = KEY_NUMBERS;

   switch( blastkey )
   {
#if defined(BLAST_PROTOCOL_PHILIP)
   case BLAST_0_KEY_MUTE:
      gkey = KEY_RC_MUTE;
      break;

   case BLAST_0_KEY_POWER:
      gkey = KEY_RC_POWER;
      break;

   case BLAST_0_KEY_EXIT:
      gkey = KEY_RC_EXIT;
      break;

   case BLAST_0_KEY_CH_PLUS:
      gkey = KEY_RC_CH_PLUS;
      break;

   case BLAST_0_KEY_CH_MINUS:
      gkey = KEY_RC_CH_MINUS;
      break;

   case BLAST_0_KEY_VOL_PLUS:
      gkey = KEY_RC_VOL_PLUS;
      break;

   case BLAST_0_KEY_VOL_MINUS:
      gkey = KEY_RC_VOL_MINUS;
      break;

   case BLAST_0_KEY_OK:
      gkey = KEY_RC_OK;
      break;

   case BLAST_0_KEY_FAV:
      gkey = KEY_RC_FAV;
      break;

   case BLAST_0_KEY_MENU:
      gkey = KEY_RC_MENU;
      break;

   case BLAST_0_KEY_TEXT:
      gkey = KEY_RC_TEXT;
      break;

   case BLAST_0_KEY_0:
      gkey = KEY_RC_0;
      break;

   case BLAST_0_KEY_1:
      gkey = KEY_RC_1;
      break;

   case BLAST_0_KEY_2:
      gkey = KEY_RC_2;
      break;

   case BLAST_0_KEY_3:
      gkey = KEY_RC_3;
      break;

   case BLAST_0_KEY_4:
      gkey = KEY_RC_4;
      break;

   case BLAST_0_KEY_5:
      gkey = KEY_RC_5;
      break;

   case BLAST_0_KEY_6:
      gkey = KEY_RC_6;
      break;

   case BLAST_0_KEY_7:
      gkey = KEY_RC_7;
      break;

   case BLAST_0_KEY_8:
      gkey = KEY_RC_8;
      break;

   case BLAST_0_KEY_9:
      gkey = KEY_RC_9;
      break;

   case BLAST_0_KEY_INFO:
      gkey = KEY_RC_INFO;
      break;

   case BLAST_0_KEY_UP:
      gkey = KEY_RC_UP;
      break;

   case BLAST_0_KEY_DOWN:
      gkey = KEY_RC_DOWN;
      break;

   case BLAST_0_KEY_LEFT:
      gkey = KEY_RC_LEFT;
      break;

   case BLAST_0_KEY_RIGHT:
      gkey = KEY_RC_RIGHT;
      break;

   case BLAST_0_KEY_PGUP:
      gkey = KEY_RC_PGUP;
      break;

   case BLAST_0_KEY_PGDOWN:
      gkey = KEY_RC_PGDOWN;
      break;

   case BLAST_0_KEY_EPG:
      gkey = KEY_RC_EPG;
      break;

   case BLAST_0_KEY_BACK:
      gkey = KEY_RC_BACK;
      break;

   case BLAST_0_KEY_RED:
      gkey = KEY_RC_RED;
      break;

   case BLAST_0_KEY_GREEN:
      gkey = KEY_RC_GREEN;
      break;

   case BLAST_0_KEY_YELLOW:
      gkey = KEY_RC_YELLOW;
      break;

   case BLAST_0_KEY_BLUE:
      gkey = KEY_RC_BLUE;
      break;

   case BLAST_0_KEY_TV_RADIO:
      gkey = KEY_RC_TV_RADIO;
      break;

   case BLAST_0_KEY_AUDIO:
      gkey = KEY_RC_AUDIO;
      break;

   case BLAST_0_KEY_SUBTITLES:
      gkey = KEY_RC_SUBTITLES;
      break;

   case BLAST_0_KEY_PLAY:
      gkey = KEY_RC_PLAY;
      break;

   case BLAST_0_KEY_STOP:
      gkey = KEY_RC_STOP;
      break;

   case BLAST_0_KEY_PAUSE:
      gkey = KEY_RC_PAUSE;
      break;

   case BLAST_0_KEY_RECORD:
      gkey = KEY_RC_RECORD;
      break;

   case BLAST_0_KEY_BACKWARD:
      gkey = KEY_RC_BACKWARD;
      break;

   case BLAST_0_KEY_FORWARD:
      gkey = KEY_RC_FORWARD;
      break;

   case BLAST_0_KEY_PIP_ON_OFF:
      gkey = KEY_RC_PIP_ON_OFF;
      break;

   case BLAST_0_KEY_PIP_CH_PLUS:
      gkey = KEY_RC_PIP_PLUS;
      break;

   case BLAST_0_KEY_PIP_ON_MINUS:
      gkey = KEY_RC_PIP_MINUS;
      break;

   default:
      gkey = KEY_NUMBERS; // not supported
      break;

#else //BLAST_PROTOCOL_USER_003
   case BLAST_0_KEY_MUTE:
      gkey = KEY_RC_MUTE;
      break;

   case BLAST_0_KEY_POWER:
      gkey = KEY_RC_POWER;
      break;

   case BLAST_0_KEY_EXIT:
      gkey = KEY_RC_EXIT;
      break;

   case BLAST_0_KEY_CH_PLUS:
      gkey = KEY_RC_CH_PLUS;
      break;

   case BLAST_0_KEY_CH_MINUS:
      gkey = KEY_RC_CH_MINUS;
      break;

   case BLAST_0_KEY_VOL_PLUS:
      gkey = KEY_RC_VOL_PLUS;
      break;

   case BLAST_0_KEY_VOL_MINUS:
      gkey = KEY_RC_VOL_MINUS;
      break;

   case BLAST_0_KEY_OK:
      gkey = KEY_RC_OK;
      break;

   case BLAST_0_KEY_FAV:
      gkey = KEY_RC_FAV;
      break;

   case BLAST_0_KEY_MENU:
      gkey = KEY_RC_MENU;
      break;

   case BLAST_0_KEY_TEXT:
      gkey = KEY_RC_TEXT;
      break;

   case BLAST_0_KEY_0:
      gkey = KEY_RC_0;
      break;

   case BLAST_0_KEY_1:
      gkey = KEY_RC_1;
      break;

   case BLAST_0_KEY_2:
      gkey = KEY_RC_2;
      break;

   case BLAST_0_KEY_3:
      gkey = KEY_RC_3;
      break;

   case BLAST_0_KEY_4:
      gkey = KEY_RC_4;
      break;

   case BLAST_0_KEY_5:
      gkey = KEY_RC_5;
      break;

   case BLAST_0_KEY_6:
      gkey = KEY_RC_6;
      break;

   case BLAST_0_KEY_7:
      gkey = KEY_RC_7;
      break;

   case BLAST_0_KEY_8:
      gkey = KEY_RC_8;
      break;

   case BLAST_0_KEY_9:
      gkey = KEY_RC_9;
      break;

   case BLAST_0_KEY_INFO:
      gkey = KEY_RC_INFO;
      break;

   case BLAST_0_KEY_UP:
      gkey = KEY_RC_UP;
      break;

   case BLAST_0_KEY_DOWN:
      gkey = KEY_RC_DOWN;
      break;

   case BLAST_0_KEY_LEFT:
      gkey = KEY_RC_LEFT;
      break;

   case BLAST_0_KEY_RIGHT:
      gkey = KEY_RC_RIGHT;
      break;

   case BLAST_0_KEY_PGUP:
      gkey = KEY_RC_PGUP;
      break;

   case BLAST_0_KEY_PGDOWN:
      gkey = KEY_RC_PGDOWN;
      break;

   case BLAST_0_KEY_PLAY:
      gkey = KEY_RC_PLAY;
      break;

   case BLAST_0_KEY_STOP:
      gkey = KEY_RC_STOP;
      break;

   case BLAST_0_KEY_PAUSE:
      gkey = KEY_RC_PAUSE;
      break;

   case BLAST_0_KEY_RECORD:
      gkey = KEY_RC_RECORD;
      break;

   case BLAST_0_KEY_BACKWARD:
      gkey = KEY_RC_BACKWARD;
      break;

   case BLAST_0_KEY_FORWARD:
      gkey = KEY_RC_FORWARD;
      break;

   case BLAST_0_KEY_A:
      gkey = KEY_RC_A;
      break;

   case BLAST_0_KEY_B:
      gkey = KEY_RC_B;
      break;

   case BLAST_0_KEY_C:
      gkey = KEY_RC_C;
      break;

   case BLAST_0_KEY_EPG:
      gkey = KEY_RC_EPG;
      break;

   case BLAST_0_KEY_BACK:
      gkey = KEY_RC_BACK;
      break;

   case BLAST_0_KEY_RED:
      gkey = KEY_RC_RED;
      break;

   case BLAST_0_KEY_GREEN:
      gkey = KEY_RC_GREEN;
      break;

   case BLAST_0_KEY_YELLOW:
      gkey = KEY_RC_YELLOW;
      break;

   case BLAST_0_KEY_BLUE:
      gkey = KEY_RC_BLUE;
      break;

   case BLAST_0_KEY_AUDIO:
      gkey = KEY_RC_AUDIO;
      break;

   case BLAST_0_KEY_HDD:
      gkey = KEY_RC_HDD;
      break;

   case BLAST_0_KEY_DVD:
      gkey = KEY_RC_DVD;
      break;

   case BLAST_0_KEY_HELP:
      gkey = KEY_RC_HELP;
      break;

   case BLAST_0_KEY_LAST:
      gkey = KEY_RC_LAST;
      break;

   case BLAST_0_KEY_INPUT:
      gkey = KEY_RC_INPUT;
      break;

   case BLAST_0_KEY_ENTER:
      gkey = KEY_RC_ENTER;
      break;

   case BLAST_0_KEY_GUIDE:
      gkey = KEY_RC_GUIDE;
      break;

   case BLAST_0_KEY_PIP_ON_OFF:
      gkey = KEY_RC_PIP_ON_OFF;
      break;

   case BLAST_0_KEY_PIP_SWAP:
      gkey = KEY_RC_PIP_SWAP;
      break;

   case BLAST_0_KEY_PIP_MOVE:
      gkey = KEY_RC_PIP_MOVE;
      break;

   case BLAST_0_KEY_PIP_CH_PLUS:
      gkey = KEY_RC_PIP_PLUS;
      break;

   case BLAST_0_KEY_PIP_ON_MINUS:
      gkey = KEY_RC_PIP_MINUS;
      break;

   case BLAST_0_KEY_MY_DVR:
      gkey = KEY_RC_MY_DVR;
      break;

   case BLAST_0_KEY_LIVE:
      gkey = KEY_RC_LIVE;
      break;

   case BLAST_0_KEY_POWER_B:
      gkey = KEY_RC_POWER_B;
      break;

   case BLAST_0_KEY_POWER_ON:
      gkey = KEY_RC_POWER_ON;
      break;

   case BLAST_0_KEY_POWER_OFF:
      gkey = KEY_RC_POWER_OFF;
      break;

   case BLAST_0_KEY_PREV_CH:
      gkey = KEY_RC_PREV_CH;
      break;

   case BLAST_0_KEY_REPLAY:
      gkey = KEY_RC_REPLAY;
      break;

   case BLAST_0_KEY_LIST:
      gkey = KEY_RC_LIST;
      break;

   case BLAST_0_KEY_SELECT:
      gkey = KEY_RC_SELECT;
      break;

   case BLAST_0_KEY_ADVANCE:
      gkey = KEY_RC_ADVANCE;
      break;

   case BLAST_0_KEY_FORMAT:
      gkey = KEY_RC_FORMAT;
      break;

   case BLAST_0_KEY_ACTIVE:
      gkey = KEY_RC_ACTIVE;
      break;

   case BLAST_0_KEY_WIDE:
      gkey = KEY_RC_WIDE;
      break;

   case BLAST_0_KEY_TIMESHIFT:
      gkey = KEY_RC_TIMESHIFT;
      break;

   case BLAST_0_KEY_MOSAIC:
      gkey = KEY_RC_MOSAIC;
      break;

   case BLAST_0_KEY_SEEK_START:
      gkey = KEY_RC_SEEK_START;
      break;

   case BLAST_0_KEY_SEEK_END:
      gkey = KEY_RC_SEEK_END;
      break;

   case BLAST_0_KEY_SUBTITLES:
      gkey = KEY_RC_SUBTITLES;
      break;

   case BLAST_0_KEY_TV_RADIO:
      gkey = KEY_RC_TV_RADIO;
      break;

   default:
      gkey = KEY_NUMBERS; // not supported

#endif
   }

   return gkey;
}


//namespace GBS {
//namespace STM {
//namespace Framework {
//////////////////////////////////////////////////////////////////////////////////////
void GKeyManager::KeyListenerTask(void* Parameter)
{
   U32 lastkey = KEY_NUMBERS;
   U8 mode;
   U32 key;

   BOOL KeyListenerTaskRunning = FALSE;

   while( KeyListenerTaskRunning == TRUE )
   {
      Task_Delay(ST_GetClocksPerSecond()/50);

      // Fill key queue
      mode = GKeyManager::getInstance().getInputMode();
      key = KEY_NUMBERS;

      if( mode & KEY_INPUT_RC )
      {
         //TODO: move implementation of BLASTi_PollTask in stapp/blast/blast.c:2589 here and remove that task to reduce number of concurrent tasks
         ST_ErrorCode_t ErrCode= BLAST_GetKey(0, &key, 1);

         if( ErrCode != ST_NO_ERROR )
         {
            key = KEY_NUMBERS;
         }
         else
         {
            // convert the key
            key = mapBlastKey(key);
         }

         if (key != KEY_NUMBERS)
         {
            //_DTraceDebug("Remote key: 0x%X (%d, %c)\n",key, key, key);
         }
      }

      if( (key == KEY_NUMBERS) && (mode & KEY_INPUT_UART) )
      {
         key = pollkey_kb(1);
         if( key != KEY_NUMBERS )
         {
            //_DTraceDebug("Uart key: 0x%X (%d, %c)\n",key, key, key);
         }
      }

      //TODO: mode nay tam thoi bi disable trong ham setInputMode vi ham SYS_PollKey de doc key tu console dung getc() se wait forever until press ENTER --> cai nay lock task
      //      muon chay dc mode nay thi phai implement vao task rieng
      if( (key == KEY_NUMBERS) && (mode & KEY_INPUT_GDB) )
      {
         key = pollkey_kb(0);
         //_DTraceDebug("Console key: 0x%X (%d, %c)\n",key, key, key);
         // khi input tu console thi ko co (chua tim ra cach :-)) che do iterative, thao tac nhap key phai ket thuc = ENTER, nen can loai bo phim ENTER khoi su kien key, muon nhap phim ENTER, thi phai press ENTER 2 lan
         if( key == KEY_KB_ENTER )
         {
            if( lastkey != KEY_KB_ENTER )
            {
               lastkey = key;
               key = KEY_NUMBERS;
            }
            else
            {
               //lastkey = key;       // lan ENTER tiep theo chi can bam 1 lan ENTER
               lastkey = KEY_NUMBERS; // ep phai bam 2 lan ENTER de input phim ENTER tiep theo
            }
         }
         else
         {
            lastkey = key;
         }
      }

      if( key == KEY_NUMBERS )
      {
         // try to get front key
         key = (U32)_D_.GetKeyStatus();

         if( key == 0 )
         {
            key = KEY_NUMBERS;
         }
      }

      if( key != KEY_NUMBERS )
      {
         //_keyContext.KeyQueue[_keyContext.KeyProducer++]=key;
         //Semaphore_Signal(_keyContext.Sem_NewAction);
         GKeyManager::getInstance().dispatchEvent(GEVENT_KEY_PRESSED, (void*) &key);
         char keyname[32] = "Unknown key";
         GKeyManager::getInstance().getKeyName(key, keyname);
      }

      _D_.PrintFPLED();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
U32 GKeyManager::scanKeyPressed()
{
   U32 lastkey = KEY_NUMBERS;
   U32 keyPressed = KEY_NUMBERS;
   U8 mode = getInputMode();

   // Fill key queue
   if( mode & KEY_INPUT_RC )
   {
      U32 blastKey = KEY_NUMBERS;

      if( ST_NO_ERROR == BLAST_PollKey(0, &blastKey))
      {
         //string strDebugKeyCode;
         //GStringUtils::format(strDebugKeyCode, "0x%08x", key);
         //_DTraceError("\n KEYPRESSED = %s", strDebugKeyCode.c_str());
         // Convert the key
         keyPressed = mapBlastKey(blastKey);
         //char keyname[32] = "Unknown key";
         //GKeyManager::getInstance().getKeyName(key, keyname);
         //_DTraceError("\n KEYPRESSED = %s", keyname);
      }
      else
      {
         keyPressed = KEY_NUMBERS;
      }
   }

   if( keyPressed == KEY_NUMBERS )
   {
      // try to get front key
      keyPressed = (U32)_D_.GetKeyStatus();

      if( keyPressed == 0 )
      {
         keyPressed = KEY_NUMBERS;
      }
      else
      {
         Task_Delay(ST_GetClocksPerSecond()/3);
      }
   }

#ifdef DEBUG_VERSION

   if( (keyPressed == KEY_NUMBERS) && (mode & KEY_INPUT_UART) )
   {
      keyPressed = pollkey_kb(1);
      if( keyPressed != KEY_NUMBERS )
      {
         //_DTraceDebug("Uart key: 0x%X (%d, %c)\n",key, key, key);
      }
   }

   //TODO: mode nay tam thoi bi disable trong ham setInputMode vi ham SYS_PollKey de doc key tu console dung getc() se wait forever until press ENTER --> cai nay lock task
   //      muon chay dc mode nay thi phai implement vao task rieng
   if( (keyPressed == KEY_NUMBERS) && (mode & KEY_INPUT_GDB) )
   {
      keyPressed = pollkey_kb(0);
      //_DTraceDebug("Console key: 0x%X (%d, %c)\n",key, key, key);
      // khi input tu console thi ko co (chua tim ra cach :-)) che do iterative, thao tac nhap key phai ket thuc = ENTER, nen can loai bo phim ENTER khoi su kien key, muon nhap phim ENTER, thi phai press ENTER 2 lan
      if( keyPressed == KEY_KB_ENTER )
      {
         if( lastkey != KEY_KB_ENTER )
         {
            lastkey = keyPressed;
            keyPressed = KEY_NUMBERS;
         }
         else
         {
            //lastkey = key;       // lan ENTER tiep theo chi can bam 1 lan ENTER
            lastkey = KEY_NUMBERS; // ep phai bam 2 lan ENTER de input phim ENTER tiep theo
         }
      }
      else
      {
         lastkey = keyPressed;
      }
   }

#endif

   return keyPressed;
}

//////////////////////////////////////////////////////////////////////////////////////
void GKeyManager::flushAllKey()
{
   // Flush all key already pressed if any
   U32 flushKey;
   while( ST_NO_ERROR == BLAST_PollKey( 0, &flushKey) )
   {
      _DTraceError("\n Flush KEY = %d", flushKey);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GKeyManager::GKeyManager() : GObject()
{
   KeyListenerTaskRunning = FALSE;
   KeyListenerTaskId = NULL;
   //initialize key context
   //memset(_keyContext.KeyQueue,0,256);
   //_keyContext.KeyProducer=_keyContext.KeyConsumer=0;
   //Semaphore_Init_Fifo(_keyContext.Sem_EventOccured,0);
   //Semaphore_Init_Fifo_TimeOut(_keyContext.Sem_NewAction,0);

   this->setInputMode(KEY_INPUT_FP | KEY_INPUT_RC);
   //this->setInputMode(KEY_INPUT_FP | KEY_INPUT_RC | KEY_INPUT_UART);
   //startKeyManagerTask();
}

//////////////////////////////////////////////////////////////////////////////////////
GKeyManager::~GKeyManager()
{
   stopKeyManagerTask();
   //Semaphore_Delete(_keyContext.Sem_NewAction);
   //Semaphore_Delete(_keyContext.Sem_EventOccured);
}

//////////////////////////////////////////////////////////////////////////////////////
GKeyManager& GKeyManager::getInstance()
{
   static GKeyManager instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
void GKeyManager::stopKeyManagerTask()
{
   if( KeyListenerTaskId != NULL )
   {
      Task_Suspend(KeyListenerTaskId);
      KeyListenerTaskRunning = FALSE;
      Task_Delete(KeyListenerTaskId);
      KeyListenerTaskId = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GKeyManager::startKeyManagerTask()
{
   if( KeyListenerTaskRunning == TRUE )
   {
      if( KeyListenerTaskId != NULL )
      {
         Task_Suspend(KeyListenerTaskId);
         KeyListenerTaskRunning = FALSE;
         Task_Delete(KeyListenerTaskId);
         KeyListenerTaskId = NULL;
      }
   }

   U32 ParamId = 0;

   //KeyListenerTaskId = Task_Create(GKeyManager::KeyListenerTask, (void*)ParamId, 8192, MIN_USER_PRIORITY, "KeyListenerTask", 0, KeyListenerTaskId);
   KeyListenerTaskId = Task_Create(GKeyManager::KeyListenerTask_PS2, (void*)ParamId, 8192, MIN_USER_PRIORITY, "KeyListenerTask_PS2", 0, KeyListenerTaskId);

   if( KeyListenerTaskId == (Task_t)NULL )
   {
      _DTraceError("Cannot create KeyListener Task");
   }
   else
   {
      KeyListenerTaskRunning = TRUE;
      _DTraceDebug("KeyListener Task created");
   }
}

//////////////////////////////////////////////////////////////////////////////////////
//TODO: for debug only
void GKeyManager::getKeyName(U32 gkey, char* keyname)
{
   switch( gkey )
   {
   case KEY_KB_ESC:
      strcpy(keyname, "KEY_KB_ESC");
      break;

   case KEY_KB_BACKSPACE:
      strcpy(keyname, "KEY_KB_BACKSPACE");
      break;

   case KEY_KB_ENTER:
      strcpy(keyname, "KEY_KB_ENTER");
      break;

   case KEY_KB_END:
      strcpy(keyname, "KEY_KB_END");
      break;

   case KEY_KB_HOME:
      strcpy(keyname, "KEY_KB_HOME");
      break;

   case KEY_KB_UP:
      strcpy(keyname, "KEY_KB_UP");
      break;
   case KEY_KB_DOWN:
      strcpy(keyname, "KEY_KB_DOWN");
      break;

   case KEY_KB_LEFT:
      strcpy(keyname, "KEY_KB_LEFT");
      break;

   case KEY_KB_RIGHT:
      strcpy(keyname, "KEY_KB_RIGHT");
      break;

   case KEY_KB_INSERT:
      strcpy(keyname, "KEY_KB_INSERT");
      break;

   case KEY_KB_DELETE:
      strcpy(keyname, "KEY_KB_DELETE");
      break;

   case KEY_KB_TAB:
      strcpy(keyname, "KEY_KB_TAB");
      break;

   case KEY_KB_CLEAR_LINE:
      strcpy(keyname, "KEY_KB_CLEAR_LINE");
      break;

   case KEY_KB_CLEAR_EOL:
      strcpy(keyname, "KEY_KB_CLEAR_EOL");
      break;

   case KEY_KB_CTRL_LEFT:
      strcpy(keyname, "KEY_KB_CTRL_LEFT");
      break;

   case KEY_KB_CTRL_RIGHT:
      strcpy(keyname, "KEY_KB_CTRL_RIGHT");
      break;
   case KEY_KB_CTRL_UP:
      strcpy(keyname, "KEY_KB_CTRL_UP");
      break;

   case KEY_KB_CTRL_DOWN:
      strcpy(keyname, "KEY_KB_CTRL_DOWN");
      break;

   case KEY_KB_PGUP:
      strcpy(keyname, "KEY_KB_PGUP");
      break;
   case KEY_KB_PGDN:
      strcpy(keyname, "KEY_KB_PGDN");
      break;

   case KEY_KB_F1:
      strcpy(keyname, "KEY_KB_F1");
      break;

   case KEY_KB_F2:
      strcpy(keyname, "KEY_KB_F2");
      break;
   case KEY_KB_F3:
      strcpy(keyname, "KEY_KB_F3");
      break;

   case KEY_KB_F4:
      strcpy(keyname, "KEY_KB_F4");
      break;

   case KEY_KB_F5:
      strcpy(keyname, "KEY_KB_F5");
      break;

   case KEY_KB_F6:
      strcpy(keyname, "KEY_KB_F6");
      break;

   case KEY_KB_F7:
      strcpy(keyname, "KEY_KB_F7");
      break;
   case KEY_KB_F8:
      strcpy(keyname, "KEY_KB_F8");
      break;

   case KEY_KB_F9:
      strcpy(keyname, "KEY_KB_F9");
      break;

   case KEY_KB_F10:
      strcpy(keyname, "KEY_KB_F10");
      break;
   case KEY_KB_F11:
      strcpy(keyname, "KEY_KB_F11");
      break;

   case KEY_KB_F12:
      strcpy(keyname, "KEY_KB_F12");
      break;

   case KEY_RC_MUTE:
      strcpy(keyname, "KEY_RC_MUTE");
      break;

   case KEY_RC_POWER:
      strcpy(keyname, "KEY_RC_POWER");
      break;

   case KEY_RC_EXIT:
      strcpy(keyname, "KEY_RC_EXIT");
      break;

   case KEY_RC_CH_PLUS:
      strcpy(keyname, "KEY_RC_CH_PLUS");
      break;

   case KEY_RC_CH_MINUS:
      strcpy(keyname, "KEY_RC_CH_MINUS");
      break;
   case KEY_RC_VOL_PLUS:
      strcpy(keyname, "KEY_RC_VOL_PLUS");
      break;

   case KEY_RC_VOL_MINUS:
      strcpy(keyname, "KEY_RC_VOL_MINUS");
      break;

   case KEY_RC_OK:
      strcpy(keyname, "KEY_RC_OK");
      break;
   case KEY_RC_FAV:
      strcpy(keyname, "KEY_RC_FAV");
      break;

   case KEY_RC_MENU:
      strcpy(keyname, "KEY_RC_MENU");
      break;

   case KEY_RC_TEXT:
      strcpy(keyname, "KEY_RC_TEXT");
      break;

   case KEY_RC_0:
      strcpy(keyname, "KEY_RC_0");
      break;

   case KEY_RC_1:
      strcpy(keyname, "KEY_RC_1");
      break;

   case KEY_RC_2:
      strcpy(keyname, "KEY_RC_2");
      break;

   case KEY_RC_3:
      strcpy(keyname, "KEY_RC_3");
      break;

   case KEY_RC_4:
      strcpy(keyname, "KEY_RC_4");
      break;

   case KEY_RC_5:
      strcpy(keyname, "KEY_RC_5");
      break;

   case KEY_RC_6:
      strcpy(keyname, "KEY_RC_6");
      break;

   case KEY_RC_7:
      strcpy(keyname, "KEY_RC_7");
      break;

   case KEY_RC_8:
      strcpy(keyname, "KEY_RC_8");
      break;
   case KEY_RC_9:
      strcpy(keyname, "KEY_RC_9");
      break;

   case KEY_RC_INFO:
      strcpy(keyname, "KEY_RC_INFO");
      break;

   case KEY_RC_UP:
      strcpy(keyname, "KEY_RC_UP");
      break;

   case KEY_RC_DOWN:
      strcpy(keyname, "KEY_RC_DOWN");
      break;

   case KEY_RC_LEFT:
      strcpy(keyname, "KEY_RC_LEFT");
      break;

   case KEY_RC_RIGHT:
      strcpy(keyname, "KEY_RC_RIGHT");
      break;

   case KEY_RC_PGUP:
      strcpy(keyname, "KEY_RC_PGUP");
      break;

   case KEY_RC_PGDOWN:
      strcpy(keyname, "KEY_RC_PGDOWN");
      break;

   case KEY_RC_PLAY:
      strcpy(keyname, "KEY_RC_PLAY");
      break;

   case KEY_RC_STOP:
      strcpy(keyname, "KEY_RC_STOP");
      break;

   case KEY_RC_PAUSE:
      strcpy(keyname, "KEY_RC_PAUSE");
      break;

   case KEY_RC_RECORD:
      strcpy(keyname, "KEY_RC_RECORD");
      break;

   case KEY_RC_BACKWARD:
      strcpy(keyname, "KEY_RC_BACKWARD");
      break;

   case KEY_RC_FORWARD:
      strcpy(keyname, "KEY_RC_FORWARD");
      break;

   case KEY_RC_A:
      strcpy(keyname, "KEY_RC_A");
      break;

   case KEY_RC_B:
      strcpy(keyname, "KEY_RC_B");
      break;

   case KEY_RC_C:
      strcpy(keyname, "KEY_RC_C");
      break;

   case KEY_RC_EPG:
      strcpy(keyname, "KEY_RC_EPG");
      break;

   case KEY_RC_BACK:
      strcpy(keyname, "KEY_RC_BACK");
      break;

   case KEY_RC_RED:
      strcpy(keyname, "KEY_RC_RED");
      break;

   case KEY_RC_GREEN:
      strcpy(keyname, "KEY_RC_GREEN");
      break;

   case KEY_RC_YELLOW:
      strcpy(keyname, "KEY_RC_YELLOW");
      break;

   case KEY_RC_BLUE:
      strcpy(keyname, "KEY_RC_BLUE");
      break;

   case KEY_RC_AUDIO:
      strcpy(keyname, "KEY_RC_AUDIO");
      break;

   case KEY_RC_HDD:
      strcpy(keyname, "KEY_RC_HDD");
      break;

   case KEY_RC_DVD:
      strcpy(keyname, "KEY_RC_DVD");
      break;

   case KEY_RC_HELP:
      strcpy(keyname, "KEY_RC_HELP");
      break;

   case KEY_RC_LAST:
      strcpy(keyname, "KEY_RC_LAST");
      break;

   case KEY_RC_INPUT:
      strcpy(keyname, "KEY_RC_INPUT");
      break;

   case KEY_RC_ENTER:
      strcpy(keyname, "KEY_RC_ENTER");
      break;

   case KEY_RC_GUIDE:
      strcpy(keyname, "KEY_RC_GUIDE");
      break;

   case KEY_RC_PIP_ON_OFF:
      strcpy(keyname, "KEY_RC_PIP_ON_OFF");
      break;

   case KEY_RC_PIP_SWAP:
      strcpy(keyname, "KEY_RC_PIP_SWAP");
      break;

   case KEY_RC_PIP_MOVE:
      strcpy(keyname, "KEY_RC_PIP_MOVE");
      break;

   case KEY_RC_PIP_PLUS:
      strcpy(keyname, "KEY_RC_PIP_PLUS");
      break;

   case KEY_RC_PIP_MINUS:
      strcpy(keyname, "KEY_RC_PIP_MINUS");
      break;

   case KEY_RC_MY_DVR:
      strcpy(keyname, "KEY_RC_MY_DVR");
      break;

   case KEY_RC_LIVE:
      strcpy(keyname, "KEY_RC_LIVE");
      break;

   case KEY_RC_POWER_B:
      strcpy(keyname, "KEY_RC_POWER_B");
      break;

   case KEY_RC_POWER_ON:
      strcpy(keyname, "KEY_RC_POWER_ON");
      break;

   case KEY_RC_POWER_OFF:
      strcpy(keyname, "KEY_RC_POWER_OFF");
      break;

   case KEY_RC_PREV_CH:
      strcpy(keyname, "KEY_RC_PREV_CH");
      break;

   case KEY_RC_REPLAY:
      strcpy(keyname, "KEY_RC_REPLAY");
      break;

   case KEY_RC_LIST:
      strcpy(keyname, "KEY_RC_LIST");
      break;

   case KEY_RC_SELECT:
      strcpy(keyname, "KEY_RC_SELECT");
      break;

   case KEY_RC_ADVANCE:
      strcpy(keyname, "KEY_RC_ADVANCE");
      break;

   case KEY_RC_FORMAT:
      strcpy(keyname, "KEY_RC_FORMAT");
      break;

   case KEY_RC_ACTIVE:
      strcpy(keyname, "KEY_RC_ACTIVE");
      break;

   case KEY_RC_WIDE:
      strcpy(keyname, "KEY_RC_WIDE");
      break;

   case KEY_RC_TIMESHIFT:
      strcpy(keyname, "KEY_RC_TIMESHIFT");
      break;

   case KEY_RC_MOSAIC:
      strcpy(keyname, "KEY_RC_MOSAIC");
      break;

   case KEY_RC_SEEK_START:
      strcpy(keyname, "KEY_RC_SEEK_START");
      break;

   case KEY_RC_SEEK_END:
      strcpy(keyname, "KEY_RC_SEEK_END");
      break;

   case KEY_RC_SUBTITLES:
      strcpy(keyname, "KEY_RC_SUBTITLES");
      break;

   case KEY_RC_TV_RADIO:
      strcpy(keyname, "KEY_RC_TV_RADIO");
      break;

   case KEY_FP_UP:
      strcpy(keyname, "KEY_FP_UP");
      break;

   case KEY_FP_DOWN:
      strcpy(keyname, "KEY_FP_DOWN");
      break;

   case KEY_FP_LEFT:
      strcpy(keyname, "KEY_FP_LEFT");
      break;

   case KEY_FP_RIGHT:
      strcpy(keyname, "KEY_FP_RIGHT");
      break;

   case KEY_FP_OK:
      strcpy(keyname, "KEY_FP_OK");
      break;

   case KEY_FP_MENU:
      strcpy(keyname, "KEY_FP_MENU");
      break;

   default:
      strcpy(keyname, "Unknown key");
   }

   if( (gkey > 31) && (gkey < 256) )
   {
      strcpy(keyname, "ASCII char");
   }

   //return;
}

//////////////////////////////////////////////////////////////////////////////////////
void GKeyManager::setInputMode(U8 mode)
{
   if( (mode<=0) || (mode>(KEY_INPUT_FP | KEY_INPUT_RC | KEY_INPUT_GDB | KEY_INPUT_UART | KEY_INPUT_PIO)) )
   {
      _DTraceError("Input mode %d not supported", mode);
      return;
   }

   if( mode & KEY_INPUT_GDB )
   {
      _DTraceError("Input from GDB Console not supported yet");
      mode &= (~KEY_INPUT_GDB);
   }

   this->m_inputMode = mode;

   _DTraceDebug("Input mode has been set to %d", mode);
}

//////////////////////////////////////////////////////////////////////////////////////
U8 GKeyManager::getInputMode()
{
   return this->m_inputMode;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GKeyManager::isKeyWakeup(const U32 keyCode)
{
   BOOL ret = FALSE;

   if( (keyCode == KEY_RC_POWER) || (keyCode == KEY_RC_0) || (keyCode == KEY_FP_POWER) ||
       (keyCode == KEY_RC_1) || (keyCode == KEY_RC_2) || (keyCode == KEY_RC_3) ||
       (keyCode == KEY_RC_4) || (keyCode == KEY_RC_5) || (keyCode == KEY_RC_6) ||
       (keyCode == KEY_RC_7) || (keyCode == KEY_RC_8) || (keyCode == KEY_RC_9) ||
       (keyCode == KEY_RC_CH_PLUS)  || (keyCode == KEY_RC_CH_MINUS)  ||
       (keyCode == KEY_RC_VOL_PLUS) || (keyCode == KEY_RC_VOL_MINUS) ||
       (keyCode == KEY_RC_OK)   || (keyCode == KEY_RC_UP)    || (keyCode == KEY_RC_DOWN) ||
       (keyCode == KEY_RC_LEFT) || (keyCode == KEY_RC_RIGHT) || (keyCode == KEY_RC_EXIT) ||
       (keyCode == KEY_RC_FAV)  || (keyCode == KEY_RC_MENU)  || (keyCode == KEY_RC_INFO) )
   {
      ret = TRUE;
   }

   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
///////// BEGIN - THIS BELLOW CODES ARE IMPLEMENTED FOR PS2 KEYBOARD ONLY ////////////
//////////////////////////////////////////////////////////////////////////////////////
#define _HIGHT_ TRUE
#define _LOW_   FALSE
#define _DELAY_ 5
#define GPS2_PIO_ID 2
#define GPS2_SCL 4
#define GPS2_SDA 5
#define _MAX_TIMEOUT_ 100

typedef struct Ps2_Key_Bind_s
{
   U8 key_code;
   char* key_string;
}
Ps2_Key_Bind_t;

static const Ps2_Key_Bind_t ps2_key_binds[]= {
 {0x1C, "A"}, {0x32, "B"}, {0x21, "C"}, {0x23, "D"}, {0x24, "E"}, {0x2B, "F"}, {0x34, "G"}, {0x33, "H"},
 {0x43, "I"}, {0x3B, "J"}, {0x42, "K"}, {0x4B, "L"}, {0x3A, "M"}, {0x31, "N"}, {0x44, "O"}, {0x4D, "P"},
 {0x15, "Q"}, {0x2D, "R"}, {0x1B, "S"}, {0x2C, "T"}, {0x3C, "U"}, {0x2A, "V"}, {0x1D, "W"}, {0x22, "X"},
 {0x35, "Y"}, {0x1A, "Z"}, {0x45, "0"}, {0x16, "1"}, {0x1E, "2"}, {0x26, "3"}, {0x25, "4"}, {0x2E, "5"},
 {0x36, "6"}, {0x3D, "7"}, {0x3E, "8"}, {0x46, "9"}
};

BOOL GPS2_SDA_STATE();
BOOL GPS2_SCL_STATE();
void GPS2_DelayUS(int);
bool GPS2_hasClock();
void GPS2_ReadWord(U8*, int&);
char* getKey(U8);

//////////////////////////////////////////////////////////////////////////////////////
char* getKey(U8 keyCode)
{
   int numOfKeys = (sizeof(ps2_key_binds)/sizeof(Ps2_Key_Bind_t));
   for( int i=0; i < numOfKeys; i++ )
   {
      if( ps2_key_binds[i].key_code == keyCode )
      {
         return ps2_key_binds[i].key_string;
      }
   }

   return "Not Defined!";
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL GPS2_SDA_STATE()
{
   BOOL ret = _LOW_;
   PIO_BIT_Get(GPS2_PIO_ID, GPS2_SDA, &ret);
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GPS2_SCL_STATE()
{
   BOOL ret = _LOW_;
   PIO_BIT_Get(GPS2_PIO_ID, GPS2_SCL, &ret);
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
void GPS2_DelayUS(int numUs)
{
   if( numUs < 1 )
   {
      numUs = 1;
   }
   int clockDelay = (int)((numUs*ST_GetClocksPerSecond())/1000000);
   Task_Delay(clockDelay);
}

//////////////////////////////////////////////////////////////////////////////////////
bool GPS2_hasClock()
{
   bool ret = true;
   int numOfTry = 3;
   for(int i = 0; i < numOfTry; i++)
   {
      ret = ret && (GPS2_SCL_STATE() == _LOW_);
   }
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
void GPS2_ReadWord(U8* pData, int& numOfBit)
{
   PIO_BIT_Configure(GPS2_PIO_ID, GPS2_SCL, PIO_IN);
   PIO_BIT_Configure(GPS2_PIO_ID, GPS2_SDA, PIO_IN);

   int timeout = 0;
   numOfBit = 0;
   memset(pData, 0, 16);

   for( numOfBit = 0; numOfBit < 11; ++numOfBit )
   {
      while( !GPS2_hasClock() ){;}
      GPS2_DelayUS(_DELAY_/5);
      if( GPS2_SDA_STATE() == _HIGHT_ )
      {
         pData[numOfBit] = 1;
      }
      //numOfBit++;
      while( GPS2_hasClock() )
      {
         if( ++timeout > _MAX_TIMEOUT_ )
         {
            break;
         }
         GPS2_DelayUS(_DELAY_/5);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GKeyManager::KeyListenerTask_PS2(void* Parameter)
{
   U8 lastKeyCode = 0;
   while( KeyListenerTaskRunning == TRUE )
   {
      U8 datas[16];
      int numOfBits;

      GPS2_ReadWord(datas, numOfBits);

      if( numOfBits > 0 )
      {
         _DTraceDebug("SDA(%d): %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", numOfBits,
                       datas[0], datas[1], datas[2], datas[3], datas[4], datas[5], datas[6], datas[7],
                       datas[8], datas[9], datas[10], datas[11], datas[12], datas[13], datas[14], datas[15]);
         if( datas[0] == 0 && datas[10] == 1)
         {
            U8 bitMask = 0x80;//1000 0000
            U8 rByte = 0x00;
            for(int i = 8; i > 0; i--)
            {
               if( datas[i] == 1 )
                {
                   rByte = rByte|bitMask;
                }
                bitMask = bitMask >> 1;
            }
           _DTraceDebug("Key pressed: %d: %s", rByte, getKey(rByte));
         }
         else
         {
            _DTraceDebug("Keycode scans failed !!!");
         }
      }
      Task_Delay(ST_GetClocksPerSecond()/5);
   }
}
//////////////////////////////////////////////////////////////////////////////////////
///////// END   - THIS BELLOW CODES ARE IMPLEMENTED FOR PS2 KEYBOARD ONLY ////////////
//////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
