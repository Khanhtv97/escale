//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GInputBox.hpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GINPUTBOX_HPP_
#define GINPUTBOX_HPP_

#include "GFocusable.hpp"
#include "GDrawable.hpp"

typedef enum InputMode_e
{
   INPUT_MODE_NORMAL,
   INPUT_MODE_CAPLOCK,
   INPUT_MODE_SHIFT,
   INPUT_MODE_NUMERIC,
   INPUT_MODE_SYMBOLS1,
   INPUT_MODE_SYMBOLS2
}
InputMode_t;

#define KEY_REPEAT_TIMEOUT 1000 //in ms

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GInputBox: public GFocusable
{
protected:
   std::string m_text;
   std::string m_hintText;

   GText* m_pNormalText;
   GText* m_pFocusedText;
   GText* m_pTextView;

   // vi tri dau nhac nhap lieu
   int m_inputIndex;

   // offset cua text
   int m_xOffset;
   int m_yOffset;

   int m_lastKeyTime;
   int m_repeatCount;
   U32 m_lastKey;
   InputMode_t m_inputMode;

   void onFocusChanged();
   U32 key2Char(U32 key);

   U8 getCharOnRepeatKey(int key, int repetition);
   void init();

public:
   int m_maxCharCount;
   bool m_forceProcessKey;

   GInputBox(int x = 0, int y = 0);
   ~GInputBox();

   const char* classname() { return "GInputBox"; };
   const char* getText();

   GInputBox& resizeText(int length);
   GInputBox& setText(std::string text);
   GInputBox& setTextOffset(int xOffset, int yOffset);
   GInputBox& setHintText(std::string text);
   GInputBox& setFont(GFont* normalFont, GFont* focusFont = NULL);
   GInputBox& setInputMode(InputMode_t inputMode);

   GError_t draw();
   bool processKey(U32 key);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GINPUTBOX_HPP_ */
