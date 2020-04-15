//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GStringUtils.hpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GSTRINGUTILS_HPP_
#define GSTRINGUTILS_HPP_

#include "stapp_main.h"
#include "GDebug.hpp"

#define STRING_DATA_DELIMITER "##"
#define STRING_ITEM_DELIMITER "**"
#define STRING_MEMBER_DELIMITER "|"

#define STRING_SUB_MEMBER_DELIMITER "-=-"
#define STRING_SUB_MEMBER_DELIMITER_SETTING "==="

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
class GStringUtils
{
public:
   static int nextToken(std::string input, std::string delim, U32 startPos, std::string& result);
   static int find(U8* buffer, U32 size, const char* pattern, U32 startPos = 0);

   // length = 0 <--> get to end of string
   static U8* subArray(U8* buffer, U32 size, U32& outlen, U32 startPos, U32 length = 0);
   static int nextToken(U8* input, U32 size, char* delim, U32 startPos, U8*& result, U32& outlen);
   static int parseInt(U8* buffer, int size, int defaultValue = 0);
   static time_t parseUInt(U8* buffer, int size, int defaultValue=0);
   static float parseFloat(U8* buffer, int size, double defaultValue=0.0);
   static int str2int(std::string input, int defaultValue=0);
   static time_t str2long(std::string input, int defaultValue=0);
   static float str2float(std::string input);
   static int assignStr(char* outStr, const std::string inStr);
   static int detectSignature(U8* buffer, int size, const char* signature);
   static char* md5(U8* buffer, int size);
   static void getStringFromArray(std::string& s, char* buffer, int pos, int size);
   static void getStringFromArray(char*, char* buffer, int pos, int size);

   // get U32 4 byte value from buffer, starting at start_index
   static U32 getU32FromBuffer(U8* buffer, int start_index);
   static void putU32ToBuffer(U32 value, U8* buffer, int start_index);
   static void format(std::string& oString, const char* szFormat, ...);
   static void time2Str(const time_t time, const char* szFormat, std::string& oString);
   static void replaceAll(std::string& strInOutTxt, const std::string oldTxt, const std::string newTxt);
   static void remove(std::string& ioString, const std::string pattern);
   static std::string trimLeft(std::string iString);
   static std::string trimRight(std::string iString);
   static std::string trimAll(std::string iString);

   void   toUpper(char* text, char* nText);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif /* GSTRINGUTILS_HPP_ */
