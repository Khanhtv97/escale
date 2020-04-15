//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GDebug.cpp                                                                 //
//////////////////////////////////////////////////////////////////////////////////////

#include "GDebug.hpp"
#include "stapp_main.h"  // for print, printf
#include <cstddef>       // for NULL
#include "frontpanel.hpp"
#include "GSystem.hpp"
#include "TM1650.hpp"
#include "TM1668.hpp"
const char* GDebug::m_ppLogLevelName[] = {"", "ERROR","WARN","M","I","D"};

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
GDebug::GDebug()
{
   //this->m_logLevel = LOG_LEVEL_DEBUG;
   //this->m_logLevel = LOG_LEVEL_ERROR;
   this->m_logLevel = LOG_LEVEL_INFO;
   // Thuc: changed default log output to UART only
   //this->m_logOutput = LOG_OUTPUT_GBD | LOG_OUTPUT_UART;
   this->m_logOutput = LOG_OUTPUT_UART;

   for( int i = 0; i < 12; i++ )
   {
      m_pStrToPrint[i] = -1;
      m_pPrevStrToPrint[i] = -1;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GDebug::Log(LogLevel_e level, char* s, bool cleanOutput)
{
   return;

   if( this->m_logLevel >= level )
   {
      // output log here
      if( this->m_logOutput & LOG_OUTPUT_GBD )
      {
         if( cleanOutput )
         {
            printf("%s", s);
         }
         else
         {
            printf("[%s %u] %s", m_ppLogLevelName[(int) level], _S_.now(), s);
         }
      }

      if( this->m_logOutput & LOG_OUTPUT_UART )
      {
         //TODO: move "print" from testtool to implement right here, inside GDebug class
         if( cleanOutput )
         {
            //print("%s", s);
            TRACE_Print("%s", s);
         }
         else
         {
            //print("[%s %u] %s", m_ppLogLevelName[(int) level], _S_.now(), s);
            TRACE_Print("[%s %u] %s", m_ppLogLevelName[(int) level], _S_.now(), s);
         }
      }

      if( this->m_logOutput & LOG_OUTPUT_OSD )
      {
         //TODO: implement OSD debug output
         print("OSD Debug output hasn't been supported yet!\n");
         printf("OSD Debug output hasn't been supported yet!\n");
         this->m_logLevel = LOG_LEVEL_ERROR;
         this->m_logOutput = LOG_OUTPUT_GBD | LOG_OUTPUT_UART;
      }

      fflush(stdout);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GDebug::SetLogLevel(LogLevel_t level)
{
   this->m_logLevel = level;
}

//////////////////////////////////////////////////////////////////////////////////////
LogLevel_t GDebug::GetLogLevel()
{
   return m_logLevel;
}

//////////////////////////////////////////////////////////////////////////////////////
void GDebug::SetLogOutput(int output)
{
   if( (output<=0) || (output>(LOG_OUTPUT_GBD | LOG_OUTPUT_OSD | LOG_OUTPUT_UART)) )
   {
      print("Log output not supported!\n");
      printf("Log output not supported!\n");
   }
   this->m_logOutput = output;
}

//////////////////////////////////////////////////////////////////////////////////////
void GDebug::Error(char* format,...)
{ // ?? ko the chuyen tiep va_list them mot lan nua ??
   va_list list;
   va_start(list,format);
   vsnprintf(m_pFormatBuffer, sizeof(m_pFormatBuffer), format, list);
   this->Log(LOG_LEVEL_ERROR, m_pFormatBuffer);
   va_end(list);
}

//////////////////////////////////////////////////////////////////////////////////////
void GDebug::Warning(char* format,...)
{
   va_list list;
   va_start(list,format);
   vsnprintf(m_pFormatBuffer, sizeof(m_pFormatBuffer), format, list);
   this->Log(LOG_LEVEL_WARNING, m_pFormatBuffer);
   va_end(list);
}

//////////////////////////////////////////////////////////////////////////////////////
void GDebug::Monitor(char* format,...)
{
   va_list list;
   va_start(list,format);
   vsnprintf(m_pFormatBuffer, sizeof(m_pFormatBuffer), format, list);
   this->Log(LOG_LEVEL_MONITOR, m_pFormatBuffer);
   va_end(list);
}

//////////////////////////////////////////////////////////////////////////////////////
void GDebug::Info(char* format,...)
{
   va_list list;
   va_start(list,format);
   vsnprintf(m_pFormatBuffer, sizeof(m_pFormatBuffer), format, list);
   this->Log(LOG_LEVEL_INFO, m_pFormatBuffer);
   va_end(list);
}

//////////////////////////////////////////////////////////////////////////////////////
void GDebug::Print(const char* format,...)
{
   va_list list;
   va_start(list,format);
   vsnprintf(m_pFormatBuffer, sizeof(m_pFormatBuffer), format, list);
   this->Log(m_logLevel, m_pFormatBuffer, true);
   va_end(list);
}

//////////////////////////////////////////////////////////////////////////////////////
void GDebug::Debug(char* format,...)
{
   va_list list;
   va_start(list,format);
   vsnprintf(m_pFormatBuffer, sizeof(m_pFormatBuffer), format, list);
   //this->Log(LOG_LEVEL_DEBUG, m_pFormatBuffer);
   printf(m_pFormatBuffer);
   va_end(list);
}

//////////////////////////////////////////////////////////////////////////////////////
void GDebug::PrintFP(char* s, int colon)
{
   sprintf(m_pStrToPrint, "%s", s);
}

//////////////////////////////////////////////////////////////////////////////////////
#if defined(WPG_ST253_TM1650)
void GDebug::PrintFPLED()
{
   char* s = m_pStrToPrint;

   //==true nghia la string s can print giong voi string dang hien thi tren FP -> ko can lam gi
   int isOldString = true;

   for( int index = 0; index < 12; index++ )
   {
      if( m_pPrevStrToPrint[index] != m_pStrToPrint[index] )
      {
         isOldString = false;
      }
   }

   if( isOldString )
   { //van la string dang hien thi tren FP -> return luon
      return;
   }
   sprintf(m_pPrevStrToPrint, "%s", m_pStrToPrint);

   //TODO: print colon
   U8 buf[14];

   if( strlen(s) != 8 )
   {
      Error("Wrong input string '%s', proper format: 'C{ |.}C{ |.}C{ |.}C{ |.}', ex.: '5 1.8 9 '\n", s);
   }

   int i = 0, j = 0;
   while(i<8)
   {
      switch( s[i] )
      {
      case ' ':
      buf[j++] = CHAR_SPACE;
      break;

      case 'A': case 'a':
      buf[j++] = CHAR_A;
      break;

      case 'B': case 'b':
      buf[j++] = CHAR_b;
      break;

      case 'C':
      buf[j++] = CHAR_C;
      break;

      case 'c':
      buf[j++] = CHAR_c;
      break;

      case 'D': case 'd':
      buf[j++] = CHAR_d;
      break;

      case 'E': case 'e':
      buf[j++] = CHAR_E;
      break;

      case 'F': case 'f':
      buf[j++] = CHAR_F;
      break;

      case 'G': case 'g':
      buf[j++] = CHAR_g;
      break;

      case 'H':
      buf[j++] = CHAR_H;
      break;

      case 'h':
      buf[j++] = CHAR_h;
      break;

      case 'I':
      buf[j++] = CHAR_I;
      break;

      case 'i':
      buf[j++] = CHAR_i;
      break;

      case 'J': case 'j':
      buf[j++] = CHAR_J;
      break;

      case 'K': case 'k':
      buf[j++] = CHAR_K;
      break;

      case 'L':
      buf[j++] = CHAR_L;
      break;

      case 'l':
      buf[j++] = CHAR_l;
      break;

      case 'M': case 'm':
      buf[j++] = CHAR_m;
      break;

      case 'N':
      buf[j++] = CHAR_N;
      break;

      case 'n':
      buf[j++] = CHAR_n;
      break;

      case 'O':
      buf[j++] = CHAR_O;
      break;

      case 'o':
      buf[j++] = CHAR_o;
      break;

      case 'P': case 'p':
      buf[j++] = CHAR_P;
      break;

      case 'Q': case 'q':
      buf[j++] = CHAR_q;
      break;

      case 'R': case 'r':
      buf[j++] = CHAR_r;
      break;

      case 'S': case 's':
      buf[j++] = CHAR_S;
      break;

      case 'T': case 't':
      buf[j++] = CHAR_t;
      break;

      case 'U':
      buf[j++] = CHAR_U;
      break;

      case 'u':
      buf[j++] = CHAR_u;
      break;

      case 'V':
      buf[j++] = CHAR_V;
      break;

      case 'v':
      buf[j++] = CHAR_v;
      break;

      case 'W': case 'w':
      buf[j++] = CHAR_w;
      break;

      case 'X': case 'x':
      buf[j++] = CHAR_X;
      break;

      case 'Y': case 'y':
      buf[j++] = CHAR_y;
      break;

      case 'Z': case 'z':
      buf[j++] = CHAR_z;
      break;

      case '_':
      buf[j++] = CHAR__;
      break;

      case '-':
      buf[j++] = CHAR_HYPHEN;
      break;

      case '0':
      buf[j++] = CHAR_0;
      break;
      case '1':
      buf[j++] = CHAR_1;
      break;

      case '2':
      buf[j++] = CHAR_2;
      break;

      case '3':
      buf[j++] = CHAR_3;
      break;

      case '4':
      buf[j++] = CHAR_4;
      break;

      case '5':
      buf[j++] = CHAR_5;
      break;

      case '6':
      buf[j++] = CHAR_6;
      break;

      case '7':
      buf[j++] = CHAR_7;
      break;

      case '8':
      buf[j++] = CHAR_8;
      break;

      case '9':
      buf[j++] = CHAR_9;
      break;

      default:
      Warning("Unsupported LED char: '%c' (0x%02X)\n", s[i], s[i]);
      //return;
      buf[j++]=CHAR_;
      }
      if(s[i+1] == '.') buf[j-1] |= SEG_DOT;
      i+=2;
   }
   TM1650_LedDisplay(buf);
}
#else //TM1668
void GDebug::PrintFPLED()
{
   char* s = m_pStrToPrint;

   //==true nghia la string s can print giong voi string dang hien thi tren FP -> ko can lam gi
   int isOldString = true;

   for( int index = 0; index < 12; index++ )
   {
      if( m_pPrevStrToPrint[index] != m_pStrToPrint[index] )
      {
         isOldString = false;
      }
   }

   if( isOldString )
   { //van la string dang hien thi tren FP -> return luon
      return;
   }
   sprintf(m_pPrevStrToPrint, "%s", m_pStrToPrint);

   //TODO: print colon
   U8 buf[14];

   if( strlen(s) != 8 )
   {
      Error("Wrong input string '%s', proper format: 'C{ |.}C{ |.}C{ |.}C{ |.}', ex.: '5 1.8 9 '\n", s);
   }

   int i, j = 0;
   for( i = 0; i < 8; i++ )
   {
      if( i%2 )
      { // dau cham
         if( s[i] == '.' )
         {
#if defined (CHANGHONG)
            buf[0] |= SEG_DOT;
#else
            buf[2] |= SEG_DOT;
#endif
         }
      }
      else
      { // ky tu that
         switch( s[i] )
         {
         case ' ':
            buf[j++] = CHAR_SPACE;
            break;

         case 'A': case 'a':
            buf[j++] = CHAR_A;
            break;

         case 'B': case 'b':
            buf[j++] = CHAR_b;
            break;

         case 'C':
            buf[j++] = CHAR_C;
            break;

         case 'c':
            buf[j++] = CHAR_c;
            break;

         case 'D': case 'd':
            buf[j++] = CHAR_d;
            break;

         case 'E': case 'e':
            buf[j++] = CHAR_E;
            break;

         case 'F': case 'f':
            buf[j++] = CHAR_F;
            break;

         case 'G': case 'g':
            buf[j++] = CHAR_g;
            break;

         case 'H':
            buf[j++] = CHAR_H;
            break;

         case 'h':
            buf[j++] = CHAR_h;
            break;

         case 'I':
            buf[j++] = CHAR_I;
            break;

         case 'i':
            buf[j++] = CHAR_i;
            break;

         case 'J': case 'j':
            buf[j++] = CHAR_J;
            break;

         case 'K': case 'k':
            buf[j++] = CHAR_K;
            break;

         case 'L':
            buf[j++] = CHAR_L;
            break;

         case 'l':
            buf[j++] = CHAR_l;
            break;

         case 'M': case 'm':
            buf[j++] = CHAR_m;
            break;

         case 'N':
            buf[j++] = CHAR_N;
            break;

         case 'n':
            buf[j++] = CHAR_n;
            break;

         case 'O':
            buf[j++] = CHAR_O;
            break;

         case 'o':
            buf[j++] = CHAR_o;
            break;

         case 'P': case 'p':
            buf[j++] = CHAR_P;
            break;

         case 'Q': case 'q':
            buf[j++] = CHAR_q;
            break;

         case 'R': case 'r':
            buf[j++] = CHAR_r;
            break;

         case 'S': case 's':
            buf[j++] = CHAR_S;
            break;

         case 'T': case 't':
            buf[j++] = CHAR_t;
            break;

         case 'U':
            buf[j++] = CHAR_U;
            break;

         case 'u':
            buf[j++] = CHAR_u;
            break;

         case 'V':
            buf[j++] = CHAR_V;
            break;

         case 'v':
            buf[j++] = CHAR_v;
            break;

         case 'W': case 'w':
            buf[j++] = CHAR_w;
            break;

         case 'X': case 'x':
            buf[j++] = CHAR_X;
            break;

         case 'Y': case 'y':
            buf[j++] = CHAR_y;
            break;

         case 'Z': case 'z':
            buf[j++] = CHAR_z;
            break;

         case '_':
            buf[j++] = CHAR__;
            break;

         case '-':
            buf[j++] = CHAR_HYPHEN;
            break;

         case '0':
            buf[j++] = CHAR_0;
            break;
         case '1':
            buf[j++] = CHAR_1;
            break;

         case '2':
            buf[j++] = CHAR_2;
            break;

         case '3':
            buf[j++] = CHAR_3;
            break;

         case '4':
            buf[j++] = CHAR_4;
            break;

         case '5':
            buf[j++] = CHAR_5;
            break;

         case '6':
            buf[j++] = CHAR_6;
            break;

         case '7':
            buf[j++] = CHAR_7;
            break;

         case '8':
            buf[j++] = CHAR_8;
            break;

         case '9':
            buf[j++] = CHAR_9;
            break;

         default:
            Warning("Unsupported LED char: '%c' (0x%02X)\n", s[i], s[i]);
            //return;
            buf[j++]=CHAR_;
         }

         buf[j++] = 0xff;
      }
   }

   //if (colon)
   //{
   // //TODO
   //}
   TM1668_LedDisplay(buf);

}
#endif

//////////////////////////////////////////////////////////////////////////////////////
void GDebug::DisplayFPLEDDot_Red()
{
   FP_SetLockLED(TRUE);
}

//////////////////////////////////////////////////////////////////////////////////////
void GDebug::DisplayFPLEDDot_Green()
{
   FP_SetLockLED(FALSE);
}

int GDebug::GetKeyStatus()
{
   int keyCode = 0;
#if defined(LEDFRONTPANEL)
#if defined(WPG_ST253_TM1650)
   if( FP_GetStandbyKey() == 1 )
   {
      keyCode = KEY_RC_POWER;
   }
   else
   {
      keyCode = TM1650_GetKeyStatus();
   }
#else
   if( FP_GetStandbyKey() == 1 )
   {
      keyCode = KEY_FP_POWER;
   }
   else
   {
      keyCode = TM1668_GetKeyStatus();
   }
#endif
#endif
   return keyCode;
}
//////////////////////////////////////////////////////////////////////////////////////
GDebug& GDebug::getInstance()
{
   static GDebug instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
GDebug::~GDebug()
{
}

//////////////////////////////////////////////////////////////////////////////////////
char* GDebug::getClassMethodName(const std::string& strFunction)
{
   std::string strRet = "";
   size_t p1 = strFunction.find("::");
   size_t p2 = strFunction.substr(0, p1).rfind(" ") + 1;
   size_t p3 = strFunction.rfind("(") - p2;
   strRet = strFunction.substr(p2, p3) + " - ";
   return (char*)strRet.c_str();
}
//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
