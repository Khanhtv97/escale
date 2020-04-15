//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// Description: wrap all functions related to Text processing                       //
// file: APIText.cpp                                                                //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef APITEXT_HPP_
#define APITEXT_HPP_

#include "stapp_main.h"

#define   ALIGN_LEFT   0
#define   ALIGN_RIGHT  1
#define   ALIGN_CENTER 2

class TextParams_t
{
   public:
      int planeId;
      int posX, posY;
      int fontId;// 0 - 1 - 2 ... (see .h font files included in stapp/grafix/text.c)
      int color; // 0x00------
      int alpha; // 0 -0x80
      int wrap;  // TEXT_WRAP_NO || TEXT_WRAP_CHARACTER

      TextParams_t()
      {
         fontId = 0;
         color = 0x00FFFFFF;
         alpha = 0x80;
         wrap = TEXT_WRAP_NO;
      }

      ~TextParams_t(){}
};

extern "C"
{
   void APITextInit();
   int getNextFreeTextId();
   BOOL PrintText(int TextId, const char* TextString, TextParams_t obj_TextParams, int textWidth = -1, int textHeight = -1);
   BOOL PrintAlignedText(int TextId, const char* TextString, TextParams_t obj_TextParams, int TextWidth, int AlignType); // AlighType: ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER
   BOOL PrintLines(int startTextId, TextParams_t obj_TextParams, int hSpace);
   BOOL GetTextSize(const char* TextString, int fontId, int* TextWidth, int* TextHeight);
   BOOL GetTextSizeAfterPrint(int TextId, STGXOBJ_Rectangle_t* textRect);
   ST_ErrorCode_t DeleteText(int TextId);
   ST_ErrorCode_t FreeTextHandle(int TextId);
}

//////////////////////////////////////////////////////////////////////////////////////
#endif /* APITEXT_HPP_ */
