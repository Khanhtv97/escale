//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: vnconv.c                                                                   //
//////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "vnconv.h"
#include "vncharmap.h"

//namespace GBS {
//namespace STM {
//namespace Framework {
//////////////////////////////////////////////////////////////////////////////////////

int getChar(U8*, VNCharset_t, int, int*);
int fillChar(U8*, VNCharset_t, int, int);
int lookUpUnicodeTable(U16 chr);
//////////////////////////////////////////////////////////////////////////////////////
// return number of bytes in outData
//////////////////////////////////////////////////////////////////////////////////////
int vnconv(U8* inData, VNCharset_t inCode, U8* outData, VNCharset_t outCode)
{
   int i = 0, tmp =-1;
   int oi = 0;
   int charIdx; // index cua ky tu trong bang ma Viet (vncharmap.h)
   bool bUTF8Composite;

   while( inData[i] )
   {
      bUTF8Composite = false;
      if(inData[i] < 0x20)
      {
         i++;
         continue;
      }
      int j = getChar(inData, inCode, i, &charIdx);
      //printf("Next char from %d to %d, charIdx found: %d, (0x%X)\n", i, j, charIdx, TCVN3Table[charIdx]);
      if( j <= i )
      {
         break; // no more character
      }
      if(charIdx == -1 && tmp >0 && inCode == UTF8 && inData[i] > 0x7f) //not found in UTF8, continue search in UTF8 composite
      {
         int k = getChar(inData, UTF8Composite, tmp, &charIdx);
         if(charIdx >=0)
         {
            bUTF8Composite = true;
            j = k;
         }
      }

      if( charIdx >= 0 )
      {   // tim thay ky tu trong bang ma
         // tra ma cua no trong bang ma VN (outCode) va dien ky tu do vao xau ra
         if(bUTF8Composite)
         {
            if(oi >0) oi -=1;
         }
         oi = fillChar(outData, outCode, oi, charIdx);
      }
      else
      {   // ko tim thay ky tu trong bang ma
         // loai bo ky tu escape
         i += (-charIdx) - 1;
         while( i < j )
         {
            if( outCode == VIQR )
            {    // xu ly cac ky tu dac biet cua VIQR
               U8 chr = inData[i];
               if(   (chr == '^')  ||
               (chr == '+')  ||
               (chr == '\'') ||
               (chr == '`')  ||
               (chr == '?')  ||
               (chr == '~')  ||
               (chr == '.') )
               {
                  outData[oi++] = '\\';
                  outData[oi++] = chr;
                  i++;
               }
               else
               {
                  outData[oi++] = inData[i++];
               }
            }
            else
            {
               outData[oi++] = inData[i++];
            }
         }
      }
      tmp = i;
      i = j;

   }
   outData[oi] = '\0';

   return oi;
}

//////////////////////////////////////////////////////////////////////////////////////
int getTCVN3Char(U8* inData, int from_idx, int * charIdx)
{
   int i = from_idx;
   int j;
   if( ! inData[i] )
   {
      return i;
   }

   U8 chr = inData[i];
   for( j = VIET_MAX_CHARS-1; j>=0; j-- )
   {
      if( chr == TCVN3Table[j] )
      {
         *charIdx = j;
         return i+1;
      }
   }

   *charIdx = -1;
   return i+1;
}

