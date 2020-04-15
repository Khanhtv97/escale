//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GStringUtils.cpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#include "GStringUtils.hpp"
#include <cstdlib>
#include "md5.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
int GStringUtils::nextToken(std::string input, std::string delim, U32 startPos, std::string& result)
{
   if( startPos >= input.length() )
   {
      result = "";
      return -1;
   }

   int delimPos = input.find(delim, startPos);

   if( delimPos == std::string::npos )
   {
      result = input.substr(startPos);
      return input.length();
   }
   else
   {
      result = input.substr(startPos, delimPos-startPos);
      return delimPos + delim.length();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
int GStringUtils::find(U8* buffer, U32 size, const char* pattern, U32 startPos)
{
   int len = strlen(pattern);

   if( len + startPos > size )
   {
      return -1;
   }

   int i = startPos;
   int j = 0;

   while( i < size )
   {
      if( buffer[i] == pattern[j] )
      {
         j ++;
         if( j >= len )
         {
            return i-j+1;
         }
      }
      else
      {
         i -= j;
         j = 0;
      }
      i++;
   }
   return -1;
}

//////////////////////////////////////////////////////////////////////////////////////
U8* GStringUtils::subArray(U8* buffer, U32 size, U32& outlen, U32 startPos, U32 length)
{
   if( startPos < 0 )
   {
      outlen = 0;
      return NULL;
   }

   if( length <= 0 )
   {
      length = size - startPos;
   }

   if( startPos + length > size )
   {
      outlen = 0;
      return NULL;
   }

   U8* res = (U8*)malloc((length+1) * sizeof(U8));
   // memcpy some how not work
   memcpy(res, buffer + startPos, length);
   //for (int i = 0; i < length; i++)
   //{
   //   res[i] = buffer[i+startPos];
   //}
   res[length] = '\0'; // for log output and strcpy
   outlen = length;

   return res;
}

//////////////////////////////////////////////////////////////////////////////////////
int GStringUtils::nextToken(U8* input, U32 size, char* delim, U32 startPos, U8* &result, U32& outlen)
{
   if( startPos >= size )
   {
      if(result != NULL)
      {
         result[0] = '\0';
      }
      outlen = 0;
      return -1;
   }

   U32 delimPos = find(input, size, delim, startPos);
   U32 res_lenght;

   if( delimPos < 0 )
   {
      result = subArray(input, size, res_lenght, startPos);
      outlen = res_lenght;
      return size;
   }
   else
   {
      result = subArray(input, size, res_lenght, startPos, delimPos-startPos);
      outlen = res_lenght;
      return delimPos + strlen(delim);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
int GStringUtils::parseInt(U8* buffer, int size, int defaultValue)
{
   U8 savedValue = buffer[size];
   buffer[size] = '\0';

   char* pEnd = NULL;
   int res = strtol((char*)buffer, &pEnd, 10);

   if( res == 0 )
   {
      res = defaultValue;
   }
   buffer[size] = savedValue;

   return res;
}

//////////////////////////////////////////////////////////////////////////////////////
time_t GStringUtils::parseUInt(U8* buffer, int size, int defaultValue)
{
   U8 savedValue = buffer[size];
   buffer[size] = '\0';

   char* pEnd = NULL;
   time_t res = (time_t)strtoul((char*)buffer, &pEnd, 10);

   if( res == 0 )
   {
      res = defaultValue;
   }
   buffer[size] = savedValue;

   return res;
}

//////////////////////////////////////////////////////////////////////////////////////
float GStringUtils::parseFloat(U8* buffer, int size, double defaultValue)
{
   U8 savedValue = buffer[size];
   buffer[size] = '\0';

   char* pEnd = NULL;
   float res = (float)strtod((char*)buffer, &pEnd);

   if( res == 0.0 )
   {
      res = defaultValue;
   }
   buffer[size] = savedValue;

   return res;
}

//////////////////////////////////////////////////////////////////////////////////////
int GStringUtils::str2int(std::string input, int defaultValue)
{
   char* pEnd = NULL;
   int res = strtol(input.c_str(), &pEnd, 10);

   if( res == 0 )
   {
      res = defaultValue;
   }

   return res;
}

time_t GStringUtils::str2long(std::string input, int defaultValue)
{
   char* pEnd = NULL;
   time_t res = (time_t)strtol(input.c_str(), &pEnd, 10);

   if( res == 0 )
   {
      res = defaultValue;
   }

   return res;
}

float GStringUtils::str2float(std::string input)
{
   char* pEnd = NULL;
   float ret = strtof(input.c_str(), &pEnd);
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
int GStringUtils::assignStr(char* outStr, const std::string inStr)
{
   strncpy(outStr, inStr.c_str(), inStr.length());
   outStr[inStr.length()] = '\0';

   return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
char* GStringUtils::md5(U8* buffer, int size)
{
   MD5* md5 = new MD5(buffer, size);
   char* res = md5->hex_digest();
   delete md5;
   md5 = NULL;

   return res;
}

//////////////////////////////////////////////////////////////////////////////////////
void GStringUtils::getStringFromArray(std::string& s, char* buffer, int pos, int size)
{
   char* s2 = buffer + pos;
   char c = s2[size];
   s2[size] = '\0';
   s.assign(s2);
   s2[size] = c;
}

//////////////////////////////////////////////////////////////////////////////////////
void GStringUtils::getStringFromArray(char* s, char* buffer, int pos, int size)
{
   memcpy(s, buffer + pos, size);
   s[size] = '\0';
}

//////////////////////////////////////////////////////////////////////////////////////
U32 GStringUtils::getU32FromBuffer(U8* buffer, int start_index)
{
   U32 val = buffer[start_index++];
   val += buffer[start_index++] << 8;
   val += buffer[start_index++] << 16;
   val += buffer[start_index++] << 24;

   return val;
}

//////////////////////////////////////////////////////////////////////////////////////
void GStringUtils::putU32ToBuffer(U32 value, U8* buffer, int start_index)
{
   buffer[start_index++] = value & 0x000000FF;
   buffer[start_index++] = (value & 0x0000FF00) >> 8;
   buffer[start_index++] = (value & 0x00FF0000) >> 16;
   buffer[start_index++] = (value & 0xFF000000) >> 24;
}

//////////////////////////////////////////////////////////////////////////////////////
void GStringUtils::toUpper(char* text, char* nText)
{
    for( int i = 0; i <= strlen(text); i++ )
    {
        if( (text[i] > 96 ) && (text[i] < 123) ) // is the char lower case
        {
            nText[i] = text[i] - 'a' + 'A';   //make upper
        }
        else
        {
            nText[i] = text[i]; //do nothing
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////
void GStringUtils::format(std::string& oString, const char* szFormat, ...)
{
   va_list args;
   char tmpBuffer[255];

   va_start(args, szFormat);
   vsprintf(tmpBuffer, szFormat, args);
   va_end(args);

   oString.assign(tmpBuffer);
}

//////////////////////////////////////////////////////////////////////////////////////
void GStringUtils::time2Str(const time_t time, const char* szFormat, std::string& oString)
{
   char buffer[80];
   strftime(buffer, 80, szFormat, localtime(&time));
   oString.assign(buffer);
}

//////////////////////////////////////////////////////////////////////////////////////
std::string GStringUtils::trimLeft(std::string iString)
{
   return iString.erase( 0, iString.find_first_not_of( " \f\n\r\t\v" ) );
}

//////////////////////////////////////////////////////////////////////////////////////
std::string GStringUtils::trimRight(std::string iString)
{
   return iString.erase( iString.find_last_not_of( " \f\n\r\t\v" ) + 1 );
}

//////////////////////////////////////////////////////////////////////////////////////
std::string GStringUtils::trimAll(std::string iString)
{
   return trimLeft( trimRight( iString ) );
}

//////////////////////////////////////////////////////////////////////////////////////
void GStringUtils::remove(std::string& ioString, const std::string pattern)
{
   size_t posFound = ioString.find(pattern);

   while( posFound != std::string::npos )
   {
      ioString.erase(posFound, pattern.length());
      posFound = ioString.find(pattern);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GStringUtils::replaceAll(std::string& strInOutTxt, const std::string oldTxt, const std::string newTxt)
{
   if( strInOutTxt.size() < oldTxt.size() )
   {
      return;
   }

   size_t found = strInOutTxt.find(oldTxt);
   while( found != std::string::npos )
   {
      strInOutTxt.replace(found, oldTxt.length(), newTxt);
      found = strInOutTxt.find(oldTxt);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
int GStringUtils::detectSignature(U8* buffer, int size, const char* signature)
{
   int len = strlen(signature);
   if( len > size )
   {
      return -1;
   }

   int i = 0;
   int j = 0;
   while( i < size )
   {
      if( buffer[i] == signature[j] )
      {
         j ++;
         if( j >= len )
         {
            return i-j+1;
         }
      }
      else
      {
         i -= j;
         j = 0;
      }

      i++;
   }

   return -1;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
