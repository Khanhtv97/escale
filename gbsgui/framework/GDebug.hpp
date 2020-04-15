//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GDebug.hpp                                                                 //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GDEBUG_HPP_
#define GDEBUG_HPP_

#include <string>
#include <list>

//////////////////////////////////////////////////////////////////////////////////////
//               --- Configuration for releasing version ---                        //
//////////////////////////////////////////////////////////////////////////////////////

#define LOG_OUTPUT_GBD   0x01
#define LOG_OUTPUT_UART  0x02
#define LOG_OUTPUT_OSD   0x04

#define MAX_LINE_LEN 128

#define _D_ GDebug::getInstance()

typedef enum LogLevel_e
{
   LOG_LEVEL_NONE,
   LOG_LEVEL_ERROR,
   LOG_LEVEL_WARNING,
   LOG_LEVEL_MONITOR,
   LOG_LEVEL_INFO,
   LOG_LEVEL_DEBUG
}
LogLevel_t;

#if defined(DEBUG_VERSION)

#define _METHOD_NAME_ _D_.getClassMethodName(__PRETTY_FUNCTION__)

#define _DTraceError(szFormat, args...)\
{\
   std::string strFormat(_METHOD_NAME_);\
   strFormat.append(szFormat);\
   strFormat.append("\n");\
   _D_.Error((char*)strFormat.c_str(), ##args);\
}

#define _DTraceWarning(szFormat, args...)\
{\
   std::string strFormat(_METHOD_NAME_);\
   strFormat.append(szFormat);\
   strFormat.append("\n");\
   _D_.Warning((char*)strFormat.c_str(), ##args);\
}

#define _DTraceMonitor(szFormat, args...)\
{\
   std::string strFormat(_METHOD_NAME_);\
   strFormat.append(szFormat);\
   strFormat.append("\n");\
   _D_.Monitor((char*)strFormat.c_str(), ##args);\
}

#define _DTraceInfo(szFormat, args...)\
{\
   std::string strFormat(_METHOD_NAME_);\
   strFormat.append(szFormat);\
   strFormat.append("\n");\
   _D_.Info((char*)strFormat.c_str(), ##args);\
}

#define _DTraceDebug(szFormat, args...)\
{\
   std::string strFormat(_METHOD_NAME_);\
   strFormat.append(szFormat);\
   strFormat.append("\n");\
   _D_.Debug((char*)strFormat.c_str(), ##args);\
}

#else

//Release mode, does not trace
#define _DTraceError(szFormat, args...)   ; // do nothing
#define _DTraceWarning(szFormat, args...) ; // do nothing
#define _DTraceMonitor(szFormat, args...) ; // do nothing
#define _DTraceInfo(szFormat, args...)    ; // do nothing
#define _DTraceDebug(szFormat, args...)   ; // do nothing

#endif
//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
class GDebug
{
private:
   static const char* m_ppLogLevelName[];
   char m_pFormatBuffer[1024];
   int m_logOutput;
   LogLevel_t m_logLevel;

   GDebug();
   ~GDebug();

   void Log(LogLevel_e level, char* s, bool cleanOutput = false);

public:
   char m_pStrToPrint[12];
   char m_pPrevStrToPrint[12]; // dung de so sanh voi m_pStrToPrint, neu giong nhau thi ko can print

   static GDebug& getInstance();

   char* getClassMethodName(const std::string& strFunction);

   LogLevel_t GetLogLevel();
   void SetLogLevel(LogLevel_t);
   void SetLogOutput(int);

   void Error(char* format,...);
   void Warning(char* format,...);
   void Monitor(char* format,...);
   void Info(char* format,...);
   void Debug(char* format,...);

   // print output without any extra info, at current LOG_LEVEL
   void Print(const char* format,...);

   // print to 4 digit LED display, format of the string: "H E L L O" to display "HELLO"
   // without "dots", "5 1.8 9 " to display "5189" with a dot after "1"
   // strlen(s) should be 8
   // if "colon" == 1 then the colon will be displayed
   void PrintFPLED(); // hien thi ra den LED
   void PrintFP(char* s, int colon = 0); // luu lai string can print ra FP thoi, khi nao goi PrintFPLED thi moi show ra
   void DisplayFPLEDDot_Red();
   void DisplayFPLEDDot_Green();
   int GetKeyStatus();
};

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif // GDEBUG_HPP_