//////////////////////////////////////////////////////////////////////////////////////
int getVIQRChar(U8* inData, int from_idx, int* charIdx)
{
   int i = from_idx;
   int j;

   if( !inData[i] )
   {
      return i;
   }

   if( inData[i] == '\\' )
   {
      if( !inData[i+1] )
      {
         return i;
      }
      // danh dau mot ky tu escape truoc du lieu that
      *charIdx = -2;
      return i+2;
   }

   U32 chr1B = 0;
   U32 chr2B = 0;
   U32 chr3B = 0;

   chr1B = inData[i];

   if( inData[i+1] )
   {
      chr2B = chr1B | (inData[i+1] << 8);
   }

   if( inData[i+2] )
   {
      chr3B = chr2B | (inData[i+2] << 16);
   }

   int found1 = 0, found2 = 0, found3 = 0;
   for( j = 0; j < VIET_MAX_CHARS; j++ )
   {
      if( chr3B == VIQRTable[j] )
      {
         found3 = j;
         break;
      }
      else if( chr2B == VIQRTable[j] )
      {
         found2 = j;
      }
      else if( chr1B == VIQRTable[j] )
      {
         found1 = j;
      }
   }

   if( found3 )
   {
      *charIdx = found3;
      return i+3;
   }

   if( found2 )
   {
      *charIdx = found2;
      return i+2;
   }

   if( found1 )
   {
      *charIdx = found1;
      return i+1;
   }

   *charIdx = -1;

   return i+1;
}

//////////////////////////////////////////////////////////////////////////////////////
int getUTF8Char(U8* inData, int from_idx, int* charIdx)
{
   int i = from_idx;
   int j;
   *charIdx = -1;
   if( !inData[i] )
   {
      return i;
   }

   U8 chr = inData[i];
   U16 uChr;

   if( chr < 0x7F )
   { // 1-byte char
      uChr = chr;
      i++;
   }
   else if( (chr & 0xE0) == 0xC0 )
   { // 2-byte char
      if( !inData[i+1] )
      {
         return i;
      }
      uChr = (inData[i] & 0x1F) << 6;
      uChr |= inData[i+1] & 0x3F;
      i += 2;
   }
   else if( (chr & 0xF0) == 0xE0 )
   { // 3-byte char
      if( !inData[i+1] )
      {
         return i;
      }
      else if(((inData[i+1] & 0xC0) != 0x80 ) ||(!inData[i+2]))
      {
         i++;
         uChr = chr;
         *charIdx = lookUpUnicodeTable(uChr);
         return i;
      }
      else
      {
        uChr = (inData[i] & 0x0F) << 12;
        uChr |= (inData[i+1] & 0x3F) << 6;
        uChr |= (inData[i+2] & 0x3F);
        i += 3;
      }
   }
   else
   {
      i++;
      uChr = chr;
   }
   //printf("getUTF8Char from %#x to %#x \n",chr,uChr);
   *charIdx = lookUpUnicodeTable(uChr);
   // TODO: phai xu ly trong hop tim dc ky tu unicode nhung ko co trong ma VN --> thay bang ky tu nao do ('?' chang han)
   return i;
}

//////////////////////////////////////////////////////////////////////////////////////
int lookUpUnicodeTable(U16 chr)
{
   for(int j = 0; j < VIET_MAX_CHARS; j++ )
   {
      if( chr == UnicodeTable[j] )
      {
        return j;
      }
   }
   return -1;
}
//////////////////////////////////////////////////////////////////////////////////////
int getUTF8CompChar(U8* inData, int from_idx, int* charIdx)
{
   int i = from_idx;
   int j = 0;

   if( !inData[i] )
   {
      return i;
   }

   U32 uChr32 = 0;

   while(j++ < 2)
   {
      U8 chr = inData[i];
      U16 uChr;
      uChr32 >>=16;
      if( chr < 0x7F )
      { // 1-byte char
         uChr = chr;
         i++;
      }
      else if( (chr & 0xE0) == 0xC0 )
      { // 2-byte char
         if( !inData[i+1] )
         {
            return i;
         }
         uChr = (inData[i] & 0x1F) << 6;
         uChr |= inData[i+1] & 0x3F;
         i += 2;
      }
      else if( (chr & 0xF0) == 0xE0 )
      { // 3-byte char
         if( !inData[i+1] )
         {
            return i;
         }
         if( !inData[i+2] )
         {
            for( j = 0; j < VIET_MAX_CHARS; j++ )
            {
               if( chr == UnicodeTable[j] )
               {
                  *charIdx = j;
                  return i+1;
               }
            }
            return i+1;
         }

         uChr = (inData[i] & 0x0F) << 12;
         uChr |= (inData[i+1] & 0x3F) << 6;
         uChr |= (inData[i+2] & 0x3F);
         i += 3;
      }
      else
      {
         *charIdx = -1;
         return i+1;
      }
      uChr32 |= (U32)uChr << 16;
   }

   for( j = 0; j < VIET_MAX_CHARS; j++ )
   {
      if( uChr32 == UnicodeCompTable[j] )
      {
         *charIdx = j;
         return i;
      }
   }
   *charIdx = -1;
   return i;
}

//////////////////////////////////////////////////////////////////////////////////////
// * return next position to read
// * out: charIdx chua idx trong bang vncharset (vncharmap.h), =-1 neu ko tim thay
// * charIdx < -1 neu ko tim thay, va co ky tu escape, so luong ky tu escape = abs(charIdx)-1
//////////////////////////////////////////////////////////////////////////////////////
int getChar(U8* inData, VNCharset_t charset, int from_idx, int* charIdx)
{
   switch( charset )
   {
   case TCVN3 :
      return getTCVN3Char(inData, from_idx, charIdx);
      break;

   case VIQR :
      return getVIQRChar(inData, from_idx, charIdx);
      break;

   case UTF8 :
      return getUTF8Char(inData, from_idx, charIdx);
      break;

   case UTF8Composite:
      return getUTF8CompChar(inData, from_idx, charIdx);
      break;
   default:
      return -1;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
int fillTCVN3Char(U8* outData, int from_idx, int charIdx)
{
   outData[from_idx] = TCVN3Table[charIdx];
   return from_idx+1;
}

//////////////////////////////////////////////////////////////////////////////////////
int fillVIQRChar(U8* outData, int from_idx, int charIdx)
{
   int i = from_idx;
   U32 chrL = VIQRTable[charIdx];
   U8 chr = chrL & 0xFF;

   if( (i > 0)  &&   (outData[i-1] == 'd')  && (chrL == 'd') )
   {
      printf("Special d found: %c\n", 'd');
      outData[i++] = '\\';
      outData[i++] = 'd';
      return i;
   }

   outData[i++] = chr;
   chr = (chrL & 0xFF00) >> 8;

   if( chr )
   {
      outData[i++] = chr;
   }

   chr = (chrL & 0xFF0000) >> 16;

   if( chr )
   {
      outData[i++] = chr;
   }

   return i;
}

//////////////////////////////////////////////////////////////////////////////////////
int fillUTF8Char(U8* outData, int from_idx, int charIdx)
{
   int i = from_idx;
   U16 uChr = UnicodeTable[charIdx];

   if( uChr <= 0x7F )
   {
      outData[i++] = (U8) (uChr & 0xFF);
      return i;
   }

   if( uChr <= 0x7ff )
   {
      outData[i++] = (U8) ((uChr >> 6) | 0xC0); // C0 = 110 00000 lay 5 bit cao
      outData[i++] = (U8) ((uChr & 0x3F) | 0x80); // lay 6 bit thap
      return i;
   }

   if( uChr <= 0xFFFF )
   {
      outData[i++] = (U8) ((uChr >> 12) | 0xE0); // E0 = 1110 0000 lay 4 bit cao
      outData[i++] = (U8) (((uChr >> 6) & 0x3F) | 0x80); // lay 6 bit thap hon
      outData[i++] = (U8) ((uChr & 0x3F) | 0x80); // lay 6 bit thap nhat
      return i;
   }

   return i;
}

//////////////////////////////////////////////////////////////////////////////////////
int fillChar(U8* outData, VNCharset_t charset, int from_idx, int charIdx)
{
   switch( charset )
   {
   case TCVN3 :
      return fillTCVN3Char(outData, from_idx, charIdx);
      break;

   case VIQR :
      return fillVIQRChar(outData, from_idx, charIdx);
      break;

   case UTF8 :
      return fillUTF8Char(outData, from_idx, charIdx);
      break;

   default:
      return -1;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
